
#ifndef GBKFILEIO_H
#define GBKFILEIO_H

#include <QString>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class GBKFileIO
{
public:
    GBKFileIO();
    ~GBKFileIO();
    QString readFile(const QString &filePath);
    void writeFile(const QString &filePath, const QString &Content);
};

#endif // GBKFILEIO_H
