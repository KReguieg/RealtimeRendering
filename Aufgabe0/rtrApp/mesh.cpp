
#include "mesh.h"
#include "objloader.h"

#include <iostream>

using namespace std;

Mesh::Mesh()
{
}

Mesh::Mesh(const string& filename,
           QOpenGLShaderProgram* prog)

    : program_(prog)
{
    // create loader and load vertex data from OBJ file
    ObjLoader loader;
    loader.setMeshCenteringEnabled(true);
    loader.setLoadTextureCoordinatesEnabled(true);
    if (!loader.load(filename.c_str()))
        qFatal("Could not load mesh");

    // create OpenGL vertex attribute object
    if (!vao_.create())
        qFatal("Unable to create VAO");

    // bind vertex array object so successive commands refer to this
    vao_.bind();

    // copy data from the loader's data structures into OpenGL buffer(s)
    position_ = make_unique<VertexBuffer<QVector3D>>(loader.vertices());
    normal_   = make_unique<VertexBuffer<QVector3D>>(loader.normals());
    texcoord_ = make_unique<VertexBuffer<QVector2D>>(loader.textureCoordinates());
    index_    = make_unique<IndexBuffer>(loader.indices());

    // calculate bounding box from all vertices
    bbox_ = BoundingBox(loader.vertices());

    // debug
    qDebug() << "vbo has" << position_->numElements() << "vertices";
    qDebug() << "vbo has" << index_->numElements() << "indices";
    qDebug() << "bbox: min=" << bbox_.minPoint() << ", max=" << bbox_.maxPoint();

    // unbind vao, now the buffers' state is saved within
    vao_.release();

    // do not forget to set the program and bind the VAO to it
    setProgram(prog);
}

void Mesh::bind()
{
    if(vao_.isCreated())
        vao_.bind();
    else
        qFatal("Cannot bind VAO that has not been created ");

    if(program_)
        program_->bind(); // optional, you do not always need a program
    else
        qFatal("Cannot bind program to draw mesh");

}

void Mesh::release()
{
    if(program_)
        program_->release();

    if(vao_.isCreated())
        vao_.release();
    else
        qFatal("Cannot release VAO that has not been created ");

}

void Mesh::draw()
{

    bind();
    glDrawElements(GL_TRIANGLES, index_->numElements(), GL_UNSIGNED_INT, Q_NULLPTR);
    release();
}

const BoundingBox &Mesh::bbox() const
{
    return bbox_;
}

void Mesh::setProgram(QOpenGLShaderProgram* prog)
{
    program_ = prog;
    if(!program_)
        qFatal("Mesh: no program provided to setProgram");

    program_->bind();
    vao_.bind();

    if(position_->numElements()) {
        position_->bind();
        program_->enableAttributeArray("position_MC");
        program_->setAttributeBuffer("position_MC", GL_FLOAT, 0, 3);
    }

    if(normal_->numElements()) {
        normal_->bind();
        program_->enableAttributeArray("normal_MC");
        program_->setAttributeBuffer("normal_MC", GL_FLOAT, 0, 3);
    }

    if(texcoord_->numElements()) {
        texcoord_->bind();
        program_->enableAttributeArray("texcoord");
        program_->setAttributeBuffer("texcoord", GL_FLOAT, 0, 2);
    }

    vao_.release();
    program_->release();
}
