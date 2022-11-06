#pragma once

#include <QWidget>
#include "message_type.h"

namespace Ui
{
class Window;
}

class QLineEdit;
class UdpCommunicator;


class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);

    ~Window();

    void setBoundedPort(std::uint16_t port);

    void addText(const QString& text, MessageType msgType);

protected:
    bool event(QEvent* ev) override;

private slots:
    void slot_sendMessageBtnClicked();

private:
    bool validateUserInput(QLineEdit* lineEdit, const char* warningMsg);

    void addErrorText(const QString& text);

    void trySetSettingsToLineEdit(QLineEdit* lineEdit, const char* settings_name);

private:
    Ui::Window *ui;
    std::unique_ptr<UdpCommunicator> udpComm;
};
