#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>

//--constructeur--//
//----------------//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //--parse data--//
    //--------------//
    serialBuffer = "";

    parseData1 = "";
    parseData2 = "";
    parseData3 = "";

    parseData4 = "";
    parseData5 = "";
    parseData6 = "";

    parseData7 = "";
    parseData8 = "";
    parseData9 = "";

    //--declare arduino--//
    //-------------------//
    arduino = new QSerialPort(this);

    qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";

    //--detect ID arduino--//
    //---------------------//
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Description: " << serialPortInfo.description() << "\n";
        qDebug() << "Has vendor id?: " << serialPortInfo.hasVendorIdentifier() << "\n";
        qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier() << "\n";
        qDebug() << "Has product id?: " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: " << serialPortInfo.productIdentifier() << "\n";
    }

    bool arduino_is_available = false;
    QString arduino_uno_port_name;

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if((serialPortInfo.productIdentifier() == arduino_uno_product_id)
                    && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id))
            {
                arduino_is_available = true;
                arduino_uno_port_name = serialPortInfo.portName();
            }
        }
    }

    //--config port serie--//
    //---------------------//
    if(arduino_is_available)
    {
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_uno_port_name);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud115200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));

        ui->label_daq->setText("CONNECTED");
    }
    else
    {
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");

        ui->label_daq->setText("DISCONNECTED");
    }
}

//--destructeur--//
//---------------//
MainWindow::~MainWindow()
{
    if(arduino->isOpen())
    {
        arduino->close();
    }
    delete ui;
}

//--bouton start--//
//----------------//
void MainWindow::on_pushButton_clicked()
{
    cap.open(0);

    while(true)
    {
        cap >> frame;

        //--reticule--//
        //------------//
        if(reticule)
        {
            Square(frame, Point(frame.cols/2, frame.rows/2), couleur, taille, 1, 8);

            center = Point(frame.cols/2, frame.rows/2);
            circle(frame, center, 1, couleur, 2);

            pt1 = Point(0, frame.rows/2);
            pt2 = Point(frame.cols/2 - offset, frame.rows/2);
            line(frame, pt1, pt2, couleur, 1);

            pt3 = Point(frame.cols/2 + offset, frame.rows/2);
            pt4 = Point(frame.cols, frame.rows/2);
            line(frame, pt3, pt4, couleur, 1);

            pt5 = Point(frame.cols/2, 0);
            pt6 = Point(frame.cols/2, frame.rows/2 - offset);
            line(frame, pt5, pt6, couleur, 1);

            pt7 = Point(frame.cols/2, frame.rows/2 + offset);
            pt8 = Point(frame.cols/2, frame.rows);
            line(frame, pt7, pt8, couleur, 1);
        }

        //--masque bas--//
        //--------------//
        rectangle(frame, Point(0, frame.rows), Point(frame.cols, frame.rows-epais), noir, -1);

        //--echelle vitesse--//
        //-------------------//
        for(int i=0; i<frame.cols; i+=delta)
        {
              line(frame, Point(i, frame.rows), Point(i, frame.rows-scale), blanc,1);
        }

        //--masque haut--//
        //---------------//
        rectangle(frame, Point(0,0), Point(frame.cols, epais), noir, -1);

        //--parse data -> string--//
        //------------------------//
        satellites = parseData1.toFloat();
        string a = parseData1.toStdString().c_str();

        altitude = parseData2.toFloat();
        string b = parseData2.toStdString().c_str();

        vitesse = parseData3.toFloat();
        string c = parseData3.toStdString().c_str();

        //--parse date--//
        //--------------//
        jour = parseData4.toInt();
        mois = parseData5.toInt();
        annee = parseData6.toInt();

        ui->label_jour->setText(QString::number(jour));
        ui->label_mois->setText(QString::number(mois));
        ui->label_annee->setText(QString::number(annee));

        //--parse heure--//
        //---------------//
        heure = parseData7.toInt();
        heure += 2;
        minute = parseData8.toInt();
        seconde = parseData9.toInt();

        ui->label_heure->setText(QString::number(heure));
        ui->label_minute->setText(QString::number(minute));
        ui->label_seconde->setText(QString::number(seconde));

        //--afichage data masque haut--//
        //-----------------------------//
        putText(frame, "Satellites : ", Point(10,25),1,1,blanc,1);
        putText(frame, a, Point(110,25),1,1,blanc,1);

        putText(frame, "Altitude : ", Point(230,25),1,1,blanc,1);
        putText(frame, b, Point(320,25),1,1,blanc,1);

        putText(frame, "Vitesse : ", Point(470,25),1,1,blanc,1);
        putText(frame, c, Point(560,25),1,1,blanc,1);

        //--cuseur vitesse masque bas--//
        //-----------------------------//
        TriangleDown(frame, Point(vitesse, frame.rows - offset), blanc, 20, 1, 8);

        //--int -> string--//
        //-----------------//
        stringstream val;
        val<<(vitesse*vMax)/frame.cols;

        //--affiche valeur curseur masque bas--//
        //-------------------------------------//
        putText(frame, val.str(), Point(vitesse -5,frame.rows-offset-7),1,1, blanc, 1);

        Frame();

        waitKey(32);
    }
}

