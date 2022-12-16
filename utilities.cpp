#include "utilities.h"

QString Utilities::getDataPath()
{
    QString userPicturesPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
    QDir picturesDir(userPicturesPath);
    picturesDir.mkpath("Facetious");

    return picturesDir.absoluteFilePath("Facetious");
}

QString Utilities::newPhotoName()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toString("yyyy-MM-dd+HH:mm:ss");
}

QString Utilities::getPhotoPath(QString name, QString postfix)
{
    return QString("%1/%2.%3").arg(Utilities::getDataPath(), name, postfix);
}
