#ifndef MAINWIND_H
#define MAINWIND_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QMap>
#include <QSettings>

#include <QtMidiFile.h>
#include "../Player.h"
#include "Widgets/SeekSlider.h"

namespace Ui {
class MainWind;
}

class MainWind : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWind(QWidget *parent = 0);
    ~MainWind();

    static QMap<int,QColor> trackColors;
    static QMap<int,bool> trackStatus;
    static SeekSlider* playLocSilder;
    
private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();

    void on_tracksEdit_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_tracksEdit_itemClicked(QTreeWidgetItem *item, int column);

    void on_actionTrackAdd_triggered();

    void on_actionPlay_triggered();
    void on_actionStop_triggered();
    void on_actionRewind_triggered();
    void on_songPosSlider_sliderReleased();

    void on_actionViewAllEvents_triggered();

    void on_actionAbout_Raging_Midi_triggered();

private:
    Ui::MainWind *ui;
    QtMidiFile* midiFile;
    Player* player;
    QSettings* appSettings;

    void openMidiFile(QString filename);
    void updateTrackOn();
};

#endif // MAINWIND_H
