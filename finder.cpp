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
    qDebug() << "Find Object... ";

    cv::Mat fr;

    std::string nameOnWindow;
    QString bufString = NULL;

	Finder::FindsVect findsVector[__list.size() ];
    QString strToConvert;

    cv::vector<cv::Rect> finds;

    qint8 indexOfright = 0;
    qint8 tempFinds = 0;
    qint8 numpack;

    cam -= 0x30;
    finds.clear();
    frame.copyTo( fr );

	cv::CascadeClassifier faceClassificator[__list.size()];

    for(int i = 0; i < __list.size(); ++i ){

        std::string patch = __list[i].filePath().toStdString();

        if( !faceClassificator[i].load( patch ) ){

            qDebug() << "Error: loading classificator";
            return false;

        }
    }

    switch ( cam ) {                                                                    // magick for name cam
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

    if ( !(( numpack == 2 ) || ( numpack == 5 ) || ( numpack == 8 )|| ( numpack == 11 ))){   //+ brightness

            frame *= 1.01;

        }

    int colorIndex = Color_conv( fr );

    if( colorIndex == 0 ){

            //-- Detect pacs
            for ( int indexOfcascade = 0; indexOfcascade < __list.size(); ++indexOfcascade ){

					faceClassificator[indexOfcascade].detectMultiScale( frame, finds, 1.01, 0,
					                                                    CV_HAAR_DO_ROUGH_SEARCH|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size( 100, 40 ), cv::Size( 540, 216 ) );

                    if ( finds.size() != 0 ){

                            for( size_t i = 0; i < finds.size(); i++ ){

                                    cv::Point center( finds[i].x + finds[i].width*0.5, finds[i].y + finds[i].height*0.5 );
                                    cv::Point a( center.x + 75, center.y + 75 );
                                    cv::Point b( center.x - 75, center.y - 75 );

									//cv::rectangle( fr, a, b, cv::Scalar( 255, 0, 255 ), 3, 8, 0);

                                    center.x -= 90;
									QString bufString = __list[indexOfcascade].fileName();
                                    bufString.chop( 4 );
                                    std::string nameOnWindow = bufString.toStdString();
									//cv::putText( fr, nameOnWindow, center, cv::FONT_HERSHEY_PLAIN, 2.5,  cv::Scalar( 0, 0, 255, 255 ), 4, 8);
                                    nameOnWindow = ( QString("/mnt/smb/pack_#%1.jpg" ).arg( numpack )).toStdString();

                                }

                        } else {

                            std::string nameOnWindow = bufString.toStdString();
                            nameOnWindow = ( QString("/mnt/smb/pack_#%1.jpg" ).arg( numpack )).toStdString();

                        }

                    strToConvert = __list[ indexOfcascade ].fileName();
					strToConvert.chop( strToConvert.length() - 2);
                    findsVector[ indexOfcascade ].numberCascade = indexOfcascade;
                    findsVector[ indexOfcascade ].numberOnList =  strToConvert.toInt();


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
        } else {

			    indexOfright = colorIndex;
				tempFinds = 6;

        }

        if( indexOfright < __list.size() ){

                bufString = __list[ indexOfright ].fileName();
                bufString.chop( 4 );

            } else {

                bufString = indexOfright;

            }

            nameOnWindow = bufString.toStdString();

			//cv::putText( fr, nameOnWindow, cv::Point( 0, 30 ), cv::FONT_HERSHEY_PLAIN, 2,  cv::Scalar( 0, 250, 0), 4, 8);
            cv::imwrite(( QString("/mnt/smb/pack_#%1.jpg" ).arg( numpack )).toStdString(), fr );
			//cv::imwrite(( QString("/mnt/smb/pack_#%1_%2.jpg" ).arg( numpack ).arg( size )).toStdString(), frame );

            qDebug() << "found " << QString::fromStdString( nameOnWindow ) << QString( " on pack_#%1 #%2 matches "
                                                                    ).arg( numpack ).arg( tempFinds ) ;


			bufString.chop( bufString.length() - 2 );


			emit FindEndMaySend( place, numpack, bufString.toInt() , size, tempFinds );
            fr.release();

    return true;
}

qint8 Finder::Color_conv( cv::Mat frame ){  // This is CRUTCH TIME!!!

    cv::Mat HSV, threshold, blurred;

    cv::GaussianBlur( frame, frame, cv::Size( 3, 3 ), 4, 4);

    cv::cvtColor( frame, HSV, CV_BGR2HSV );


    cv::medianBlur(HSV, blurred, 21);

//	cv::inRange( HSV, cv::Scalar( 24, 62, 68 ), cv::Scalar( 57, 255, 249 ), threshold);
//	for(int y = 0; y < threshold.rows; y++){
//			for(int x = 0; x < threshold.cols; x++){
//					int value = threshold.at<uchar>(y, x);
//					if(value == 255){
//							cv::Rect rect;
//							int count = cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
//							Q_UNUSED( count );
//							if(rect.width >= 120 && rect.width <= 700
//							&& rect.height >= 120 && rect.height <= 700){
//									cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
//									return 6; //Glamour

//								}
//						}
//				}
//		}

//	cv::inRange( HSV, cv::Scalar( 160, 194, 120 ), cv::Scalar( 229, 243, 255 ), threshold);
//	for(int y = 0; y < threshold.rows; y++){
//			for(int x = 0; x < threshold.cols; x++){
//					int value = threshold.at<uchar>(y, x);
//					if(value == 255){
//							cv::Rect rect;
//							int count = cv::floodFill( threshold, cv::Point(x, y), cv::Scalar(200), &rect);
//							Q_UNUSED( count );
//							if( rect.width >= 180 && rect.width <= 310
//							&& rect.height >= 180 && rect.height <= 310){
//									cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
//									return 2; // Winstone red

//								}
//						}
//				}
//		}

//	cv::inRange( HSV, cv::Scalar( 16, 79, 68 ), cv::Scalar( 29, 255, 200 ), threshold);
//	for(int y = 0; y < threshold.rows; y++){
//			for(int x = 0; x < threshold.cols; x++){
//					int value = threshold.at<uchar>(y, x);
//					if(value == 255){
//							cv::Rect rect;
//							int count = cv::floodFill( threshold, cv::Point(x, y), cv::Scalar(200), &rect);
//							Q_UNUSED( count );
//							if( rect.width >= 125 && rect.width <= 700
//							&& rect.height >= 125 && rect.height <= 700){
//									cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
//									return 7; // Glamour Safari

//								}
//						}
//				}
//		}

	cv::inRange( HSV, cv::Scalar( 140, 78, 46 ), cv::Scalar( 179, 201, 173 ), threshold);
	for(int y = 0; y < threshold.rows; y++){
			for(int x = 0; x < threshold.cols; x++){
					int value = threshold.at<uchar>(y, x);
					if(value == 255){
							cv::Rect rect;
							int count = cv::floodFill( threshold, cv::Point(x, y), cv::Scalar(200), &rect);
							Q_UNUSED( count );
							if( rect.width >= 300 && rect.width <= 460
							&& rect.height >= 160 && rect.height <= 220){
									//cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
									return 4; // LD_S_Red
								}
						}
				}
		}

	cv::inRange( HSV, cv::Scalar( 112, 120, 84 ), cv::Scalar( 121, 255, 255 ), threshold);
	for(int y = 0; y < threshold.rows; y++){
			for(int x = 0; x < threshold.cols; x++){
					int value = threshold.at<uchar>(y, x);
					if(value == 255){
							cv::Rect rect;
							int count = cv::floodFill( threshold, cv::Point(x, y), cv::Scalar(200), &rect);
							Q_UNUSED( count );
							if( rect.width >= 170 && rect.width <= 390
							&& rect.height >= 170 && rect.height <= 230){
									//cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
									return 2; // LD_Blue

								}
						}
				}
		}

	cv::inRange( HSV, cv::Scalar( 102, 98, 100 ), cv::Scalar( 117, 193, 197 ), threshold);
	for(int y = 0; y < threshold.rows; y++){
			for(int x = 0; x < threshold.cols; x++){
					int value = threshold.at<uchar>(y, x);
					if(value == 255){
							cv::Rect rect;
							int count = cv::floodFill( threshold, cv::Point(x, y), cv::Scalar(200), &rect);
							Q_UNUSED( count );
							if( rect.width >= 150 && rect.width <= 380
							&& rect.height >= 150 && rect.height <= 380 ){
									//cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
									return 5; // LD_Silver

								}
						}
				}
		}

	cv::inRange( HSV, cv::Scalar( 140, 78, 46 ), cv::Scalar( 179, 223, 221 ), threshold);
	for(int y = 0; y < threshold.rows; y++){
			for(int x = 0; x < threshold.cols; x++){
					int value = threshold.at<uchar>(y, x);
					if(value == 255){
							cv::Rect rect;
							int count = cv::floodFill( threshold, cv::Point(x, y), cv::Scalar(200), &rect);
							Q_UNUSED( count );
							if( rect.width >= 140 && rect.width <= 335
							&& rect.height >= 140 && rect.height <= 250 ){
									//cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
									return 3; // LD_Red
								}
						}
				}
		}

//	cv::inRange( HSV, cv::Scalar( 100, 98, 108 ), cv::Scalar( 127, 193, 197 ), threshold);
//	for(int y = 0; y < threshold.rows; y++){
//			for(int x = 0; x < threshold.cols; x++){
//					int value = threshold.at<uchar>(y, x);
//					if(value == 255){
//							cv::Rect rect;
//							int count = cv::floodFill( threshold, cv::Point(x, y), cv::Scalar(200), &rect);
//							Q_UNUSED( count );
//							if( rect.width >= 160 && rect.width <= 400
//							&& rect.height >= 160 && rect.height <= 400){
//									cv::rectangle(frame, rect, cv::Scalar(255, 0, 255, 4));
//									return 22; // LD_Silver

//								}
//						}
//				}
//		}

    return 0;
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

        if( __list.empty() ){

                qDebug() << "Error: list of cascades not loaded";

        }

        qDebug() << "Loaded " << __list.size() << " cascades";

    for(int i = 0; i < __list.size(); ++i ){


			qDebug() << i << " : " << __list[i].baseName();

            }

    return __list;
}















