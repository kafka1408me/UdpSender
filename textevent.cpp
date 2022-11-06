#include "textevent.h"

TextEvent::TextEvent(const QString& _text, MessageType _msgType):
    QEvent(staticType()),
    msgType(_msgType),
    text(_text)
{
}

QEvent::Type TextEvent::staticType()
{
    static QEvent::Type type = static_cast<QEvent::Type>(registerEventType());
    return type;
}

QString TextEvent::getMessage() const
{
    return text;
}

MessageType TextEvent::getMessageType() const
{
    return msgType;
}
