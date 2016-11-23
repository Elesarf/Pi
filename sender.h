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

signals:

    void SendEnd( QByteArray & );

public slots:

    bool SendPlease ( const qint8, const qint8, const qint8, int, qint8 );
    bool IGetImage( const int, const char, const char );

private:

    QDateTime __datetime;
    qint8 __sz;
    QNetworkAccessManager __man;
    QTimer __sendTimer;
    int __timToTimer;
    QByteArray __sendStr;
    QByteArray __buffer;
    QNetworkRequest __request;

};

#endif // SENDER_H
