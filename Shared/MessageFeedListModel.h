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

#ifndef MESSAGEFEEDLISTMODEL_H
#define MESSAGEFEEDLISTMODEL_H

#include <QAbstractListModel>

class MessagesOverlay;
class MessageFeed;

class MessageFeedListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum MessageFeedRoles
  {
    MessageFeedNameRole = Qt::DisplayRole,
    MessageFeedTypeRole = Qt::UserRole + 1,
    MessageFeedEnabledRole
  };

  explicit MessageFeedListModel(QObject* parent = nullptr);
  ~MessageFeedListModel() = default;

  bool isEmpty() const;

  void append(MessageFeed* messageFeed);

  MessageFeed* at(int index) const;

  MessageFeed* messageFeedByType(const QString& type) const;

  void clear();

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  Q_DISABLE_COPY(MessageFeedListModel)

  void setupRoles();

  QHash<int, QByteArray> m_roles;
  QStringList m_messageTypes;
  QList<MessageFeed*> m_messageFeeds;
};

#endif // MESSAGEFEEDLISTMODEL_H
