/**
  @file
  @author Leopoldo Armesto
*/

#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "httprequesthandler.h"
#include <QtSerialPort>
#include <QProcess>

using namespace stefanfrings;

/**
  The request handler receives incoming HTTP requests and generates responses.
*/

class RequestHandler : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(RequestHandler)
public:

    /**
      Constructor.
      @param parent Parent object
    */
    RequestHandler(QObject* parent=nullptr, const QSettings* settings=nullptr);
    ~RequestHandler();

    /**
      Process an incoming HTTP request.
      @param request The received HTTP request
      @param response Must be used to return the response
    */
    void service(HttpRequest& request, HttpResponse& response);

private:
    void serialPortClose();
    void serialPortOpen(const QString &portname, const qint32 &baudrate);
    void readSerial();
    QStringList portList();
    //void updateSerialPorts();
    bool listIsEqual(const QStringList &listOne,const QStringList &listTwo);
    const QSettings* settings;
    QStringList serialPortList;
    QSerialPort *serial;
    QString dataString;
private slots:
    void updateSerialPorts();
};

#endif // REQUESTHANDLER_H
