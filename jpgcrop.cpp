#include "jpgcrop.h"

#include <QDebug>

#include <stdio.h>
#include <cstdlib>

JPGcrop::JPGcrop( QObject *parent )
    :QObject( parent )
{

//    roi1 = cv::Rect( 10, 200, 470, 305 );
//    roi2 = cv::Rect( 360, 215, 755, 355 );
//    roi3 = cv::Rect( 1020, 210, 595, 300 );

    roi[0] = cv::Rect( 10, 200, 470, 305 );
    roi[1] = cv::Rect( 360, 215, 755, 355 );
    roi[2] = cv::Rect( 1000, 210, 595, 300 );

    numPic = 0;

}

bool JPGcrop::MakeMat( QByteArray array, char cam, char place ){

    qDebug() << "Start make Mat ... ";

        int w = 0;
        std::string nameStr = "/mnt/smb/pack_%1_%2_#%3" + std::to_string( array.size() ) + ".jpg";

        for( int q = 1; q < array.size() ; ++q ){

                if( array[q-1] == 0xff && array[q] == 0xd9 )
                w = q;
        }

        if( w <= 0 )
        {
            array[array.size() - 1] = 0xd9;
            array[array.size() - 2] = 0xff;
        }

        if( array[0] == 0xff && array[1] == 0xd8){

            cv::waitKey( 10 );

            cv::Mat img = cv::Mat( 1600, 896, CV_32SC4, array.data() );
            cv::Mat matImg = cv::imdecode( img, CV_LOAD_IMAGE_ANYCOLOR );
            if( cv::imwrite( nameStr,matImg )){

                    for(uint8_t i = 1; i <= 3; ++i ){

                            std::string str = ( QString("/mnt/smb/pack_%1_cam#%2_place#%3_pic#%4.jpg"
                                                        ).arg( i ).arg( cam ).arg( place ).arg( numPic )
                                                                                        ).toStdString();

                            cv::imwrite( str, matImg( roi[i-1] ) );

                        }
                }

            cv::waitKey(10);

            return true;
        }

    return false;
}

bool JPGcrop::StartCrop( cv::Mat mt ){

    return true;
}



























