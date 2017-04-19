#ifndef FINDER_H
#define FINDER_H

#ifndef QT_WARNING_DISABLE_DEPRECATED
# define QT_WARNING_DISABLE_DEPRECATED
#endif

#pragma once

#include <QObject>

#include "opencv2/core/core.hpp"

#include "long_hash.h"

class Finder : public QObject {
    Q_OBJECT

public:
    explicit Finder(QObject * parent = 0);

    void    GetHash(cv::Mat, Long_hash *);

    struct BaseVectors {
        QString		name;
        Long_hash * hash;
    };

signals:

    void FindEndMaySend(qint8,  qint8,  qint32, int, qint8);
    void    FindEnd();

public slots:

    bool FindObject(const cv::Mat&, qint8, qint8,  int);
    void LoadBase();
    void Hash_Checker(Long_hash *, QList<BaseVectors>&);


    bool Color_conv(cv::Mat, cv::Scalar, cv::Scalar);
    void LD_Checker(cv::Mat);
    qint8 Slim_Checker(cv::Mat);
    void Sovering_Checker(cv::Mat);
    void Winstone_Checker(cv::Mat);
    qint8 More_Checker(cv::Mat);
    void Mevius_Cheker(cv::Mat);

private slots:

private:
    QList<BaseVectors> _list[25];
    cv::Mat _fr;

    quint64 _hammingDistance	= 0;
    quint64 _tempDistance		= 90;
    qint8 _indexOfright			= 0;
    QString _bufString;

    Long_hash * _hashFr;
};

#endif  // FINDER_H
