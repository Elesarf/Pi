#include "finder.h"

#include <QDebug>
#include <QString>
#include <QThread>

#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"



Finder::Finder( QObject *parent )
    :QObject( parent )
{

}

bool Finder::FindObject( cv::Mat frame, char cam, qint8 place  )
{
    cv::CascadeClassifier faceClassificator[list.size()];

     qDebug() << "Finder Thread " << QThread::currentThreadId();

    for(int i = 0; i < list.size(); ++i ){

        std::string patch = list[i].filePath().toStdString();

        if( !faceClassificator[i].load( patch ) ){

            qDebug() << "--(!)Error loading\n";
            return false;

        }
    }

    //qDebug() << "<<<<<<<<<<<<<<<<<<<<<<" << "Cascades already load";

    std::string nameOnWindow;
    QString bufString;

    cv::vector<cv::Rect> finds;
    //finds.clear();

    qDebug() << QString( "finding on pack %1" ).arg( place );

    //-- Detect pacs
    for ( int indexOfcascade = 0; indexOfcascade < list.size(); ++indexOfcascade ){

        faceClassificator[indexOfcascade].detectMultiScale( frame, finds, 1.01, 3,
                                                    0|CV_HAAR_DO_ROUGH_SEARCH, cv::Size( 60, 60 ), cv::Size( 400, 250 ) );

        for( size_t i = 0; i < finds.size(); i++ )
        {

            bufString = list[indexOfcascade].fileName();
            bufString.chop( 4 );
            nameOnWindow = bufString.toStdString();

            qDebug()<<"found "<<QString::fromStdString( nameOnWindow ) << QString( " on cam_#%1_pack_#%2"
                                                                                 ).arg( cam ).arg( place ) ;

        }

    }



                qDebug() << QString( "finding end on pack %1" ).arg( place );

    return true;
}

QFileInfoList Finder::LoadCascades(){

    QDir cascadeDir;                                                                    //cascades directory
    QStringList filters;                                                                //only .xml

    filters << "*.xml";
    cascadeDir.setNameFilters(filters);
        if( !cascadeDir.exists("/home/pi/Haars/") )
            qDebug() << "No such cascade directory \n";

        cascadeDir.cd("/home/pi/Haars/");
        list = cascadeDir.entryInfoList();

        if(list.empty()){

                qDebug() << "<<<<<<<<<<<<<<< List of cascades not loaded";

        }

    qDebug() << QString( "Loaded %1 cascades" ).arg( list.size() );

    return list;
}




















