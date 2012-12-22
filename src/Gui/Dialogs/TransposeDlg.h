#ifndef TRANSPOSEDLG_H
#define TRANSPOSEDLG_H

/****h* RagingMidi/TransposeDlg.h
 * PURPOSE
 *   Shows a dialog that allows the user to
 *   transpose any number of tracks down or up.
 ******
 */


#include <QDialog>
#include <QTreeWidget>
#include <QList>

#include "../Widgets/TracksEdit.h"

namespace Ui {
class TransposeDlg;
}

/****c* TransposeDlg.h/TransposeDlg
 * SYNOPSIS
 */
class TransposeDlg : public QDialog
/********/
{
    Q_OBJECT
    
public:
    explicit TransposeDlg(QWidget *parent = 0, TracksEdit* tracksEdit = 0);
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
