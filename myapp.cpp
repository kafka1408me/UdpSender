#include <QApplication>
#include "myapp.h"
#include "textevent.h"

static MyApp* myApp = nullptr;

MyApp::MyApp(int argc, char **argv):
    app(new QApplication(argc, argv))
{
    myApp = this;
}

MyApp::~MyApp()
{
    delete app;
}

void MyApp::setReceiver(QObject *receiver)
{
    window = receiver;
}

void MyApp::sendText(const std::string &text, MessageType msgType)
{
    app->postEvent(window, new TextEvent(QString::fromStdString(text), msgType));
}

MyApp *MyApp::getInstance()
{
    return myApp;
}

int MyApp::exec()
{
    return app->exec();
}
