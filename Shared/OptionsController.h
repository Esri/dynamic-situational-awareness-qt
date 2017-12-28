#ifndef OPTIONSCONTROLLER_H
#define OPTIONSCONTROLLER_H

class LocationController;

#include "AbstractTool.h"

using namespace Esri::ArcGISRuntime;

class OptionsController : public Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool locationControllerReady READ locationControllerReady NOTIFY locationControllerReadyChanged)
  Q_PROPERTY(bool simulateLocation READ simulateLocation WRITE setSimulateLocation NOTIFY simulateLocationChanged)

private:
  bool m_locationControllerReady = false;
  bool m_simulateLocation;
  LocationController* m_locationController = nullptr;
  void getUpdatedTools();

signals:
  void locationControllerReadyChanged();
  void simulateLocationChanged();

public:
  explicit OptionsController(QObject* parent = nullptr);
  ~OptionsController();

  QString toolName() const override;
  bool locationControllerReady();
  bool simulateLocation();
  void setSimulateLocation(bool simulate);
};

#endif // OPTIONSCONTROLLER_H
