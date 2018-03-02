// Copyright 2018 ESRI
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

#ifndef VIEWSHEDLISTMODEL_H
#define VIEWSHEDLISTMODEL_H

#include <QAbstractListModel>

namespace Esri {
  namespace ArcGISRuntime {
    class AnalysisOverlay;
  }
}

class Viewshed360;

class ViewshedListModel : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
  enum ViewshedRoles
  {
    ViewshedNameRole = Qt::DisplayRole,
    ViewshedVisibleRole = Qt::UserRole + 1,
    ViewshedMinDistanceRole,
    ViewshedMaxDistanceRole,
    ViewshedHorizontalAngleRole,
    ViewshedVerticalAngleRole,
    ViewshedHeadingRole,
    ViewshedPitchRole,
    Viewshed360ModeRole
  };

  explicit ViewshedListModel(QObject* parent = nullptr);
  ~ViewshedListModel() = default;

  bool isEmpty() const;

  Q_INVOKABLE void append(Viewshed360* viewshed);

  Q_INVOKABLE Viewshed360* at(int index) const;

  Q_INVOKABLE int indexOf(Viewshed360* viewshed) const;

  Q_INVOKABLE bool removeOne(Viewshed360* viewshed);

  Q_INVOKABLE void removeAt(int index);

  Q_INVOKABLE void clear();

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

signals:
  void countChanged();
  void viewshedAdded(int index);
  void viewshedRemoved(Viewshed360* viewshed);

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  Q_DISABLE_COPY(ViewshedListModel)

  void setupRoles();

  QHash<int, QByteArray> m_roles;
  QList<Viewshed360*> m_viewsheds;
};

#endif // VIEWSHEDLISTMODEL_H
