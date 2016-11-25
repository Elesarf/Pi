#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QTimer>

class Sender : public QObject
{

    Q_OBJECT

public:

    explicit Sender( QObject *parent = 0 );

    struct Dispenser {

        qint8 place;
        qint8 pack;
        qint8 find;
        QString timestamp;
        qint8 matches;

    };

signals:

    void SendEnd( QByteArray & );

public slots:

    bool SendPlease ( const qint8, const qint8, const qint8, int, qint8 );
    bool IGetImage( const int, const char, const char );

private:

    void __FillDispenser();

    QDateTime __datetime;
    qint8 __sz;
    Dispenser __dispenser[13][13];
    QNetworkAccessManager __man;
    QTimer __sendTimer;
    int __timToTimer;
    QByteArray __sendStr;
    QByteArray __buffer;
    QNetworkRequest __request;

};

#endif // SENDER_H
