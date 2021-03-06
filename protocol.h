#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifndef QT_WARNING_DISABLE_DEPRECATED
#define QT_WARNING_DISABLE_DEPRECATED
#endif

#include <RF24.h>

#include <QObject>
#include <QTimer>

class Protocol : public QObject {
  Q_OBJECT

 public:
  enum State {
    S_INIT,
    S_READ_DATA,
  };

  explicit Protocol(QObject* parent = 0);

  struct Descriptor {
    char id;
    char numPlace;
    char numCam;
    char sizeJPG[2];
    char sizeJPG1[2];
    char data[25];
  };

  struct Buffer {
    char id;
    char data[31];
  };

 signals:

  void EndOfRecive(const int, const char, const char);
  void EndReadPlace(const uint8_t);
  void GoToCrop(QByteArray, const char, const qint8);

 public slots:

  bool InitRF();
  bool StopRF();
  void Start();

 private slots:

  void ReadyRead();

 private:
  QTimer watchDogOnRecieve;
  QTimer watchDogOnPlace;
  State state;
  RF24 radio;
  Descriptor descriptor;
  Buffer buffer;
  QByteArray data;
  uint8_t placeNumber;
  qint8 __camwatcher;
};

#endif  // PROTOCOL_H
