#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QTextCodec>
//#include <QDate>
//#include <QChartView>
//#include <QLineSeries>
//#include <QPieSeries>
#include <QtDebug>
//#include <math.h>
#include <QTimer>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

     QStringList getPortNameList();//获取所有可用的串口列表
     bool ReceiveProcesser(QByteArray ba);//处理接收的信息
     void DataProcess(QByteArray ba, double& max, double& min);//处理接收的数据
     bool judgeCurrent(int n, double max, double min);
     void readSetting();
     void writeSetting();

private slots:

    //void on_btn_open_clicked();
    void refresh();
    void openPort();//打开串口
    void read();
    void send();
    void timer1Slot();  //  定时器time up槽函数
    void timer2Slot();
    void timer3Slot();
    void timer4Slot();

private:
    Ui::MainWindow *ui;

    QSerialPort* m_serialPort; //串口类
    QStringList m_portNameList;//保存可用的串口名
    QByteArray sendbuff;//发送缓冲器
    QByteArray dataarray;//存放数据的HEX序列
    int receivecoutner;//接收计数
    QByteArray buf;//接收缓冲器
    QTimer timer1;//定时器1
    QTimer timer2;//定时器2
    QTimer timer3;//定时器3
    QTimer timer4;//定时器4
    QPixmap green;
    QPixmap gray;
    QPixmap red;
    QByteArray sdata;//下位机数据缓存
    int receivestatus;
};

#endif // MAINWINDOW_H
