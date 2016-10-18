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

bool Finder::FindObject(const cv::Mat &frame, char cam, qint8 pic, qint8 place)
{


    cv::CascadeClassifier faceClassificator[list.size()];

    for(int i = 0; i < list.size(); ++i ){

        std::string patch = list[i].filePath().toStdString();

        if( !faceClassificator[i].load( patch ) ){

            qDebug() << "--(!)Error loading\n";
            return false;

        }
    }

    std::string nameOnWindow;
    QString bufString;

    cv::vector<cv::Rect> finds;
    cv::vector<cv::Rect> tempFinds;

    qint8 indexOfright = 0;

    tempFinds.clear();
    finds.clear();

    //-- Detect pacs
    for ( int indexOfcascade = 0; indexOfcascade < list.size(); ++indexOfcascade ){

        faceClassificator[indexOfcascade].detectMultiScale( frame, finds, 1.02, 2,
                                CV_HAAR_DO_ROUGH_SEARCH, cv::Size( 60, 60 ), cv::Size( 500, 250 ) );

        if ( finds.size() >= tempFinds.size()){

                tempFinds = finds;
                indexOfright = indexOfcascade;

            }

    }

            bufString = list[indexOfright].fileName();
            bufString.chop( 4 );
            nameOnWindow = bufString.toStdString();

            qDebug()<<"found "<<QString::fromStdString( nameOnWindow ) << QString( " on pack_#%1"
                                                                    ).arg( pic + ( cam - 0x30 ) + 1 ) ;

            emit FindEnd( place, (pic + ( cam - 0x30 ) + 1), indexOfright );

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




















