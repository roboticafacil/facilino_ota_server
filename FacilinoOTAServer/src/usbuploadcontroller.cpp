#include <QDir>
#include <QMap>
#include <QCoreApplication>
#include <QProcess>
#include <QHostInfo>
#include <QHostAddress>
#include "usbuploadcontroller.h"

/*QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}*/

const QMap<QString,QString> USBUploadController::name2board={{"ArduinoNano","arduino:avr:nano:cpu=atmega328"},{"ArduinoNanoOld","arduino:avr:nano:cpu=atmega328old"},{"ArduinoUno","arduino:avr:uno"},{"ArduinoMega2560","arduino:avr:mega"},{"ESP-01",""},{"ESP8266","esp8266:esp8266:generic"}, {"NodeMCU","esp8266:esp8266:nodemcuv2"}, {"WEMOS_D1R2","esp8266:esp8266:generic"}, {"ESP32","esp8266:esp8266:generic"},{"WEMOS_D1R32_SHIELD","esp8266:esp8266:generic"},{"ESP32_DevKitC","esp8266:esp8266:generic"}};

USBUploadController::USBUploadController()
{
    /*process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
    connect(process,SIGNAL(started()),this,SLOT(onProcessStarted()));
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(onProcessOutputUpdated()));
    connect(process,SIGNAL(finished(int)),this,SLOT(onProcessFinished(int)));*/
}

USBUploadController::~USBUploadController()
{
    //delete process;
}

