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

#include "Configuration.h"

// DSA headers
#include "ConfigurationController.h"

// Qt headers
#include <QRegularExpression>

namespace Dsa {

Configuration::Configuration(const QString& name, const QString& url, bool selected, bool loaded, int percentDownloaded):
  m_name(name),
  m_urlStr(url),
  m_url(QUrl{url}),
  m_selected(selected),
  m_loaded(loaded),
  m_percentDownloaded(percentDownloaded),
  m_percentExtracted(percentDownloaded == 100 ? 100 : 0)
{
  static QRegularExpression regexPortalItem{ConfigurationController::REGEX_PORTAL_ITEM_URL};
  m_isCancellable = !regexPortalItem.match(url).hasMatch();
}

QString Configuration::name() const
{
  return m_name;
}

void Configuration::setName(const QString& name)
{
  m_name = name;
}

QString Configuration::urlStr() const
{
  return m_urlStr;
}

void Configuration::setUrl(const QString& url)
{
  m_urlStr = url;
  m_url = QUrl{url};
}

QUrl Configuration::url() const
{
  return m_url;
}

void Configuration::download()
{
  m_downloadCancelled = false;
  m_percentDownloaded = 0;
  m_percentExtracted = 0;
}

bool Configuration::downloaded() const
{
  return m_percentDownloaded == 100;
}

bool Configuration::downloading() const
{
  return m_percentDownloaded > 0 && m_percentDownloaded < 100;
}

bool Configuration::extracted() const
{
  return m_percentExtracted == 100;
}

bool Configuration::extracting() const
{
  return m_percentExtracted > 0 && m_percentExtracted < 100;
}

bool Configuration::requiresRestart() const
{
  return m_selected && downloaded() && !m_loaded;
}

bool Configuration::canDownload() const
{
  return !downloaded() && !downloading() && !m_urlStr.isEmpty();
}

bool Configuration::canDelete() const
{
  return !m_selected && !m_loaded && !downloading() && !extracting();
}

bool Configuration::isCancellable() const
{
  return m_isCancellable;
}

bool Configuration::downloadCancelled() const
{
  return m_downloadCancelled;
}

bool Configuration::selected() const
{
  return m_selected;
}

void Configuration::setSelected(bool selected)
{
  m_selected = selected;
}

bool Configuration::loaded() const
{
  return m_loaded;
}

int Configuration::percentDownloaded() const
{
  return m_percentDownloaded;
}

void Configuration::setPercentDownloaded(int percentDownloaded)
{
  m_percentDownloaded = percentDownloaded;
}

int Configuration::percentExtracted() const
{
  return m_percentExtracted;
}

void Configuration::setPercentExtracted(int percentExtracted)
{
  m_percentExtracted = percentExtracted;
}

void Configuration::cancelDownload()
{
  setPercentDownloaded(0);
  setPercentExtracted(0);
  m_downloadCancelled = true;
}

} // Dsa
