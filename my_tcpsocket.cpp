#include "my_tcpsocket.h"
#include <QDebug>
#include <QTime>

My_TCPSocket::My_TCPSocket(QObject * parent) : QObject(parent)
{
    initStruct();
    _server = new QTcpServer(this);
    _time	= _time.addSecs(3600 * 3);

    qDebug() << "Message: server up, listen. " << _server->listen(QHostAddress::AnyIPv4, 1235);

    connect( _server, SIGNAL(newConnection()), this, SLOT(incommingConnect()));
}

My_TCPSocket::My_TCPSocket(quint16 port)
{
    initStruct();
    _server = new QTcpServer(this);
    _time	= _time.addSecs(3600 * 3);

    qDebug() << "Message: server up, listen. " << _server->listen(QHostAddress::AnyIPv4, port);

    connect( _server, SIGNAL(newConnection()), this, SLOT(incommingConnect()));
}

void My_TCPSocket::incommingConnect()
{
    QTcpSocket * _socket = _server->nextPendingConnection();

    connect( _socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    _socket->write("client");
    qDebug() << "Message: connect socket #" << _socket->socketDescriptor();
    emit talking("Message: connect estabilished");
}

void My_TCPSocket::readyRead()
{
    QObject * object = QObject::sender();

    if ( !object )
        qDebug() << "Warning: something wrong";
    QTcpSocket * _socket = static_cast<QTcpSocket *>(object);
    _buffer = _socket->readAll();

    int numPlace = _buffer.at(0) - 0x30;

    if ( _dispencer[numPlace].socket->socketDescriptor() != _socket->socketDescriptor() ) {
        _dispencer[numPlace].frame.clear();
        _dispencer[numPlace].state = S_START;
    }
    _dispencer[numPlace].socket = _socket;

    int indx = 0;
    switch ( _dispencer[numPlace].state ) {
        case S_START: {
            for ( int i = 1; i < _buffer.length(); ++i ) {
                if ( _buffer.at(i - 1) == _jpg[0] && _buffer.at(i) == _jpg[1] ) {
                    if ( i > 6 ) {
                        _dispencer[numPlace].place	= _buffer.at(i - 7) - 0x30;
                        _dispencer[numPlace].cam	= _buffer.at(i - 6);

                        char c_to_ui[2] = { _buffer.at(i - 2), _buffer.at(i - 3) };
                        _dispencer[numPlace].size = *(uint16_t *) &c_to_ui;
                    }
                    qDebug() << "Message: get frame from place #" << _dispencer[numPlace].place << " pack #" << _dispencer[numPlace].cam;
                    emit talking( QString("get frame from place #%1, pack #%2").arg(_dispencer[numPlace].place).arg(_dispencer[numPlace].cam) );
                    indx = i;
                }
            }

            if ( indx ) {
                _dispencer[numPlace].frame.clear();
                _dispencer[numPlace].frame	= _buffer.mid(indx - 1, -1);
                _dispencer[numPlace].state	= S_READ;
            }
        } break;

        case S_READ: {
            _buffer.remove(0, 1);
            for ( qint32 i = 1; i < _buffer.length(); ++i ) {
                if ( (_buffer.at(i - 1) == _jpg[0]) && (_buffer.at(i) == _jpg[2]) ) {
                    qDebug() << "Debug: get frame";
                    indx = i;
                }
            }
            if ( !indx ) {
                _dispencer[numPlace].frame.append(_buffer);
            } else {
                _dispencer[numPlace].frame.append( _buffer.left(indx + 1) );

                for ( qint32 i = 1; i <  _dispencer[numPlace].frame.length(); ++i ) {
                    if (  _dispencer[numPlace].frame.at(i - 1) == _jpg[0] &&  _dispencer[numPlace].frame.at(i) == _jpg[3] )
                        _dispencer[numPlace].frame.replace(i - 1, 2, _replace, 1);

                    if (  _dispencer[numPlace].frame.at(i - 1) == _jpg[0] &&  _dispencer[numPlace].frame.at(i) == _jpg[4] )
                        _dispencer[numPlace].frame.replace(i - 1, 2, _replace + 1, 1);
                }

                qDebug() << "Message: frame recieve full";
                emit talking( QString("frame recieve end") );

                if ( _cropperFree ) {
                    emit imgComplete(_dispencer[numPlace].frame,  _dispencer[numPlace].cam,  _dispencer[numPlace].place);
                    _cropperFree = false;
                }

                _dispencer[numPlace].state = S_START;

                if ( _changeState ) {
                    _dispencer[numPlace].socket->write(&_thisCam);
                    _changeState = false;
                }
            }
        } break;
    }
}   // My_TCPSocket::readyRead

void My_TCPSocket::sendToSocket(int val)
{
    _changeState	= true;
    _thisCam		= val;
}

void My_TCPSocket::WaitCropper()
{
    _cropperFree = true;
}

void My_TCPSocket::initStruct()
{
    for ( int i = 0; i < 6; ++i ) { // init dispencer stuct
        _dispencer[i].frame.clear();
        _dispencer[i].socket	= new QTcpSocket;
        _dispencer[i].state		= S_START;
    }

    _jpg[0]	= static_cast<char>(0xff);  // identificator jpg frame + replace symbols
    _jpg[1]	= static_cast<char>(0xd8);
    _jpg[2]	= static_cast<char>(0xd9);
    _jpg[3]	= static_cast<char>(0xac);
    _jpg[4]	= static_cast<char>(0xdc);

    _replace[0] = 0x0a;
    _replace[1] = 0x0d;

    _changeState	= false;
    _cropperFree	= true;
}
