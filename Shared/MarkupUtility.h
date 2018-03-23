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

class MarkupUtility
{

public:
  static QJsonObject graphicsToJson(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay, const QString& name);

private:
};

#endif // MARKUPUTILITY_H
