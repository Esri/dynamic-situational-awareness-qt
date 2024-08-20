#ifndef DYNAMICENTITYALERTSOURCE_H
#define DYNAMICENTITYALERTSOURCE_H

// dsa app headers
#include "AlertSource.h"

namespace Esri::ArcGISRuntime {
class DynamicEntity;
class Point;
}

namespace Dsa {

class MessagesOverlay;

class DynamicEntityAlertSource : public AlertSource
{
  Q_OBJECT

public:
  explicit DynamicEntityAlertSource(Esri::ArcGISRuntime::DynamicEntity* dynamicEntity, Dsa::MessagesOverlay* messagesOverlay);
  ~DynamicEntityAlertSource();

  Esri::ArcGISRuntime::Point location() const override;
  QVariant value(const QString& key) const override;

  void setSelected(bool selected) override;

private:
  Esri::ArcGISRuntime::DynamicEntity* m_dynamicEntity = nullptr;
  Dsa::MessagesOverlay* m_messagesOverlay = nullptr;
};

} // Dsa

#endif // DYNAMICENTITYALERTSOURCE_H
