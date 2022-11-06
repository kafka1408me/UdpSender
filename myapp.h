#pragma once

#include <string>
#include "message_type.h"

class QCoreApplication;
class QObject;

class MyApp
{
public:
    MyApp(int argc, char** argv);

    ~MyApp();

    void setReceiver(QObject* receiver);

    void sendText(const std::string& text, MessageType msgType);

    static MyApp *getInstance();

    int exec();

private:
    QCoreApplication* app;
    QObject* window;
};

#define ADD_TEXT MyApp::getInstance()->sendText

