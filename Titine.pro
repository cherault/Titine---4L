#-------------------------------------------------
#
# Project created by QtCreator 2017-06-12T15:43:23
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Titine
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio\
        -lopencv_imgproc -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d\
        -lopencv_objdetect -lopencv_flann

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
