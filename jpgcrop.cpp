#include "jpgcrop.h"

#include <QDebug>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

JPGcrop::JPGcrop(QObject * parent) : QObject(parent)
{
    fillRoi();
}

bool JPGcrop::MakeMat(QByteArray array, char cam, qint8 place)
{
    qDebug() << "Start crop picture ";

    cam -= 0x30;
    qint8 numpack = '\0';

    if ( !array.isEmpty() ) {
        cv::Mat img = cv::Mat( 1600, 400, CV_32SC4, array.data() );
        img = cv::imdecode(img, CV_LOAD_IMAGE_COLOR);

        if ( !img.data ) {
            qDebug() << "Error: JPEG is wrong";
            array.clear();
            img.release();
            return false;
        } else {
            for ( qint8 i = 1; i <= 3; ++i ) {
                switch ( cam ) {    // magick for name cam
                    case 1:
                        numpack = i * cam;
                        break;

                    case 2:
                        numpack = i + cam + 1;
                        break;

                    case 3:
                        numpack = i + cam + 3;
                        break;
                    case 4:
                        numpack = i + cam + 5;
                        break;

                    case 5:
                        numpack = i + 6;
                        break;

                    default:
                        break;
                }
                qDebug() << "Corpped: parts " << i << ", place  #" << place << " pack #" << numpack;
                emit EndOfCrop( img(_roi_arr[place][numpack]), numpack, place, array.size() );
            }
            cv::imwrite( ( QString("/home/pi/shr/pack%1_%2.jpg").arg(numpack).arg(place) ).toStdString(),  img );
            array.clear();
            img.release();
            qDebug() << "Stop crop picture\n";
            emit( ImFree() );

            return true;
        }
    }

    return false;
}   // JPGcrop::MakeMat

void JPGcrop::fillRoi()
{
    _roi_arr[1][1]	= cv::Rect(1021, 57, 487, 222);//
    _roi_arr[1][2]	= cv::Rect(489, 76, 519, 224);//
    _roi_arr[1][3]	= cv::Rect(76, 39, 372, 224);//
    _roi_arr[1][4]	= cv::Rect(971, 4, 481, 195);//
    _roi_arr[1][5]	= cv::Rect(421, 12, 529, 212);//
    _roi_arr[1][6]	= cv::Rect(22, 10, 390, 216);//
    _roi_arr[1][7]	= cv::Rect(1013, 94, 513, 216);//
    _roi_arr[1][8]	= cv::Rect(463, 101, 544, 220);///
    _roi_arr[1][9]	= cv::Rect(48, 45, 413, 230);//
    _roi_arr[1][10] = cv::Rect(1025, 92, 566, 197);//
    _roi_arr[1][11] = cv::Rect(354, 146, 669, 162);//
    _roi_arr[1][12] = cv::Rect(2, 90, 348, 167);//

    _roi_arr[2][1]	= cv::Rect(1065, 26, 445, 204);//
    _roi_arr[2][2]	= cv::Rect(554, 20, 517, 212);//
    _roi_arr[2][3]	= cv::Rect(143, 14, 401, 210);//
    _roi_arr[2][4]	= cv::Rect(1009, 18, 461, 237);//
    _roi_arr[2][5]	= cv::Rect(566, 58, 478, 212);//
    _roi_arr[2][6]	= cv::Rect(46, 45, 423, 230);//
    _roi_arr[2][7]	= cv::Rect(1094, 41, 449, 224);//
    _roi_arr[2][8]	= cv::Rect(566, 55, 499, 214);//
    _roi_arr[2][9]	= cv::Rect(145, 37, 388, 212);//
    _roi_arr[2][10] = cv::Rect(1084, 22, 513, 160);//
    _roi_arr[2][11] = cv::Rect(396, 30, 685, 177);//
    _roi_arr[2][12] = cv::Rect(4, 16, 392, 167);//

    _roi_arr[3][1]	= cv::Rect(1065, 26, 445, 204);//
    _roi_arr[3][2]	= cv::Rect(554, 20, 517, 212);//
    _roi_arr[3][3]	= cv::Rect(143, 14, 401, 210);//
    _roi_arr[3][4]	= cv::Rect(1009, 18, 461, 237);//
    _roi_arr[3][5]	= cv::Rect(46, 45, 423, 230);//
    _roi_arr[3][6]	= cv::Rect(46, 45, 423, 230);//
    _roi_arr[3][7]	= cv::Rect(1094, 41, 449, 224);//
    _roi_arr[3][8]	= cv::Rect(566, 55, 499, 214);//
    _roi_arr[3][9]	= cv::Rect(145, 37, 388, 212);//
    _roi_arr[3][10] = cv::Rect(1084, 22, 513, 160);//
    _roi_arr[3][11] = cv::Rect(396, 30, 685, 177);//
    _roi_arr[3][12] = cv::Rect(4, 16, 392, 167);//
}   // JPGcrop::fillRoi

//

//

//

//

//

//

//

//

//

//

//

//

//

//

//

//

//

//

//

//

//

/////////////////////////
