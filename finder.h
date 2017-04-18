#ifndef FINDER_H
#define FINDER_H

#pragma once

#include <QObject>

#include "opencv2/core/core.hpp"

#include "long_hash.h"

class Finder : public QObject {
  Q_OBJECT

 public:
  explicit Finder(QObject* parent = 0);

  void GetHash(cv::Mat, Long_hash*);

  struct BaseVectors {
    QString name;
    Long_hash* hash;
  };

 signals:

  void FindEndMaySend(const qint8, const qint8, const qint32, int, qint8);
  void FindEnd();

 public slots:

  bool FindObject(const cv::Mat&, qint8, qint8, qint8, int);
  void LoadBase();
  void Checker(Long_hash*, QList<BaseVectors>&);


  bool Color_conv(cv::Mat, cv::Scalar, cv::Scalar);
  qint8 LD_Checker(cv::Mat);
  qint8 Slim_Checker(cv::Mat);
  qint8 Sovering_Checker(cv::Mat);
  qint8 Winstone_Checker(cv::Mat);
  qint8 More_Checker(cv::Mat);
  qint8 Mevis_Cheker(cv::Mat);

 private slots:

 private:
  QList<BaseVectors> __list[25];
  cv::Mat __fr;

  quint64 __hammingDistance = 0;
  quint64 __tempDistance = 90;
  qint8 __indexOfright = 0;
  QString __bufString;

  Long_hash* __hashFr;
};

#endif  // FINDER_H
