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
  //  __fr *= 2;
  GetHash(__fr, __hashFr);

/*  if ((cam == 10) || (cam == 11) ||
      (cam == 12)) {  // cam 10-12 checked on slim method
    Slim_Checker(__fr, __list[(place - 1) * 12 + cam - 1]);
  } else {*/  // other cams checked normaly

    // int colorIndex = Color_conv(__fr, __list[(place - 1) * 12 + cam - 1]);

    qDebug() << "Find on #" << place << " place, #" << cam << " cam";
    qDebug() << "Using base #" << (place - 1) * 12 + cam;

    Checker(__hashFr, __list[(place - 1) * 12 + cam - 1]);

//    if (__indexOfright == 8) {
////      qint8 index = Mevis_Cheker(__fr);
////      if (index == 0)
////        index = __indexOfright;

////      __hammingDistance =
////          __list[(place - 1) * 12 + cam - 1][index].hash->distance(__hashFr);
////      qDebug() << "Mevis distance: " << __hammingDistance;
////      if (__hammingDistance < 2000) {
////        qDebug() << "Mevis Replaced Hash";
////        GetHash(__fr, __list[(place - 1) * 12 + cam - 1][index].hash);
////      }
//__bufString = "00_Free";
//    }

    qDebug() << "Index: " << __indexOfright;
    if ((__indexOfright == 2) || (__indexOfright == 4) ||
        (__indexOfright == 9))  // if cam have LD, check it by color
    {
      qint8 index = LD_Checker(__fr, __list[(place - 1) * 12 + cam - 1]);
      __hammingDistance =
          __list[(place - 1) * 12 + cam - 1][index].hash->distance(__hashFr);
      qDebug() << "LD distance: " << __hammingDistance;
      if (__hammingDistance < 2000) {
        qDebug() << "LD Replaced Hash";
        GetHash(__fr, __hashFr);
      }
    }

    if ((__indexOfright == 5) || (__indexOfright == 3)) {
      qint8 index = Sovering_Checker(__fr);
      __hammingDistance =
          __list[(place - 1) * 12 + cam - 1][index].hash->distance(__hashFr);
      qDebug() << "LD Sovering distance: " << __hammingDistance;
      if (__hammingDistance < 2000) {
        qDebug() << "LD Sovering Replaced Hash";
        GetHash(__fr, __list[(place - 1) * 12 + cam - 1][index].hash);
      }
    }

    if ((__indexOfright == 1) || (__indexOfright == 8)) {
      qint8 index = Winstone_Checker(__fr);

      if ((index == 77) || (index == 0))
        index = __indexOfright;

      __hammingDistance =
          __list[(place - 1) * 12 + cam - 1][index].hash->distance(__hashFr);
      qDebug() << "Winstone distance: " << __hammingDistance;
      if (__hammingDistance < 2000) {
        qDebug() << "Winstone Replaced Hash";
        GetHash(__fr, __list[(place - 1) * 12 + cam - 1][index].hash);
      }
    }

    if ((__indexOfright == 7) || (__indexOfright == 6)){
        More_Checker(__fr);
        __bufString = "11_More_Blue";
    }
//  }

  nameOnWindow = __bufString.toStdString();
  cv::imwrite((QString("/home/pi/shr/#%2_place_#%1_pack.jpg").arg(cam).arg(place)).toStdString(),
              __fr);
  qDebug() << "Finder -> found " << QString::fromStdString(nameOnWindow)
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
}

qint8 Finder::Color_conv(cv::Mat frame, QList<BaseVectors>& __list) {
  qDebug() << "Finder -> Color_conv";
  cv::Mat HSV, threshold, blurred;

  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);

  cv::cvtColor(frame, HSV, CV_BGR2HSV);

  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, cv::Scalar(110, 221, 145), cv::Scalar(119, 250, 250), threshold);
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
          qDebug() << "Color find 8";

          __bufString = "08_LD_Soveri_Blue";

          return 8;
        }
      }
    }
  }

  return 0;
}

