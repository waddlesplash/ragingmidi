#ifndef ALLEVENTS_H
#define ALLEVENTS_H

#include <QDialog>
#include <QTreeWidget>
#include <QtMidiFile.h>

namespace Ui {
class AllEvents;
}

class GuiMidiEvent : public QTreeWidgetItem
{
public:
    inline explicit GuiMidiEvent(QTreeWidget *p = 0)
        : QTreeWidgetItem(p) {}
    inline ~GuiMidiEvent() {}

    void init(QtMidiEvent *e);
};

class AllEvents : public QDialog
{
    Q_OBJECT
public:
    explicit AllEvents(QWidget *parent = 0, QtMidiFile *f = 0);
    ~AllEvents();
    
private:
    Ui::AllEvents *ui;
};

#endif // ALLEVENTS_H
