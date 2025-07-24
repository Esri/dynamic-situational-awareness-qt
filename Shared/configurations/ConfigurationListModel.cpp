/*******************************************************************************
 *  Copyright 2012-2025 Esri
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

#include "pch.hpp"

#include "ConfigurationListModel.h"

namespace Dsa {

ConfigurationListModel::ConfigurationListModel(QObject* parent):
  QAbstractListModel(parent)
{
}

void ConfigurationListModel::select(int index)
{
  if (index < 0 || index >= m_configurations.size())
    return;

  int loc = 0;
  for (auto& configuration : m_configurations)
  {
    configuration.setSelected(index == loc);
    auto idx = createIndex(loc, 0);
    emit dataChanged(idx, idx);
    loc++;
  }
}

void ConfigurationListModel::cancel(int index)
{
  if (index < 0 || index >= m_configurations.size())
    return;

  auto& configuration = m_configurations[index];
  configuration.cancelDownload();
  auto idx = createIndex(index, 0);
  emit dataChanged(idx, idx);
}

Configuration ConfigurationListModel::at(int index) const
{
  if (index < 0 || index >= m_configurations.size())
    return Configuration{};

  return m_configurations[index];
}

Configuration ConfigurationListModel::byName(const QString& name) const
{
  for (qsizetype i = 0; i < m_configurations.count(); i++)
  {
    if (m_configurations[i].name() == name)
      return m_configurations[i];
  }
  return Configuration{};
}

int ConfigurationListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_configurations.count();
}

QVariant ConfigurationListModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant{};

  const auto& configuration = m_configurations.at(index.row());
  switch (role)
  {
    case Roles::Name:
      return configuration.name();
    case Roles::Url:
      return configuration.urlStr();
    case Roles::Downloaded:
      return configuration.downloaded();
    case Roles::Selected:
      return configuration.selected();
    case Roles::Loaded:
      return configuration.loaded();
    case Roles::PercentDownloaded:
      return configuration.percentDownloaded();
    case Roles::Downloading:
      return configuration.downloading();
    case Roles::RequiresRestart:
      return configuration.requiresRestart();
    case Roles::CanDownload:
      return configuration.canDownload();
    case Roles::CanDelete:
      return configuration.canDelete();
    case Roles::DownloadCancelled:
      return configuration.downloadCancelled();
    case Roles::IsCancellable:
      return configuration.isCancellable();
    case Roles::Extracting:
      return configuration.extracting();
    case Roles::Extracted:
      return configuration.extracted();
    case Roles::PercentExtracted:
      return configuration.percentExtracted();
    case Roles::InProgress:
      return configuration.inProgress();
    case Roles::PercentComplete:
      return configuration.percentComplete();
  }

  return QVariant{};
}

QVariant ConfigurationListModel::dataByName(const QString& name, int role)
{
  for (qsizetype i = 0; i < m_configurations.count(); i++)
  {
    if (m_configurations[i].name() == name)
    {
      return data(createIndex(i, 0), role);
    }
  }
  return QVariant{};
}

bool ConfigurationListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || value.isNull())
    return false;

  auto& configuration = m_configurations[index.row()];

  switch (role)
  {
    case Roles::Name:
      configuration.setName(value.toString());
      break;
    case Roles::Url:
      configuration.setUrl(value.toString());
      break;
    case Roles::Selected:
      configuration.setSelected(value.toBool());
      break;
    case Roles::PercentDownloaded:
      configuration.setPercentDownloaded(value.toInt());
      break;
    case Roles::PercentExtracted:
      configuration.setPercentExtracted(value.toInt());
      break;
  }

  emit dataChanged(index, index);
  return true;
}

bool ConfigurationListModel::setDataByName(const QString& name, const QVariant& value, int role)
{
  for (qsizetype i = 0; i < m_configurations.count(); i++)
  {
    if (m_configurations[i].name() == name)
    {
      setData(createIndex(i, 0), value, role);
      return true;
    }
  }
  return false;
}

bool ConfigurationListModel::add(const QString& name, const QString& url, bool selected, bool loaded, int percentDownloaded)
{
  beginInsertRows(QModelIndex{}, m_configurations.count(), m_configurations.count());
  m_configurations.emplaceBack(name, url, selected, loaded, percentDownloaded);
  endInsertRows();
  return true;
}

bool ConfigurationListModel::remove(int index)
{
  beginRemoveRows(QModelIndex{}, index, index);
  m_configurations.removeAt(index);
  endRemoveRows();
  return true;
}

void ConfigurationListModel::download(const QString& configurationName)
{
  for (qsizetype i = 0; i < m_configurations.count(); i++)
  {
    if (m_configurations[i].name() == configurationName)
    {
      m_configurations[i].download();
      const auto idx = createIndex(i, 0);
      emit dataChanged(idx, idx);
      return;
    }
  }
}

bool ConfigurationListModel::clear()
{
  beginRemoveRows(QModelIndex{}, 0, m_configurations.count());
  m_configurations.clear();
  m_configurations.squeeze();
  endRemoveRows();
  return true;
}

QHash<int, QByteArray> ConfigurationListModel::roleNames() const
{
  static const QHash<int, QByteArray> roles = {
    { Name, "Name" },
    { Url, "Url" },
    { Downloaded, "Downloaded" },
    { Selected, "Selected" },
    { Loaded, "Loaded" },
    { PercentDownloaded, "PercentDownloaded" },
    { Downloading, "Downloading" },
    { RequiresRestart, "RequiresRestart" },
    { CanDownload, "CanDownload" },
    { CanDelete, "CanDelete" },
    { DownloadCancelled, "DownloadCancelled" },
    { IsCancellable, "IsCancellable" },
    { Extracting, "Extracting" },
    { Extracted, "Extracted" },
    { PercentExtracted, "PercentExtracted" },
    { InProgress, "InProgress" },
    { PercentComplete, "PercentComplete" }
  };

  return roles;
}

} // Dsa
