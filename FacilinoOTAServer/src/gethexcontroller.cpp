/**
  @file
  @author Stefan Frings
*/
#include <QDir>
#include <QCoreApplication>
#include "gethexcontroller.h"

GetHexController::GetHexController()
{}

void GetHexController::service(HttpRequest& request, HttpResponse& response, const QSettings *settings)
{
    QString fileName=request.getParameter("file");
    response.setHeader("Content-Type", "application/x-binary");
    //response.setHeader("Content-Type","application/binary");
    response.setHeader("Access-Control-Allow-Origin","*");
    //response.setHeader("Content-Disposition","attachment; filename=\"filename.bin\"");
    QFile tmpBinFile(fileName);
    printf("%s",qUtf8Printable(fileName)); printf("\n");
    QByteArray data;
    if (tmpBinFile.exists()) {
        QFile outFile(fileName);
        if (outFile.open(QIODevice::ReadOnly))
            data=outFile.readAll();
    }
    response.write(data,true);
    /*if (request.getParameter("file")=="upload")
    {
        QTemporaryFile* file=request.getUploadedFile("file1");
        if (file)
        {
            response.setHeader("Content-Type", "application/x-binary");
            response.setHeader("Content-Disposition","attachment; filename=\"filename.bin\"");
            while (!file->atEnd() && !file->error())
            {
                QByteArray buffer=file->read(65536);
                response.write(buffer);
            }
        }
        else
        {
            response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
            response.write("<html><body>");
            response.write("Upload failed!");
            response.write("</body></html>",true);
        }
    }

    else
    {
        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        response.write("<html><body>");
        response.write("Upload a INO file<p>");
        response.write("<form method=\"post\" enctype=\"multipart/form-data\">");
        response.write("  <input type=\"hidden\" name=\"action\" value=\"upload\">");
        response.write("  File: <input type=\"file\" name=\"file1\"><br>");
        response.write("  <input type=\"submit\" value=\"Upload\">");
        response.write("</form>");
        response.write("</body></html>",true);
    }
    */
    /*response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
    response.setHeader("Access-Control-Allow-Origin","*");
    response.write("<html><body>");
    response.write("</body></html>",true);
    */
}

QString GetHexController::relativePath(const QSettings *settings, const QString &value,
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
    //qDebug("%s",qUtf8Printable(settingsValue));
    return settingsValue;
}


