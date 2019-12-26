#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_portNameList = getPortNameList();//获取可用串口名
    ui->comboBox->addItems(m_portNameList);//将可用串口添加到comboBox中

    //创建一个QSerialPort串口对象
    m_serialPort = new QSerialPort();

    //设置标题
    QFont font("Microsoft YaHei", 20, 75); //第一个属性是字体（微软雅黑），第二个是大小，第三个是加粗（权重是75）
    ui->label_title->setFont(font);

    //设置文本框字体大小

    //获取图片资源
    green = QPixmap(":/new/prefix1/res/green.bmp");
    gray = QPixmap(":/new/prefix1/res/gray.bmp");
    red = QPixmap(":/new/prefix1/res/red.bmp");
    ui->label_icon1->setPixmap(gray);
    ui->label_icon2->setPixmap(gray);
    ui->label_icon3->setPixmap(gray);
    ui->label_icon4->setPixmap(gray);

    //读取注册表中数据
    readSetting();

    //连接信号和槽
    connect(ui->btn_open,SIGNAL(clicked()),this,SLOT(openPort()));
    //connect(ui->btn_send,SIGNAL(clicked()),this,SLOT(send()));
    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::read);
    connect(ui->btn_refresh,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(&timer1, SIGNAL(timeout()), this, SLOT(timer1Slot()));
    connect(&timer2, SIGNAL(timeout()), this, SLOT(timer2Slot()));
    connect(&timer3, SIGNAL(timeout()), this, SLOT(timer3Slot()));
    connect(&timer4, SIGNAL(timeout()), this, SLOT(timer4Slot()));
}

MainWindow::~MainWindow()
{
    writeSetting();
    delete ui;
}

//获取可用串口名
QStringList MainWindow::getPortNameList()
{
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
    }
    return m_serialPortName;
}

void MainWindow::refresh()
{
    ui->comboBox_2->clear();
    m_portNameList = getPortNameList();//获取可用串口名
    ui->comboBox->addItems(m_portNameList);//将可用串口添加到comboBox中
}

void MainWindow::openPort()
{
    if(ui->btn_open->text()=="打开串口")//如果串口未打开，则打开串口
    {
        //设置串口连接参数
        m_serialPort->setPortName(ui->comboBox->currentText());//当前选择的串口名字
        m_serialPort->setBaudRate(QSerialPort::Baud9600);//设置波特率和读写方向
        m_serialPort->setDataBits(QSerialPort::Data8);      //数据位为8位
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        m_serialPort->setParity(QSerialPort::NoParity); //无校验位
        m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
        //m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        //m_serialPort->setReadBufferSize(1024);

        if(m_serialPort->isOpen())//如果串口已经打开了 先给他关闭了
        {
            m_serialPort->clear();
            m_serialPort->close();
        }
        if(m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
        {
            m_serialPort->setDataTerminalReady(true);
            ui->btn_open->setText("关闭串口");
            return;
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), m_serialPort->errorString());

        }
        //ui->statusBar->
    }
    if(ui->btn_open->text()=="关闭串口")//如果串口处于打开状态，再点击则为关闭串口
    {
        if (m_serialPort->isOpen())
            {
                m_serialPort->close();
            }
        ui->btn_open->setText("打开串口");

    }
}

//串口发送数据函数
void MainWindow::send()
{
    //sendbuff = ui->edit_send->toPlainText().toUtf8();
    sendbuff = "ok";
    m_serialPort->write(sendbuff);
}

//串口读取数据函数
void MainWindow::read()
{
    //m_serialPort->waitForReadyRead(10);
    QByteArray buf = m_serialPort->readAll();
    //ui->edit_receive->setText(buf);
    qDebug()<<buf;
    ReceiveProcesser(buf);
}

