#include "camera.h"

using namespace std;

Camera::Camera(QVector3D from,
               QVector3D to,
               QVector3D up,
               float fovy,
               float aspect_ratio,
               float near_plane,
               float far_plane)

    : viewMatrix_(),
      projectionMatrix_(),
      from_(from), to_(to), up_(up),
      fovy_(fovy), aspect_ratio_(aspect_ratio), near_(near_plane), far_(far_plane)
{
    // calculate view+proj matrix
    calculateMatrices_();

    // init uniform names the matrices will be bound to
    setMatrixUniformNames();
}

void Camera::setAspectRatio(float aspect)
{
    aspect_ratio_ = aspect;
    calculateMatrices_();
}

void Camera::setFieldOfView(float degrees)
{
    fovy_ = degrees;
    calculateMatrices_();
}

void Camera::translateViewPoint(QVector3D delta)
{
    from_ += delta;
    calculateMatrices_();
}

void Camera::setMatrices(QOpenGLShaderProgram &prog,
                         QMatrix4x4 modelMatrix) const
{
    QMatrix4x4 mv  = viewMatrix_ * modelMatrix;
    QMatrix4x4 mvp = projectionMatrix_ * mv;

    prog.bind();
    prog.setUniformValue(name_m_.c_str(),   modelMatrix);
    prog.setUniformValue(name_v_.c_str(),   viewMatrix_);
    prog.setUniformValue(name_p_.c_str(),   projectionMatrix_);
    prog.setUniformValue(name_mv_.c_str(),  mv);
    prog.setUniformValue(name_n_.c_str(),   mv.normalMatrix());
    prog.setUniformValue(name_mvp_.c_str(), mvp);

}

void Camera::setMatrices(Material &material,
                         QMatrix4x4 modelMatrix) const
{
    setMatrices(material.program(), modelMatrix);
}



void Camera::setMatrixUniformNames(const string m,
                                   const string v,
                                   const string p,
                                   const string mv,
                                   const string n,
                                   const string mvp)
{
    name_m_   = m;
    name_v_   = v;
    name_p_   = p;
    name_mv_  = mv;
    name_n_   = n;
    name_mvp_ = mvp;
}

void Camera::calculateMatrices_()
{
    viewMatrix_ = QMatrix4x4();
    viewMatrix_.lookAt(from_,to_,up_);
    projectionMatrix_ = QMatrix4x4();
    projectionMatrix_.perspective(fovy_,aspect_ratio_,near_,far_);
}

void Camera::setViewPoint(QVector3D point)
{
    to_ = point;
    calculateMatrices_();
}

