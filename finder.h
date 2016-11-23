#ifndef FINDER_H
#define FINDER_H

#pragma once

#include <QObject>

#include <iostream>
#include <QDir>
#include <QTextStream>
#include <stdio.h>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"



class Finder: public QObject
{
        Q_OBJECT

public:

    explicit Finder(QObject *parent = 0);

    struct FindsVect{

        int numberOnList;
        int numberCascade;
        int sizeVect;

    };

signals:

    void FindEndMaySend( const qint8, const qint8, const qint8, int, qint8 );
    void FindEnd();

public slots:

   bool FindObject(const cv::Mat &, char, qint8, qint8, int );
   QFileInfoList LoadCascades();

private slots:

private:

   QFileInfoList __list;

};

#endif // FINDER_H
