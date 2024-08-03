#ifndef DYNAMICENTITYALERTSOURCE_H
#define DYNAMICENTITYALERTSOURCE_H

// dsa app headers
#include "AlertSource.h"

namespace Esri::ArcGISRuntime {
  class DynamicEntity;
}

namespace Dsa {

class DynamicEntityAlertSource : public AlertSource
{
  Q_OBJECT

public:
  explicit DynamicEntityAlertSource(Esri::ArcGISRuntime::DynamicEntity* dynamicEntity, Esri::ArcGISRuntime::DynamicEntityLayer* dynamicEntityLayer);
  ~DynamicEntityAlertSource();

  Esri::ArcGISRuntime::Point location() const override;
  QVariant value(const QString& key) const override;

  void setSelected(bool selected) override;

private:
  Esri::ArcGISRuntime::DynamicEntity* m_dynamicEntity = nullptr;
  Esri::ArcGISRuntime::DynamicEntityLayer* m_dynamicEntityLayer = nullptr;
};

} // Dsa

#endif // DYNAMICENTITYALERTSOURCE_H
