#ifndef MY_TCPSOCKET_H
#define MY_TCPSOCKET_H

#ifndef QT_WARNING_DISABLE_DEPRECATED
# define QT_WARNING_DISABLE_DEPRECATED
#endif

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>


class My_TCPSocket : public QObject
{
    Q_OBJECT
public:
    explicit My_TCPSocket(QObject * parent = NULL);
    My_TCPSocket(quint16 port);

    enum STATE {
        S_START,
        S_READ
    };

    struct TCPPlace {
        QByteArray		frame;
        QTcpSocket *	socket;
        STATE			state;
        qint8			place;
        char			cam;
        uint16_t		size;
    };

signals:
    void imgComplete(QByteArray, char, qint8);
    void startrecieve(quint32);
    void recieveGoOn(quint32);
    void talking(const QString &);

public slots:
    void incommingConnect();
    void readyRead();
    void sendToSocket(int);
    void WaitCropper();
private:

    void initStruct();

    QTcpServer * _server;

    QTime _time;
    QByteArray _buffer;
    char _jpg[5];
    char _replace[2];

    char _place;
    char _pack;
    uint16_t _size;

    STATE _state;

    TCPPlace _dispencer[6];
    bool _changeState;
    char _thisCam;
    bool _cropperFree;
};

#endif  // MY_TCPSOCKET_H
