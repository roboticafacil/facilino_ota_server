/**
  @file
  @author Leopoldo Armesto
*/

#include "static.h"
#include "startup.h"
#include "filelogger.h"
#include "requesthandler.h"
#include <QDir>
#include <QFile>
#include <QProcess>

/** Name of this application */
#define APPNAME "FacilinoOTAServer"

/** Publisher of this application */
#define ORGANISATION "Robotica Facil"

/** Short description of the Windows service */
#define DESCRIPTION "Web Server for Programming Arduino OTA devices with Facilino."

/** Logger class */
FileLogger* logger;

/** Search the configuration file */
QString searchConfigFile()
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString appName=QCoreApplication::applicationName();
    QString fileName(appName+".ini");

    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/etc");
    searchList.append(binDir+"/../etc");
    searchList.append(binDir+"/../../etc"); // for development without shadow build
    searchList.append(binDir+"/../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../../../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../../../../"+appName+"/etc"); // for development with shadow build
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (QString dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            // found
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using config file %s",qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
    }
    qFatal("Cannot find config file %s",qPrintable(fileName));
    return 0;
}

void Startup::start()
{
    // Initialize the core application
    QCoreApplication* app = application();
    app->setApplicationName(APPNAME);
    app->setOrganizationName(ORGANISATION);

    // Find the configuration file
    QString configFileName=searchConfigFile();

    //Arduino
    QSettings* arduinoSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    arduinoSettings->beginGroup("arduino");

    // Configure and start the TCP listener
    qDebug("ServiceHelper: Starting service");
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    listenerSettings->beginGroup("listener");
    listener=new HttpListener(listenerSettings,new RequestHandler(app,arduinoSettings),app);

    qDebug("Arduino-cli path: %s",qPrintable(arduinoSettings->value("arduino_cli_path").toString()));

    // Configure logging into a file
    QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    logSettings->beginGroup("logging");
    logger=new FileLogger(logSettings,10000,app);
    logger->installMsgHandler();


    qWarning("Startup: Service has started");
}

void Startup::stop()
{
    // Note that this method is only called when the application exits itself.
    // It is not called when you close the window, press Ctrl-C or send a kill signal.

    delete listener;
    qWarning("Startup: Service has been stopped");
}


Startup::Startup(int argc, char *argv[])
    : QtService<QCoreApplication>(argc,argv, APPNAME)
{
    setServiceDescription(DESCRIPTION);
    setStartupType(QtServiceController::AutoStartup);
}


