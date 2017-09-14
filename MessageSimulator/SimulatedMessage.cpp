#include "SimulatedMessage.h"

#include <QXmlStreamReader>

static const QString s_cotElementName{"event"};
static const QString s_cotTypeName{"type"};
static const QString s_cotUidName{"uid"};

SimulatedMessage::SimulatedMessage(QObject* parent) :
  QObject(parent)
{
}

SimulatedMessage::SimulatedMessage(MessageFormat messageFormat, const QString& messageId, const QString& messageAction, const QString& symbolId, QObject* parent) :
  QObject(parent),
  m_messageFormat(messageFormat),
  m_messageId(messageId),
  m_messageAction(messageAction),
  m_symbolId(symbolId)
{
}

SimulatedMessage::~SimulatedMessage()
{
}

SimulatedMessage* SimulatedMessage::createFromCoTMessage(const QByteArray& message, QObject* parent)
{
  QXmlStreamReader reader(message);

  QString messageId;
  QString symbolId;

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // CoT event
      if (QStringRef::compare(reader.name(), s_cotElementName) == 0)
      {
        const auto attrs = reader.attributes();
        symbolId = attrs.value(s_cotTypeName).toString();
        messageId = attrs.value(s_cotUidName).toString();

        if (symbolId.isEmpty() || messageId.isEmpty())
          return nullptr;
      }
    }

    reader.readNext();
  }

  return new SimulatedMessage(MessageFormat::CoT, messageId, QStringLiteral("update"), symbolId, parent);
}

SimulatedMessage::MessageFormat SimulatedMessage::messageFormat() const
{
  return m_messageFormat;
}

void SimulatedMessage::setMessageFormat(MessageFormat messageFormat)
{
  m_messageFormat = messageFormat;
}

QString SimulatedMessage::messageFormatString() const
{
  switch (m_messageFormat)
  {
  case CoT:
    return "CoT";
  default:
    return QString();
  }

  return QString();
}

QString SimulatedMessage::messageId() const
{
  return m_messageId;
}

void SimulatedMessage::setMessageId(const QString& messageId)
{
  m_messageId = messageId;
}

QString SimulatedMessage::messageAction() const
{
  return m_messageAction;
}

void SimulatedMessage::setMessageAction(const QString& messageAction)
{
  m_messageAction = messageAction;
}

QString SimulatedMessage::symbolId() const
{
  return m_symbolId;
}

void SimulatedMessage::setSymbolId(const QString& symbolId)
{
  m_symbolId = symbolId;
}