//接受数据协议
bool MainWindow::ReceiveProcesser(QByteArray ba)
{
    if('T'==ba.constData()[0] && ba.contains('W'))//收到的数据同时包含指令头和指令尾
    {
        if('A'==ba.constData()[1])          //收到“开始测试”指令
        {
            if ('1'==ba.constData()[2])
            {
                ui->edit_maxcurrent1->clear();
                ui->edit_mincurrent1->clear();
                ui->edit_result1->clear();
                ui->label_icon1->setPixmap(gray);
                timer1.start(1000);         //启动定时器
            }
            if ('2'==ba.constData()[2])
            {
                ui->edit_maxcurrent2->clear();
                ui->edit_mincurrent2->clear();
                ui->edit_result2->clear();
                ui->label_icon2->setPixmap(gray);
                timer2.start(1000);
            }
            if ('3'==ba.constData()[2])
            {
                ui->edit_maxcurrent3->clear();
                ui->edit_mincurrent3->clear();
                ui->edit_result3->clear();
                ui->label_icon3->setPixmap(gray);
                timer3.start(1000);         //启动定时器
            }
            if ('4'==ba.constData()[2])
            {
                ui->edit_maxcurrent4->clear();
                ui->edit_mincurrent4->clear();
                ui->edit_result4->clear();
                ui->label_icon4->setPixmap(gray);
                timer4.start(1000);
            }
        }
    }
    else if ('D'==ba.constData()[0] && ba.contains('W'))    //收到测试数据
    {
        //qDebug()<<maxI<<"\t"<<minI;
        //判断是否合格
        if ('1'==ba.constData()[1])
        {
            double maxI, minI;
            DataProcess(ba, maxI, minI);
            ui->edit_maxcurrent1->setText(QString::number(maxI));
            ui->edit_mincurrent1->setText(QString::number(minI));
            if (judgeCurrent(1, maxI, minI))
            {
                ui->edit_result1->setHtml("<font color='green' size='24'><green>合格</font>");
                ui->label_icon1->setPixmap(green);
            }
            else
            {
                ui->edit_result1->setHtml("<font color='red' size='24'><red>不合格</font>");
                ui->label_icon1->setPixmap(red);
            }
        }
        else if ('2'==ba.constData()[1])
        {
            double maxI, minI;
            DataProcess(ba, maxI, minI);
            ui->edit_maxcurrent2->setText(QString::number(maxI));
            ui->edit_mincurrent2->setText(QString::number(minI));
            if (judgeCurrent(2, maxI, minI))
            {
                ui->edit_result2->setHtml("<font color='green' size='24'><green>合格</font>");
                ui->label_icon2->setPixmap(green);
            }
            else
            {
                ui->edit_result2->setHtml("<font color='red' size='24'><red>不合格</font>");
                ui->label_icon2->setPixmap(red);
            }
        }
        else if ('3'==ba.constData()[1])
        {
            double maxI, minI;
            DataProcess(ba, maxI, minI);
            ui->edit_maxcurrent3->setText(QString::number(maxI));
            ui->edit_mincurrent3->setText(QString::number(minI));
            if (judgeCurrent(3, maxI, minI))
            {
                ui->edit_result3->setHtml("<font color='green' size='24'><green>合格</font>");
                ui->label_icon3->setPixmap(green);
            }
            else
            {
                ui->edit_result3->setHtml("<font color='red' size='24'><red>不合格</font>");
                ui->label_icon3->setPixmap(red);
            }
        }
        else if ('4'==ba.constData()[1])
        {
            double maxI, minI;
            DataProcess(ba, maxI, minI);
            ui->edit_maxcurrent4->setText(QString::number(maxI));
            ui->edit_mincurrent4->setText(QString::number(minI));
            if (judgeCurrent(4, maxI, minI))
            {
                ui->edit_result4->setHtml("<font color='green' size='24'><green>合格</font>");
                ui->label_icon4->setPixmap(green);
            }
            else
            {
                ui->edit_result4->setHtml("<font color='red' size='24'><red>不合格</font>");
                ui->label_icon4->setPixmap(red);
            }
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

//处理从串口接收的数据
void MainWindow::DataProcess(QByteArray ba, double &max, double &min)   //处理接收的数据
{
    QByteArray bamax, bamin;
    for (int i =0;i<6;i++) {
        bamax.append(ba.constData()[i+2]);
        bamin.append(ba.constData()[i+8]);
    }
    //qDebug() <<bamax<<"\t"<<bamin;
    max = bamax.toDouble();
    min = bamin.toDouble();
}

//判断是否合格的规则
bool MainWindow::judgeCurrent(int n, double max, double min)
{
    switch(n)
    {
    case 1:
        return min>=ui->edit_lowerlimit1->toPlainText().toDouble() && max <= ui->edit_upperlimit1->toPlainText().toDouble();
    case 2:
        return min>=ui->edit_lowerlimit2->toPlainText().toDouble() && max <= ui->edit_upperlimit2->toPlainText().toDouble();
    case 3:
        return min>=ui->edit_lowerlimit3->toPlainText().toDouble() && max <= ui->edit_upperlimit3->toPlainText().toDouble();
    case 4:
        return min>=ui->edit_lowerlimit4->toPlainText().toDouble() && max <= ui->edit_upperlimit4->toPlainText().toDouble();
    default:
        return false;
    }

}

//定时器 time up槽函数
void MainWindow::timer1Slot()               //待稳定运行后向下位机发送"采集"指令
{
    QByteArray sb = "TE1W";
    m_serialPort->write(sb);
    timer1.stop();
}
void MainWindow::timer2Slot()               //待稳定运行后向下位机发送"采集"指令
{
    QByteArray sb = "TE2W";
    m_serialPort->write(sb);
    timer2.stop();
}
void MainWindow::timer3Slot()
{
    QByteArray sb = "TE3W";
    m_serialPort->write(sb);
    timer3.stop();
}
void MainWindow::timer4Slot()               //待稳定运行后向下位机发送"采集"指令
{
    QByteArray sb = "TE4W";
    m_serialPort->write(sb);
    timer4.stop();
}

//写入注册表
void MainWindow::writeSetting()
{
    QSettings set("kongzaidianliu","limits");

    set.setValue("upperlimit1",ui->edit_upperlimit1->toPlainText());
    set.setValue("lowerlimit1",ui->edit_lowerlimit1->toPlainText());
    set.setValue("upperlimit2",ui->edit_upperlimit2->toPlainText());
    set.setValue("lowerlimit2",ui->edit_lowerlimit2->toPlainText());
    set.setValue("upperlimit3",ui->edit_upperlimit3->toPlainText());
    set.setValue("lowerlimit3",ui->edit_lowerlimit3->toPlainText());
    set.setValue("upperlimit4",ui->edit_upperlimit4->toPlainText());
    set.setValue("lowerlimit4",ui->edit_lowerlimit4->toPlainText());
}

//读取注册表
void MainWindow::readSetting()
{
    QSettings set("kongzaidianliu","limits");

    ui->edit_upperlimit1->setText(set.value("upperlimit1",0).toString());
    ui->edit_lowerlimit1->setText(set.value("lowerlimit1",0).toString());
    ui->edit_upperlimit2->setText(set.value("upperlimit2",0).toString());
    ui->edit_lowerlimit2->setText(set.value("lowerlimit2",0).toString());
    ui->edit_upperlimit3->setText(set.value("upperlimit3",0).toString());
    ui->edit_lowerlimit3->setText(set.value("lowerlimit3",0).toString());
    ui->edit_upperlimit4->setText(set.value("upperlimit4",0).toString());
    ui->edit_lowerlimit4->setText(set.value("lowerlimit4",0).toString());
}
