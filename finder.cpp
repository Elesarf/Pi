#include "finder.h"

#include <QDebug>
#include <QString>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"



Finder::Finder( QObject *parent )
    :QObject( parent )
{

}

bool Finder::FindObject( cv::Mat frame, char cam, char place  )
{
    cv::CascadeClassifier faceClassificator[list.size()];

    for(int i = 0; i < list.size(); ++i ){
        std::string patch = list[i].filePath().toStdString();
        if( !faceClassificator[i].load( patch ) ){
            qDebug() << "--(!)Error loading\n";
            return false;
        }
    }

    qDebug() << "<<<<<<<<<<<<<<<<<<<<<<" << "Cascades already load";

    qDebug() << "finding...";
    std::string nameOnWindow;
    QString bufString;

    cv::vector<cv::Rect> finds;

    //-- Detect pacs
    for ( int indexOfcascade = 0; indexOfcascade < list.size(); ++indexOfcascade ){

        faceClassificator[indexOfcascade].detectMultiScale( frame, finds, 1.01, 1.1, 1, cv::Size(180, 180) );

        for( size_t i = 0; i < finds.size(); i++ )
        {

            bufString=list[indexOfcascade].fileName();
            bufString.chop(4);
            nameOnWindow=bufString.toStdString();

            qDebug()<<"found "<<QString::fromStdString(nameOnWindow) << QString( " on cam_#%1_pack_#%2"
                                                                                 ).arg( cam ).arg( place ) ;

        }
    }


    qDebug() << "finding end";
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




















