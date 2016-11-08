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

bool Finder::FindObject(const cv::Mat &frame, char cam, qint8 pic, qint8 place, int size )
{
    qDebug() << "FindObject... ";


    cv::CascadeClassifier faceClassificator[__list.size()];

    for(int i = 0; i < __list.size(); ++i ){

        std::string patch = __list[i].filePath().toStdString();

        if( !faceClassificator[i].load( patch ) ){

            qDebug() << "Error: loading classificator";
            return false;

        }
    }

    qint8 numpack;

    cam -= 0x30;
    switch ( cam ) {
    case 1:
            numpack = pic * cam;
        break;

    case 2:
            numpack = pic + cam + 1;
        break;

    case 3:
            numpack = pic + cam + 3;
        break;
    case 4:
            numpack = pic + cam + 5;
        break;

    default:
        break;
        }

    std::string nameOnWindow;
    QString bufString = NULL;

    Finder::FindsVect findsVector[ __list.size() ];
    QString strToConvert;

    cv::vector<cv::Rect> finds;
    //cv::vector<cv::Rect> tempFinds;

    qint8 indexOfright = 0;
    qint8 tempFinds = 0;

    //tempFinds.clear();
    finds.clear();

    cv::Mat fr =  frame;

    //-- Detect pacs
    for ( int indexOfcascade = 0; indexOfcascade < __list.size(); ++indexOfcascade ){

        faceClassificator[indexOfcascade].detectMultiScale( frame, finds, 1.025, 1,
                                0|CV_HAAR_DO_CANNY_PRUNING, cv::Size( 100, 40 ), cv::Size( 500, 200 ) );

        if ( finds.size() != 0 ){

                for( size_t i = 0; i < finds.size(); i++ ){

                        cv::Point center( finds[i].x + finds[i].width*0.5, finds[i].y + finds[i].height*0.5 );
                        cv::Point a( center.x + 75, center.y + 75 );
                        cv::Point b( center.x - 75, center.y - 75 );

                        cv::rectangle( fr, a, b, cv::Scalar( 255, 0, 255 ), 3, 8, 0);

                        center.x -= 90;
                        QString bufString=__list[indexOfcascade].fileName();
                        bufString.chop( 4 );
                        std::string nameOnWindow = bufString.toStdString();
                        cv::putText( fr, nameOnWindow, center, cv::FONT_HERSHEY_PLAIN, 2,  cv::Scalar( 0, 0, 255, 255 ), 4, 8);
                        nameOnWindow = ( QString("/mnt/smb/pack_#%1.jpg" ).arg( numpack )).toStdString();
                        cv::imwrite( nameOnWindow, fr );

                    }

            } else {

                std::string nameOnWindow = bufString.toStdString();
                nameOnWindow = ( QString("/mnt/smb/pack_#%1.jpg" ).arg( numpack )).toStdString();
                cv::imwrite( nameOnWindow, fr );

            }

        strToConvert = __list[ indexOfcascade ].fileName();
        strToConvert.chop( strToConvert.length() - 1);
        findsVector[ indexOfcascade ].numberCascade = indexOfcascade;
        findsVector[ indexOfcascade ].numberOnList = strToConvert.toInt();
        findsVector[ indexOfcascade ].sizeVect = finds.size();

    }


    for( qint8 i = 0; i < __list.size(); ++i ){
            for( qint8 q = i; q < __list.size(); ++q ){

                    if (( q != i ) && (findsVector[i].numberOnList == findsVector[q].numberOnList) ){
                        findsVector[i].sizeVect += findsVector[q].sizeVect;
                        findsVector[q].sizeVect =0;
                        }
                }
        }

    for( qint8 i = 0; i < __list.size(); ++i ){

            if (( findsVector[i].sizeVect >= tempFinds ) && ( findsVector[i].sizeVect != 0 )){

                    tempFinds = findsVector[i].sizeVect;
                    indexOfright = findsVector[i].numberCascade;

                }
        }

            bufString = __list[indexOfright].fileName();
            bufString.chop( 4 );
            nameOnWindow = bufString.toStdString();

            qDebug() << "found " << QString::fromStdString( nameOnWindow ) << QString( " on pack_#%1 #%2 matches "
                                                                    ).arg( numpack ).arg( tempFinds ) ;

            bufString.chop( bufString.length() - 1 );
            emit FindEnd( place, numpack, bufString.toInt() , size );

    return true;
}

QFileInfoList Finder::LoadCascades(){

    QDir cascadeDir;                                                                    //cascades directory
    QStringList filters;                                                                //only .xml

    filters << "*.xml";
    cascadeDir.setNameFilters(filters);
        if( !cascadeDir.exists( "/home/pi/Haars/" ) )
            qDebug() << "No such cascade directory \n";

        cascadeDir.cd( "/home/pi/Haars/" );
        __list = cascadeDir.entryInfoList();

        if(__list.empty()){

                qDebug() << "Error: list of cascades not loaded";

        }


    for(int i = 0; i < __list.size(); ++i ){

            qDebug() << __list[i].baseName();

            }

    return __list;
}




















