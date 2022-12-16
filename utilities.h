#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QApplication>

class Utilities
{
public:
    static QString getDataPath();
    static QString newPhotoName();
    static QString getPhotoPath(QString name, QString postfix);
};

#endif // UTILITIES_H
