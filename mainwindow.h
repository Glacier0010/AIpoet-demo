#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString curImgPath;
    QString curPoemPath;
    QTcpSocket* client;

    void ReadPoem();                    //在UI里展示poem
    void ShowImage();                   //在UI里展示图片
    void SendPath();                    //发送图片地址

    void setSocket();                   //连接服务器
    void getServerMsg();                //接受服务器端信息
};
#endif // MAINWINDOW_H
