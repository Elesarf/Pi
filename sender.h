#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>

class Sender : public QObject
{

    Q_OBJECT

public:

    explicit Sender( QObject *parent = 0 );

signals:

    void SendEnd( QByteArray & );

public slots:

    bool SendPlease ( const qint8, const qint8, const qint8 );

private:

    qint8 sz;
    QByteArray sendStr;
    QByteArray buffer;
    QNetworkAccessManager *man;
    QNetworkReply *reply;
    QNetworkRequest request;

};

#endif // SENDER_H
