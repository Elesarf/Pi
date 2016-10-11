#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QTimer>

class Protocol:public QObject
{
    Q_OBJECT
public:

    //~Protocol();

    enum State {
        S_INIT,
        S_READ_HEADER,
        S_READ_DATA,
        S_READ_PLACE
    };

    explicit Protocol(QObject *parent = 0);

    struct Descriptor{
        char id;
        char numPlace;
        char numCam;
        char sizeJPG[2];
        char sizeJPG1[2];
        char data[25];
    };

    struct Buffer {
        char id;
        char data[31];
    };

signals:

    void EndOfRecive( const QByteArray &, const Descriptor & );
    void EndReadPlace( const uint8_t);
    void GoToCrop( QByteArray, const char, const char );

public slots:

    bool InitRF();
    bool InitRF( uint8_t );
    bool StopRF();
    void Start();

private slots:

    void ReadyRead();

private:

    Descriptor descriptor;
    Buffer buffer;
    QByteArray data;
    QTimer watchDogOnRecieve;
    QTimer watchDogOnPlace;
    State state;
    uint8_t placeNumber;


};

#endif // PROTOCOL_H
