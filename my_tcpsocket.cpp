#include "my_tcpsocket.h"
#include <QDebug>

My_TCPSocket::My_TCPSocket(QObject* parent) : QObject(parent) {
  _server = new QTcpServer(this);
  _state = S_START;

  fillSign();

  qDebug() << "Message: server up, listen. "
           << _server->listen(QHostAddress::AnyIPv4, 1235);

  connect(_server, SIGNAL(newConnection()), this, SLOT(incommingConnect()));
}

My_TCPSocket::My_TCPSocket(quint16 port) {
  _server = new QTcpServer(this);
  _state = S_START;

  fillSign();

  qDebug() << "Message: server up, listen. "
           << _server->listen(QHostAddress::AnyIPv4, port);
  emit talking("server up, listen");

  connect(_server, SIGNAL(newConnection()), this, SLOT(incommingConnect()));
}

void My_TCPSocket::incommingConnect() {
  QTcpSocket* _socket = _server->nextPendingConnection();
  connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
          SLOT(stateChanged(QAbstractSocket::SocketState)));
  if (!_firstSocket) {
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    _socket->write("server");
    _firstSocket = _socket;
    qDebug() << "Message: connect first socket #"
             << _socket->socketDescriptor();
    emit talking("connect");
    _buffer.clear();
    _frame.clear();
  } else {
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    _socket->write("client");
    _sockets << _socket;
    qDebug() << "Message: connect socket #" << _socket->socketDescriptor()
             << " size: " << _sockets.size()
             << " lenght: " << _sockets.length();
    emit talking("connect");
    _buffer.clear();
    _frame.clear();
  }
}

void My_TCPSocket::readyRead() {
  QObject* object = QObject::sender();
  if (!object)
    qDebug() << "Warning: something wrong";
  QTcpSocket* _socket = static_cast<QTcpSocket*>(object);
  _buffer = _socket->readAll();
  qDebug() << _socket->socketDescriptor() << " " << _buffer.length();
  int indx = 0;
  switch (_state) {
    case S_START: {
      for (int i = 1; i < _buffer.length(); ++i) {
        if (_buffer.at(i - 1) == _jpg[0] && _buffer.at(i) == _jpg[1]) {
          if (i > 5) {
            _place = _buffer.at(i - 7);
            _pack = _buffer.at(i - 6);
            char c_to_ui[2] = {_buffer.at(i - 2), _buffer.at(i - 3)};
            _size = *(uint16_t*)&c_to_ui;
          }

          qDebug() << "get frame";
          qDebug() << "place #" << _place << " pack #" << _pack;
          emit talking(QString("get frame from place #%1, pack #%2")
                           .arg(_place)
                           .arg(_pack));
          indx = i;
        }
      }
      if (indx) {
        _frame.clear();
        _frame = _buffer.mid(indx - 1, -1);
        _state = S_READ;
      }
    } break;

    case S_READ: {
      for (qint32 i = 1; i < _buffer.length(); ++i) {
        if ((_buffer.at(i - 1) == _jpg[0]) && (_buffer.at(i) == _jpg[2])) {
          qDebug() << "get frame";
          indx = i;
        }
      }
      if (!indx) {
        _frame.append(_buffer);
      } else {
        _frame.append(_buffer.left(indx + 1));

        for (qint32 i = 1; i < _frame.length(); ++i) {
          if (_frame.at(i - 1) == _jpg[0] && _frame.at(i) == _jpg[3])
            _frame.replace(i - 1, 2, _replace, 1);

          if (_frame.at(i - 1) == _jpg[0] && _frame.at(i) == _jpg[4])
            _frame.replace(i - 1, 2, _replace + 1, 1);
        }

        qDebug() << "frame recieve full";
        emit talking(QString("frame recieve end"));
        emit imgComplete(_frame, _place, static_cast<qint8>(_pack - 0x30));
        _state = S_START;
      }
    } break;
  }
}

void My_TCPSocket::stateChanged(QAbstractSocket::SocketState stat) {
  qDebug() << "Message: state change " << stat;
  QObject* object = QObject::sender();
  if (!object)
    return;
  QTcpSocket* socket = static_cast<QTcpSocket*>(object);
  qDebug() << _state;
  if (socket == _firstSocket && _state == QAbstractSocket::UnconnectedState)
    _firstSocket = NULL;
}

void My_TCPSocket::fillSign() {
  _jpg[0] = static_cast<char>(0xff);
  _jpg[1] = static_cast<char>(0xd8);
  _jpg[2] = static_cast<char>(0xd9);
  _jpg[3] = static_cast<char>(0xac);
  _jpg[4] = static_cast<char>(0xdc);
  _replace[0] = 0x0a;
  _replace[1] = 0x0d;
}
