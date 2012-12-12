#ifndef SELECTINSTRUMENT_H
#define SELECTINSTRUMENT_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
class SelectInstrument;
}

class SelectInstrument : public QDialog
{
    Q_OBJECT
    
public:
    explicit SelectInstrument(QWidget *parent = 0);
    ~SelectInstrument();

    QString insName();
    void setInsName(QString name);
    int insNum();
    void setInsNum(int num);
    
private slots:
    void on_searchLE_textChanged(const QString&);

    void on_midiInstr_itemClicked();
    void on_midiInstr_itemDoubleClicked(QTreeWidgetItem*, int);

private:
    Ui::SelectInstrument *ui;
};

#endif // SELECTINSTRUMENT_H
