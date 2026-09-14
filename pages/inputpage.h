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
    // state
    enum class InputState
    {
        Empty,
        Editing,
        Viewing,
    };
    InputState input_state_ = {InputState::Empty};
    void set_input_state(InputState input_state);
    void update_ui_for_state();
    // repo
    const MatrixRepository *repository_{nullptr};
    std::optional<MatrixRecord> current_record_;

    //repo list helper function
    void filter_repo_list(const QString keyword);

    //input helper function
    QGridLayout *matrixLayout_{nullptr};
    void clear_layout(QLayout *layout);
    void build_matrix();
    bool load_input();
    void demonstrate_current_matrix();
    QString name_matrix();

    Ui::InputPage *ui;
};

#endif // INPUTPAGE_H
