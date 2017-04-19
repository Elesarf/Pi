#include "protocol.h"
#include "jpgcrop.h"
#include "finder.h"
#include "sender.h"
#include "my_tcpsocket.h"

#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QString>
#include <QThread>
#include <QFile>

Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(Protocol::Descriptor)
Q_DECLARE_METATYPE(std::uint8_t)

#define OPENSSL_CPUID_SETUP

QT_USE_NAMESPACE

#include <dlfcn.h>  // it just be here

///////////
/// \brief checkerBase
/// \return
/// checked base on server
bool checkerBase();

///////////
/// \brief uartSetter
/// setting raspberry to uart out
void uartSetter();

int main(int argc, char * argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "";

    //  if (!checkerBase())
    //    qDebug() << "Warning: base not upgrade";

    uartSetter();

    system("mount -t tmpfs -o size=50m tmpfs /home/pi/shr/");   // it may be neednt

    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::uint8_t>("std::uint8_t");
    qRegisterMetaType<Protocol::Descriptor>("Protocol::Descriptor");

    //    Protocol pr;
    JPGcrop cr;
    Finder fd;
    Sender sd;
    My_TCPSocket skt;

    QThread thFS;
    QThread thRecieve;
    QObject thGroup;

    QObject::connect( &fd, SIGNAL(FindEndMaySend(qint8,qint8,
      qint32,int,qint8)),
      &sd, SLOT(SendPlease(qint8,qint8,qint32,int,qint8)));

    //    QObject::connect( &thRecieve, SIGNAL(started()), &pr, SLOT(Start()));

    //    QObject::connect( &pr, SIGNAL(GoToCrop(QByteArray,const char,const qint8)),
    //      &cr, SLOT(MakeMat(QByteArray,char,qint8)));

    QObject::connect( &cr, SIGNAL(EndOfCrop(cv::Mat,qint8,qint8,int)),
      &fd, SLOT(FindObject(cv::Mat,qint8,qint8,int)));

    //    QObject::connect( &pr, SIGNAL(EndOfRecive(int,char,char)), &sd,
    //      SLOT(IGetImage(int,char,char)));

    QObject::connect( &skt, SIGNAL(imgComplete(QByteArray,const char,const qint8)), &cr,
      SLOT(MakeMat(QByteArray,char,qint8)));

    QObject::connect( &cr, SIGNAL(ImFree()), &skt, SLOT(WaitCropper()));

    fd.setParent(&thGroup);
    sd.setParent(&thGroup);
    cr.setParent(&thGroup);

    // pr.moveToThread(&thRecieve);
    skt.moveToThread(&thRecieve);
    thGroup.moveToThread(&thFS);

    thRecieve.start();
    thFS.start();

    fd.LoadBase();

    return a.exec();
}   // main

bool checkerBase()
{
    qDebug() << "Checking base to upgrade";

    system(
        "sshpass -p xXu3di2Ac ssh -p 6682 fedale@5.178.86.82 ls > "
        "~/Base/out_server.txt");
    system("ls > ~/Base/out_local.txt");

    QFile out_ls_server("/home/pi/Base/out_local.txt");
    QFile out_ls_local("/home/pi/Base/out_server.txt");

    if ( ( !out_ls_local.open(QIODevice::ReadOnly | QIODevice::Text) ) ||
      ( !out_ls_server.open(QIODevice::ReadOnly | QIODevice::Text) ) )
    {
        qDebug() << "Error: canot open base upgrade file";
        return false;
    }

    while ( out_ls_server.atEnd() ) {
        if ( out_ls_local.readLine() != out_ls_server.readLine() ) {
            qDebug() << "Base files is different, start load base from server";

            //////////////
            /// \brief system
            /// may load base off server, if it sloooooow system, remove it))
            system(
                "(sshpass -p "
                "xXu3di2Ac"
                " scp -r -P 6682 fedale@5.178.86.82:/home/fedale/*.jpg "
                "/home/pi/Base;exit");
            qDebug() << "Loaded base off server, wait any seconds)";
            return true;
        }
    }
    return true;
};

void uartSetter()
{
    system(
        "sudo echo 4 > /sys/class/gpio/export && echo out > "
        "/sys/class/gpio/gpio4/direction"); // gpio set on pin 4
    system("sudo echo 1 > /sys/class/gpio/gpio4/value");
}
