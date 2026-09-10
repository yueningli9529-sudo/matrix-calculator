#ifndef INPUTPAGE_H
#define INPUTPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <MatrixSaveSystem.h>

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
    const MatrixRepository *repository_{nullptr};
    Ui::InputPage *ui;
    MatrixRecord current_record_;

    //repo list helper function
    void filter_repo_list(const QString keyword);

    //input helper function
    QGridLayout *matrixLayout_{nullptr};
    void clear_layout(QLayout *layout);
    void build_matrix();
    void load_input();
    void switch_input_to_demonstrate();
    QString name_matrix();
};

#endif // INPUTPAGE_H
