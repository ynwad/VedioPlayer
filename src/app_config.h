#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__

#include <QFile>
#include <QString>
#include <QDateTime>


class AppConfig
{
public:
    AppConfig();

    static QString m_strAppID;
    static int m_nAppVersion;
    static QString m_strVerName;

    static QString m_appDataPath_Main;

};

#endif // __APPCONFIG_H__
