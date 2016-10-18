#include "protocol.h"
#include "jpgcrop.h"
#include "finder.h"
#include "sender.h"

#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QString>
#include <QFile>
#include <QThread>

#include <stdio.h>
#include <cstdlib>

Q_DECLARE_METATYPE( cv::Mat )
Q_DECLARE_METATYPE( Protocol::Descriptor )
Q_DECLARE_METATYPE( std::uint8_t )

QT_USE_NAMESPACE

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
    Sender   sd;

    QThread  thFd;
    QThread  thRf;
    QThread  thCr;

    QObject::connect( &fd, SIGNAL( FindEnd( const qint8, const qint8, const qint8 )),
                      &sd, SLOT( SendPlease(const qint8, const qint8, const qint8)));

    QObject::connect( &thRf, SIGNAL( started() ), &pr, SLOT( Start() ));

    QObject::connect( &pr, SIGNAL( GoToCrop( QByteArray, const char, const char )),
                      &cr, SLOT( MakeMat( QByteArray, char, char ))
                    );

    QObject::connect( &cr, SIGNAL( EndOfCrop( cv::Mat, char, qint8, qint8 )),
                      &fd, SLOT ( FindObject( cv::Mat, char, qint8, qint8 ))
                    );


    pr.moveToThread( &thRf );
    cr.moveToThread( &thCr );
    fd.moveToThread( &thFd );

    thRf.start();
    thCr.start();
    thFd.start();

    fd.LoadCascades();

    return a.exec();
}