qint8 Finder::LD_Checker(
    cv::Mat frame,
    QList<Finder::BaseVectors>&
        list) {  // TODO color coefficient!!!!!!! to blue, red LD
  qDebug() << "Finder -> LD_Checker";

  //  Point pts[1][4];
  //  pts[0][0] = Point(0, 0);
  //  pts[0][1] = Point(frame.cols / 2, 0);
  //  pts[0][2] = Point(frame.cols / 2, frame.rows);
  //  pts[0][3] = Point(0, frame.rows);

  //  const Point* points[1] = {pts[0]};
  //  int npoints = 4;
  //  Mat1b mask(frame.rows, frame.cols, uchar(0));
  //  fillPoly(mask, points, &npoints, 1, Scalar(255));

  //  Scalar average = mean(frame, mask);

  //  qreal diff1 = average[0] - average[1];
  //  qreal diff2 = average[1] - average[2];

  //  if (diff2 < 0) {
  //    qDebug() << "Red";
  //    qDebug() << "0: " << average[0] << " 1: " << average[1]
  //             << " 2: " << average[2];
  //    __bufString = "09_LD_Red";
  //  } else {
  //      qDebug() << "Blue";
  //      qDebug() << "0: " << average[0] << " 1: " << average[1]
  //               << " 2: " << average[2];
  //      __bufString = "07_LD_Blue";
  //  }

  qint8 sumBlue = 0;
  qint8 sumSilver = 0;
  qint8 sumRed = 0;
  qint8 selectedColor = 0;

  cv::Mat HSV, threshold, blurred;
  cv::GaussianBlur(frame, frame, cv::Size(3, 3), 4, 4);
  cv::cvtColor(frame, HSV, CV_BGR2HSV);
  cv::medianBlur(HSV, blurred, 21);

  inRange(HSV, cv::Scalar(0, 56, 50), cv::Scalar(21, 241, 255), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        // qDebug() << "FloodFill: " << count;

        sumRed += count;

        // Q_UNUSED(count);
        if (rect.width >= 40 && rect.width <= 400 && rect.height >= 40 &&
            rect.height <= 400) {
          qDebug() << "Color find 4";

          __bufString = "09_LD_Red";

          selectedColor = 4;
          // return 4;
        }
      }
    }
  }

  if (selectedColor == 0) {
    inRange(HSV, cv::Scalar(106, 152, 72), cv::Scalar(115, 251, 247),
            threshold);
    for (int y = 0; y < threshold.rows; y++) {
      for (int x = 0; x < threshold.cols; x++) {
        int value = threshold.at<uchar>(y, x);
        if (value == 255) {
          cv::Rect rect;
          int count =
              cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);

          sumBlue += count;
          // qDebug() << "FloodFill: " << count;
          // Q_UNUSED(count);
          if (rect.width >= 40 && rect.width <= 400 && rect.height >= 40 &&
              rect.height <= 400) {
            qDebug() << "Color find 2";

            __bufString = "07_LD_Blue";

            selectedColor = 2;
            // return 4;
          }
        }
      }
    }
  }

  inRange(HSV, cv::Scalar(171, 130, 64), cv::Scalar(177, 223, 255), threshold);
  for (int y = 0; y < threshold.rows; y++) {
    for (int x = 0; x < threshold.cols; x++) {
      int value = threshold.at<uchar>(y, x);
      if (value == 255) {
        cv::Rect rect;
        int count =
            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200), &rect);
        // qDebug() << "FloodFill: " << count;

        sumRed += count;

        // Q_UNUSED(count);
        if (rect.width >= 40 && rect.width <= 400 && rect.height >= 40 &&
            rect.height <= 400) {
          qDebug() << "Color find 4";

          __bufString = "09_LD_Red";

          selectedColor = 4;
          // return 4;
        }
      }
    }
  }

  //    if (selectedColor == 0) {
  //      inRange(HSV, cv::Scalar(68, 16, 66), cv::Scalar(113, 89, 139),
  //      threshold);
  //      for (int y = 0; y < threshold.rows; y++) {
  //        for (int x = 0; x < threshold.cols; x++) {
  //          int value = threshold.at<uchar>(y, x);
  //          if (value == 255) {
  //            cv::Rect rect;
  //            int count =
  //                cv::floodFill(threshold, cv::Point(x, y),
  //                cv::Scalar(200),
  //                &rect);
  //            // qDebug() << "FloodFill: " << count;

  //            sumSilver += count;

  //            // Q_UNUSED(count);
  //            if (rect.width >= 40 && rect.width <= 400 && rect.height >=
  //            40
  //            &&
  //                rect.height <= 400) {
  //              qDebug() << "Color find 10";

  //              __bufString = "17_LD_Silver";

  //              selectedColor = 10;
  //              // return 10;
  //            }
  //          }
  //        }
  //      }
  //    }

  if (selectedColor == 0)
    __bufString = "00_Free";

  //    qDebug() << "Red: " << sumRed << " Blue: " << sumBlue
  //             << " Silver: " << sumSilver;

  return selectedColor;
}

qint8 Finder::Slim_Checker(cv::Mat frame, QList<Finder::BaseVectors>& list) {
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
if(__tempDistance > 1500)
  {
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


  //  inRange(HSV, cv::Scalar(103, 49, 42), cv::Scalar(128, 175, 87),
  //  threshold);
  //  for (int y = 0; y < threshold.rows; y++) {
  //    for (int x = 0; x < threshold.cols; x++) {
  //      int value = threshold.at<uchar>(y, x);
  //      if (value == 255) {
  //        cv::Rect rect;
  //        int count =
  //            cv::floodFill(threshold, cv::Point(x, y), cv::Scalar(200),
  //            &rect);
  //        Q_UNUSED(count);
  //        if (rect.width >= 40 && rect.width <= 600 && rect.height >= 40
  //        &&
  //            rect.height <= 600) {
  //          qDebug() << "Color find 1";

  //          __bufString = "18_Mevis";

  //          return 11;
  //        }
  //      }
  //    }
  //  }

  return 0;
}

void Finder::LoadBase() {
  QFile base;
  bool checkBase = true;
  base.setFileName("/home/pi/Base/Base.cfg");
  if (base.open(QIODevice::ReadOnly) && checkBase) {
    qDebug() << "Base file not open, calculating base";

    QDir baseDir;         // Base directory
    QStringList filters;  // only image
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
