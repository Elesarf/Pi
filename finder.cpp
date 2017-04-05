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

Finder::Finder(QObject* parent) : QObject(parent) {}

bool Finder::FindObject(const cv::Mat& frame,
                        qint8 cam,
                        qint8 pic,
                        qint8 place,
                        int size) {
  qDebug() << "Finder -> Starting find Object";
  std::string nameOnWindow;

  __bufString.clear();
  __hammingDistance = 0;
  __tempDistance = 2500;
  __indexOfright = 0;
  qint8 tempFinds = 4;
  __hashFr = new Long_hash(15000);
  frame.copyTo(__fr);
  GetHash(__fr, __hashFr);

  qDebug() << "Find on #" << place << " place, #" << cam << " cam";
  qDebug() << "Using base #" << (place - 1) * 12 + cam;

  Checker(__hashFr, __list[(place - 1) * 12 + cam - 1]);
  qDebug() << "Index: " << __indexOfright;

  if ((__bufString == "01_Winstone_Blue") ||
      (__bufString == "15_Winstone_Silver"))
    Winstone_Checker(__fr);

  if ((__bufString == "07_LD_Blue") || (__bufString == "09_LD_Red"))
    LD_Checker(__fr);

  if ((__bufString == "08_LD_Soveriegn_Blue") ||
      (__bufString == "10_LD_Soveriegn_Red"))
    Sovering_Checker(__fr);

  if ((__bufString == "11_More_Blue") || (__bufString == "13_More_Red"))
    More_Checker(__fr);

  if (__bufString == "14_Mevis")
    More_Checker(__fr);

  cv::imwrite(
      (QString("/home/pi/shr/#%2_place_#%1_pack.jpg").arg(cam).arg(place))
          .toStdString(),
      __fr);

  qDebug() << "Finder -> found " << __bufString
           << QString(" on pack_#%1 #%2 matches ").arg(cam).arg(__tempDistance);

  __bufString.chop(__bufString.length() - 2);

  emit FindEndMaySend(place, cam, __bufString.toInt(), size, tempFinds);

  __fr.release();

  return true;
}

void Finder::Checker(Long_hash* hashFr, QList<BaseVectors>& __list) {
  for (qint8 index = 0; index < __list.length(); ++index) {
    __hammingDistance = __list[index].hash->distance(hashFr);

    qDebug() << "Base name: " << __list[index].name
             //<< " base hash:  " << __list[index].hash->Dump().c_str()
             //<< " frame hash: " << hashFr.Dump().c_str()
             << " distance: " << __hammingDistance;

    if (!__hammingDistance) {
      __tempDistance = __hammingDistance;
      __indexOfright = index;
      break;
    }
    if (__tempDistance > __hammingDistance) {
      __tempDistance = __hammingDistance;
      __indexOfright = index;
    }
  }

  if (__indexOfright < __list.size()) {
    __bufString = __list[__indexOfright].name;

  } else {
    __bufString = QChar(__indexOfright);
  }

  __bufString.chop(3);
}

