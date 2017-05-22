#include "geometrybuffers.h"

#include "objloader.h"

#include <iostream>
#include <assert.h>

using namespace std;

const BoundingBox&
GeometryBuffers::bbox() const
{
    return bbox_;
}

void
GeometryBuffers::bind(QOpenGLVertexArrayObject& vao, QOpenGLShaderProgram& prog) const
{
    vao.bind();
    prog.bind();

    if(position_->numElements()) {
        position_->bind();
        prog.enableAttributeArray("position_MC");
        prog.setAttributeBuffer("position_MC", GL_FLOAT, 0, 3);
    }

    if(normal_->numElements()) {
        normal_->bind();
        prog.enableAttributeArray("normal_MC");
        prog.setAttributeBuffer("normal_MC", GL_FLOAT, 0, 3);
    }

    if(texcoord_->numElements()) {
        texcoord_->bind();
        prog.enableAttributeArray("texcoord");
        prog.setAttributeBuffer("texcoord", GL_FLOAT, 0, 2);
    }

    // do not forget: bind index buffer!
    if(index_->numElements())
        index_->bind();

    vao.release();

}

GeometryOBJ::GeometryOBJ(const string& filename)
{
    // create loader and load vertex data from OBJ file
    ObjLoader loader;
    loader.setMeshCenteringEnabled(true);
    loader.setLoadTextureCoordinatesEnabled(true);
    if (!loader.load(filename.c_str()))
        qFatal("Could not load mesh");

    // copy data from the loader's data structures into OpenGL buffer(s)
    position_ = make_unique<VertexBuffer<QVector3D>>(loader.vertices());
    normal_   = make_unique<VertexBuffer<QVector3D>>(loader.normals());
    texcoord_ = make_unique<VertexBuffer<QVector2D>>(loader.textureCoordinates());
    index_    = make_unique<IndexBuffer>(loader.indices());

    // calculate bounding box from all vertices
    bbox_ = BoundingBox(loader.vertices());

    // debug
    qDebug() << "created a new goemetry from OBJ";
    qDebug() << "vbo has" << position_->numElements() << "vertices,"
             << index_->numElements() << "indices";
    qDebug() << "bbox: min=" << bbox_.minPoint() << ", max=" << bbox_.maxPoint();
    qDebug() << "";


}

