#ifndef OPTIONSCONTROLLER_H
#define OPTIONSCONTROLLER_H

class ArcGISCompassController;

#include "AbstractTool.h"
#include "ArcGISCompassController.h"
#include <QObject>

class OptionsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

//  Q_PROPERTY(bool compassControllerReady READ compassControllerReady NOTIFY compassControllerReadyChanged)
//  Q_PROPERTY(bool compassVisible READ compassVisible WRITE setCompassVisible NOTIFY compassVisibleChanged)

public:
  explicit OptionsController(QObject* parent = nullptr);
  ~OptionsController();

  QString toolName() const override;
//  bool compassControllerReady();
//  bool compassVisible();
//  void setCompassVisible(bool visible);

signals:
//  void compassControllerReady();
//  void compassVisible();

private:
  void getUpdatedTools();

  //Esri::ArcGISRuntime::Toolkit::ArcGISCompassController* m_compassController = nullptr;
};

#endif // OPTIONSCONTROLLER_H