void USBUploadController::service(HttpRequest& request, HttpResponse& response, const QSettings *settings)
{
    QString action=request.getParameter("action");
    if (QString::compare(action,"upload", Qt::CaseInsensitive)==0)
    {
        qWarning("USB upload request received!");
        QStringList arguments;
        QString code = request.getParameter("code");
        QString user = request.getParameter("user");
        QString port = request.getParameter("port");
        QString compilation_flags = request.getParameter("compilation_flags");

        //Check port
        bool valid_port=true;

        QString tmpDirName=QString("%1%2-%3").arg(relativePath(settings,"tmp_dir_name","temp")+QDir::separator()).arg(settings->value("tmp_dir_name","temp").toString()).arg(user);
        QDir dir(tmpDirName);
        if (dir.exists() == false) {
            dir.mkdir(tmpDirName);
        }

        // Check if tmp file exists
        QString tmp_file_name = QDir(tmpDirName).filePath(QString("%1-%2.ino").arg(settings->value("tmp_file_name","temp").toString()).arg(user));
        QFile tmpFile(tmp_file_name);
        if (tmpFile.exists()) {
            tmpFile.remove();
        }
        tmpFile.open(QIODevice::WriteOnly);

        tmpFile.write(code.toLocal8Bit());
        tmpFile.close();

        arguments <<"upload";

        arguments << "--fqbn" << compilation_flags;

        arguments << "-p" << port;

        arguments << tmp_file_name;

        QString arduinoPath = settings->value("arduino_cli_path","C:\\Facilino\\arduino\\arduino-cli.exe").toString();

        QString argString = QString("%1 %2\n").arg(arduinoPath).arg(arguments.join(" "));

        qWarning("%s",qUtf8Printable(argString));
        printf("%s",qUtf8Printable(argString)); printf("\n");

        QProcess *process = new QProcess();
        if (valid_port)
        {
            process->setProcessChannelMode(QProcess::MergedChannels);
            connect(process,SIGNAL(started()),this,SLOT(onProcessStarted()));
            connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(onProcessOutputUpdated()));
            connect(process,SIGNAL(finished(int)),this,SLOT(onProcessFinished(int)));

            currentResponse=&response;
            currentProcess = process;
        }

        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        response.setHeader("Access-Control-Allow-Origin","*");
        response.write("<html><body>");

        if (valid_port)
            process->start(arduinoPath, arguments);

        response.write(argString.toUtf8());
        response.flush();

        if (valid_port)
        {
            process->waitForFinished(60000);
            QString process_output= QString(process->readAllStandardOutput());
            delete process;
        }

        //response.write(QString("%1 %2\n").arg(arduinoPath).arg(arguments.join(" ")).toUtf8());
        //response.write(process_output.toUtf8());
        //response.write(hostip.toUtf8()+"\n");
        //response.write(processor.toUtf8()+"\n");
        //response.write(code.toUtf8());
        response.write("</body></html>",true);
    }
    else if (QString::compare(action,"compile", Qt::CaseInsensitive)==0)
    {
        qDebug("USB compile request received!");
        QStringList arguments;
        QString code = request.getParameter("code");
        QString user = request.getParameter("user");
        QString compilation_flags = request.getParameter("compilation_flags");

        QString tmpDirName=QString("%1%2-%3").arg(relativePath(settings,"tmp_dir_name","temp")+QDir::separator()).arg(settings->value("tmp_dir_name","temp").toString()).arg(user);
        QDir dir(tmpDirName);
        if (dir.exists() == false) {
            dir.mkdir(tmpDirName);
        }

        // Check if tmp file exists
        QString tmp_file_name = QDir(tmpDirName).filePath(QString("%1-%2.ino").arg(settings->value("tmp_file_name","temp").toString()).arg(user));
        QFile tmpFile(tmp_file_name);
        if (tmpFile.exists()) {
            tmpFile.remove();
        }
        tmpFile.open(QIODevice::WriteOnly);

        tmpFile.write(code.toLocal8Bit());
        tmpFile.close();

        arguments << "compile";
        arguments << "--fqbn" << compilation_flags;
        arguments << tmp_file_name;

        //ui->textBrowser->append(QString("%1 %2").arg(settings->arduinoCLIPath()).arg(arguments.join(" ")));
        //upload=false;
        //process->start(settings->arduinoCLIPath(), arguments);

        QString arduinoPath = settings->value("arduino_cli_path","C:\\Facilino\\arduino\\arduino-cli.exe").toString();

        QString argString = QString("%1 %2\n").arg(arduinoPath).arg(arguments.join(" "));

        qWarning("%s",qUtf8Printable(argString));
        printf("%s",qUtf8Printable(argString)); printf("\n");

        QProcess *process = new QProcess();
        process->setProcessChannelMode(QProcess::MergedChannels);
        connect(process,SIGNAL(started()),this,SLOT(onProcessStarted()));
        connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(onProcessOutputUpdated()));
        connect(process,SIGNAL(finished(int)),this,SLOT(onProcessFinished(int)));

        currentResponse=&response;
        currentProcess = process;

        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        response.setHeader("Access-Control-Allow-Origin","*");
        response.write("<html><body>");

        process->start(arduinoPath, arguments);

        response.write(argString.toUtf8());
        response.flush();

        process->waitForFinished(60000);
        QString process_output= QString(process->readAllStandardOutput());
        delete process;

        //response.write(QString("%1 %2\n").arg(arduinoPath).arg(arguments.join(" ")).toUtf8());
        //response.write(process_output.toUtf8());
        //response.write(hostip.toUtf8()+"\n");
        //response.write(processor.toUtf8()+"\n");
        //response.write(code.toUtf8());
        response.write("</body></html>",true);
    }
    else
    {
        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        response.setHeader("Access-Control-Allow-Origin","*");
        response.write("<html><body>");
        response.write("Upload Arduino code<p>");
        //response.write("<textarea name=\"code\" form=\"usrform\" style=\"float: none; width: 100%; height: 80%;\">void setup()\n{\n}\n \nvoid loop()\n{\n}\n</textarea>");
        response.write("<textarea name=\"code\" form=\"usrform\" style=\"float: none; width: 100%; height: 80%;\">#include <ESP8266WiFi.h>\n#include <ESP8266mDNS.h>\n#include <WiFiUdp.h>\n#include <ArduinoOTA.h>\n#ifndef STASSID\n#define STASSID \"RoboticaFacil\"\n#define STAPSK  \"FRQ3DfPK\"\n#endif\nconst char* ssid = STASSID;\nconst char* password = STAPSK;\nvoid setup() {\nSerial.begin(115200);\nSerial.println(\"Booting\");\nWiFi.mode(WIFI_STA);\nWiFi.begin(ssid, password);\nwhile (WiFi.waitForConnectResult() != WL_CONNECTED) {\nSerial.println(\"Connection Failed! Rebooting...\");\ndelay(5000);\nESP.restart();\n}\nArduinoOTA.setPort(8266);\nArduinoOTA.setHostname(\"myesp8266\");\nArduinoOTA.onStart([]() {\nString type;\nif (ArduinoOTA.getCommand() == U_FLASH) {\ntype = \"sketch\";\n} else { // U_FS\ntype = \"filesystem\";\n}\nSerial.println(\"Start updating \" + type);\n});\nArduinoOTA.onEnd([]() {\nSerial.println(\"\\\"\\nEnd\");\n});\nArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {\nSerial.printf(\"Progress: %u%%\\r\", (progress / (total / 100)));\n});\nArduinoOTA.onError([](ota_error_t error) {\nSerial.printf(\"Error[%u]: \", error);\nif (error == OTA_AUTH_ERROR) {\nSerial.println(\"Auth Failed\");\n} else if (error == OTA_BEGIN_ERROR) {\nSerial.println(\"Begin Failed\");\n} else if (error == OTA_CONNECT_ERROR) {\nSerial.println(\"Connect Failed\");\n} else if (error == OTA_RECEIVE_ERROR) {\nSerial.println(\"Receive Failed\");\n} else if (error == OTA_END_ERROR) {\nSerial.println(\"End Failed\");\n}\n});\nArduinoOTA.begin();\nSerial.println(\"Ready\");\nSerial.print(\"IP address: \");\nSerial.println(WiFi.localIP());\n}\nvoid loop() {\nArduinoOTA.handle();\n}</textarea>");
        //response.write(QString("<textarea name=\"code\" form=\"usrform\" style=\"float: none; width: 100%; height: 80%;\">%1").arg(code));
        response.write("<form method=\"post\" enctype=\"multipart/form-data\" id=\"usrform\">");
        response.write("  <input type=\"hidden\" name=\"action\" value=\"upload\">");
        response.write("  Board: <select id=\"processor\" name=\"processor\"><option value=\"ESP8266\">ESP8266</option><option value=\"NodeMCU\">NodeMCU</option><option value=\"WEMOS D1R2\">WeMos D1R2</option><option value=\"ESP32\">ESP32</option><option value=\"WEMOS D1R32\">WeMos D1R32</option><option value=\"WEMOS D1R32 SHIELD\">WeMos D1R32 (Sensor Shield)</option></select>");
        response.write("  Hostname/Board IP: <input type=\"text\" name=\"hostip\" value=\"\">");
        response.write("  <input type=\"submit\" value=\"Upload\">");
        response.write("</form>");
        response.write("</body></html>",true);
    }
}

