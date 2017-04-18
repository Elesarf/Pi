#ifndef MY_TCPSOCKET_H
#define MY_TCPSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>


class My_TCPSocket : public QObject
{
    Q_OBJECT
public:
    explicit My_TCPSocket(QObject *parent = NULL);
    My_TCPSocket(quint16 port);

    enum STATE{
        S_START,
        S_READ
    };

signals:
    void imgComplete( const QByteArray &, char, char, uint16_t);
    void imgComplete( QByteArray, char, qint8);
    void talking(const QString &);
public slots:
    void incommingConnect();
    void readyRead();
    void stateChanged(QAbstractSocket::SocketState stat);

private:

    void fillSign();

    QTcpServer *_server;
    QList<QTcpSocket *>_sockets;
    QTcpSocket *_firstSocket;
    QTcpSocket *_nowsocket;

    QByteArray _frame;
    QByteArray _buffer;
    char _jpg[5];
    char _replace[2];

    char _place;
    char _pack;
    uint16_t _size;

    STATE _state;

};

#endif // MY_TCPSOCKET_H
