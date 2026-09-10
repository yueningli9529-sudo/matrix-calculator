#ifndef MATRIXJSONSERIALIZER_H
#define MATRIXJSONSERIALIZER_H

#include "MatrixRecord.h"
#include "MatrixRepository.h"
#include <QJsonObject>
#include <QJsonArray>
#include <optional>

namespace JsonSerializer
{
// to json
QJsonObject record_to_QJsonObj(const MatrixRecord &record);
QJsonDocument repository_to_QJsonDoc(const MatrixRepository &repository);
// from json
std::optional<MatrixRecord> QJsonObj_to_record(const QJsonObject &obj);
std::optional<MatrixRepository> QJsonDoc_to_repository(const QJsonDocument &doc);
}

#endif // MATRIXJSONSERIALIZER_H
