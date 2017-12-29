// Copyright 2016 ESRI
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

#ifndef DSACONTROLLER_H
#define DSACONTROLLER_H

#include <QObject>

#include <QAbstractListModel>
#include <QUrl>
#include <QVariantMap>
#include <QFile>

namespace Esri
{
namespace ArcGISRuntime
{
  class Error;
  class Scene;
  class GeoView;
}
}

class DsaController : public QObject
{
  Q_OBJECT

public:
  DsaController(QObject* parent = nullptr);
  ~DsaController();

  Esri::ArcGISRuntime::Scene* scene() const;

  void init(Esri::ArcGISRuntime::GeoView* geoView);

public slots:
  void onError(const Esri::ArcGISRuntime::Error& error);

private slots:
  void onPropertyChanged(const QString& propertyName, const QVariant& propertyValue);

signals:
  void showErrorMessage(const QString& message, const QString& additionalMessage);

private:
  void setupConfig();
  void createDefaultSettings();
  void saveSettings(QFile& configFile);

private:
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;

  QString m_dataPath;
  QVariantMap m_dsaSettings;
  QString m_configFilePath;
};

#endif // DSACONTROLLER_H
