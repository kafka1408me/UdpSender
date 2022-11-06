#pragma once

#include <QEvent>
#include "window.h"

class TextEvent : public QEvent
{
public:
    TextEvent(const QString& _text, MessageType _msgType);

    static QEvent::Type staticType();

    QString getMessage() const;

    MessageType getMessageType() const;

private:
    MessageType msgType;
    QString text;
};


