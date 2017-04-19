#include "finder.h"
#include <QDebug>
#include <QDir>
#include <QString>
#include <QFile>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;

Finder::Finder(QObject * parent) : QObject(parent){ }

bool Finder::FindObject(const cv::Mat& frame, qint8 cam, qint8 place, int size)
{
    qDebug() << "Finder -> Starting find Object";

    _bufString.clear();
    _hammingDistance	= 0;
    _tempDistance		= 2500;
    _indexOfright		= 0;
    qint8 tempFinds = 4;
    _hashFr = new Long_hash(15000);
    frame.copyTo(_fr);
    GetHash(_fr, _hashFr);

    qDebug() << "Find on #" << place << " place, #" << cam << " cam";
    qDebug() << "Using base #" << (place - 1) * 12 + cam;

    Hash_Checker(_hashFr, _list[(place - 1) * 12 + cam - 1 ]);
    qDebug() << "Index: " << _indexOfright;

    if ( (_bufString == "01_Winstone_Blue") || (_bufString == "15_Winstone_Silver") )
        Winstone_Checker(_fr);

    if ( (_bufString == "07_LD_Blue") || (_bufString == "09_LD_Red") || (_bufString == "12_LD_Silver") )
        LD_Checker(_fr);

    if ( (_bufString == "08_LD_Sovereign_Blue") || (_bufString == "10_LD_Sovereign_Red") )
        Sovering_Checker(_fr);

    if ( (_bufString == "11_More_Blue") || (_bufString == "13_More_Red") )
        More_Checker(_fr);

    if ( _bufString == "14_Mevius" )
        Mevius_Cheker(_fr);

    cv::imwrite( ( QString("/home/pi/shr/#%2_place_#%1_pack.jpg").arg(cam).arg(place) ).toStdString(), _fr );

    qDebug()	<< "Finder -> found " << _bufString
                << QString(" on place_#%3 pack_#%1 #%2 matches ").arg(cam).arg(_tempDistance).arg(place);

    _bufString.chop(_bufString.length() - 2);

    emit FindEndMaySend(place, cam, _bufString.toInt(), size, tempFinds);

    _fr.release();

    return true;
}   // Finder::FindObject

void Finder::Hash_Checker(Long_hash * hashFr, QList<BaseVectors>& _list)
{
    for ( qint8 index = 0; index < _list.length(); ++index ) {
        _hammingDistance = _list[index].hash->distance(hashFr);

        qDebug() << "Base name: " << _list[index].name << " distance: " << _hammingDistance;

        if ( !_hammingDistance ) {
            _tempDistance	= _hammingDistance;
            _indexOfright	= index;
            break;
        }
        if ( _tempDistance > _hammingDistance ) {
            _tempDistance	= _hammingDistance;
            _indexOfright	= index;
        }
    }

    if ( _indexOfright < _list.size() ) {
        _bufString = _list[_indexOfright].name;
    } else {
        _bufString = QChar(_indexOfright);
    }
    _bufString.chop(8);
}

void Finder::LD_Checker(cv::Mat frame)
{
    qDebug() << "Finder -> LD_Checker";

    if ( Color_conv( frame, cv::Scalar(0, 56, 50), cv::Scalar(21, 241, 255) ) )
        _bufString = "09_LD_Red";

    if ( Color_conv( frame, cv::Scalar(171, 130, 64), cv::Scalar(177, 223, 255) ) )
        _bufString = "09_LD_Red";

    if ( Color_conv( frame, cv::Scalar(106, 152, 72), cv::Scalar(115, 251, 247) ) )
        _bufString = "07_LD_Blue";

    if ( Color_conv( frame, cv::Scalar(98, 36, 112), cv::Scalar(119, 157, 251) ) )
        _bufString = "12_LD_Silver";

    qDebug() << "Color conv found " <<  _bufString;
}

void Finder::Sovering_Checker(Mat frame)
{
    qDebug() << "Finder -> Sovering_Checker";

    if ( Color_conv( frame, cv::Scalar(0, 113, 139), cv::Scalar(16, 255, 255) ) )
        _bufString = "10_LD_Sovering_Red";

    if ( Color_conv( frame, cv::Scalar(110, 216, 160), cv::Scalar(125, 255, 255) ) )
        _bufString = "08_LD_Sovering_Blue";

    qDebug() << "Color conv found " <<  _bufString;
}   // Finder::Sovering_Checker

void Finder::Winstone_Checker(Mat frame) // TODO: organization silver && blue == true
{
    qDebug() << "Finder -> Winstone_Checker";

    if ( Color_conv( frame, cv::Scalar(100, 0, 104), cv::Scalar(125, 69, 133) ) )
        _bufString = "15_Winstone_Silver";

    if ( Color_conv( frame, cv::Scalar(110, 108, 55), cv::Scalar(140, 235, 220) ) )
        _bufString = "01_Winstone_Blue";

    qDebug() << "Color conv found " <<  _bufString;

}   // Finder::Winstone_Checker

