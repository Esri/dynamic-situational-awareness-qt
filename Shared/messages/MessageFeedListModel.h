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

#ifndef MESSAGEFEEDLISTMODEL_H
#define MESSAGEFEEDLISTMODEL_H

// Qt headers
#include <QAbstractListModel>

namespace Dsa {

class MessageFeed;

class MessageFeedListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum MessageFeedRoles
  {
    MessageFeedNameRole = Qt::DisplayRole,
    MessageFeedTypeRole = Qt::UserRole + 1,
    MessageFeedVisibleRole,
    MessageFeedThumbnailUrlRole
  };

  explicit MessageFeedListModel(QObject* parent = nullptr);
  ~MessageFeedListModel() = default;

  bool isEmpty() const;

  void append(MessageFeed* messageFeed);

  MessageFeed* at(int index) const;

  MessageFeed* messageFeedByType(const QString& type) const;

  void clear();

  int count() const;

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

} // Dsa

#endif // MESSAGEFEEDLISTMODEL_H
