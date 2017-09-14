// Copyright 2016 ESRI
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

#ifndef DATAITEMLISTMODEL_H
#define DATAITEMLISTMODEL_H

class DataItem;

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

class DataItemListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  //Q_PROPERTY(int size READ size NOTIFY sizeChanged)

  enum DataItemRoles
  {
    FullPathRole = Qt::UserRole + 1,
    FileNameRole,
    DataTypeRole
  };

  explicit DataItemListModel(QObject* parent = nullptr);
  ~DataItemListModel() = default;

  Q_INVOKABLE DataItem* get(int index) const { return m_dataItems.at(index); }
  void addDataItem(DataItem* dataItem);
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  int size() const { return m_dataItems.size(); }
  void setupRoles();
  void clear();

signals:
  void sizeChanged();

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  QHash<int, QByteArray> m_roles;
  QList<DataItem*> m_dataItems;
};

#endif // DATAITEMLISTMODEL_H
