#include "MatrixRepository.h"
bool MatrixRepository::add(MatrixRecord matrix_record)
{
    if(matrix_record.id_.isNull()){ return false; }
    if(is_dupulicated(matrix_record.id_)){ return false; }
    matrixrepository_.push_back(matrix_record);
    return true;

}
const std::vector<MatrixRecord> &MatrixRepository::matrixRepository() const
{
    return matrixrepository_;
}
bool MatrixRepository::is_dupulicated(QUuid ui) const
{
    for(const auto &record : matrixrepository_)
    {
        if(record.id_ == ui)
        {
            return true;
        }
    }
    return false;
}

const MatrixRecord *MatrixRepository::find(QUuid id) const
{
    for(const auto &record : matrixrepository_)
    {
        if(record.id_ == id)
        {
            return &record;
        }
    }
    return nullptr;
}