#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QString>
#include <QListWidgetItem>

namespace Ui {
class Dialog;
}

// Class Dialog window for Other Colonies (maps), which can be selected to change current map
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    // selected colony file name
    QString selected_colony;
    // selected colony file link
    QString selected_colony_link;
    // unordered_map of colonies to colony file links
    std::unordered_map<QString, QString> map_link;

public slots:

    // current row changed on list widget
    void on_listWidget_currentRowChanged(int currentRow);
    // item clicked on list widget
    void on_listWidget_itemClicked();
    // select button clicked
    void on_select_clicked();

signals:
    // select button in Dialog window clicked
    void on_select_clicked_signal(bool);

private slots:
    void on_listWidget_doubleClicked(const QModelIndex &index);

private:
    // instanciate ui
    Ui::Dialog *ui;
    // initialise selected row
    int row_selected = -1;

};

#endif // DIALOG_H
