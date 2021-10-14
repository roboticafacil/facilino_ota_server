# This project demonstrates how to use QtWebAppLib by including the
# sources into this project.

TARGET = FacilinoOTAServer
TEMPLATE = app
QT = core network serialport
CONFIG += console c++11

HEADERS += \
           src/otauploadcontroller.h \
           src/startup.h \
           src/requesthandler.h \
           src/usbuploadcontroller.h

SOURCES += src/main.cpp \
           src/otauploadcontroller.cpp \
           src/startup.cpp \
           src/requesthandler.cpp \
           src/usbuploadcontroller.cpp

OTHER_FILES += etc/* logs/* ../readme.txt

#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(../QtWebApp/qtservice/qtservice.pri)
include(../QtWebApp/httpserver/httpserver.pri)
include(../QtWebApp/logging/logging.pri)
# Not used: include(../QtWebApp/templateengine/templateengine.pri)
