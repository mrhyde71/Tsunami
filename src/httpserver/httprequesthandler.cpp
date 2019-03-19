/**
  @file
  @author Stefan Frings
*/

#include "httprequesthandler.h"

using namespace stefanfrings;

HttpRequestHandler::HttpRequestHandler(QObject* parent)
    : QObject(parent)
{}

HttpRequestHandler::~HttpRequestHandler()
{}

void HttpRequestHandler::service(HttpRequest& request, HttpResponse& response)
{
    qCritical("HttpRequestHandler: you need to override the service() function");
    auto req_path = request.getPath();
    qDebug("HttpRequestHandler: request=%s %s %s",request.getMethod().data(),req_path.data(),request.getVersion().data());
    response.setStatus(501,"not implemented");
    response.write("501 not implemented",true);
}
