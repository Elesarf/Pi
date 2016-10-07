#include "protocol.h"
#include "jpgcrop.h"

#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QThread>

QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "";

    Protocol pr;
    JPGcrop  cr;
    QThread  thRf;
    QThread  thCr;

    int numberOfPic = 0;

    QObject::connect( &pr, &Protocol::EndOfRecive, [&a, &pr, &numberOfPic]( QByteArray &data, Protocol::Descriptor &descriptor ){

        qDebug() << "Size = " << data.size();

        QFile f( QString("/mnt/smb/screen_%1_%2_#%3.jpg").arg( descriptor.numCam ).arg( descriptor.numPlace ).arg( numberOfPic ) );
        f.open( QFile::WriteOnly );
        f.write( data );
        numberOfPic++;

        qDebug() << QString( "Save JPG cam #%1 place #%2  #%3 ... Ok" ).arg( descriptor.numCam ).arg( descriptor.numPlace ).arg( numberOfPic );

    });

    QObject::connect( &pr, &Protocol::EndReadPlace, [&a, &pr](){


            pr.InitRF();

        });

    //QObject::connect( &pr, SIGNAL(Testrec(QByteArray)), &cr, SLOT(MakeMat( QByteArray )));


    pr.moveToThread( &thRf );
    cr.moveToThread( &thCr );

    thCr.start();
    thRf.start();

    pr.InitRF();

    return a.exec();
}


















