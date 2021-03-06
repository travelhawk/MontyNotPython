#include "server.h"
#include <cstdio>
#include <QtDebug>
#include <QtCore/QFile>
#include <QString>

static const char* TAKE_PICTURE = "#TAKE_PICTURE";
static const char* ROTATE_RIGHT = "#ROTATE_RIGHT";
static const char* ROTATE_LEFT = "#ROTATE_LEFT";
static const char* ZOOM_IN = "#ZOOM_IN";
static const char* ZOOM_OUT = "#ZOOM_OUT";
static const char* ARM_UP = "#ARM_UP";
static const char* ARM_DOWN = "#ARM_DOWN";
static const char* QUIT_COMMAND = "#QUIT_COMMAND";
static const char* GET_IMAGE_COMMAND = "#GET_IMAGE_COMMAND";
static const char* AUTOMATIC_OFF = "#AUTOMATIC_OFF";
static const char* AUTOMATIC_ON = "#AUTOMATIC_ON";

int int_from_bytes(const char * bytes, bool reverse)
{
    // usage: int_from_bytes(bytes, SYSTEM_ENDIANNESS != ARRAY_ENDIANNESS)
    if(!reverse)
        return *(int *)(void *)bytes;

    char tmp[sizeof(int)];

    for(size_t i = sizeof(tmp); i--; ++bytes)
        tmp[i] = *bytes;

    return *(int *)(void *)tmp;
}

Server::Server() : QObject()
{
    this->server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()),
    this, SLOT(on_newConnection()));
}

void Server::listen()
{
    qDebug()<<"listen";
    server->listen(QHostAddress::Any, 5100);
}

void Server::on_newConnection()
{
    socket = server->nextPendingConnection();

    if(socket->state() == QTcpSocket::ConnectedState)
    {
        printf("New connection established.\n");
        qDebug()<<socket->peerAddress();
    }
    connect(socket, SIGNAL(disconnected()),
    this, SLOT(on_disconnected()));
    connect(socket, SIGNAL(readyRead()),
    this, SLOT(on_readyRead()));
}

void Server::on_readyRead()
{
    qDebug() << "read input";
    QByteArray tlength = socket->read(4); // read int32
    if(tlength.isNull()){
            qDebug()<<"server: bytes are null";
    }
    int length = int_from_bytes(tlength.data(), true);
    qDebug()<<"read "<<length<<" bytes from client";
    
    socket->waitForReadyRead();
    QByteArray message = socket->read(length);
    if(message.isNull()){
        qDebug()<<"message is null";
    }

    if(strcmp(message.constData(), QUIT_COMMAND) == 0)
    {
        qDebug() << "SERVER: disconnect from host";
        socket->write(QUIT_COMMAND);
        socket->flush();
        socket->disconnectFromHost();
    }
    else if(strcmp(message.constData(), TAKE_PICTURE) == 0)
    {
        qDebug() << "SERVER: take picture";
        emit takePicture();
    }
    else if(strcmp(message.constData(), ROTATE_LEFT) == 0)
    {
        qDebug() << "SERVER: rotate left";
        emit rotateLeft();
    }
    else if(strcmp(message.constData(), ROTATE_RIGHT) == 0)
    {
        qDebug() << "SERVER: rotate right";
        emit rotateRight();
    }
    else if(strcmp(message.constData(), ZOOM_IN) == 0)
    {
        qDebug() << "SERVER: zoom in";
        emit zoomIn();
    }
    else if(strcmp(message.constData(), ZOOM_OUT) == 0)
    {
        qDebug() << "SERVER: zoom out";
        emit zoomOut();
    }
    else if(strcmp(message.constData(), ARM_UP) == 0)
    {
        qDebug() << "SERVER: arm up";
        emit armUp();
    }
    else if(strcmp(message.constData(), ARM_DOWN) == 0)
    {
        qDebug() << "SERVER: arm down";
        emit armDown();
    } else if(strcmp(message.constData(), AUTOMATIC_OFF) == 0)
    {
        qDebug() << "SERVER: automatic off";
        emit automaticOff();
    } else if(strcmp(message.constData(), AUTOMATIC_ON) == 0)
    {
        qDebug() << "SERVER: automatic on";
        emit automaticOn();
    }
}

bool Server::sendPicture(QString fileName)
{
    // send picture
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can't open image.";
        return 0;
    }

    QByteArray image;
   /**
    while (!file.atEnd()) {
        image.append(file.readLine());
    }**/
    image = file.readAll();
    socket->write(GET_IMAGE_COMMAND);
    socket->waitForBytesWritten();
    socket->write(image);
    socket->waitForBytesWritten();
    socket->flush();
}

void Server::on_disconnected()
{
    printf("Connection disconnected.\n");
    disconnect(socket, SIGNAL(disconnected()));
    disconnect(socket, SIGNAL(readyRead()));
    socket->deleteLater();
}

void Server::testMethode()
{
    // test
    qDebug()<<"Testmethode";
    char bytes[4];
    int n = 5;

    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;

    socket->write(bytes);
    socket->waitForBytesWritten(3000);
    socket->write("#TEST");
    socket->waitForBytesWritten();
    socket->flush();
    qDebug()<<"Data written";
}
