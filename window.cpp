#include <QRegExpValidator>
#include <QIntValidator>
#include <QDateTime>
#include "window.h"
#include "ui_window.h"
#include "udpcommunicator.h"
#include "textevent.h"
#include "myapp.h"
#include "settings.h"

const char* ipRegExp = "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$";

const char* messageExp = "[a-zA-Z0-9 ,;!\\.\\*\\-\\+_\\?=]+";

#ifdef ADD_TEXT
#undef ADD_TEXT
#define ADD_TEXT addText
#endif


const char* getTextColor(MessageType msgType)
{
    switch (msgType)
    {
    case MessageType::Information:
        return "white";
    case MessageType::Error:
        return "red";
    case MessageType::MessageSended:
        return "#0ce46c";
    case MessageType::MessageReceived:
        return "#1debed";
    case MessageType::Warning:
        return "yellow";
    }
}

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

    addText("Hello, User 😈 😈 😈", MessageType::Information);

    MyApp::getInstance()->setReceiver(this);

    udpComm = std::make_unique<UdpCommunicator>();

    auto ipValidator = new QRegExpValidator(QRegExp(ipRegExp), this);
    ui->ip_line_edit->setValidator(ipValidator);

    auto portValidator = new QIntValidator(0, 65535, this);
    ui->port_line_edit->setValidator(portValidator);

    auto messageValidator = new QRegExpValidator(QRegExp(messageExp), this);
    ui->message_line_edit->setValidator(messageValidator);
    ui->message_line_edit->setText("");

    connect(ui->send_message_btn, &QPushButton::clicked, this, &Window::slot_sendMessageBtnClicked);

    trySetSettingsToLineEdit(ui->ip_line_edit, REMOTE_IP);
    trySetSettingsToLineEdit(ui->port_line_edit, REMOTE_PORT);

    auto opt = MySettings->getParameter(BINDING_PORT);
    uint16_t bind_port = 0;

    if(opt)
    {
        bool isOk = false;
        bind_port = opt.value().toUInt(&isOk);

        if(!isOk || bind_port < 1024 || bind_port > 49151)
        {
            ADD_TEXT("invalid \"" + QString(BINDING_PORT) + "\"in settings file. Bind port must be "
                                                            "from 1024 to 49151. Bind to default port = "
                     + QString::number(default_binding_port), MessageType::Warning);
            bind_port = default_binding_port;
        }
    }

    try {
        udpComm->start(bind_port);
    }  catch (const boost::system::system_error& e) {
        addErrorText(QString::fromLocal8Bit(e.what()));
    }

    setBoundedPort(bind_port);

    setWindowTitle("UDP Sender");
}

Window::~Window()
{
    delete ui;
}

void Window::setBoundedPort(std::uint16_t port)
{
    ui->bounded_port_label->setText("Udp socket is bounded to <font color=\"green\">" + QString::number(port) +"</font> port");
}

void Window::addText(const QString &text, MessageType msgType)
{
//    QDateTime currentTime = QDateTime::currentDateTime();
    auto t = std::time(nullptr);
    QString curTimeString = QString::fromStdString(std::ctime(&t));
    ui->log_text_edit->append(QString("<font color=\"%1\">%2 : %3</font>\n")
                              .arg(getTextColor(msgType), curTimeString, text)
                              );

}

bool Window::event(QEvent *ev)
{
    if(ev->type() == TextEvent::staticType())
    {
        TextEvent* tEV = static_cast<TextEvent*>(ev);
        addText(tEV->getMessage(), tEV->getMessageType());
        return true;
    }

    return QWidget::event(ev);
}

void Window::slot_sendMessageBtnClicked()
{
    if(!validateUserInput(ui->port_line_edit, "Port number must be from 0 to 65535"))
    {
        return;
    }

    if(!validateUserInput(ui->ip_line_edit, "Invalid ip"))
    {
        return;
    }

    if(!validateUserInput(ui->message_line_edit, "Message must be not empty"))
    {
        return;
    }

    std::string ip = ui->ip_line_edit->text().toStdString();
    uint16_t port = ui->port_line_edit->text().toUShort();
    std::string msg = ui->message_line_edit->text().toStdString();

    try {
        udpComm->send_message(msg, ip, port);
    }  catch (const boost::system::system_error& e) {
        addErrorText(QString::fromLocal8Bit(e.what()));
    }
}

bool Window::validateUserInput(QLineEdit *lineEdit, const char *warningMsg)
{
    int pos = 0;
    QString input = lineEdit->text();
    const QValidator* validator = lineEdit->validator();
    if(validator->validate(input, pos) != QValidator::Acceptable)
    {
        ADD_TEXT(warningMsg, MessageType::Warning);
        return false;
    }
    return true;
}

void Window::addErrorText(const QString &text)
{
    addText(text, MessageType::Error);
}

void Window::trySetSettingsToLineEdit(QLineEdit *lineEdit, const char *settings_name)
{
    auto opt = MySettings->getParameter(settings_name);

    if(opt)
    {
        QString val = opt.value();
        auto validator = lineEdit->validator();

        int t = 0;
        if(validator->validate(val, t) == QValidator::State::Acceptable)
        {
            lineEdit->setText(val);
        }
        else
        {
            lineEdit->setText("");
            ADD_TEXT("invalid \"" + QString(settings_name) + "\" in settings file", MessageType::Warning);
        }
    }
    else
    {
        lineEdit->setText("");
        //     ADD_TEXT("cannot retrieve \"" + QString(settings_name) + "\" from settings file", MessageType::Warning);
    }
}
