#include "jpgcrop.h"

#include <QDebug>

#include <stdio.h>
#include <cstdlib>

JPGcrop::JPGcrop( QObject *parent )
    :QObject( parent )
{

    roi[2] = cv::Rect( 20, 180, 500, 270 );
    roi[1] = cv::Rect( 410, 200, 755, 300 );
    roi[0] = cv::Rect( 1030, 240, 555, 240 );

    numPic = 0;

}

bool JPGcrop::MakeMat( QByteArray array, char cam, char place ){

    qDebug() << "Start crop picture ";;

        int w = 0;

        for( int q = 1; q < array.size() ; ++q ){

                if( array[q-1] == char( 0xff ) && array[q] == char( 0xd9 ) )
                w = q;
        }

        if( w <= 0 ){

            array[array.size() - 1] = 0xd9;
            array[array.size() - 2] = 0xff;

        }

        if( array[0] == char( 0xff ) && array[1] == char( 0xd8 )){

            cv::Mat img = cv::Mat( 1600, 896, CV_32SC4, array.data() );
            img = cv::imdecode( img, CV_LOAD_IMAGE_ANYCOLOR );

            if( !img.data ){

                qDebug() << "Error: JPEG is wrong";

                } else {

                    for(uint8_t i = 1; i <= 3; ++i ){

                            std::string str = ( QString("/mnt/smb/pack_#%1_place#%2_pic#%3.jpg" )
                                                .arg( ( place - 0x30 ) + ( cam - 0x30 ) + i - 1 )
                                                .arg( place ).arg( numPic ) ).toStdString();

                            cv::imwrite( str, img( roi[i-1] ) );
                            ++numPic;

                            emit EndOfCrop( img( roi[i-1] ), cam, i, place );

                        }
                }

            array.clear();
            img.release();
            img.release();

            qDebug() << "Stop crop picture";

            return true;
        }

    return false;
}



























