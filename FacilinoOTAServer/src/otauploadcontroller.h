#ifndef OTAUPLOADCONTROLLER_H
#define OTAUPLOADCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QProcess>
#include <QCryptographicHash>

using namespace stefanfrings;

/**
  This controller displays a HTML form for code upload.
*/


class OTAUploadController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(OTAUploadController)
public:

    /** Constructor */
    OTAUploadController();
    /** Destructor */
    ~OTAUploadController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response, const QSettings *settings);
private:
    QString relativePath(const QSettings *settings, const QString &value,const QString &defaultValue);
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);
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
