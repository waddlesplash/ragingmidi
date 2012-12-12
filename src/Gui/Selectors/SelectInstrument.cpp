#include "SelectInstrument.h"
#include "ui_SelectInstrument.h"

SelectInstrument::SelectInstrument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectInstrument)
{
    ui->setupUi(this);

    ui->acceptBB->setEnabled(false);
}

SelectInstrument::~SelectInstrument()
{
    delete ui;
}

void SelectInstrument::setInsName(QString name)
{
    int top = ui->midiInstr->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        if(curItem->text(0) == name)
        {
            ui->midiInstr->setCurrentItem(curItem);
            return;
        }
    }
}

void SelectInstrument::setInsNum(int num)
{
    QString var = QString::number(num);
    int top = ui->midiInstr->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        if(curItem->text(0) == var)
        {
            ui->midiInstr->setCurrentItem(curItem);
            return;
        }
    }
}

QString SelectInstrument::insName()
{
    return ui->midiInstr->selectedItems().at(0)->text(1);
}

int SelectInstrument::insNum()
{
    return ui->midiInstr->selectedItems().at(0)->text(0).toInt();
}

void SelectInstrument::on_searchLE_textChanged(const QString &)
{
    int top = ui->midiInstr->topLevelItemCount();
    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        curItem->setHidden(false);
    }
    if(ui->searchLE->text() == "") { return; }

    QStringList terms = ui->searchLE->text().split(" ");
    QString curTerm;

    for(int i = 0;i < top;i++)
    {
        QTreeWidgetItem* curItem = ui->midiInstr->topLevelItem(i);
        foreach(curTerm,terms)
        {
            if(!curItem->text(1).contains(curTerm,Qt::CaseInsensitive))
            { curItem->setHidden(true); }
        }
    }

}

void SelectInstrument::on_midiInstr_itemClicked()
{
    ui->acceptBB->setEnabled(true);
}

void SelectInstrument::on_midiInstr_itemDoubleClicked(QTreeWidgetItem*, int)
{
    this->accept();
}
