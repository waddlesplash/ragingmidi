#ifndef SEEKSLIDER_H
#define SEEKSLIDER_H

#include <QSlider>

namespace Ui {
class SeekSlider;
}

class SeekSlider : public QSlider
{
    Q_OBJECT
    
public:
    explicit SeekSlider(QWidget *parent = 0);
    ~SeekSlider();

    inline bool doUpdate() { return enableUpdate; }

protected:
    inline void mousePressEvent(QMouseEvent *e)
    { enableUpdate = false; QSlider::mousePressEvent(e); }
    inline void mouseReleaseEvent(QMouseEvent *e)
    { enableUpdate = true; QSlider::mouseReleaseEvent(e); }
    
private:
    Ui::SeekSlider *ui;
    bool enableUpdate;
    int newPos;
};

#endif // SEEKSLIDER_H
