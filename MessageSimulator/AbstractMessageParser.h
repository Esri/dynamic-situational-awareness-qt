#ifndef ABSTRACTMESSAGEPARSER_H
#define ABSTRACTMESSAGEPARSER_H

#include <QObject>

class AbstractMessageParser : public QObject
{
  Q_OBJECT

public:
  ~AbstractMessageParser();

  virtual QByteArray nextMessage() = 0;

  virtual void reset() = 0;

  virtual bool atEnd() const = 0;

  QString filePath() const;

signals:
  void errorOccurred(const QString& error);

protected:
  explicit AbstractMessageParser(const QString& filePath, QObject* parent = nullptr);

private:
  Q_DISABLE_COPY(AbstractMessageParser)
  AbstractMessageParser() = delete;

  QString m_filePath;
};

#endif // ABSTRACTMESSAGEPARSER_H
