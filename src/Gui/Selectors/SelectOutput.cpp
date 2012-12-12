#include "SelectOutput.h"
#include "ui_SelectOutput.h"

#include <QtMidi.h>
#include <QStringList>

SelectOutput::SelectOutput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectOutput)
{
    ui->setupUi(this);
    ui->refreshBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
    on_refreshBtn_clicked();
    ui->acceptBB->setEnabled(false);
}

SelectOutput::~SelectOutput()
{
    delete ui;
}

QString SelectOutput::midiOutId()
{
    return ui->midiOutNames->selectedItems().at(0)->text(0);
}

void SelectOutput::on_searchLE_textChanged(const QString &)
{
    int top = ui->midiOutNames->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiOutNames->topLevelItem(i);
        curItem->setHidden(false);
    }
    if(ui->searchLE->text() == "") { return; }

    QStringList terms = ui->searchLE->text().split(" ");
    QString curTerm;

    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiOutNames->topLevelItem(i);
        foreach(curTerm,terms)
        {
            if(!curItem->text(1).contains(curTerm,Qt::CaseInsensitive))
            { curItem->setHidden(true); }
        }
    }

}

void SelectOutput::on_midiOutNames_itemClicked()
{
    ui->acceptBB->setEnabled(true);
}

void SelectOutput::on_midiOutNames_itemDoubleClicked(QTreeWidgetItem *, int)
{
    this->accept();
}

void SelectOutput::on_refreshBtn_clicked()
{
    ui->midiOutNames->clear();
    QMap<QString,QString> outDev = QtMidi::outDeviceNames();
    QStringList ids = outDev.keys();
    QTreeWidgetItem* newItem;
    foreach(QString id,ids)
    {
        newItem = new QTreeWidgetItem(ui->midiOutNames);
        newItem->setText(0,id);
        newItem->setText(1,outDev.value(id));
    }
}
