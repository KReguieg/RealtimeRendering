#ifndef IMAGEDISPLAYBUTTON_H
#define IMAGEDISPLAYBUTTON_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class ImageDisplayButton;
}

class ImageDisplayButton : public QWidget
{
    Q_OBJECT

public:
    ImageDisplayButton(QWidget *parent = 0, QPixmap pixmap=QPixmap(), QString label=QString(""));
    ~ImageDisplayButton();

    void update(QPixmap pixmap, QString label);

private:
    Ui::ImageDisplayButton *ui;

    QString label_;
    QPixmap pixmap_;
};

#endif // IMAGEDISPLAYBUTTON_H
