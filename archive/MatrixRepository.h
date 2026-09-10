#ifndef MATRIXREPOSITORY_H
#define MATRIXREPOSITORY_H

#include <MatrixRecord.h>
#include <vector>

class MatrixRepository
{
public:
    bool add(MatrixRecord matrix_record);
    const std::vector<MatrixRecord> &matrixRepository() const;
    const MatrixRecord *find(QUuid id) const;

private:
    std::vector<MatrixRecord> matrixrepository_;
    bool is_dupulicated(QUuid ui) const;
};

#endif // MATRIXREPOSITORY_H
