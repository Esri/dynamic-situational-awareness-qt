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

#ifndef MARKUPUTILITY_H
#define MARKUPUTILITY_H

namespace Esri {
namespace ArcGISRuntime {
class GraphicsOverlay;
}
}

class QJsonObject;

#include "AbstractTool.h"

class MarkupUtility : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:
  explicit MarkupUtility(QObject* parent = nullptr);
  ~MarkupUtility();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  QJsonObject graphicsToJson(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay);
  QStringList colors() const;

private:
  static const QString ARROW;
  static const QString CENTER;
  static const QString COLOR;
  static const QString ELEMENTS;
  static const QString FILLED;
  static const QString GEOMETRY;
  static const QString MARKUP;
  static const QString NAME;
  static const QString SCALE;
  static const QString SHAREDBY;
  static const QString USERNAME_PROPERTYNAME;
  static const QString VERSION;
  static const QString VERSIONNUMBER;
  QString m_username;  
};

#endif // MARKUPUTILITY_H
