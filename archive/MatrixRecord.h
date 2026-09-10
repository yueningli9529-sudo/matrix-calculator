#ifndef MATRIXRECORD_H
#define MATRIXRECORD_H

#include <QUuid>
#include <QString>
#include "MatrixType.h"

struct MatrixRecord
{
    QUuid id_;
    QString name_;
    MatrixVariant matrix_;
};

#endif // MATRIXRECORD_H
