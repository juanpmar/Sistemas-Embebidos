#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QString>
#include <QtSerialPort>
#include <stdio.h>
#include <QVector>
#include <QTimer>

QTimer *timer1 = new QTimer();
Widget::Widget(QWidget *parent) :

    QWidget(parent),
    ui(new Ui::Widget)

{
    ui->setupUi(this);
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
        /*qDebug()<<"Name:"<<info.portName();
        qDebug()<<"Manufacturer:"<<info.manufacturer();
        qDebug()<<"ID:"<<info.vendorIdentifier();
        qDebug()<<"Vendor:"<<info.hasVendorIdentifier();
        qDebug()<<"Product:"<<info.hasProductIdentifier();
        qDebug()<<"VendorID:"<<info.vendorIdentifier();
        qDebug()<<"ProductID:"<<info.productIdentifier();*/
        //product 67 de un arduino y vendor 9025
        //if(info.vendorIdentifier()==vendor && info.productIdentifier()==product){
        if(info.vendorIdentifier()==vendor && info.productIdentifier()==product){
            portn=info.portName();
            ui->pushButton->setEnabled(true);            
            ui->conect->setText("Conectado");
            qDebug()<<"si";
            connect(timer1,                            //conexion entre el timer y la funcion incrementar
                    SIGNAL(timeout()),
                    this,
                    SLOT(request())
                    );
            timer1->setInterval(100);//cada 100 msegundo llamará a segundo, para enviar request
            timer1->start();
            break;
        }else{
            ui->pushButton->setEnabled(false);
            ui->conect->setText("Desconectado");
        }
     }
        Widget:Grafo();
        sl=new QSerialPort(this);
}

Widget::~Widget()
{
    delete ui;
}
///EDIT CREACION DE SET GRAFO
void Widget::Grafo(){

    x.resize(101);
    y.resize(101);
    //z.resize(101);
    //w.resize(101);
    for (int i=0; i<101; ++i)
    {
      x[i] = (double)i;
      y[i] = (double)2;
      //z[i] = (double)i;//segunda grafica
      //w[i] = (double)4;
    }

    ui->CustomPlot->addGraph(ui->CustomPlot->xAxis, ui->CustomPlot->yAxis);
    ui->CustomPlot->addGraph(ui->CustomPlot->xAxis, ui->CustomPlot->yAxis2);
    ui->CustomPlot->graph(0)->setData(x, y);
    //ui->CustomPlot->graph(1)->setData(z, w);//segunda grafica
    ui->CustomPlot->graph(0)->setPen(QPen(Qt::green));//color verde brush gris
    ui->CustomPlot->graph(0)->setBrush(QBrush(QColor(153, 153, 153, 20)));
    ui->CustomPlot->graph(0)->setName("RPM");
    //ui->CustomPlot->graph(1)->setName("Corriente");
    ui->CustomPlot->plotLayout()->insertRow(0);
    ui->CustomPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->CustomPlot, "Velocidad del Motor vs Tiempo", QFont("sans", 10, QFont::Cursive)));


    //ui->CustomPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->CustomPlot->legend->setFont(legendFont);
    ui->CustomPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    //ui->CustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    ui->CustomPlot->xAxis->setLabel("Time Relative");
    ui->CustomPlot->yAxis->setLabel("RPM.");
    ui->CustomPlot->xAxis->setRange(0, 100);
    ui->CustomPlot->yAxis->setRange(0, 1000);
    //ui->CustomPlot->yAxis2->setLabel("mA");Segunda grafica
    //ui->CustomPlot->yAxis2->setRange(0, 1000);//Segunda Grafica
    //ui->CustomPlot->yAxis2->setVisible(true);
    ui->CustomPlot->replot();
}
///

void Widget::Plsplot(double rpm){

    for (int i=0; i<100; ++i)
    {
      y[i] = y[i + 1];
      //w[i] = w[i + 1];
    }
    y[100] = rpm;
    //w[100] = corriente_ma;
    ui->CustomPlot->graph(0)->setData(x, y);
   // ui->customPlot->graph(1)->setData(z, w);
    ui->CustomPlot->replot();
}
void Widget::request(){ //con esta funcion incrementamos el valor de una variable cada vez que el timer me dice
    char a[]={0x6D,0x03,0x01, 0x06, 0x02, 0x10, 0x7B, 0x29 };//protocolo de request
    sl->write(a,8);//envio al micro.
    }

void Widget::OpenSerialPort(QString p)
{
    if(sl->isOpen()){
        sl->close();
    }
    disconnect(sl,SIGNAL(readyRead()),this,SLOT(readSerial()));
    sl->setPortName(p);
    sl->setBaudRate(QSerialPort::Baud115200);
    sl->setDataBits(QSerialPort::Data8);
    sl->setParity(QSerialPort::NoParity);
    sl->setStopBits(QSerialPort::OneStop);
    sl->setFlowControl(QSerialPort::NoFlowControl);
    connect(sl,SIGNAL(readyRead()),this,SLOT(readSerial()));    
    if(sl->open(QIODevice::ReadWrite)){
        ui->pushButton->setText("Cerrar");
    }else{
        QMessageBox::critical(this,tr("Error"),sl->errorString());
    }
}


void Widget::on_pushButton_clicked()
{
    if(sl->isOpen()){
        sl->close();
        if(timer1->isActive()){
            timer1->stop();
            ui->pushButton->setText("Abrir");
        }
    }else{
        OpenSerialPort(portn);
    }   
}

void Widget::readSerial()
{
    static int a=0,RPAM=0,RPAm=0;
    QString text1;
    QByteArray serialData=sl->readAll();
    int tam=serialData.size();
    char com=0x6D^0x04^0x01^(*(serialData.data()+3))^(*(serialData.data()+4))^(*(serialData.data()+5))^(*(serialData.data()+6));
    char text[4]={*(serialData.data()+3),*(serialData.data()+4),*(serialData.data()+5),*(serialData.data()+6)};
    //qDebug()<<"data:"<<(uint64_t)(serialData.data());
    int d=0;
    if (0x29==*(serialData.data()+(tam-1)) ){//&& com==*(serialData.data()+(tam-2))) {
        d=(uint8_t)text[0]<<24;
        d|=(uint8_t)text[1]<<16;
        d|=(uint8_t)text[2]<<8;
        d|=(uint8_t)text[3];
        d=30*d/1000;
        if (a==0 && d!=0) {
            RPAm=d;
            a=1;
            RPAM=d;
        }
        if(d>RPAM && d!=0) {
        RPAM=d;
        }
        if (d<RPAm && d!=0) {
        RPAm=d;
        }
        ui->label->setNum(d);
        ui->LMin->setNum(RPAm);
        ui->LMax->setNum(RPAM);
        text1.setNum((uint32_t)d);
        Plsplot((double)d);
        ui->plainTextEdit->appendPlainText(text1);//impresion para posterior uso en excel
     }

}
/*
 *la funcion de transmicion char no recibe datos mayores a 0x7F  TENER EN CUENTA PROTOCOLO
  */
void Widget::on_dial_valueChanged(int value)
{
    uint16_t tiempo;
    char valuec=(uint8_t)value;
    ui->Progres->setValue(value);//value as the percentaje of the dial.
    tiempo=ui->TMotor->text().toUInt();
    char t1=(uint8_t)(tiempo>>8);
    char t2=(uint8_t)tiempo;
    char com=0x6D^0x03^0x02^valuec^t1^t2;
    char a[]={0x6D,0x03,0x02,valuec,t1,t2,com,0x29 };//protocolo de request
    if (sl->isOpen()){
     sl->write(a,8);//envio al micro.
    }
}
