
#include "gbkfileio.h"

GBKFileIO::GBKFileIO()
{

}

// 文件内容读取（按照GBK编码进行读取）
QString GBKFileIO::readFile(const QString &filePath)
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QFile path(filePath);
    QTextStream in(&path);
    if (path.open(QFile::ReadOnly))
    {
        QByteArray data = path.readAll();
        path.close();
        return codec->toUnicode(data);
    }
    else
    {
        qDebug() << filePath << "open failed !";
        return QString();
    }
}

// 文件写入（按照GBK编码进行写入）
void GBKFileIO::writeFile(const QString &filePath, const QString &Content)
{
    QFile file(filePath);
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QByteArray gbkData = codec->fromUnicode(Content);
        file.write(gbkData);
        file.close();
    }
    else
    {
        qDebug() << filePath << "open failed !";
    }
}
