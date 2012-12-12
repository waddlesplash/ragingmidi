#include "SeekSlider.h"
#include "ui_SeekSlider.h"

SeekSlider::SeekSlider(QWidget *parent) :
    QSlider(parent), ui(new Ui::SeekSlider)
{
    ui->setupUi(this);
    enableUpdate = true;
    setTracking(false);
}

SeekSlider::~SeekSlider()
{
    delete ui;
}