qint8 Finder::LD_Checker(cv::Mat frame) {
  qDebug() << "Finder -> LD_Checker";

  if (Color_conv(frame, cv::Scalar(0, 56, 50), cv::Scalar(21, 241, 255)))
    __bufString = "09_LD_Red";

  if (Color_conv(frame, cv::Scalar(171, 130, 64), cv::Scalar(177, 223, 255)))
    __bufString = "09_LD_Red";

  if (Color_conv(frame, cv::Scalar(106, 152, 72), cv::Scalar(115, 251, 247)))
    __bufString = "09_LD_Blue";

  qDebug() << "Color conv found " <<  __bufString;

//  cv::Mat HSV, threshold, blurred;
//  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
//  cv::cvtColor(frame, HSV, CV_BGR2HSV);
//  cv::medianBlur(HSV, blurred, 21);

//  inRange(HSV, cv::Scalar(0, 56, 50), cv::Scalar(21, 241, 255), threshold);
//  for (int y = 0; y < threshold.rows; y++) {
//    for (int x = 0; x < threshold.cols; x++) {
//      int value = threshold.at<uchar>(y, x);
//      if (value == 255) {
//        cv::Rect rect;
//        int count =
//            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
//        Q_UNUSED(count);
//        if (rect.width >= 40 && rect.width <= 400 && rect.height >= 40 &&
//            rect.height <= 400) {
//          qDebug() << "Color find Red";

//          __bufString = "09_LD_Red";
//        }
//      }
//    }
//  }

//  inRange(HSV, cv::Scalar(106, 152, 72), cv::Scalar(115, 251, 247), threshold);
//  for (int y = 0; y < threshold.rows; y++) {
//    for (int x = 0; x < threshold.cols; x++) {
//      int value = threshold.at<uchar>(y, x);
//      if (value == 255) {
//        cv::Rect rect;
//        int count =
//            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
//        Q_UNUSED(count);
//        if (rect.width >= 40 && rect.width <= 400 && rect.height >= 40 &&
//            rect.height <= 400) {
//          qDebug() << "Color find Blue";
//          __bufString = "07_LD_Blue";
//        }
//      }
//    }
//  }

//  inRange(HSV, cv::Scalar(171, 130, 64), cv::Scalar(177, 223, 255), threshold);
//  for (int y = 0; y < threshold.rows; y++) {
//    for (int x = 0; x < threshold.cols; x++) {
//      int value = threshold.at<uchar>(y, x);
//      if (value == 255) {
//        cv::Rect rect;
//        int count =
//            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
//        if (rect.width >= 40 && rect.width <= 400 && rect.height >= 40 &&
//            rect.height <= 400) {
//          qDebug() << "Color find Red";

//          __bufString = "09_LD_Red";
//        }
//      }
//    }
//  }
}

qint8 Finder::Slim_Checker(cv::Mat frame) {
  qDebug() << "Finder -> Slim_Cheker";

  cv::Mat HSV, threshold, blurred;
  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
  cv::cvtColor(frame, HSV, CV_BGR2HSV);
  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, cv::Scalar(20, 143, 48), cv::Scalar(38, 255, 255), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 90 && rect.width <= 600 && rect.height >= 90 &&
            rect.height <= 600) {
          qDebug() << "Color find 4";

          __bufString = "04_Glamour";

          return 4;  // Glamour
        }
      }
    }
  }
  inRange(HSV, cv::Scalar(102, 0, 0), cv::Scalar(141, 101, 101), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 90 && rect.width <= 600 && rect.height >= 40 &&
            rect.height <= 600) {
          qDebug() << "Color find 4";

          __bufString = "18_LD_Club";

          return 4;  //
        }
      }
    }
  }
  inRange(HSV, cv::Scalar(130, 86, 72), cv::Scalar(165, 209, 249), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 70 && rect.width <= 500 && rect.height >= 70 &&
            rect.height <= 500) {
          qDebug() << "Color find 16";

          __bufString = "16_LD_Slim";

          return 10;
        }
      }
    }
  }

  inRange(HSV, cv::Scalar(106, 158, 80), cv::Scalar(223, 255, 255), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 70 && rect.width <= 500 && rect.height >= 70 &&
            rect.height <= 500) {
          qDebug() << "Color find 19";

          __bufString = "19_Kent_Inanenane";

          return 19;
        }
      }
    }
  }

  return 0;
}

qint8 Finder::Sovering_Checker(Mat frame) {
  qDebug() << "Finder -> Sovering_Checker";
  cv::Mat HSV, threshold, blurred;
  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
  cv::cvtColor(frame, HSV, CV_BGR2HSV);
  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, cv::Scalar(152, 38, 0), cv::Scalar(182, 178, 181), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 60 && rect.width <= 600 && rect.height >= 70 &&
            rect.height <= 600) {
          qDebug() << "Color find 10";

          __bufString = "10_LD_Sovering_Red";

          return 5;
        }
      }
    }
  }

  inRange(HSV, cv::Scalar(108, 154, 84), cv::Scalar(119, 227, 239), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 40 && rect.width <= 600 && rect.height >= 40 &&
            rect.height <= 600) {
          qDebug() << "Color find 8";

          __bufString = "08_LD_Sovering_Blue";

          return 3;
        }
      }
    }
  }

  return 0;
}

