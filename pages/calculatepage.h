#ifndef CALCULATEPAGE_H
#define CALCULATEPAGE_H

#include <QWidget>
#include <vector>
#include "MatrixRepository.h"
#include "calculation/MatrixStep.h"
#include <optional>

namespace Ui {
class CalculatePage;
}

enum class CalculationMode
{
    Single,
    Double,
};

class CalculatePage : public QWidget
{
    Q_OBJECT

public:
    explicit CalculatePage(QWidget *parent = nullptr);
    ~CalculatePage();

    //repo
    void setRepository(const MatrixRepository *repository);
    void refreshOptions();
private:
    CalculationMode mode_ = CalculationMode::Single;
    void refreshOperationOptions();
    void updateRowInputRanges(int rowCount);
    //show matrix
    void showUpperMatrix();
    void showLowerMatrix();
    //
    void setMode(CalculationMode mode);
    //calculate
    void calculateDouble();
    //step helper fucntions
    void applySingleStep();
    std::optional<ScalarVariant> QStringToScalarVariant(const QString &Sfactor);
    void showSingleStep(int stepIndex);
    QString rowOperationToQString(const RowOperation &operation) const;
    QString scalarToQString(const ScalarVariant &factor) const;
    //step data
    std::vector<MatrixStep> steps_;
    Ui::CalculatePage *ui;
    const MatrixRepository *repository_ = nullptr;
};

#endif // CALCULATEPAGE_H
