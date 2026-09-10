#include "MatrixJsonSerializer.h"
#include <variant>
#include <QJsonDocument>
#include <limits>
#include <cmath>

QJsonObject JsonSerializer::record_to_QJsonObj(const MatrixRecord &record)
{
    QJsonObject JObj;
    JObj["id"] = record.id_.toString();
    JObj["name"] = record.name_;

    auto visitor = [&JObj](const auto & matrix)
    {
        using T =std::decay_t<decltype(matrix)>;
        if constexpr (std::is_same_v<T,Matrix<int>>)
        {
            JObj["datatype"] = QString("int");
            JObj["row"] =  static_cast<qint64>(matrix.row());
            JObj["column"] = static_cast<qint64>(matrix.column());
            QJsonArray JArray;
            for(std::size_t i = 0; i<matrix.row(); i++)
            {
                for(std::size_t j = 0; j<matrix.column(); j++)
                {
                    JArray.append(matrix(i,j));
                }
            }
            JObj["values"] = JArray;
        }
        if constexpr (std::is_same_v<T,Matrix<double>>)
        {
            JObj["datatype"] = QString("double");
            JObj["row"] =  static_cast<qint64>(matrix.row());
            JObj["column"] = static_cast<qint64>(matrix.column());
            QJsonArray JArray;
            for(std::size_t i = 0; i<matrix.row(); i++)
            {
                for(std::size_t j = 0; j<matrix.column(); j++)
                {
                    JArray.append(matrix(i,j));
                }
            }
            JObj["values"] = JArray;
        }
        if constexpr (std::is_same_v<T,Matrix<Rational>>)
        {
            JObj["datatype"] = QString("rational");
            JObj["row"] =  static_cast<qint64>(matrix.row());
            JObj["column"] = static_cast<qint64>(matrix.column());
            QJsonArray JArray;
            for(std::size_t i = 0; i<matrix.row(); i++)
            {
                for(std::size_t j = 0; j<matrix.column(); j++)
                {
                    QJsonArray cell;
                    cell.append(matrix(i,j).numer());
                    cell.append(matrix(i,j).denom());
                    JArray.append(cell);
                }
            }
            JObj["values"] = JArray;
        }
    };
    std::visit(visitor, record.matrix_);
    return JObj;
}
QJsonDocument JsonSerializer::repository_to_QJsonDoc(const MatrixRepository &repository)
{
    QJsonObject JObj;

    JObj["version"] = 1;
    QJsonArray array;
    for(const auto &record : repository.matrixRepository())
    {
        array.append(record_to_QJsonObj(record));
    }
    JObj["matrices"] = array;
    QJsonDocument doc(JObj);

    return doc;
}

std::optional<MatrixRecord> JsonSerializer::QJsonObj_to_record(const QJsonObject &obj)
{
    if(!obj["id"].isString()){ return std::nullopt; }
    QUuid id = QUuid::fromString(obj["id"].toString());
    if(id.isNull()){ return std::nullopt; }

    if(!obj["name"].isString()){ return std::nullopt; }
    QString name = obj["name"].toString();
    if(name.isEmpty()){ return std::nullopt; }

    if(!obj["datatype"].isString()){ return std::nullopt; }
    QString datatype = obj["datatype"].toString();
    if(datatype != QString("int") && datatype != QString("double") && datatype != QString("rational")){ return std::nullopt; }

    if(!obj["row"].isDouble()){ return std::nullopt; }
    int row = obj["row"].toInt();
    if(row <= 0 || row >= 100){ return std::nullopt; }

    if(!obj["column"].isDouble()){ return std::nullopt; }
    int column = obj["column"].toInt();
    if(column <= 0 || column >= 100){ return std::nullopt; }

    if(!obj["values"].isArray()){ return std::nullopt; }
    QJsonArray array = obj["values"].toArray();
    if(array.size() != row*column){ return std::nullopt; }

    if(datatype == QString("int"))
    {
        Matrix<int> matrix(row,column);
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                int index = i * column + j;
                QJsonValue value = array.at(index);
                if (!value.isDouble()) { return std::nullopt; }
                qint64 number = value.toInteger(std::numeric_limits<qint64>::min());
                if (number < std::numeric_limits<int>::min() ||
                    number > std::numeric_limits<int>::max()) {
                    return std::nullopt;
                }

                matrix(i, j) = static_cast<int>(number);
            }
        }
        return MatrixRecord{id,name,matrix};
    }
    else if(datatype == QString("double"))
    {
        Matrix<double> matrix(row,column);
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                int index = i * column + j;
                QJsonValue value = array.at(index);
                if (!value.isDouble()) { return std::nullopt; }
                double number = value.toDouble();
                if (!std::isfinite(number)) {
                    return std::nullopt;
                }
                matrix(i, j) = number;
            }
        }
        return MatrixRecord{id,name,matrix};
    }
    else if(datatype == QString("rational"))
    {
        Matrix<Rational> matrix(row,column);
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                int index = i * column + j;
                QJsonArray value = array.at(index).toArray();
                if (value.isEmpty()) { return std::nullopt; }
                if(value.size()!=2){ return std::nullopt; }
                QJsonValue Qnumer = value.at(0);
                QJsonValue Qdenom = value.at(1);
                qint64 qnumer =Qnumer.toInteger(std::numeric_limits<qint64>::min());
                if (qnumer < std::numeric_limits<int>::min() ||
                    qnumer > std::numeric_limits<int>::max()) {
                    return std::nullopt;
                }
                qint64 qdenom =Qdenom.toInteger(std::numeric_limits<qint64>::min());
                if (qdenom < std::numeric_limits<int>::min() ||
                    qdenom == 0||
                    qdenom > std::numeric_limits<int>::max()) {
                    return std::nullopt;
                }
                int numer = static_cast<int>(qnumer);
                int denom = static_cast<int>(qdenom);
                Rational rational(numer,denom);
                matrix(i, j) = rational;
            }
        }
        return MatrixRecord{id,name,matrix};
    }

    return std::nullopt;
}
std::optional<MatrixRepository> JsonSerializer::QJsonDoc_to_repository(const QJsonDocument &doc)
{
    MatrixRepository repository;

    if(!doc.isObject()){ return std::nullopt; }
    QJsonObject obj = doc.object();

    if(!obj["version"].isDouble()){ return std::nullopt; }
    int version = obj["version"].toInt();
    if(version != 1){ return std::nullopt; }

    if(!obj["matrices"].isArray()){ return std::nullopt; }
    QJsonArray array = obj["matrices"].toArray();

    for(const QJsonValue & r : array)
    {
        QJsonObject r1 = r.toObject();
        std::optional<MatrixRecord> record = QJsonObj_to_record(r1);
        if(!record.has_value()){ return std::nullopt; }
        if(!repository.add(record.value())){ return std::nullopt; }
    }

    return repository;
}