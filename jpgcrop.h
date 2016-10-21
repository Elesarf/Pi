#ifndef JPGCROP_H
#define JPGCROP_H

#pragma once

#include <QObject>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "protocol.h"

class JPGcrop : public QObject
{
        Q_OBJECT
public:

    explicit JPGcrop( QObject *parent = 0 );

public:

signals:

    void EndOfCrop( const cv::Mat &, char, qint8, qint8, const int );

public slots:

    bool MakeMat( QByteArray, char, char);

private slots:

private:

    cv::Rect roi[3];
    uint32_t numPic;

};

#endif // JPGCROP_H
