#include "jpgcrop.h"

#include <QDebug>

#include <stdio.h>
#include <cstdlib>

JPGcrop::JPGcrop( QObject *parent )
    :QObject( parent )
{

    __roi[2] = cv::Rect( 5, 90, 540, 216 );
    __roi[1] = cv::Rect( 410, 58, 700, 280 );
    __roi[0] = cv::Rect( 1060, 90, 540, 216 );

}

bool JPGcrop::MakeMat( QByteArray array, char cam, char place ){

    qDebug() << "Start crop picture ";

    if( !array.isEmpty() ){

            if( ((array[0] == char( 0xff ) && array[1] == char( 0xd8 )) && ( array.size() < 75000 ))
                    && (( array[array.size() - 1] == char( 0xd9 )) && (array[array.size() - 2] == char( 0xff )))){

                    cv::Mat img = cv::Mat( 1600, 896, CV_32SC4, array.data() );
                    img = cv::imdecode( img, CV_LOAD_IMAGE_COLOR );

                    if( !img.data ){

                            qDebug() << "Error: JPEG is wrong";

                        } else {

                            for(uint8_t i = 1; i <= 3; ++i ){

                                    emit EndOfCrop( img( __roi[i-1] ), cam, i, place, array.size());

                                }
                        }

                    array.clear();
                    img.release();

                    qDebug() << "Stop crop picture";

                    return true;
                }
        }

    return false;
}



























