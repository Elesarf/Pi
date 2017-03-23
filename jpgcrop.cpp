#include "jpgcrop.h"

#include <QDebug>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

JPGcrop::JPGcrop(QObject* parent) : QObject(parent) {
  int margin = 0;
  __roi[3] = cv::Rect(80 + margin, 30, 360 + margin, 215);
  __roi[2] = cv::Rect(438 + margin, 41, 512 + margin, 217);
  __roi[1] = cv::Rect(1000 + margin, 37, 460 + margin, 193);
  __roi[6] = cv::Rect(70 + margin, 20, 420 - 70 + margin, 250 - 20);
  __roi[5] = cv::Rect(446 + margin, 26, 954 - 446 + margin, 267 - 26);
  __roi[4] = cv::Rect(956 + margin, 24, 1420 - 956 + margin, 240 - 24);
  __roi[9] = cv::Rect(81 + margin, 10, 430 - 81 + margin, 222 - 10);
  __roi[8] = cv::Rect(447 + margin, 20, 930 - 447 + margin, 230 - 20);
  __roi[7] = cv::Rect(992 + margin, 9, 1420 - 956 + margin, 240 - 24);
  __roi[12] = cv::Rect(20 + margin, 25, 426 - 86 + margin, 236 - 18);
  __roi[11] = cv::Rect(451 + margin, 46, 936 - 451 + margin, 230 - 46);
  __roi[10] =
      cv::Rect(975 + margin, 40 + 40, 1448 - 975 + margin, 234 - 40 + 40);

//  __roi1[3] = cv::Rect(144, 76, 529-144, 270-76);
//  __roi1[2] = cv::Rect(630, 82, 1103-630, 281-82);
//  __roi1[1] = cv::Rect(1160, 97, 1548-1160, 266-97);
//  __roi1[6] = cv::Rect(175, 73, 543-175, 261-73);
//  __roi1[5] = cv::Rect(636, 93, 1089-636, 267-93);
//  __roi1[4] = cv::Rect(1170, 82, 1560-1170, 232-82);
//  __roi1[9] = cv::Rect(114, 63, 482-114, 239-63);
//  __roi1[8] = cv::Rect(559, 58, 1007-559, 218-58);
//  __roi1[7] = cv::Rect(1080, 77, 1463-1080, 220-77);
//  __roi1[12] = cv::Rect(90, 26, 459-90, 207-26);
//  __roi1[11] = cv::Rect(506, 28, 967-506, 191-28);
//  __roi1[10] = cv::Rect(1065, 21, 1434-1065, 157-21);
}

bool JPGcrop::MakeMat(QByteArray array, char cam, qint8 place) {
  qDebug() << "Start crop picture ";

  cam -= 0x30;
  qint8 numpack = '\0';

  if (!array.isEmpty()) {
    if (((array[0] == char(0xff) && array[1] == char(0xd8)) &&
         (array.size() < 75000)) &&
        ((array[array.size() - 1] == char(0xd9)) &&
         (array[array.size() - 2] == char(0xff)))) {
      cv::Mat img = cv::Mat(1600, 896, CV_32SC4, array.data());
      img = cv::imdecode(img, CV_LOAD_IMAGE_COLOR);

      if (!img.data) {
        qDebug() << "Error: JPEG is wrong";
        array.clear();
        img.release();
        return false;

      } else {
        for (qint8 i = 1; i <= 3; ++i) {
          qDebug() << "Corpped: " << i << " parts";

          switch (cam) {  // magick for name cam
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

//          if (place == 1) {
//            switch (numpack) {
//              case 7:
//                numpack = 10;
//                break;
//              case 8:
//                numpack = 11;
//                break;
//              case 9:
//                numpack = 12;
//                break;
//              case 10:
//                numpack = 7;
//                break;
//              case 11:
//                numpack = 8;
//                break;
//              case 12:
//                numpack = 9;
//                break;
//            }
//          }

              emit EndOfCrop(img(__roi[numpack]), numpack, i, place, array.size());

        }

        cv::imwrite(
            (QString("/home/pi/shr/pack%1_%2.jpg").arg(numpack).arg(place))
                .toStdString(),
            img);
        array.clear();
        img.release();

        qDebug() << "Stop crop picture";

        return true;
      }
    }
  }

  return false;
}
