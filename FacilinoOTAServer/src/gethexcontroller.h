/**
  @file
  @author Stefan Frings
*/

#ifndef GETHEXCONTROLLER_H
#define GETHEXCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

/**
  This controller displays a HTML form for file upload.
*/


class GetHexController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(GetHexController)
public:

    /** Constructor */
    GetHexController();
    QString relativePath(const QSettings *settings, const QString &value,const QString &defaultValue);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response, const QSettings *settings);
};

#endif // GETHEXCONTROLLER_H