void Finder::Mevius_Cheker(Mat frame)
{
    qDebug() << "Finder -> Mevis_Checker";

    if ( Color_conv( frame, cv::Scalar(92, 118, 160), cv::Scalar(147, 255, 255) ) )
        _bufString = "14_Mevis";

    qDebug() << "Color conv found " <<  _bufString;

}   // Finder::Mevis_Cheker

qint8 Finder::More_Checker(Mat frame)
{
    qDebug() << "Finder -> More_Checker";
    cv::Mat HSV, threshold, blurred;
    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
    cv::cvtColor(frame, HSV, CV_BGR2HSV);
    cv::medianBlur(HSV, blurred, 21);

    inRange(HSV, cv::Scalar(96, 52, 40), cv::Scalar(117, 187, 111), threshold);
    for ( int y = 0; y < threshold.rows; y++ ) {
        for ( int x = 0; x < threshold.cols; x++ ) {
            int value = threshold.at<uchar>(y, x);
            if ( value == 255 ) {
                cv::Rect rect;
                int count =
                  cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
                Q_UNUSED(count);
                if ( rect.width >= 40 && rect.width <= 600 && rect.height >= 40 &&
                  rect.height <= 600 )
                {
                    qDebug() << "Color find 6";

                    _bufString = "11_More_Blue_#4";

                    return 6;
                }
            }
        }
    }

    inRange(HSV, cv::Scalar(0, 0, 43), cv::Scalar(255, 104, 87), threshold);
    for ( int y = 0; y < threshold.rows; y++ ) {
        for ( int x = 0; x < threshold.cols; x++ ) {
            int value = threshold.at<uchar>(y, x);
            if ( value == 255 ) {
                cv::Rect rect;
                int count =
                  cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
                Q_UNUSED(count);
                if ( rect.width >= 40 && rect.width <= 600 && rect.height >= 40 && rect.height <= 600 ) {
                    qDebug() << "Color find  7";

                    _bufString = "11_More_Blue";

                    return 7;
                }
            }
        }
    }

    return 0;
}   // Finder::More_Checker

/////////////
/// \brief Finder::LoadBase
/// Load in list Base Vectors names, and hash`s all packs from base
void Finder::LoadBase()
{
    QFile base;
    bool checkBase = true;

    base.setFileName("/home/pi/Base/Base.cfg");
    if ( base.open(QIODevice::ReadOnly) && checkBase ) {
        qDebug() << "Base file not open, calculating base";

        QDir baseDir;
        QStringList filters;
        QFileInfoList listImage[25];
        cv::Mat fr;
        Finder::BaseVectors bv;

        filters << "*.bmp"
                << "*.jpg";
        baseDir.setNameFilters(filters);
        if ( !baseDir.exists("/home/pi/Base/") )
            qDebug() << "Finder -> Error: No such image directory \n";

        for ( qint8 j = 1; j <= 2; ++j ) {
            for ( qint8 i = 1; i < 13; ++i ) {
                baseDir.cd( QString("/home/pi/Base/%1/%2").arg(j).arg(i) );

                listImage[(j - 1) * 12 + (i - 1)] = baseDir.entryInfoList();

                if ( listImage[i].empty() ) {
                    qDebug()	<< "Finder: Warning: Base not loaded, directory# " << i
                                << "fail";
                }
            }
        }

        for ( qint8 j = 0; j < 25; ++j ) {
            for ( int i = 0; i < listImage[j].size(); ++i ) {
                qDebug() << i << " : " << listImage[j][i].baseName();

                fr = cv::imread( listImage[j][i].absoluteFilePath().toStdString() );

                if ( !fr.empty() ) {
                    bv.name = listImage[j][i].baseName();
                    bv.hash = new Long_hash(15000);
                    GetHash(fr, bv.hash);
                    _list[j].append(bv);

                    // qDebug() << "  " << bv.hash->Dump().c_str();
                } else {
                    qDebug() << "Finder -> Error: base image broken";
                }
            }
            qDebug()	<< "List #" << j << ", loaded: " << listImage[j].size()
                        << " objects";
        }
        base.close();
        if ( !base.open(QIODevice::WriteOnly) ) {   // TODO: write base checker
            qDebug() << "Writing base fail";
        } else {
            for ( qint8 j = 0; j < 25; ++j ) {
                for ( qint8 i = 0; i < _list[j].size(); ++i ) {
                    base.write( _list[j][i].name.toLatin1() );
                    base.write(" : ");
                    base.write("\n");
                }
            }
            base.close();
        }
    }
}   // Finder::LoadBase

