#include "TransposeDlg.h"
#include "ui_TransposeDlg.h"

TransposeDlg::TransposeDlg(QWidget *parent, TracksEdit *tracksEdit) :
    QDialog(parent),
    ui(new Ui::TransposeDlg)
{
    ui->setupUi(this);
    ui->tracksChoose->hideColumn(2);

    foreach(TrackItem* t,tracksEdit->tracks()) {
        if(t->inst() == tr("(no instrument)")) { continue; }

        QTreeWidgetItem* n = new QTreeWidgetItem(ui->tracksChoose);
        n->setText(0,t->name());
        n->setBackgroundColor(0,t->backgroundColor(TrackItem::Name));
        n->setText(1,t->inst());
        n->setText(2,t->text(TrackItem::TrackNumber));
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
