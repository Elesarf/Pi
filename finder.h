#ifndef FINDER_H
#define FINDER_H

#pragma once

#include <QObject>

#include "opencv2/core/core.hpp"

class Finder : public QObject {
  Q_OBJECT

 public:
  explicit Finder(QObject* parent = 0);

  qint8 Color_conv(cv::Mat);

  quint64 GetHash(cv::Mat);
  quint64 CalcHammingDistance(quint64, quint64);

  struct FindsVect {
    int numberOnList;
    int numberCascade;
    int sizeVect;
  };

  struct BaseVectors {
    QString name;
    quint64 hash;
  };

 signals:

  void FindEndMaySend(const qint8, const qint8, const qint32, int, qint8);
  void FindEnd();

 public slots:

  bool FindObject(const cv::Mat&, char, qint8, qint8, int);
  QList<BaseVectors> LoadBase();

 private slots:

 private:
  QList<BaseVectors> __list;
  cv::Mat fr;
};

#endif  // FINDER_H
