#include "nodenavigator.h"
#include <QtMath>

#include <iostream>

using namespace std;

void PositionNavigator::keyPressEvent(QKeyEvent *event)
{
    // translation vector in eye coords
    QVector4D translation_ec(0,0,0,0);

    // depending on key press, change position
    bool isShiftPressed = event->modifiers() & Qt::ShiftModifier;
    switch(event->key()) {
        case Qt::Key_Up:
            if(isShiftPressed)
                translation_ec[2] = -speed;  // shift: Z
            else
                translation_ec[1] = speed; // Y
            break;
        case Qt::Key_Down:
            if(isShiftPressed)
                translation_ec[2] = speed; // shift: Z
            else
                translation_ec[1] = -speed; // Y
            break;
        case Qt::Key_Left:
            translation_ec[0] = -speed; // X
            break;
        case Qt::Key_Right:
            translation_ec[0] = speed; // X
            break;
        default:
            return;
    }

    // translate from camera coords into model coords
    QMatrix4x4 camToWorld = world_->toWorldTransform(camera_);
    QMatrix4x4 worldToModel = world_->toWorldTransform(node_).inverted();
    QVector4D translation_mc = worldToModel * camToWorld * translation_ec;
    node_->transformation.translate(translation_mc.toVector3D());

    // debugging for positioning
    QMatrix4x4 nodeToWorld = world_->toWorldTransform(node_);
    QVector4D pos = nodeToWorld * QVector4D(0,0,0,1);
    qDebug() << pos;

}

void RotateZoom::keyPressEvent(QKeyEvent *event)
{
    // translate Y axis from camera coords into model coords
    QMatrix4x4 camToWorld = world_->toWorldTransform(camera_);
    QMatrix4x4 worldToModel = world_->toWorldTransform(node_).inverted();
    QVector3D  y_axis_mc = (worldToModel * camToWorld * QVector4D(0,1,0,0) ).toVector3D();


    // depending on key press, change position
    switch(event->key()) {
        case Qt::Key_Up:
            camera_->transformation.translate(QVector3D(0,0,-0.1f));
            break;
        case Qt::Key_Down:
            camera_->transformation.translate(QVector3D(0,0,0.1f));
            break;
        case Qt::Key_Left:
            node_->transformation.rotate(-speed, y_axis_mc);
            break;
        case Qt::Key_Right:
            node_->transformation.rotate(speed, y_axis_mc);
            break;
        default:
            return;
    }

}

void ModelTrackball::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        last_pos_ = QVector2D(event->x(), event->y());
}

void ModelTrackball::mouseMoveEvent(QMouseEvent *event)
{
    // how much did the mouse move?
    QVector2D now = QVector2D(event->x(), event->y());
    QVector2D delta = now - last_pos_;
    last_pos_= now;

    // apply to panning or rotation
    if(event->modifiers() & Qt::ShiftModifier)
        pan(QVector3D(delta * QVector2D(1,-1), 0));
    else
        rotate(delta*rotation_sensitivity);
}

void ModelTrackball::mouseReleaseEvent(QMouseEvent *)
{
    // qDebug() << "pos = " << node_->transformation.column(3);
}

void ModelTrackball::wheelEvent(QWheelEvent *event)
{

    camera_->transformation.translate(QVector3D(0,0,-event->delta()/100.0));
}

void ModelTrackball::rotate(QVector2D xy)
{
    // transformation from camera coords to model coords
    auto camToWorld = world_->toWorldTransform(camera_);
    auto nodeToWorld = world_->toWorldTransform(node_);
    auto camToNode = nodeToWorld.inverted()*camToWorld;

    // main axes converted to model space
    auto xAxis = camToNode*QVector4D(1,0,0,0);
    auto yAxis = camToNode*QVector4D(0,1,0,0);

    // rotate
    node_->transformation.rotate(qDegreesToRadians(xy[1]), xAxis.toVector3D());
    node_->transformation.rotate(qDegreesToRadians(xy[0]), yAxis.toVector3D());

}

void ModelTrackball::pan(QVector4D translation_ec)
{
    // transformation from camera coords to model coords
    auto camToWorld = world_->toWorldTransform(camera_);
    auto nodeToWorld = world_->toWorldTransform(node_);
    auto camToNode = nodeToWorld.inverted()*camToWorld;

    QVector4D translation_mc = camToNode * translation_ec * pan_sensitivity;
    node_->transformation.translate(translation_mc.toVector3D());

}

void RotateCameraY::keyPressEvent(QKeyEvent *event)
{
    // depending on key press, change position
    // bool isShiftPressed = event->modifiers() & Qt::ShiftModifier;
    float degrees = 0;
    switch(event->key()) {
        case Qt::Key_Left:
            degrees = -speed; // X
            break;
        case Qt::Key_Right:
            degrees = speed; // X
            break;
        default:
            return;
    }

    // we rotate around the camera's Y axis,
    // so just modify camera's transformation matrix directly
    QMatrix4x4 rot;
    rot.rotate(degrees, QVector3D(0,1,0));
    camera_->transformation = rot * camera_->transformation;

}
