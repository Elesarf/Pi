
#include "protocol.h"
#include <QCoreApplication>
#include <QDebug>
#include <qendian.h>

#define RF24_LINUX

Protocol::Protocol(QObject* parent)
  : QObject(parent)
  , watchDogOnRecieve(this)
  , watchDogOnPlace(this)
  , state(S_INIT)
  , radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ)

{

  connect(&watchDogOnRecieve, &QTimer::timeout, [this]() {

    StopRF();
    InitRF();

  });

  connect(&watchDogOnPlace, &QTimer::timeout, [this]() {

    Protocol::ReadyRead();

  });

  watchDogOnPlace.setSingleShot(true);
  watchDogOnRecieve.setSingleShot(true);
  __camwatcher = 0;
}

void
Protocol::ReadyRead()
{

  if (radio.available()) {

    switch (state) {

      case S_INIT: {

        radio.read(((char*)(&descriptor)), sizeof(descriptor));

        if (descriptor.id == 0x01) {

          watchDogOnPlace.stop();
          watchDogOnRecieve.stop();
          watchDogOnRecieve.start(15000);

          memset(&buffer, 0, sizeof(Protocol::buffer));
          data.clear();

          for (uint8_t i = 0; i < sizeof(descriptor.data); ++i) {
            data += descriptor.data[i];
          }

          qDebug() << "Start recieve JPG";

          state = S_READ_DATA;

          if ((descriptor.data[0] != 0xff) && (descriptor.data[1] != 0xd8)) {

            qDebug() << "Error recieve picture";
            state = S_INIT;
          }

          break;
        }
        break;
      }


      case S_READ_DATA: {

        radio.read(((char*)&buffer), 32);

        uint8_t end = 0;

        if (buffer.id == 0x00) {
          for (uint8_t i = 0; i < sizeof(buffer.data); ++i) {
            data += buffer.data[i];
          }
          break;

        } else if (buffer.id == 0x02) {

          for (uint8_t i = 0; i < sizeof(buffer.data); ++i) {

            if (!(buffer.data[i] == 0xff && buffer.data[i + 1] == 0xd9)) {

              data += buffer.data[i];
              end = i;

            } else {

              data += buffer.data[end + 1];
              data += buffer.data[end + 2];

              qDebug() << "Recieve picture ok ";

              emit EndOfRecive(data.size(), descriptor.numPlace, descriptor.numCam);
              emit GoToCrop(data, descriptor.numCam, descriptor.numPlace);

              memset(&buffer, 0, sizeof(Protocol::buffer));

              __camwatcher += descriptor.numCam - 0x30;

              watchDogOnRecieve.stop();

              if (__camwatcher >= 5) {

                data.clear();
                __camwatcher = 0;
                state = S_INIT;
                watchDogOnRecieve.start(4000);

              } else {

                watchDogOnRecieve.start(11000);
              }

              data.clear();
              state = S_INIT;

              break;
            }
          }
        }

        break;
      }
    }
  }
  watchDogOnPlace.start(0);
}

bool
Protocol::InitRF()
{

//  if (placeNumber < 102 && placeNumber >= 100) {
//    placeNumber += 2;
//  }

//  else {
    placeNumber = 100;
//  }

  radio.setChannel(placeNumber);   // Chanel #add
  radio.setPALevel(RF24_PA_MAX);   // Power
  radio.setDataRate(RF24_1MBPS);   // Speed =1Mb
  radio.setCRCLength(RF24_CRC_16); // Use 16-bit CRC

  radio.openReadingPipe(0, 0x0130303037LL); // Listening address 100007 in HEX
  radio.startListening();

  qDebug() << QString("Init RF on address #%1 ....Ok").arg(placeNumber);

  watchDogOnRecieve.start(10000);
  watchDogOnPlace.start(0);

  return true;
}

bool
Protocol::StopRF()
{

  radio.stopListening();
  radio.closeReadingPipe(0);
  radio.powerDown();

  memset(&buffer, 0, sizeof(Protocol::buffer));
  data.clear();

  delay(1000);
  radio.powerUp();
  delay(1000);

  return true;
}

void
Protocol::Start()
{

  if (!radio.begin()) {

    qDebug() << "Error: init RF24 fail"; // Begin crash
  }

  InitRF();
}
