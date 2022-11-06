#pragma once

#include <QMap>
#include <boost/optional/optional.hpp>

#define BINDING_PORT "bind_port"
#define REMOTE_PORT  "remote_port"
#define REMOTE_IP    "remote_ip"

constexpr uint16_t default_binding_port = 14679;

class Settings
{
public:
    static Settings* getInstance();

    boost::optional<QString> getParameter(const QString& parameterName);

private:
    Settings();

    Settings(const Settings&) = delete;

    Settings(Settings&&) = delete;

private:
    QMap<QString, QString> parameters;
};

#define MySettings Settings::getInstance()

