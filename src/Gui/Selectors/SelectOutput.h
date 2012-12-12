#ifndef SELECTOUTPUT_H
#define SELECTOUTPUT_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
class SelectOutput;
}

class SelectOutput : public QDialog
{
    Q_OBJECT
    
public:
    explicit SelectOutput(QWidget *parent = 0);
    ~SelectOutput();

    QString midiOutId();

private slots:
    void on_searchLE_textChanged(const QString &);

    void on_midiOutNames_itemClicked();
    void on_midiOutNames_itemDoubleClicked(QTreeWidgetItem *, int);

    void on_refreshBtn_clicked();

private:
    Ui::SelectOutput *ui;
};

#endif // SELECTOUTPUT_H
