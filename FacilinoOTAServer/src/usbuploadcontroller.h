/**
  @file
  @author Leopoldo Armesto
*/

#ifndef USBUPLOADCONTROLLER_H
#define USBUPLOADCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QProcess>

using namespace stefanfrings;

/**
  This controller displays a HTML form for code upload.
*/


class USBUploadController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(USBUploadController)
public:

    /** Constructor */
    USBUploadController();
    /** Destructor */
    ~USBUploadController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response, const QSettings *settings);
private:
    QString relativePath(const QSettings *settings, const QString &value,const QString &defaultValue);

    static const QMap<QString,QString> name2board;
    HttpResponse* currentResponse;
    QProcess *currentProcess;
    /*QProcess *process;*/
public slots:
    void onProcessFinished(int exitCode);
    void onProcessOutputUpdated();
    void onProcessStarted();
};

#endif // OTAUPLOADCONTROLLER_H
