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

 public:
 signals:

  void EndOfCrop(const cv::Mat&, char, qint8, qint8, const int);

 public slots:

  bool MakeMat(QByteArray, char, char);
 private slots:

 private:
  QByteArray __matArray;
  cv::Rect __roi[3];
};

#endif  // JPGCROP_H
