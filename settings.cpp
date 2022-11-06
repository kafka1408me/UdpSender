#include <QFile>
#include <QTextStream>
#include "settings.h"
#include "myapp.h"

constexpr uint16_t default_remote_port = 12345;

const char* default_ip = "127.0.0.1";

const char* settings_file_name = "settings.txt";

Settings *Settings::getInstance()
{
    static Settings settings;
    return &settings;
}

boost::optional<QString> Settings::getParameter(const QString &parameterName)
{
    auto it = parameters.find(parameterName);

    if(it != parameters.end())
    {
        return *it;
    }
    return {};
}

Settings::Settings()
{
    QFile f(settings_file_name);
    if(f.exists())
    {
        if(f.open(QIODevice::ReadOnly))
        {
            QString line;
            QStringList l;
            while(!f.atEnd())
            {
                line = f.readLine();
                line.remove("\r\n");
                l = line.split("=");

                if(l.size() == 2 &&
                        !l.at(0).isEmpty() && !l.at(1).isEmpty())
                {
                    parameters[l.at(0)] = l.at(1);
                }
            }

            f.close();
        }
    }
    else
    {
        parameters[REMOTE_IP] = QString::fromLatin1(default_ip);
        parameters[BINDING_PORT] = QString::number(default_binding_port);
        parameters[REMOTE_PORT] = QString::number(default_remote_port);

        ADD_TEXT("There is no settings file. Settings file will be created.", MessageType::Warning);

        if(f.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&f);
            for(auto it = parameters.begin(), end = parameters.end(); it != end; ++it)
            {
                stream << it.key() << "=" << it.value() << "\r\n";
            }

            f.close();
        }
    }
}
