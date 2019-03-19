/**
  @file
  @author Stefan Frings
*/

#include "httpresponse.h"

using namespace stefanfrings;

HttpResponse::HttpResponse(QTcpSocket* socket)
{
    this->socket=socket;
    statusCode=200;
    statusText="OK";
    sentHeaders=false;
    sentLastPart=false;
    chunkedMode=false;
}

void HttpResponse::setHeader(const QByteArray& name, const QByteArray& value)
{
    Q_ASSERT(sentHeaders==false);
    headers.insert(name,value);
}

void HttpResponse::setHeader(const QByteArray& name, int value)
{
    Q_ASSERT(sentHeaders==false);
    headers.insert(name,QByteArray::number(value));
}

QMap<QByteArray,QByteArray>& HttpResponse::getHeaders()
{
    return headers;
}

void HttpResponse::setStatus(int statusCode, const QByteArray& description)
{
    this->statusCode=statusCode;
    statusText=description;
}

int HttpResponse::getStatusCode() const
{
   return this->statusCode;
}

void HttpResponse::writeHeaders()
{
    Q_ASSERT(sentHeaders==false);
    QByteArray buffer;
    buffer.append("HTTP/1.1 ");
    buffer.append(QByteArray::number(statusCode));
    buffer.append(' ');
    buffer.append(statusText);
    buffer.append("\r\n");

    for (auto head_it = headers.constBegin(); head_it != headers.constEnd(); ++head_it)
    {
        buffer.append(head_it.key());
        buffer.append(": ");
        buffer.append(head_it.value());
        buffer.append("\r\n");

    }

    for (auto cookie_it = cookies.constBegin(); cookie_it != cookies.constEnd(); ++cookie_it)
    {
        buffer.append("Set-Cookie: ");
        buffer.append(cookie_it->toByteArray());
        buffer.append("\r\n");
    }

    buffer.append("\r\n");
    writeToSocket(buffer);
    sentHeaders=true;
}

bool HttpResponse::writeToSocket(const QByteArray& data)
{
    if (socket == nullptr)
    {
        return false;
    }

    int remaining=data.size();
    const char* ptr=data.data();
    while (socket->isOpen() && remaining>0)
    {
        // If the output buffer has become large, then wait until it has been sent.
        if (socket->bytesToWrite()>16384)
        {
            socket->waitForBytesWritten(-1);
        }

        int written=socket->write(ptr,remaining);
        if (written==-1)
        {
          return false;
        }
        ptr+=written;
        remaining-=written;
    }
    return true;
}

void HttpResponse::write(const QByteArray& data, bool lastPart)
{
    Q_ASSERT(sentLastPart==false);

    // Send HTTP headers, if not already done (that happens only on the first call to write())
    if (sentHeaders==false)
    {
        // If the whole response is generated with a single call to write(), then we know the total
        // size of the response and therefore can set the Content-Length header automatically.
        if (lastPart)
        {
           // Automatically set the Content-Length header
           headers.insert("Content-Length",QByteArray::number(data.size()));
        }
        else
        {
            // if we will not close the connection at the end, them we must use the chunked mode.
            QByteArray connectionValue=headers.value("Connection",headers.value("connection"));
            bool connectionClose=QString::compare(connectionValue,"close",Qt::CaseInsensitive)==0;
            if (!connectionClose)
            {
                headers.insert("Transfer-Encoding","chunked");
                chunkedMode=true;
            }
        }

        writeHeaders();
    }

    // Send data
    if (!data.isEmpty())
    {
        if (chunkedMode)
        {
            QByteArray size=QByteArray::number(data.size(),16);
            writeToSocket(size);
            writeToSocket("\r\n");
            writeToSocket(data);
            writeToSocket("\r\n");
        }
        else
        {
            writeToSocket(data);
        }
    }

    // Only for the last chunk, send the terminating marker and flush the buffer.
    if (lastPart)
    {
        if (chunkedMode)
        {
            writeToSocket("0\r\n\r\n");
        }
        socket->flush();
        sentLastPart=true;
    }
}


bool HttpResponse::hasSentLastPart() const
{
    return sentLastPart;
}


void HttpResponse::setCookie(const HttpCookie& cookie)
{
    Q_ASSERT(sentHeaders==false);
    if (!cookie.getName().isEmpty())
    {
        cookies.insert(cookie.getName(),cookie);
    }
}


QMap<QByteArray,HttpCookie>& HttpResponse::getCookies()
{
    return cookies;
}


void HttpResponse::redirect(const QByteArray& url)
{
    setStatus(303,"See Other");
    setHeader("Location",url);
    write("Redirect",true);
}


void HttpResponse::flush()
{
    if (socket)
    {
        socket->flush();
    }
}


bool HttpResponse::isConnected() const
{
    return ((socket != nullptr) && socket->isOpen());
}
