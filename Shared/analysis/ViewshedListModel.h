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

#ifndef VIEWSHEDLISTMODEL_H
#define VIEWSHEDLISTMODEL_H

namespace Esri::ArcGISRuntime {
  class AnalysisOverlay;
}

class QAbstractListModel;

namespace Dsa {

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
  void viewshedRemoved(Dsa::Viewshed360* viewshed);

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  Q_DISABLE_COPY(ViewshedListModel)

  void setupRoles();

  QHash<int, QByteArray> m_roles;
  QList<Viewshed360*> m_viewsheds;
};

} // Dsa

#endif // VIEWSHEDLISTMODEL_H
