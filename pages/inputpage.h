#ifndef INPUTPAGE_H
#define INPUTPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <MatrixSaveSystem.h>
#include <optional>

namespace Ui {
class InputPage;
}

class InputPage : public QWidget
{
    Q_OBJECT

public:
    explicit InputPage(QWidget *parent = nullptr);
    ~InputPage();
    MatrixRecord InputResult() const;
    //repo
    void setRepository(const MatrixRepository *repository);
    void refreshRepositoryList();
signals:
    void save_button_clicked();
private:
    // input state
    enum class InputState
    {
        Empty,
        Editing,
        Viewing,
    };
    InputState input_state_ = {InputState::Empty};
    void set_input_state(InputState input_state);
    void update_ui_for_state();
    // error state
    enum class InputErrorType
    {
        InvalidValue,
        InternalStateError,
    };
    enum class InputPart
    {
        Value,
        Numerator,
        Denominator,
    };
    struct InputPosition
    {
        std::size_t row_number_;    //base 1
        std::size_t column_number_; //base 1
        InputPart input_part_;
    };
    struct InputError
    {
        InputErrorType input_error_type_;
        std::optional<InputPosition> position_;
    };
    void show_input_error(const InputError &error);
    // repo
    const MatrixRepository *repository_{nullptr};
    std::optional<MatrixRecord> current_record_;

    //repo list helper function
    void filter_repo_list(const QString keyword);

    //input helper function
    QGridLayout *matrixLayout_{nullptr};
    void clear_layout(QLayout *layout);
    void build_matrix();
    std::optional<InputError> load_input();
    void demonstrate_current_matrix();
    QString name_matrix();


    Ui::InputPage *ui;
};

#endif // INPUTPAGE_H
