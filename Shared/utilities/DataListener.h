// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef DATALISTENER_H
#define DATALISTENER_H


// Qt headers
#include <QIODevice>
#include <QObject>
#include <QPointer>

namespace Dsa {
namespace Utilities {

class DataListener : public QObject
{
  Q_OBJECT

public:
  explicit DataListener(QObject* parent = nullptr);
  explicit DataListener(QIODevice* device, QObject* parent = nullptr);
  ~DataListener();

  void setDevice(QIODevice* device);
  QIODevice* device() const;

  bool isEnabled() const;
  void setEnabled(bool enabled);

signals:
  void dataReceived(const QByteArray& data);

private:
  Q_DISABLE_COPY(DataListener)

  void connectDevice();
  void disconnectDevice();

  bool processUdpDatagrams();

  QPointer<QIODevice> m_device;
  QMetaObject::Connection m_deviceConn;

  bool m_enabled = true;
};

} // Utilities
} // Dsa

#endif // DATALISTENER_H
