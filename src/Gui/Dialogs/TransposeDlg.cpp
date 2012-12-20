#include "TransposeDlg.h"
#include "ui_TransposeDlg.h"

TransposeDlg::TransposeDlg(QWidget *parent, QTreeWidget *tracksEdit) :
    QDialog(parent),
    ui(new Ui::TransposeDlg)
{
    ui->setupUi(this);
    ui->tracksChoose->hideColumn(2);

    for(int i = 0;i<tracksEdit->topLevelItemCount();i++) {
        QTreeWidgetItem* t = tracksEdit->topLevelItem(i);
        QTreeWidgetItem* n = new QTreeWidgetItem(ui->tracksChoose);
        n->setText(0,t->text(0));
        n->setBackgroundColor(0,t->backgroundColor(0));
        n->setText(1,t->text(5));
        n->setText(2,t->text(8));
    }
}

TransposeDlg::~TransposeDlg()
{
    delete ui;
}

QList<int> TransposeDlg::tracksToTranspose()
{
    QList<int> ret;
    foreach(QTreeWidgetItem* itm,ui->tracksChoose->selectedItems()) {
        ret.append(itm->text(2).toInt());
    }
    return ret;
}

int TransposeDlg::transposeSteps()
{
    return ui->spinBox->value();
}

void TransposeDlg::on_selectNoneBtn_clicked()
{
    ui->tracksChoose->selectAll();
}

void TransposeDlg::on_selectAllBtn_clicked()
{
    ui->tracksChoose->clearSelection();
}
