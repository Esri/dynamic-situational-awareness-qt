#ifndef OPTIONSCONTROLLER_H
#define OPTIONSCONTROLLER_H

class LocationController;

#include "AbstractTool.h"

using namespace Esri::ArcGISRuntime;

class OptionsController : public Toolkit::AbstractTool
{
  Q_OBJECT

  explicit OptionsController(QObject* parent = nullptr);
  ~OptionsController();

  Q_PROPERTY(bool locationControllerReady READ locationControllerReady NOTIFY locationControllerReadyChanged)
  Q_PROPERTY(bool simulateLocation READ simulateLocation WRITE setSimulateLocation NOTIFY simulateLocationChanged)
  Q_PROPERTY(QString simulationFile READ simulationFile WRITE setSimulationFile NOTIFY simulationFileChanged)

public:
  QString toolName() const override;
  bool locationControllerReady();
  bool simulateLocation();
  void setSimulateLocation(bool simulate);
  QString simulationFile() const;
  void setSimulationFile(const QString& file);

signals:
  void locationControllerReadyChanged();
  void simulateLocationChanged();
  void simulationFileChanged();

private:
  void getUpdatedTools();

  LocationController* m_locationController = nullptr;
};

#endif // OPTIONSCONTROLLER_H
