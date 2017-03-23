#include "sender.h"

#include <QNetworkReply>
#include <QDebug>
#include <QFile>

Sender::Sender(QObject* parent)
    : QObject(parent),
      __man(this),
      __sendTimer(this)

{
  __datetime = __datetime.addSecs(3600 * 3);  // local time
  __sz = 0;
  __timToTimer = 7900;
  __FillDispenser();
  __request.setUrl((QUrl(QString("https://kz0.vt-serv.com:500/?"))));
  __request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QString("application/x-www-form-urlencoded"));
  __sendTimer.setSingleShot(true);

  Sender::connect(&__sendTimer, &QTimer::timeout, [this]() {

    qDebug() << "Send on timer";
    Sender::IGetImage(0, 48, 48);

  });

  Sender::connect(&__man, &QNetworkAccessManager::finished,
                  [this](QNetworkReply* reply) {

                    reply->deleteLater();

                  });

  Sender::connect(&__man, &QNetworkAccessManager::sslErrors,
                  [this](QNetworkReply* reply, const QList<QSslError>& errors) {

                    reply->ignoreSslErrors(errors);

                  });

  __sendTimer.start(__timToTimer);
}

bool Sender::SendPlease(const qint8 place,
                        const qint8 pack,
                        const qint32 find,
                        int size,
                        qint8 matches) {
  Q_UNUSED(size);

  __sendTimer.stop();

  __dispenser[place][pack].place = place;
  __dispenser[place][pack].pack = pack;
  __dispenser[place][pack].timestamp =
      QString(__datetime.currentDateTime().toString("yyMMddHHmmss"));

  if ((__dispenser[place][pack].find == find) ||
      (__dispenser[place][pack].find == -1)) {
    __dispenser[place][pack].matches =
        __dispenser[place][pack].matches + matches;
    __dispenser[place][pack].find = find;

  } else if (!(__dispenser[place][pack].find == find)) {
    if (__dispenser[place][pack].matches >= matches) {
      __dispenser[place][pack].matches =
          __dispenser[place][pack].matches - matches;

    } else {
      __dispenser[place][pack].find = find;
      __dispenser[place][pack].matches = matches;
    }
  }

  if (find == 0) {
    __dispenser[place][pack].matches = __dispenser[place][pack].matches - 2;
  }

  QString magicString;

  for (qint8 packs = 1; packs < 13; ++packs) {
    if (__dispenser[place][packs].matches > 9) {  // 9 find points

      __dispenser[place][packs].matches = 9;
    }

    if (__dispenser[place][packs].matches < -5) {  // 9 find points

      __dispenser[place][packs].matches = -5;
    }
  }

  for (qint8 pl = 2; pl < 3; ++pl) {
    for (qint8 pk = 1; pk < 13; ++pk) {
      magicString.append(QString("%1;%2;%3;%4;%5|")

                             .arg(__dispenser[pl][pk].place)
                             .arg(pk)
                             .arg(__dispenser[pl][pk].find)
                             .arg(__dispenser[pl][pk].timestamp)
                             .arg(__dispenser[pl][pk].matches));
    }
  }

  if (__sz < 5) { //freq send messages
    ++__sz;
    qDebug() << magicString;

  } else {
    __buffer.append(magicString);
    qDebug() << magicString;

    __buffer.chop(1);

    //system("sudo echo '\n" + __buffer + "\r' > /dev/ttyAMA0");  // UART

    //UartChecker(__buffer);

    __buffer.prepend("key=pdz9uFCr&id=00006&brand=");
    __sendStr.append(__buffer);

    auto reply = __man.post(__request, __sendStr);
    Q_UNUSED(reply);

    __SaveToTxt(__buffer);

    qDebug() << "Send to server ";
    __sendStr.clear();
    __buffer.clear();
    __sz = 0;
    __sendTimer.start(__timToTimer);

    return true;
  }

  __sendTimer.start(__timToTimer);
  return false;
}

//////////////
/// \brief Sender::IGetImage
/// \param size
/// \param place
/// \param cam
/// \return
/// Just add to __buffer string, about get image: size, place/pack.
bool Sender::IGetImage(const int size, const char place, const char cam) {
  Q_UNUSED(place);

  __buffer.append(
      QString("%1;%2;%3;%4|")
          .arg(0)
          .arg(cam - 0x30)
          .arg(size)
          .arg(__datetime.currentDateTime().toString("yyMMddHHmmss")));

  return true;
}

///////////
/// \brief Sender::UartChecker
/// \param ba QByteArray &
/// \return bool
/// Read Uart log file and add to buffer.
bool Sender::UartChecker(QByteArray& ba) {
  QFile logUart;
  logUart.setFileName("/home/pi/shr/log1.txt");

  if (!logUart.open(QIODevice::ReadOnly)) {
    qDebug() << "Error: UART log file not open";
  } else {
    qDebug() << "Uart log file open, read";
    ba.append("|");
    ba.append(logUart.readAll());
    ba.append("|");

    qDebug() << "Read: " << logUart.readAll();
    logUart.close();
    return true;
  }
  return false;
}

////////////
/// \brief Sender::__FillDispenser
/// Fill structure dispenser start values.
void Sender::__FillDispenser() {
  for (qint8 places = 0; places < 3; ++places) {
    for (qint8 packs = 0; packs < 13; ++packs) {
      __dispenser[places][packs].place = places;
      __dispenser[places][packs].pack = packs;
      __dispenser[places][packs].find = -1;
      __dispenser[places][packs].matches = -1;
    }
  }
}

///////////
/// \brief Sender::__SaveToTxt
/// \param ba
/// Save log to txt file, if file not exist, create his. Saving to /mnt/smb
/// (mount how TMPFS).
void Sender::__SaveToTxt(const QByteArray& ba) {
  QFile fl;
  fl.setFileName("/home/pi/shr/log.txt");

  if (!fl.open(QIODevice::WriteOnly)) {
    qDebug() << "Error: log file not open";
  } else {
    qDebug() << "Log file open, saving";
    fl.write(ba);
    fl.close();
  }
}
