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

Q_DECLARE_METATYPE( cv::Mat )
Q_DECLARE_METATYPE( Protocol::Descriptor )
Q_DECLARE_METATYPE( std::uint8_t )

#define OPENSSL_CPUID_SETUP

QT_USE_NAMESPACE

#include <dlfcn.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "";

    qRegisterMetaType < cv::Mat >( "cv::Mat" );
    qRegisterMetaType < std::uint8_t >( "std::uint8_t" );
    qRegisterMetaType < Protocol::Descriptor >( "Protocol::Descriptor" );

    Protocol pr;
    JPGcrop  cr;
    Finder   fd;
    Sender   sd;

    QThread  thFS;
    QThread  thRf;
    QThread  thCr;
    QObject  thGroup;

    QObject::connect( &fd, SIGNAL( FindEnd( const qint8, const qint8, const qint8, const int)),
                      &sd, SLOT( SendPlease( qint8, qint8, qint8, int))
                      );

    QObject::connect( &thRf, SIGNAL( started() ),
                      &pr,   SLOT( Start() )
                      );

    QObject::connect( &pr, SIGNAL( GoToCrop( QByteArray, const char, const char )),
                      &cr, SLOT( MakeMat( QByteArray, char, char ))
                    );

    QObject::connect( &cr, SIGNAL( EndOfCrop( cv::Mat, char, qint8, qint8, int )),
                      &fd, SLOT ( FindObject( cv::Mat, char, qint8, qint8, int ))
                    );

    QObject::connect( &pr, SIGNAL(EndOfRecive( int, char, char )),
                      &sd, SLOT( IGetImage( int, char, char ))
                      );

    fd.setParent( &thGroup );
    sd.setParent( &thGroup );

    pr.moveToThread( &thRf );
    cr.moveToThread( &thCr );
    thGroup.moveToThread( &thFS );

    thRf.start();
    thCr.start();
    thFS.start();

    fd.LoadCascades();

    return a.exec();
}