QString USBUploadController::relativePath(const QSettings *settings, const QString &value,
                                    const QString &defaultValue) {

    QString settingsValue = settings->value(value,defaultValue).toString();

    if (settingsValue.left(2) == "~/") {
        // Substitute with home dir
        return QDir::homePath() + settingsValue.remove(0,1);
    } else if (QDir::isRelativePath(settingsValue)) {
        // Append the binary path if relative
        return QDir(QCoreApplication::applicationDirPath()).
                filePath(settingsValue);
    }
    qWarning("%s",qUtf8Printable(settingsValue));
    return settingsValue;
}

void USBUploadController::onProcessFinished(int exitCode) {
    qWarning("Finished with code: %d.",exitCode);
    printf("Finished with code: %d.\n",exitCode);
    currentResponse->write(QString("Finished with code: %1\n").arg(exitCode).toUtf8());
    currentResponse->write("<br/>");
    currentResponse->flush();
}

void USBUploadController::onProcessOutputUpdated() {
    qWarning("%s",qUtf8Printable(QString(currentProcess->readAllStandardOutput())));
    printf("%s",qUtf8Printable(QString(currentProcess->readAllStandardOutput())));
    currentResponse->write(qUtf8Printable(QString(currentProcess->readAllStandardOutput())));
    //currentResponse->write(".");
    currentResponse->flush();
}

void USBUploadController::onProcessStarted() {
    qWarning("Building...");
    printf("Building...\n");
    currentResponse->write("Building...");
    currentResponse->write("<br/>");
    currentResponse->flush();
}
