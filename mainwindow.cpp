#include <QPalette>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>
#include <QPushButton>
#include <QPixmap>
#include <QBitmap>
#include <QSize>
#include <QAbstractSocket>
#include <QHostAddress>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define BUF_LENGTH 128
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("深夜诗人");             //设置窗口标题
    setFixedSize(1080,607);                //固定窗口大小

    //设置背景图片
    setAutoFillBackground(true);
    QPalette pal;
    QPixmap pixmap("./photo/bg.jpeg");
    pal.setBrush(QPalette::Window,QBrush(pixmap));
    setPalette(pal);

    //设置标题
    ui->label_3->setStyleSheet("font-family: 'Simsun'; font-size: 34px; color: #4e7ca1");
    ui->label_3->setText("深夜诗人");
    ui->label_4->setStyleSheet("font-family: 'Simsun'; font-size: 28px; color: #1491a8");
    ui->label_4->setText("——基于图像识别的人工智能作诗系统");

    //按钮样式
    ui->pushButton->setStyleSheet("font-family: 'FangSong'; font-size: 15px;");
    ui->toolButton->setStyleSheet("font-family: 'FangSong'; font-size: 15px;");

    //选择图片
    connect(ui->toolButton, &QToolButton::clicked, this, &MainWindow::ShowImage);

    //poem展示区
    ui->label->setStyleSheet("font-family: 'FangSong'; font-size: 28px; color: #7a7374; padding: 5px; border: 1px solid rgba(172, 163, 164, 0.8); text-align: center; border-radius: 10px;");
    ui->label_5->setStyleSheet("font-family: 'Microsoft YaHei'; font-weight: 800; font-size: 18px; color: #7a7374; ");
    ui->label_5->setGeometry(870,420,100,50);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::SendPath);

    //socket连接
    client = new QTcpSocket(this);
    setSocket();
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::getServerMsg);  //收到信息

}

void MainWindow::ReadPoem()
{
    QFile fin(curPoemPath);
    QString poem_txt, pp;
    if (fin.open(QIODevice::ReadOnly))
    {
        QTextStream fin_text(&fin);
        poem_txt = fin_text.readLine() + '\n';
        poem_txt += fin_text.readLine();
        pp = fin_text.readLine();
        ui->label->setText(poem_txt);
        ui->label_5->setText("PP="+pp);
        fin.close();
    }
    else
        qDebug() << "Open file error!";
}

void MainWindow::ShowImage()
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择图像"), ".", tr("Image Files(*.jpg *.png *.jpeg)"));
    QImage* img = new QImage;
    QImage* scaledimg = new QImage;         //分别保存原图和缩放之后的图片
    if(!(img->load(path)))                  //加载图像
    {
       QMessageBox::information(this, tr("warning"), tr("打开图片失败"));
       delete img;
       return;
    }
    qDebug() << path;
    this->curImgPath = path;

    if (img->width() > 300 || img->height() > 300)
    {
        ui->label_2->setGeometry(90,150,300,300);
        *scaledimg = img->scaled(300, 300, Qt::KeepAspectRatio);    //调整大小适应窗口
    }
    else
    {
        ui->label_2->setGeometry(90,150,90,90);
        *scaledimg = img->scaled(90, 90, Qt::KeepAspectRatio);
    }
    QPixmap img1 = QPixmap::fromImage(*scaledimg);              //设置圆角
    QSize size(img1.size());
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), 7, 7);
    img1.setMask(mask);

    ui->label_2->setPixmap(img1);
}

void MainWindow::setSocket()
{
    client->connectToHost(QHostAddress("127.0.0.1"), 7500);
    QString str = "connect";
    if (client->write(str.toLocal8Bit(), BUF_LENGTH) == -1)
    {
        QMessageBox::information(this,"warning","服务器错误！",QMessageBox::Ok);
    }
}

void MainWindow::SendPath()
{
    QString str = "path " + curImgPath;
    if (client->write(str.toLocal8Bit(), BUF_LENGTH) == -1)
    {
        QMessageBox::information(this,"warning","服务器错误！",QMessageBox::Ok);
    }
}

void MainWindow::getServerMsg()
{
    auto ret = client->read(BUF_LENGTH);
    QString msg = QString::fromLocal8Bit(ret);
    if (msg[0] == 'O' && msg[1] == 'K')    //"OK"
    {
        QMessageBox::information(this,"tips","连接到服务器！",QMessageBox::Ok);
        return;
    }
    else if (msg[0] == 'P')                     //已生成poem
    {
        QStringList detail = msg.split(' ');
        curPoemPath = detail[1];
        qDebug() << curPoemPath;
        ReadPoem();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

