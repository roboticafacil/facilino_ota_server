/**
  @file
  @author Leopoldo Armesto
*/

#include "requesthandler.h"
#include "gethexcontroller.h"
#include "usbuploadcontroller.h"
#include "otauploadcontroller.h"
#include "filelogger.h"
#include <QTimer>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/** Logger class */
extern FileLogger* logger;

RequestHandler::RequestHandler(QObject* parent, const QSettings* settings)
    :HttpRequestHandler(parent)
{
    this->settings=settings;
    serial = nullptr;
    serialPortClose();

    updateSerialPorts();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateSerialPorts()));
    timer->start(5000);
}

RequestHandler::~RequestHandler()
{
    //delete process;
}

void RequestHandler::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
    QString str(path);
    printf("%s",qUtf8Printable(path)); printf("\n");
    if (path.startsWith("/usb_upload"))
    {
        USBUploadController().service(request, response,this->settings);
        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        response.setHeader("Access-Control-Allow-Origin","*");

    }
    else if (path.startsWith("/ota_upload"))
    {
        //printf("OTA request\n");
        OTAUploadController().service(request, response,this->settings);


    }
    else if (path.startsWith("/list_ports"))
    {
           updateSerialPorts();
           response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
           response.setHeader("Access-Control-Allow-Origin","*");
           printf("Detected USB ports %d",serialPortList.length()); printf("\n");
           response.write("[");
           for (int i=0;i<serialPortList.length();i++)
           {
               response.write(QString("\"%1\",").arg(serialPortList[i]).toUtf8());
           }
           //qDebug("hello");
           //printf("hello"); printf("\n");
           response.write("]",true);
    }
    else
    {
    qDebug("Controller: path=%s",path.data());

    // Set a response header
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
    response.setHeader("Access-Control-Allow-Origin","*");

    // Return a simple HTML document
    response.write("<html><body>To compile files, send GET to /ota_upload.</body></html>",true);

    }
    qDebug("Conroller: finished request");

    // Clear the log buffer
    if (logger)
    {
       logger->clear();
    }

}

void RequestHandler::serialPortClose() {
    // Close serial connection
    if (serial == nullptr) return;
    serial->close();
    serial->disconnect(serial, SIGNAL(readyRead()), this, SLOT(readSerial()));
}

void RequestHandler::serialPortOpen(const QString &portname, const qint32 &baudrate) {
    // Open serial connection
    // No available connections, nothing to do
    //if (ui->serialPortBox->currentText() == "") return;

    if (serial == nullptr) {
        // Create serial connection
        serial = new QSerialPort(this);
    } else if (serial->isOpen()) {
        serial->close();
    }

    // Set default connection parameters
    serial->setPortName(portname);
    serial->setBaudRate(baudrate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);

    // Connect
    if (serial->open(QIODevice::ReadWrite)) {
        // Execute readSerial if data is available
        connect(serial, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }
}

void RequestHandler::readSerial() {
    // Read serial port data and display it in the console
    QByteArray data = serial->readAll();

    // Read serial port data and display it in the console
    for (int i = 0; i < data.size(); i++) {
        int c = data.at(i);
        if (c > 13) {
            dataString.append(data.at(i));
        } else if (c == 13) {
            //bool ok;
            //int value = dataString.toInt(&ok);
            // Reset string
            dataString.clear();
        }
    }
}

QStringList RequestHandler::portList() {
    // Return list of serial ports
    QStringList serialPorts;

    // Get list
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        // Add to the list
        QString portName = info.portName();
#ifdef Q_OS_LINUX
        portName.insert(0, "/dev/");
#endif
#ifdef Q_OS_OSX
        portName.insert(0, "/dev/");
#endif
        serialPorts.append(portName);
    }
    return serialPorts;
}
void RequestHandler::updateSerialPorts(){
    // Update the list of available serial ports in the combo box
    QStringList ports = portList();
    if (!listIsEqual(serialPortList, ports)) {
        serialPortList = ports;
    }
}

bool RequestHandler::listIsEqual(const QStringList &listOne,const QStringList &listTwo) {
    // Compare two string lists. Return true if equal.
    if (listOne.count() != listTwo.count()) return false;
    for (int i = 0; i < listOne.count(); i++) {
        if (listOne[i] != listTwo[i]) return false;
    }
    return true;
}

/*void RequestHandler::onProcessFinished(int exitCode) {
    qDebug("Finished with code: %d.",exitCode);
}

void RequestHandler::onProcessOutputUpdated() {
    qDebug("%s",qUtf8Printable(QString(process->readAllStandardOutput())));
}

void RequestHandler::onProcessStarted() {
    qDebug("Building...");
}*/

