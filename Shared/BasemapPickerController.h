/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

#ifndef BASEMAPPICKERCONTROLLER_H
#define BASEMAPPICKERCONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>
#include <QObject>

namespace Esri {
namespace ArcGISRuntime {
  class Basemap;
}
}

class QStringListModel;

namespace Dsa {

class TileCacheListModel;

class BasemapPickerController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* tileCacheModel READ tileCacheModel NOTIFY tileCacheModelChanged)

public:
  static const QString DEFAULT_BASEMAP_PROPERTYNAME;
  static const QString BASEMAP_DIRECTORY_PROPERTYNAME;

  explicit BasemapPickerController(QObject* parent = nullptr);
  ~BasemapPickerController();

  QAbstractListModel* tileCacheModel() const;

  Q_INVOKABLE void basemapSelected(int row);
  Q_INVOKABLE void selectInitialBasemap();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  QString basemapDataPath() const { return m_basemapDataPath; }
  void setBasemapDataPath(const QString& dataPath);
  QString defaultBasemap() const { return m_defaultBasemap; }
  void setDefaultBasemap(const QString& defaultBasemap);

public slots:
  void onBasemapDataPathChanged();

signals:
  void tileCacheModelChanged();
  void basemapsDataPathChanged();
  void basemapChanged(Esri::ArcGISRuntime::Basemap* basemap, QString name = "");
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

private:
  TileCacheListModel* m_tileCacheModel;
  int                 m_defaultBasemapIndex = 0;
  QString             m_basemapDataPath;
  QString             m_defaultBasemap = "topographic";

private:
  void findBasemaps();
};

} // Dsa

#endif // BASEMAPPICKERCONTROLLER_H
