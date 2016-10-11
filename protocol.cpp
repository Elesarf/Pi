#include "protocol.h"
#include <RF24.h>
#include <QDebug>
#include <qendian.h>
#include <QApplication>
#include <QThread>

#define RF24_LINUX

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

Protocol::Protocol(QObject *parent)
    : QObject( parent )
    , watchDogOnRecieve(this)
    , watchDogOnPlace(this)
    , state(S_INIT)
{

    connect( &watchDogOnRecieve, &QTimer::timeout, [this](){

        qDebug() << "INit Thread " << QThread::currentThreadId();

        InitRF();

    });

    connect( &watchDogOnPlace, &QTimer::timeout, [this](){

            //qDebug() << "Error read place #" << descriptor.numPlace;


            Protocol::ReadyRead();


    });

    watchDogOnPlace.setSingleShot(true);
    watchDogOnRecieve.setSingleShot(true);

    watchDogOnRecieve.start(8000);
    watchDogOnPlace.start(1);

}

void Protocol::ReadyRead(){

    if ( radio.available() ){

        switch (state) {

        case S_INIT:{

            radio.read((( char * ) ( &descriptor )), sizeof( descriptor ));

            if( descriptor.id == 0x01 ){

                data.clear();
                watchDogOnPlace.stop();
                watchDogOnRecieve.stop();

                memset ( &buffer, 0, sizeof( Protocol::buffer ));

                for ( uint8_t i = 0; i < sizeof( descriptor.data ); ++i ){
                    data += descriptor.data[i];
                }

                qDebug() << "Start recieve JPG";

                state = S_READ_DATA;

                if(( descriptor.data[0] != 0xff ) && ( descriptor.data[1] != 0xd8 )){

                        qDebug() << "Error steram not JPG";
                        state = S_INIT;

                    }

                break;
            }
        }

        case S_READ_DATA :{

            radio.read(((char *) &buffer), 32);

            uint8_t end=0;

                if( buffer.id == 0x00 ){
                    for (uint8_t i = 0; i < sizeof( buffer.data ); ++i){
                        data += buffer.data[i];
                    }
                    break;

                } else if (buffer.id == 0x02){

                    for ( uint8_t i=0; i < sizeof( buffer.data ); ++i ){
                        if ( !( buffer.data[i] == 0xff && buffer.data[i+1] == 0xd9 )){
                            data += buffer.data[i];
                            end = i;
                        } else {

                            data += buffer.data[end + 1];
                            data += buffer.data[end + 2];
                            qDebug() << "Stop recieve JPG";

                            emit EndOfRecive( data, descriptor );
                            emit GoToCrop( data, descriptor.numCam, descriptor.numPlace );

                            state = S_INIT;

                            if( descriptor.numCam == 0x32 ){
                                watchDogOnRecieve.start( 0 );
                                }

                            break;
                        }

                    }

                }

            break;
        }

        default:
            break;
        }
    }
    watchDogOnPlace.start(0);
}

bool Protocol::InitRF(){


        if(placeNumber < 102 && placeNumber >= 100 ){
                placeNumber += 2;
            }

        else{
                placeNumber = 100;
            }

        if( !radio.begin() ){
            qDebug() << "Error init RF24";         // Setup and configure rf radio
            return false;
        }

        radio.setChannel(placeNumber);                   // Chanel #add
        radio.setPALevel(RF24_PA_MAX);           // Power
        radio.setDataRate(RF24_1MBPS);           // Speed =1Mb
        radio.setCRCLength(RF24_CRC_16);         // Use 16-bit CRC
        //radio.printDetails();

        radio.openReadingPipe( 0 , 0x3030303037LL ); // Listening address 000007 in HEX
        radio.startListening();
        qDebug() << QString( "Init RF on address #%1" ).arg( placeNumber );
        qDebug() << QString( "Init RF on address #%1 ....Ok" ).arg( placeNumber );

        watchDogOnRecieve.start(8000);
        watchDogOnPlace.start(0);

        return true;
}

bool Protocol::InitRF( uint8_t add){

        add++;

        return true;

}

bool Protocol::StopRF(){

    radio.stopListening();
    radio.powerDown();
    delay(1000);
    radio.powerUp();
    delay(1000);

    return true;
}

void Protocol::Start(){

    InitRF();

}












































