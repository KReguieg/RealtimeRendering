#pragma once

#include "node.h"
#include "camera.h"
#include <QMatrix4x4>
#include <assert.h>

#include <QKeyEvent>
#include <QMouseEvent>


class NodeNavigator
{
public:

    NodeNavigator(std::shared_ptr<Node> node,
                  std::shared_ptr<Node> world,
                  std::shared_ptr<Node> camera)
        : node_(node), world_(world), camera_(camera)
    {}

    virtual void keyPressEvent(QKeyEvent *) {}
    virtual void mousePressEvent(QMouseEvent *) {}
    virtual void mouseMoveEvent(QMouseEvent *) {}
    virtual void mouseReleaseEvent(QMouseEvent *) {}
    virtual void wheelEvent(QWheelEvent *) {}

protected:

    std::shared_ptr<Node> node_;
    std::shared_ptr<Node> world_;
    std::shared_ptr<Node> camera_;

};

class PositionNavigator : public NodeNavigator {
public:

    PositionNavigator(std::shared_ptr<Node> node,
                      std::shared_ptr<Node> world,
                      std::shared_ptr<Node> camera)
        : NodeNavigator(node,world,camera)
    {}

    // use (shift) cursor keys to move things around
    void keyPressEvent(QKeyEvent *event) override;

    // how much translation for one key press?
    float speed = 0.05f;

};

class RotateZoom : public NodeNavigator {
public:

    RotateZoom(std::shared_ptr<Node> node,
               std::shared_ptr<Node> world,
               std::shared_ptr<Node> camera)
        : NodeNavigator(node,world,camera)
    {}

    // use (shift) cursor keys to rotate around Y or move camera closer
    void keyPressEvent(QKeyEvent *event) override;

    // how many degrees rotation for one key press?
    float speed = 5.0;

};

class RotateCameraY : public NodeNavigator {
public:

    RotateCameraY(std::shared_ptr<Node> ,
                  std::shared_ptr<Node> ,
                  std::shared_ptr<Node> camera)
        : NodeNavigator(nullptr,nullptr,camera)
    {}

    // use cursor keys to rotate camera left/right around Y
    void keyPressEvent(QKeyEvent *event) override;

    // how many degrees rotation for one key press?
    float speed = 5.0;

};

class ModelTrackball : public NodeNavigator {
public:

    ModelTrackball(std::shared_ptr<Node> node,
                   std::shared_ptr<Node> world,
                   std::shared_ptr<Node> camera)
        : NodeNavigator(node,world,camera)
    {}

    // use mouse to rotate model around its center
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *event) override;

    float pan_sensitivity = 0.003f;
    float rotation_sensitivity = 15.0f;

private:

    QVector2D last_pos_;

    void rotate(QVector2D xy);
    void pan(QVector4D translation_ec);
};

