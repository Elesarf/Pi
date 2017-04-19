#ifndef JPGCROP_H
#define JPGCROP_H

#ifndef QT_WARNING_DISABLE_DEPRECATED
# define QT_WARNING_DISABLE_DEPRECATED
#endif

#pragma once

#include <QObject>

#include <opencv2/core/core.hpp>

#include "protocol.h"

class JPGcrop : public QObject {
    Q_OBJECT
public:
    explicit JPGcrop(QObject * parent = 0);

public:
signals:

    void EndOfCrop(const cv::Mat&, qint8, qint8, const int);
    void ImFree();

public slots:

    bool MakeMat(QByteArray, char, qint8);
private:

    void fillRoi();


private:
    QByteArray _matArray;
    cv::Rect _roi[13];
    cv::Rect _roi_arr[13][13];
};

#endif  // JPGCROP_H
