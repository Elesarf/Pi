#ifndef PHASH_H
#define PHASH_H

#include <QObject>
#include <QCryptographicHash>

# include "opencv2/core/core.hpp"
# include "opencv2/highgui/highgui.hpp"
# include "opencv2/imgproc/imgproc.hpp"

class pHash : public QObject
{
	Q_OBJECT

public:

    explicit pHash( QObject *parent = 0 );

public slots:

	quint64 GetHash ( cv:: Mat );
	quint64 CalcHammingDistance ( quint64, quint64 );
	void compareImage ( cv::Mat, cv::Mat );


private:

	cv::Mat __frame;
	cv::Mat __templ;
	cv::Scalar __average;

	QByteArray __ba;

	quint64 __hash;
	int __index;

	quint64 __hash1, __hash2, __distance;




};

#endif // PHASH_H
