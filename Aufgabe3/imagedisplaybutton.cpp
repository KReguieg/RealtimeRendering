#include "imagedisplaybutton.h"
#include "ui_imagedisplaybutton.h"
#include "imagedisplaydialog.h"
#include <qdebug.h>

ImageDisplayButton::ImageDisplayButton(QWidget *parent, QPixmap pixmap, QString label) :
    QWidget(parent),
    ui(new Ui::ImageDisplayButton)
{
    qDebug() << "creating new ImageDisplayButton: " << label;
    ui->setupUi(this);

    // set pixmap and label
    update(pixmap,label);

    // set minimal size to this current size
    setMinimumSize(this->size());

    // when user clicks on button, show full-res image in separate window
    connect(ui->button, &QToolButton::clicked, [this](){
        auto d = new ImageDisplayDialog(this,label_,pixmap_);
        d->show();
    });
}

void ImageDisplayButton::update(QPixmap pixmap, QString label)
{

    qDebug() << "updating" << label;

    // remember pixmap and label for full-res display
    pixmap_ = pixmap;
    label_ = label;

    // make icon for button
    auto icon_pixmap = pixmap.scaled(200,200,Qt::KeepAspectRatio);
    QIcon icon(icon_pixmap);

    // set icon and label in UI
    ui->button->setText(label);
    ui->button->setIcon(icon_pixmap);
    ui->button->setIconSize(icon_pixmap.rect().size());
    ui->button->setFixedSize(icon_pixmap.rect().size()+QSize(0,20));
    updateGeometry();
}

ImageDisplayButton::~ImageDisplayButton()
{
    delete ui;
}

