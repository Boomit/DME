#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnLogout->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnLogin_clicked()
{
    Username = ui->txtUser->text();
    Passwort = ui->txtPass->text();

    qDebug() << "Login: " << Username << ":" << Passwort;

    ui->btnLogin->hide();
    ui->btnLogout->show();
    loadData();
}

void MainWindow::on_btnLogout_clicked()
{
    Username = rand();
    Passwort = rand();

    qDebug() << "Logout: " << Username << ":" << Passwort;

    ui->txtPass->setText("Passwort");
    ui->txtUser->setText("Benutzername");
    ui->btnLogout->hide();
    ui->btnLogin->show();
}

std::runtime_error MainWindow::CreateSocketError()
{
    std::ostringstream temp;
#ifndef Q_OS_WIN
    temp << "Socket-Fehler #" << errno << ": " << strerror(errno);
#else
    int error = WSAGetLastError();
    temp << "Socket-Fehler #" << error;
    //char* msg;
    LPWSTR msg = NULL;
    if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     //reinterpret_cast<char*>(&msg), 0, NULL))
                     msg, 0, NULL))
    {
        try
        {
            temp << ": " << msg;
            LocalFree(msg);
        }
        catch(...)
        {
            LocalFree(msg);
            throw;
        }
    }
#endif
    return std::runtime_error(temp.str());
}

void MainWindow::SendAll(int socket, const char* const buf, const int size)
{
    int bytesSent = 0; // Anzahl Bytes die wir bereits vom Buffer gesendet haben
    do
    {
        int result = send(socket, buf + bytesSent, size - bytesSent, 0);
        if(result < 0) // Wenn send einen Wert < 0 zurück gibt deutet dies auf einen Fehler hin.
        {
            throw this->CreateSocketError();
        }
        bytesSent += result;
    } while(bytesSent < size);
}

void MainWindow::GetLine(int socket, std::stringstream& line)
{
    for(char c; recv(socket, &c, 1, 0) > 0; line << c)
    {
        if(c == '\n') {
            return;
        }
    }
    throw this->CreateSocketError();
}

int MainWindow::loadData()
{
#ifdef Q_OS_WIN
    WSADATA w;
    if(int result = WSAStartup(MAKEWORD(2,2), &w) != 0)
    {
        qDebug() << "Winsock 2 konnte nicht gestartet werden! Error #" << result;
        return 1;
    }
#endif

    hostent* phe = gethostbyname("www.feuerwache.net");

    if(phe == NULL)
    {
        qDebug() << "Host konnte nicht aufgeloest werden!";
        return 1;
    }

    if(phe->h_addrtype != AF_INET)
    {
        qDebug() << "Ungueltiger Adresstyp!";
        return 1;
    }

    if(phe->h_length != 4)
    {
        qDebug() << "Ungueltiger IP-Typ!";
        return 1;
    }

    int Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(Socket == -1)
    {
        qDebug() << "Socket konnte nicht erstellt werden!";
        return 1;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(80); // Das HTTP-Protokoll benutzt Port 80

    char** p = phe->h_addr_list; // p mit erstem Listenelement initialisieren
    int result; // Ergebnis von connect
    do
    {
        if(*p == NULL) // Ende der Liste
        {
            qDebug() << "Verbindung fehlgschlagen!";
            return 1;
        }

        service.sin_addr.s_addr = *reinterpret_cast<unsigned long*>(*p);
        ++p;
        result = ::connect(Socket, reinterpret_cast<sockaddr*>(&service), sizeof(service));
    }
    while(result == -1);

    qDebug() << "Verbindung erfolgreich!";

    string temp = "GET /feuerwache/xmlList/?username="+Username.toStdString()+"&password="+Passwort.toStdString()+" HTTP/1.1\r\nHost: www.feuerwache.net\r\nConnection: close\r\n\r\n";
    const string request = temp;

    SendAll(Socket, request.c_str(), request.size());

    ofstream fout("output.xml");

    qDebug() << "Empfange und schreibe Antwort in output.xml...";
    while(true)
        {
            std::stringstream line;
            try
            {
                GetLine(Socket, line);
            }
            catch(exception& e) // Ein Fehler oder Verbindungsabbruch
            {
                break; // Schleife verlassen
            }
            fout << line.str() << endl; // Zeile in die Datei schreiben.
        }

    fout.close();

#ifndef Q_OS_WIN
    ::close(Socket);
#else
    closesocket(Socket);
#endif
    return 0;
}

void MainWindow::on_pushButton_clicked()
{
    loadData();
}