qint8 Finder::Winstone_Checker(Mat frame) {
  qDebug() << "Finder -> Winstone_Checker";

  bool silver = false;
  bool blue = false;
  cv::Mat HSV, threshold, blurred;
  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
  cv::cvtColor(frame, HSV, CV_BGR2HSV);
  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, cv::Scalar(100, 0, 104), cv::Scalar(125, 69, 133), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 90 && rect.width <= 500 && rect.height >= 90 &&
            rect.height <= 500) {
          qDebug() << "Color find 9";
          silver = true;

          //          return 9;
        }
      }
    }
  }

  inRange(HSV, cv::Scalar(58, 24, 64), cv::Scalar(115, 129, 173), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 90 && rect.width <= 500 && rect.height >= 90 &&
            rect.height <= 500) {
          qDebug() << "Color find 9";
          silver = true;

          //          return 9;
        }
      }
    }
  }

  inRange(HSV, cv::Scalar(112, 104, 70), cv::Scalar(125, 200, 211), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 50 && rect.width <= 500 && rect.height >= 70 &&
            rect.height <= 500) {
          qDebug() << "Color find 1";
          blue = true;
          //          return 1;
        }
      }
    }
  }
  if (__tempDistance > 1500) {
    inRange(HSV, cv::Scalar(94, 32, 134), cv::Scalar(105, 103, 255), threshold);
    for (int y = 0; y < threshold.rows; y++) {
      for (int x = 0; x < threshold.cols; x++) {
        int value = threshold.at<uchar>(y, x);
        if (value == 255) {
          cv::Rect rect;
          int count =
              cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
          Q_UNUSED(count);
          if (rect.width >= 50 && rect.width <= 250 && rect.height >= 50 &&
              rect.height <= 250) {
            qDebug() << "Color find 0";
            __bufString = "00_Free";

            return 0;
          }
        }
      }
    }
  }

  if (blue && silver) {
    return 77;
  } else if (blue) {
    __bufString = "01_Winstone_Blue";
    return 1;
  } else if (silver) {
    __bufString = "15_Winstone_Silver";
    return 8;
  }
  return 0;
}

qint8 Finder::More_Checker(Mat frame) {
  qDebug() << "Finder -> More_Checker";
  cv::Mat HSV, threshold, blurred;
  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
  cv::cvtColor(frame, HSV, CV_BGR2HSV);
  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, cv::Scalar(96, 52, 40), cv::Scalar(117, 187, 111), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 40 && rect.width <= 600 && rect.height >= 40 &&
            rect.height <= 600) {
          qDebug() << "Color find 6";

          __bufString = "11_More_Blue_#4";

          return 6;
        }
      }
    }
  }

  inRange(HSV, cv::Scalar(0, 0, 43), cv::Scalar(255, 104, 87), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 40 && rect.width <= 600 && rect.height >= 40 &&
            rect.height <= 600) {
          qDebug() << "Color find  7";

          __bufString = "11_More_Blue";

          return 7;
        }
      }
    }
  }

  return 0;
}

qint8 Finder::Mevis_Cheker(Mat frame) {
  qDebug() << "Finder -> Mevis_Checker";
  cv::Mat HSV, threshold, blurred;
  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
  cv::cvtColor(frame, HSV, CV_BGR2HSV);
  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, cv::Scalar(70, 22, 178), cv::Scalar(107, 117, 213), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 30 && rect.width <= 200 && rect.height >= 30 &&
            rect.height <= 200) {
          qDebug() << "Color find 0";

          __indexOfright = 5;

          return 0;
        }
      }
    }
  }

  inRange(HSV, cv::Scalar(107, 134, 62), cv::Scalar(115, 197, 91), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 40 && rect.width <= 600 && rect.height >= 40 &&
            rect.height <= 600) {
          qDebug() << "Color find 8";

          __bufString = "14_Mevis";

          return 8;
        }
      }
    }
  }
  return 0;
}

