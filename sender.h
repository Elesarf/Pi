#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class Sender : public QObject
{

    Q_OBJECT

public:
    explicit Sender( QObject *parent = 0 );

signals:

    void SendEnd( QByteArray & );

public slots:

    bool SendPlease ( const char, const char, const qint8 );

private:

    QByteArray data;
    QNetworkAccessManager *man;
    QNetworkReply *reply;
    QNetworkRequest request;

};

#endif // SENDER_H
