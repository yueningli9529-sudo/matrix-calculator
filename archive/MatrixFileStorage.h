#ifndef MATRIXFILESTORAGE_H
#define MATRIXFILESTORAGE_H

#include <QString>
#include <QByteArray>
#include <optional>
namespace MatrixFileStorage
{
// 真实file与QByteArray之间的转化
bool write(const QString &file_path, const QByteArray &byte_array);
std::optional<QByteArray> read(const QString &file_path);
}

#endif // MATRIXFILESTORAGE_H