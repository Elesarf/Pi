#include "QDebug"
#include <QCryptographicHash>

#include "phash.h"

pHash::pHash( QObject *parent )
    :QObject ( parent )
{
	    __index = 0;
		__hash = 0;

}

quint64 pHash::GetHash(cv::Mat frame)
{
	__index = 0;
	__hash = 0;
	frame.copyTo( __frame );
	cv::resize( __frame, __frame, cv::Size( 8, 8 ), 0, 0, cv::INTER_LINEAR );
	cv::cvtColor( __frame, __frame, CV_BGR2GRAY );
	__average = cv::mean( __frame, cv::noArray() );
	cv::threshold( __frame, __frame, __average[0], 255, CV_THRESH_BINARY );

	for( int cols = 0; cols <= __frame.cols; ++cols ){

			for ( int rows = 0; rows <= __frame.rows; ++rows){
					if ( __frame.at<int>( cols, rows )){

							__hash |= static_cast<quint64>(1) << __index;
						}
					++__index;
				}
		}
	
	return __hash;
	
}

quint64 pHash::CalcHammingDistance(quint64 __hash1, quint64 __hash2)
{

	__distance = 0;
	quint64 val = __hash1 ^ __hash2;

	while ( val ) {

			++__distance;
			val &= val - 1;

		}

	return  __distance;
}
