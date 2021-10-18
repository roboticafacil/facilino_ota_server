/**
  @file
  @author Leopoldo Armesto
*/

#include <QDir>
#include <QMap>
#include <QCoreApplication>
#include <QProcess>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QCryptographicHash>
#include "otauploadcontroller.h"

const QMap<QString,QString> OTAUploadController::name2board={{"ESP8266","esp8266:esp8266:generic"}, {"NodeMCU","esp8266:esp8266:nodemcuv2"}, {"WEMOS D1R2","esp8266:esp8266:generic"}};

OTAUploadController::OTAUploadController()
{
}

OTAUploadController::~OTAUploadController()
{
}

void OTAUploadController::service(HttpRequest& request, HttpResponse& response, const QSettings *settings)
{
    QString action=request.getParameter("action");
    QString tmp_bin_file_name;
    QByteArray data;
    if ((QString::compare(action,"compile", Qt::CaseInsensitive)==0)||(QString::compare(action,"upload", Qt::CaseInsensitive)==0))
    {
        qWarning("OTA compile request received!");
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

        QString build_path=QDir(tmpDirName).filePath(QString("%1").arg(settings->value("tmp_build_dir","build").toString()));
        printf("%s",qUtf8Printable(build_path)); printf("\n");
        QDir build_dir(build_path);
        if (build_dir.exists() == true) {
            build_dir.removeRecursively();
        }

        tmpFile.open(QIODevice::WriteOnly);

        tmpFile.write(code.toLocal8Bit());
        tmpFile.close();

        arguments << "compile";
        arguments << "--fqbn" << compilation_flags;
        arguments << "-e";
        arguments << tmp_file_name;

        QString arduinoPath = settings->value("arduino_cli_path","C:/Facilino/arduino/arduino-cli.exe").toString();

        QString argString = QString("%1 %2\n").arg(arduinoPath).arg(arguments.join(" "));

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

        QString compilation_flags_reduced;
        if (compilation_flags.contains("="))
        {
            compilation_flags_reduced=compilation_flags.replace(":",".");
            int i=compilation_flags_reduced.lastIndexOf(".");
            compilation_flags_reduced.chop(i+1);
        }
        else
            compilation_flags_reduced=compilation_flags.replace(":",".");

        printf("%s",qUtf8Printable(tmpDirName)); printf("\n");
        tmp_bin_file_name = QDir(build_path).filePath(QString("%1").arg(compilation_flags_reduced)+QDir::separator()+QString("%1-%2.ino.bin").arg(settings->value("tmp_file_name","temp").toString()).arg(user));
        QString tmp_bin_file_name_short = QString("temp")+QDir::separator()+QString("temp-%1").arg(user)+QDir::separator()+QString("build")+QDir::separator()+QString("%1").arg(compilation_flags_reduced)+QDir::separator()+QString("%1-%2.ino.bin").arg(settings->value("tmp_file_name","temp").toString()).arg(user);
        QFile tmpBinFile(tmp_bin_file_name);

        if (tmpBinFile.exists()) {
            printf("%s",qUtf8Printable(tmp_bin_file_name)); printf("\n");

            QFile outFile(tmp_bin_file_name);
            if (outFile.open(QIODevice::ReadOnly))
                data=outFile.readAll();
        }
        printf("Data length: %d",data.length()); printf("\n");

        response.write(QString("Hex file:%1<br/>").arg(tmp_bin_file_name_short).toUtf8());

    }
    if (QString::compare(action,"upload", Qt::CaseInsensitive)==0)
    {
        QString deviceip=request.getParameter("deviceip");
        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        response.setHeader("Access-Control-Allow-Origin","*");
        response.write("<html><body>");
        response.write("Uploading code...");

        QEventLoop eventLoop;
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        QUrl url(QString("http://")+deviceip+QString("/update"));
        response.write(qUtf8Printable(QString("http://")+deviceip+QString("/update")));
        QNetworkRequest req(url);
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"update\" filename=\"")+tmp_bin_file_name+QString("\"")));
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
        printf("\n%s\n",qUtf8Printable(tmp_bin_file_name));
        QFile *file = new QFile(tmp_bin_file_name);
        file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart
        file->open(QIODevice::ReadOnly);
        filePart.setBodyDevice(file);

        multiPart->append(filePart);
        QNetworkReply *reply = mgr.post(req,multiPart);
        printf("Uploading code...");
        eventLoop.exec(); // blocks stack until "finished()" has been called

        response.write("  Done!<p>");
        printf(" done!");
        response.write("</body></html>",true);
    }
}

QString OTAUploadController::relativePath(const QSettings *settings, const QString &value,
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
    return settingsValue;
}

QByteArray OTAUploadController::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

void OTAUploadController::onProcessFinished(int exitCode) {
    printf("Finished with code: %d.\n",exitCode);
    currentResponse->write(QString("Finished with code: %1\n").arg(exitCode).toUtf8());
    currentResponse->write("<br/>");
    currentResponse->flush();
}

void OTAUploadController::onProcessOutputUpdated() {
    printf("%s",qUtf8Printable(QString(currentProcess->readAllStandardOutput())));
    currentResponse->write(".");
    currentResponse->flush();
}

void OTAUploadController::onProcessStarted() {
    printf("Building...\n");
    currentResponse->write("Building...");
    currentResponse->write("<br/>");
    currentResponse->flush();
}
