#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>
#include <QtCore>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Mat frame;
    VideoCapture cap;

    bool reticule = false;

private slots:
    void Frame();

    void readSerial();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_checkBox_reticule_clicked(bool checked);

    void on_radioButton_blanc_clicked();
    void on_radioButton_noir_clicked();
    void on_radioButton_rouge_clicked();
    void on_radioButton_vert_clicked();
    void on_radioButton_bleu_clicked();

    void Square(Mat&, Point, const Scalar&, int, int, int);
    void TriangleDown(Mat&, Point, const Scalar&, int, int, int);

private:
    Ui::MainWindow *ui;

    //--ID arduino--//
    //--------------//
    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 9025; //nano
    static const quint16 arduino_uno_product_id = 67; //nano

    //--serial + parsing--//
    //--------------------//
    QByteArray serialData;
    QString serialBuffer;

    QString parseData1; //satellites
    QString parseData2; //altitude
    QString parseData3; //vitesse

    QString parseData4; //jour
    QString parseData5; //mois
    QString parseData6; //annee

    QString parseData7; //heure
    QString parseData8; //minute
    QString parseData9; //seconde

    int satellites;
    float altitude;
    int vitesse;

    int jour, mois, annee;
    int heure, minute, seconde;

    //--def. couleurs--//
    //-----------------//
    Scalar blanc = Scalar(255,255,255);
    Scalar rouge = Scalar(0,0,255);
    Scalar vert = Scalar(0,255,0);
    Scalar bleu = Scalar(255,0,0);
    Scalar noir = Scalar(0,0,0);

    Scalar couleur;

    //--cst. echelle--//
    //----------------//
    int epais = 40;
    int delta = 40;
    int scale = 10;
    int offset = 20;
    int taille = 60;
    int vMax = 120;

    //--reticule--//
    //------------//
    Point center;
    Point pt1, pt2;
    Point pt3, pt4;
    Point pt5, pt6;
    Point pt7, pt8;
};

#endif // MAINWINDOW_H
