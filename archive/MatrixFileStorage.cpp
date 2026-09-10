#include "MatrixFileStorage.h"
#include <QFile>


bool MatrixFileStorage::write(const QString &file_path, const QByteArray &byte_array)
{
    QFile file(file_path);
    bool success = file.open(QIODevice::WriteOnly);
    if(!success){ return false; }
    qint64 count = file.write(byte_array);
    if(count != byte_array.size()){ return false; }
    return true;
}

std::optional<QByteArray> MatrixFileStorage::read(const QString &file_path)
{
    QFile file(file_path);
    bool success = file.open(QIODevice::ReadOnly);
    if(!success){ return std::nullopt; }
    QByteArray array = file.readAll();
    if(file.error() != QFileDevice::NoError){ return std::nullopt; }
    return array;
}