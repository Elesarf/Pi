#include "jpgcrop.h"

#include <QDebug>

#include <stdio.h>
#include <cstdlib>

JPGcrop::JPGcrop( QObject *parent )
    :QObject( parent )
{

}

bool JPGcrop::MakeMat( QByteArray ar ){

    qDebug() << "Start make Mat ... ";

    std::string nameStr="./Test/test"+std::to_string( ar.size())+".jpg";

    if( ar[0] == 0xff && ar[1] == 0xd8 ){

        cv::waitKey( 10 );

        cv::Mat img = cv::Mat( 1600, 896, CV_32SC4, ar.data() );
        cv::Mat matImg = cv::imdecode( img, CV_LOAD_IMAGE_ANYCOLOR );
        cv::imwrite( nameStr, matImg );
        cv::waitKey( 10 );

        return true;
    }

    return false;

}

bool JPGcrop::StartCrop( cv::Mat mt ){

    qDebug() << "Start crop ";

    return true;
}
