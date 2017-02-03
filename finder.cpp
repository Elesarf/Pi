#include "finder.h"

#include <QDebug>
#include <QDir>
#include <QString>

#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

Finder::Finder(QObject* parent) : QObject(parent) {}

bool Finder::FindObject(const cv::Mat& frame,
                        char cam,
                        qint8 pic,
                        qint8 place,
                        int size) {
  qDebug() << "Find Object... ";

  cam -= 0x30;
  qint8 numpack = '\0';

  switch (cam) {  // magick for name cam
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
  QString bufString;

  qint8 indexOfright = 0;
  qint8 tempFinds = 2;

  frame.copyTo(fr);

  quint64 hashFr = GetHash(fr);
  quint64 hammingDistance = 0;
  quint64 tempDistance = 100;

  for (qint8 index = 0; index < __list.length(); ++index) {
    hammingDistance = CalcHammingDistance(__list[index].hash, hashFr);

    if (tempDistance > hammingDistance) {
      tempDistance = hammingDistance;
      indexOfright = index;
    }
  }

  //    static const QString templ("%1-%2");
  //    auto result = templ.arg (indexOfright < __list.size () ?
  //    __list[indexOfright].name : QChar(indexOfright)).arg()

  if (indexOfright < __list.size()) {
    bufString = __list[indexOfright].name;

  } else {
    bufString = QChar(indexOfright);
  }

  nameOnWindow = bufString.toStdString();

  cv::putText(fr, nameOnWindow, cv::Point(0, 30), cv::FONT_HERSHEY_PLAIN, 2,
              cv::Scalar(0, 250, 0), 4, 8);

  cv::imwrite((QString("/mnt/smb/pack_#%1.jpg").arg(numpack)).toStdString(),
              fr);
  // cv::imwrite(( QString("/mnt/smb/pack_#%1_%2.jpg" ).arg( numpack ).arg( size
  // )).toStdString(), frame );

  qDebug()
      << "found " << QString::fromStdString(nameOnWindow)
      << QString(" on pack_#%1 #%2 matches ").arg(numpack).arg(hammingDistance);

  bufString.chop(bufString.length() - 2);

  emit FindEndMaySend(place, numpack, bufString.toInt(), size, tempFinds);
  fr.release();

  return true;
}

QList<Finder::BaseVectors> Finder::LoadBase() {
  QDir baseDir;         // Base directory
  QStringList filters;  // only image
  QFileInfoList listImage;
  cv::Mat fr;
  Finder::BaseVectors bv;

  filters << "*.bmp"
          << "*.jpg";
  baseDir.setNameFilters(filters);
  if (!baseDir.exists("/home/pi/Base/"))
    qDebug() << "Error: No such image directory \n";

  baseDir.cd("/home/pi/Base/");

  listImage = baseDir.entryInfoList();

  if (listImage.empty()) {
    qDebug() << "Error: Base not loaded";
  }

  for (int i = 0; i < listImage.size(); ++i) {
    qDebug() << i << " : " << listImage[i].baseName();

    fr = cv::imread(listImage[i].absoluteFilePath().toStdString());

    if (!fr.empty()) {
      bv.name = listImage[i].baseName();
      bv.hash = GetHash(fr);
      __list.append(bv);

      qDebug() << "  " << bv.hash;
    } else
      qDebug() << "Error: base image broken";
  }

  qDebug() << "Loaded " << listImage.size() << " objects";

  return __list;
}

quint64 Finder::GetHash(cv::Mat frame) {
  cv::Mat __frame;
  cv::Scalar __average;
  int __index = 0;
  quint64 __hash = 0;

  frame.copyTo(__frame);
  cv::resize(__frame, __frame, cv::Size(8, 8), 0, 0, cv::INTER_LINEAR);
  cv::cvtColor(__frame, __frame, CV_BGR2GRAY);
  __average = cv::mean(__frame, cv::noArray());
  cv::threshold(__frame, __frame, __average[0], 255, CV_THRESH_BINARY);

  for (int cols = 0; cols <= __frame.cols; ++cols) {
    for (int rows = 0; rows <= __frame.rows; ++rows) {
      if (__frame.at<int>(cols, rows)) {
        __hash |= static_cast<quint64>(1) << __index;
      }
      ++__index;
    }
  }

  return __hash;
}

quint64 Finder::CalcHammingDistance(quint64 __hash1, quint64 __hash2) {
  quint64 __distance = 0;
  quint64 val = __hash1 ^ __hash2;

  while (val) {
    ++__distance;
    val &= val - 1;
  }

  return __distance;
}
