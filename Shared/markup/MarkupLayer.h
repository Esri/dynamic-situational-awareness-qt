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

#ifndef MARKUPLAYER_H
#define MARKUPLAYER_H

// C++ API headers
#include "FeatureCollectionLayer.h"
#include "JsonSerializable.h"

// Qt headers
#include <QHash>

namespace Esri::ArcGISRuntime {
class FeatureCollection;
class GraphicsOverlay;
class SimpleLineSymbol;
class Feature;
}

namespace Dsa {

class MarkupLayer : public Esri::ArcGISRuntime::FeatureCollectionLayer,
                    public Esri::ArcGISRuntime::JsonSerializable
{
  Q_OBJECT

public:
  ~MarkupLayer();

  // helper create methods
  static MarkupLayer* createFromPath(const QString& path, QObject* parent = nullptr);
  static MarkupLayer* createFromGraphics(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay, const QString& authorName, QObject* parent = nullptr);

  static QStringList colors();

  void setPath(const QString& path);
  QString path() const;
  Esri::ArcGISRuntime::FeatureCollection* featureCollection();
  QString author() const;

  // JSON Serializable
  static MarkupLayer* fromJson(const QString& json, QObject* parent = nullptr);
  QString toJson() const override;
  QJsonObject unknownJson() const override;
  QJsonObject unsupportedJson() const override;

private:
  MarkupLayer(const QString& json, Esri::ArcGISRuntime::FeatureCollection* featureCollection, QObject* parent = nullptr);

  QString m_path;
  QString m_json;
  QString m_author;
  Esri::ArcGISRuntime::FeatureCollection* m_featureCollection = nullptr;
  QHash<QUuid, QPair<Esri::ArcGISRuntime::Feature*, Esri::ArcGISRuntime::SimpleLineSymbol*>> m_featureHash;
};

} // Dsa

#endif // MARKUPLAYER_H
