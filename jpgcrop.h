#ifndef JPGCROP_H
#define JPGCROP_H

#pragma once

#include <QObject>

#include <opencv2/core/core.hpp>

#include "protocol.h"

class JPGcrop : public QObject {
  Q_OBJECT
 public:
  explicit JPGcrop(QObject* parent = 0);

  struct __roiDispenser {};

 public:
 signals:

  void EndOfCrop(const cv::Mat&, qint8, qint8, qint8, const int);

 public slots:

  bool MakeMat(QByteArray, char, qint8);
 private slots:

 private:
  QByteArray __matArray;
  cv::Rect __roi[13];
  cv::Rect __roi1[13];
};

#endif  // JPGCROP_H