/////////////
/// \brief Finder::LoadBase
/// Load in list Base Vectors names, and hash`s all packs from base
void Finder::LoadBase() {
  QFile base;
  bool checkBase = true;
  base.setFileName("/home/pi/Base/Base.cfg");
  if (base.open(QIODevice::ReadOnly) && checkBase) {
    qDebug() << "Base file not open, calculating base";

    QDir baseDir;
    QStringList filters;
    QFileInfoList listImage[25];
    cv::Mat fr;
    Finder::BaseVectors bv;

    filters << "*.bmp"
            << "*.jpg";
    baseDir.setNameFilters(filters);
    if (!baseDir.exists("/home/pi/Base/"))
      qDebug() << "Finder -> Error: No such image directory \n";

    for (qint8 j = 1; j <= 2; ++j) {
      for (qint8 i = 1; i < 13; ++i) {
        baseDir.cd(QString("/home/pi/Base/%1/%2").arg(j).arg(i));

        listImage[(j - 1) * 12 + (i - 1)] = baseDir.entryInfoList();

        if (listImage[i].empty()) {
          qDebug() << "Finder: Warning: Base not loaded, directory# " << i
                   << "fail";
        }
      }
    }

    for (qint8 j = 1; j < 25; ++j) {
      for (int i = 0; i < listImage[j].size(); ++i) {
        qDebug() << i << " : " << listImage[j][i].baseName();

        fr = cv::imread(listImage[j][i].absoluteFilePath().toStdString());

        if (!fr.empty()) {
          bv.name = listImage[j][i].baseName();
          bv.hash = new Long_hash(15000);
          GetHash(fr, bv.hash);
          __list[j].append(bv);

          // qDebug() << "  " << bv.hash->Dump().c_str();
        } else
          qDebug() << "Finder -> Error: base image broken";
      }
      qDebug() << "List #" << j << ", loaded: " << listImage[j].size()
               << " objects";
    }
    base.close();
    if (!base.open(QIODevice::WriteOnly)) {  // TODO: write base checker
      qDebug() << "Writing base fail";
    } else {
      for (qint8 j = 0; j < 25; ++j) {
        for (qint8 i = 0; i < __list[j].size(); ++i) {
          base.write(__list[j][i].name.toLatin1());
          base.write(" : ");
          base.write("\n");
        }
      }
      base.close();
    }
  }
}

//////////////
/// \brief Finder::GetHash
/// \param frame - in format opencv Mat
/// \param hh - ref to hash
///
void Finder::GetHash(cv::Mat frame, Long_hash* hh) {
  cv::Mat __frame;
  cv::Scalar __average;
  uint16_t __index = 0;

  frame.copyTo(__frame);
  cv::resize(__frame, __frame, cv::Size(120, 120), 0, 0, cv::INTER_LINEAR);
  cv::cvtColor(__frame, __frame, CV_BGR2GRAY);
  __average = cv::mean(__frame, cv::noArray());
  cv::threshold(__frame, __frame, __average[0], 255, CV_THRESH_BINARY);

  cv::imwrite((QString("/home/pi/shr/pack_#%1.jpg").arg(222)).toStdString(),
              __frame);
  for (int cols = 0; cols <= __frame.cols; ++cols) {
    for (int rows = 0; rows <= __frame.rows; ++rows) {
      if (__frame.at<int>(cols, rows)) {
        hh->SetBit(__index);
      }
      ++__index;
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
bool Finder::Color_conv(cv::Mat frame, Scalar min, Scalar max) {
  qDebug() << "Finder -> Color_conv";

  cv::Mat HSV, threshold, blurred;
  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
  cv::cvtColor(frame, HSV, CV_BGR2HSV);
  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, min, max, threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        Q_UNUSED(count);
        if (rect.width >= 150 && rect.width <= 400 && rect.height >= 150 &&
            rect.height <= 400) {
            imwrite("/home/pi/shr/tresh.jpg", threshold);
          return true;
        }
      }
    }
  }
  return false;
}
