#ifndef TRACKSEDIT_H
#define TRACKSEDIT_H

#include <QTreeWidget>
#include <QtMidiFile.h>
#include <QStringList>
#include <QMap>

#include "VirtualPiano.h"

namespace Ui {
class TracksEdit;
}

class TrackItem : public QTreeWidgetItem
{
public:
    inline TrackItem(QTreeWidget* tree = 0, int track = 0)
        : QTreeWidgetItem(tree) { setText(TrackNumber,QString::number(track)); }

    enum ColumnIds {
        Name = 0,
        Type = 1,
        On = 2,
        Device = 3,
        Voice = 4,
        Inst = 5,
        Vol = 6,
        Bal = 7,
        TrackNumber = 8
    };

    inline void setName(QString name) { setText(Name,name); }
    inline QString name() { return text(Name); }

    inline void setType(QString type) { setText(Type,type); }
    inline QString type() { return text(Type); }

    inline void setOn(QString on) { setText(On,on); }
    inline QString on() { return text(On); }

    inline void setDevice(QString device) { setText(Device,device); }
    inline QString device() { return text(Device); }

    inline void setVoice(int voice) { setText(Voice,QString::number(voice+1)); }
    inline int voice() { return text(Voice).toInt()-1; }

    inline void setInst(QString ins) { setText(Inst,ins); }
    inline QString inst() { return text(Inst); }

    inline void setVol(int vol) { setText(Vol,QString::number(vol)); }
    inline int vol() { return text(Vol).toInt(); }

    inline void setBal(int bal) { setText(Bal,QString::number(bal)); }
    inline int bal() { return text(Bal).toInt(); }

    inline int track() { return text(TrackNumber).toInt(); }

private:
    int myTrackId;
};


class TracksEdit : public QTreeWidget
{
    Q_OBJECT
    
public:
    explicit TracksEdit(QWidget *parent = 0);
    ~TracksEdit();

    QMap<int,QColor>* trackColors() { return &myTrackColors; }
    QMap<int,bool>* trackStatus() { return &myTrackStatus; }

    void init(VirtualPiano* p);
    void setupTracks(QtMidiFile* f);

    TrackItem* createTrack(int trackNum);
    QList<TrackItem*> tracks();
    
private slots:
    void tracksEdit_itemClicked(QTreeWidgetItem *item, int column);
    void tracksEdit_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::TracksEdit *ui;
    QMap<int,QColor> myTrackColors;
    QMap<int,bool> myTrackStatus;

    QtMidiFile *midiFile;
    VirtualPiano *piano;
    QStringList colorNames;

    void updateTrackOn();
};

#endif // TRACKSEDIT_H
