#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QtSerialPort>
#include<QSerialPortInfo>
#include<QMessageBox>
#include<QVector>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void readSerial();
    void request();
    void Plsplot(double rpm);
    void on_req_windowTitleChanged(const QString &title);

    void on_req_clicked();

    void on_dial_valueChanged(int value);

private:
    Ui::Widget *ui;
    QSerialPort *sl=nullptr;
    //grafos, y creacion de vectores
    void Grafo();
    QVector<double> x;
    QVector<double> y;
    //QVector<double> z;Para N graficas hacer N*2 vectores
    //QVector<double> w;
    uint32_t vendor=1155,product=22336;
    QString portn;
    void suf(QString t1);
    void OpenSerialPort(QString p);
};

#endif // WIDGET_H
