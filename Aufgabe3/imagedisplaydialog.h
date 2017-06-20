#ifndef IMAGEDISPLAYDIALOG_H
#define IMAGEDISPLAYDIALOG_H

#include <QDialog>

namespace Ui {
class ImageDisplayDialog;
}

class ImageDisplayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDisplayDialog(QWidget *parent = 0, QString title="", QPixmap pixmap = QPixmap());
    ~ImageDisplayDialog();

private:
    Ui::ImageDisplayDialog *ui;
};

#endif // IMAGEDISPLAYDIALOG_H
