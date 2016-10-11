#include "protocol.h"
#include "jpgcrop.h"
#include "finder.h"

#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QThread>

#include <stdio.h>
#include <cstdlib>

Q_DECLARE_METATYPE( cv::Mat )
Q_DECLARE_METATYPE( Protocol::Descriptor )
Q_DECLARE_METATYPE( std::uint8_t )

QT_USE_NAMESPACE

cv::Mat ShowJPG(QByteArray array);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType < cv::Mat >( "cv::Mat" );
    qRegisterMetaType < std::uint8_t >( "std::uint8_t" );
    qRegisterMetaType < Protocol::Descriptor >( "Protocol::Descriptor" );

    qDebug() << "";

    Protocol pr;
    JPGcrop  cr;
    Finder   fd;

    QThread  thFd;
    QThread  thRf;
    QThread  thCr;

    int numberOfPic = 0;

    QObject::connect( &pr, &Protocol::EndOfRecive, [&a, &pr, &numberOfPic]( const QByteArray &data, const Protocol::Descriptor &descriptor ){

        qDebug() << "Size = " << data.size();

    });

    QObject::connect(&thRf, SIGNAL(started()), &pr, SLOT(Start()));

    QObject::connect( &pr, &Protocol::EndReadPlace, [&a, &pr](){

            pr.InitRF();

        });

    QObject::connect( &pr, SIGNAL( GoToCrop( QByteArray, const char, const char )),
                      &cr, SLOT( MakeMat( QByteArray, char, char ))
                      );

    QObject::connect( &cr, SIGNAL( EndOfCrop( cv::Mat, char, qint8 )),
                      &fd, SLOT ( FindObject( cv::Mat, char, qint8 ))
                );


    pr.moveToThread( &thRf );
    cr.moveToThread( &thCr );
    fd.moveToThread( &thFd );

    qDebug() << "Main Thread " << QThread::currentThreadId();



    thRf.start();
    thCr.start();
    thFd.start();

    //pr.InitRF();
    fd.LoadCascades();

    return a.exec();
}















