#include "imagedisplaydialog.h"
#include "ui_imagedisplaydialog.h"

ImageDisplayDialog::ImageDisplayDialog(QWidget *parent, QString title, QPixmap pixmap) :
    QDialog(parent),
    ui(new Ui::ImageDisplayDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->imgDisplayLabel->setPixmap(pixmap);
}

ImageDisplayDialog::~ImageDisplayDialog()
{
    delete ui;
}
