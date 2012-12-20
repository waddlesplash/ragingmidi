#ifndef TRANSPOSEDLG_H
#define TRANSPOSEDLG_H

#include <QDialog>
#include <QTreeWidget>
#include <QList>

namespace Ui {
class TransposeDlg;
}

class TransposeDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit TransposeDlg(QWidget *parent = 0, QTreeWidget* tracksEdit = 0);
    ~TransposeDlg();

    QList<int> tracksToTranspose();
    int transposeSteps();
    
private slots:
    void on_selectNoneBtn_clicked();
    void on_selectAllBtn_clicked();

private:
    Ui::TransposeDlg *ui;
};

#endif // TRANSPOSEDLG_H