//////////////
/// \brief Finder::GetHash
/// \param frame - in format opencv Mat
/// \param hh - ref to hash
///
void Finder::GetHash(cv::Mat frame, Long_hash * hh)
{
    cv::Mat _frame;
    cv::Scalar _average;
    uint16_t _index = 0;

    frame.copyTo(_frame);
    cv::resize(_frame, _frame, cv::Size(120, 120), 0, 0, cv::INTER_LINEAR);
    cv::cvtColor(_frame, _frame, CV_BGR2GRAY);
    _average = cv::mean( _frame, cv::noArray() );
    cv::threshold(_frame, _frame, _average[0], 255, CV_THRESH_BINARY);

    for ( int cols = 0; cols <= _frame.cols; ++cols ) {
        for ( int rows = 0; rows <= _frame.rows; ++rows ) {
            if ( _frame.at<int>(cols, rows) ) {
                hh->SetBit(_index);
            }
            ++_index;
        }
    }
}

////////////////////
/// \brief Finder::Color_conv
/// \param frame
/// \param min
/// \param max
/// \return
/// this template to future
bool Finder::Color_conv(cv::Mat frame, Scalar min, Scalar max)
{
    qDebug() << "Finder -> Color_conv";

    cv::Mat HSV, threshold, blurred;
    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
    cv::cvtColor(frame, HSV, CV_BGR2HSV);
    cv::medianBlur(HSV, blurred, 21);

    inRange(HSV, min, max, threshold);
    for ( int y = 0; y < threshold.rows; y++ ) {
        for ( int x = 0; x < threshold.cols; x++ ) {
            int value = threshold.at<uchar>(y, x);
            if ( value == 255 ) {
                cv::Rect rect;
                int count =
                  cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
                Q_UNUSED(count);
                if ( rect.width >= 150 && rect.width <= 400 && rect.height >= 150 &&
                  rect.height <= 400 )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

qint8 Finder::Slim_Checker(cv::Mat frame)
{
    qDebug() << "Finder -> Slim_Cheker";

    cv::Mat HSV, threshold, blurred;
    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
    cv::cvtColor(frame, HSV, CV_BGR2HSV);
    cv::medianBlur(HSV, blurred, 21);

    inRange(HSV, cv::Scalar(20, 143, 48), cv::Scalar(38, 255, 255), threshold);
    for ( int y = 0; y < threshold.rows; y++ ) {
        for ( int x = 0; x < threshold.cols; x++ ) {
            int value = threshold.at<uchar>(y, x);
            if ( value == 255 ) {
                cv::Rect rect;
                int count =
                  cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
                Q_UNUSED(count);
                if ( rect.width >= 90 && rect.width <= 600 && rect.height >= 90 &&
                  rect.height <= 600 )
                {
                    qDebug() << "Color find 4";

                    _bufString = "04_Glamour";

                    return 4;   // Glamour
                }
            }
        }
    }
    inRange(HSV, cv::Scalar(102, 0, 0), cv::Scalar(141, 101, 101), threshold);
    for ( int y = 0; y < threshold.rows; y++ ) {
        for ( int x = 0; x < threshold.cols; x++ ) {
            int value = threshold.at<uchar>(y, x);
            if ( value == 255 ) {
                cv::Rect rect;
                int count =
                  cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
                Q_UNUSED(count);
                if ( rect.width >= 90 && rect.width <= 600 && rect.height >= 40 &&
                  rect.height <= 600 )
                {
                    qDebug() << "Color find 4";

                    _bufString = "18_LD_Club";

                    return 4;   //
                }
            }
        }
    }
    inRange(HSV, cv::Scalar(130, 86, 72), cv::Scalar(165, 209, 249), threshold);
    for ( int y = 0; y < threshold.rows; y++ ) {
        for ( int x = 0; x < threshold.cols; x++ ) {
            int value = threshold.at<uchar>(y, x);
            if ( value == 255 ) {
                cv::Rect rect;
                int count =
                  cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
                Q_UNUSED(count);
                if ( rect.width >= 70 && rect.width <= 500 && rect.height >= 70 &&
                  rect.height <= 500 )
                {
                    qDebug() << "Color find 16";

                    _bufString = "16_LD_Slim";

                    return 10;
                }
            }
        }
    }

    inRange(HSV, cv::Scalar(106, 158, 80), cv::Scalar(223, 255, 255), threshold);
    for ( int y = 0; y < threshold.rows; y++ ) {
        for ( int x = 0; x < threshold.cols; x++ ) {
            int value = threshold.at<uchar>(y, x);
            if ( value == 255 ) {
                cv::Rect rect;
                int count =
                  cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
                Q_UNUSED(count);
                if ( rect.width >= 70 && rect.width <= 500 && rect.height >= 70 &&
                  rect.height <= 500 )
                {
                    qDebug() << "Color find 19";

                    _bufString = "19_Kent_Inanenane";

                    return 19;
                }
            }
        }
    }

    return 0;
}   // Finder::Slim_Checker