//--bonton stop--//
//---------------//
void MainWindow::on_pushButton_2_clicked()
{
    destroyAllWindows();
    close();
    exit(0);
}

//--affichage video--//
//-------------------//
void MainWindow::Frame()
{
    cv::resize(frame, frame, Size(ui->label->width(), ui->label->height()));
    cvtColor(frame, frame,CV_BGR2RGB);

    QImage imgFrame= QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    QPixmap pixFrame = QPixmap::fromImage(imgFrame);

    ui->label->setPixmap(pixFrame);
}

//--recoit serial data--//
//----------------------//
void MainWindow::readSerial()
{
    QStringList buffer_split = serialBuffer.split(",");

    if(buffer_split.length() < 9) //9 datas
    {
        serialData = arduino->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }
    else
    {
        //--parse data--//
        //--------------//
        serialBuffer = "";
        qDebug() << buffer_split << endl;

        if(serialBuffer.isEmpty())
        {
            ui->label_gps->setText("CONNECTED");
        }

        if(parseData1 != '$')
        {
            parseData1 = buffer_split[0];
            parseData2 = buffer_split[1];
            parseData3 = buffer_split[2];

            parseData4 = buffer_split[3];
            parseData5 = buffer_split[4];
            parseData6 = buffer_split[5];

            parseData7 = buffer_split[6];
            parseData8 = buffer_split[7];
            parseData9 = buffer_split[8];
        }

        qDebug() << parseData1 << " " << parseData2 << endl;

       float a = parseData1.toFloat();
       float b = parseData2.toFloat();
       qDebug() << a << " " << b << endl;
    }
}

//--cond. affichage reticule--//
//----------------------------//
void MainWindow::on_checkBox_reticule_clicked(bool checked)
{
    if(checked==true)
    {
        reticule=true;
    }
    else
    {
        reticule=false;
    }
}

//--definition des couleurs reticule--//
//------------------------------------//
void MainWindow::on_radioButton_blanc_clicked()
{
    couleur = blanc;
}

void MainWindow::on_radioButton_noir_clicked()
{
    couleur = noir;
}

void MainWindow::on_radioButton_rouge_clicked()
{
    couleur = rouge;
}

void MainWindow::on_radioButton_vert_clicked()
{
    couleur = vert;
}

void MainWindow::on_radioButton_bleu_clicked()
{
    couleur = bleu;
}

//--centre du reticule--//
//----------------------//
void MainWindow::Square(Mat& img, Point position, const Scalar& color, int markerSize, int thickness, int line_type)
{
    line(img, Point(position.x-(markerSize/2), position.y-(markerSize/2)), Point(position.x+(markerSize/2), position.y-(markerSize/2)), color, thickness, line_type);
    line(img, Point(position.x+(markerSize/2), position.y-(markerSize/2)), Point(position.x+(markerSize/2), position.y+(markerSize/2)), color, thickness, line_type);
    line(img, Point(position.x+(markerSize/2), position.y+(markerSize/2)), Point(position.x-(markerSize/2), position.y+(markerSize/2)), color, thickness, line_type);
    line(img, Point(position.x-(markerSize/2), position.y+(markerSize/2)), Point(position.x-(markerSize/2), position.y-(markerSize/2)), color, thickness, line_type);
}

//--curseur vitesse--//
//-------------------//
void MainWindow::TriangleDown(Mat& img, Point position, const Scalar& color, int markerSize, int thickness, int line_type)
{
    line(img, Point(position.x+(markerSize/2), position.y), Point(position.x, position.y+(markerSize/2)), color, thickness, line_type);
    line(img, Point(position.x, position.y+(markerSize/2)), Point(position.x-(markerSize/2), position.y), color, thickness, line_type);
    line(img, Point(position.x-(markerSize/2), position.y), Point(position.x+(markerSize/2), position.y), color, thickness, line_type);
}
