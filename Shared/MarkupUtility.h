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
class QString;

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
  static const QString USERNAME_PROPERTYNAME;
  QString m_username;
  QString m_port;
};

#endif // MARKUPUTILITY_H
