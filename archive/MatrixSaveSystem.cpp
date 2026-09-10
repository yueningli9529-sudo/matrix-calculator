#include "MatrixSaveSystem.h"
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include "MatrixJsonSerializer.h"
#include "MatrixFileStorage.h"
bool MatrixSaveSystem::save(const MatrixRepository &repository)
{
    QString directoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (directoryPath.isEmpty()) { return false; }
    if (!QDir().mkpath(directoryPath)) { return false; }
    QString filePath = QDir(directoryPath).filePath("matrices.json");

    QJsonDocument doc = JsonSerializer::repository_to_QJsonDoc(repository);
    QByteArray byteArray = doc.toJson(QJsonDocument::Indented);
    return MatrixFileStorage::write(filePath,byteArray);

}
std::optional<MatrixRepository> MatrixSaveSystem::load()
{
    QString directoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (directoryPath.isEmpty()) { return std::nullopt; }
    if (!QDir().mkpath(directoryPath)) { return std::nullopt; }
    QString filePath = QDir(directoryPath).filePath("matrices.json");

    std::optional<QByteArray> byteArray = MatrixFileStorage::read(filePath);
    if(!byteArray.has_value()){ return std::nullopt; }
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(byteArray.value(), &error);
    if (error.error != QJsonParseError::NoError) { return std::nullopt; }
    if (document.isNull()) { return std::nullopt; }
    return JsonSerializer::QJsonDoc_to_repository(document);
}