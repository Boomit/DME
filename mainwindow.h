#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept> // runtime_error
#include <sstream>

#ifndef Q_OS_WIN
    #include <sys/socket.h> // socket(), connect()
    #include <arpa/inet.h> // sockaddr_in
    #include <netdb.h> // gethostbyname(), hostent
    #include <errno.h> // errno
#else
    #include <winsock2.h>
#endif

using namespace std;

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void startRequest(QUrl url);

    ~MainWindow();

private slots:
    void on_btnLogin_clicked();
    void on_btnLogout_clicked();
    int loadData();
    std::runtime_error CreateSocketError();
    void SendAll(int socket, const char* const buf, const int size);
    void GetLine(int socket, std::stringstream& line);
    void on_btnRefresh_clicked();

private:
    Ui::MainWindow *ui;

    QString Username;
    QString Passwort;
};

#endif // MAINWINDOW_H
