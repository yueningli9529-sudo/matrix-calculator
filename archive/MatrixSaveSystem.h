#ifndef MATRIXSAVESYSTEM_H
#define MATRIXSAVESYSTEM_H

#include "MatrixRepository.h"
#include <optional>

namespace MatrixSaveSystem
{
    bool save(const MatrixRepository &repository);
    std::optional<MatrixRepository> load();
}

#endif // MATRIXSAVESYSTEM_H
