#include "geometrybuffers.h"

#include "objloader.h"

#include <iostream>
#include <assert.h>
#include <vector> // std::vector


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

    if(texcoord_ && texcoord_->numElements()) {
        texcoord_->bind();
        prog.enableAttributeArray("texcoord");
        prog.setAttributeBuffer("texcoord", GL_FLOAT, 0, 2);
    }

    if(tangent_ && tangent_->numElements()) {
        tangent_->bind();
        prog.enableAttributeArray("tangent_MC");
        prog.setAttributeBuffer("tangent_MC", GL_FLOAT, 0, 3);
    }

    if(bitangent_ && bitangent_->numElements()) {
        bitangent_->bind();
        prog.enableAttributeArray("bitangent_MC");
        prog.setAttributeBuffer("bitangent_MC", GL_FLOAT, 0, 3);
    }

    // do not forget: bind index buffer!
    if(index_->numElements())
        index_->bind();

    vao.release();

}

void GeometryBuffers::generateTriangleTangents(const std::vector<QVector3D>& position,
                                               const std::vector<QVector3D>& normal,
                                               const std::vector<QVector2D>& texcoord,
                                               const std::vector<unsigned int> &index)
{
    // check what we need in order to generate tangents
    if(index.empty())
        qFatal("GeometryBuffers: need triangle indices to generate tangents");
    if(texcoord.empty())
        qFatal("GeometryBuffers: need tex coords to generate tangents");
    if(normal.empty())
        qFatal("GeometryBuffers: need normals to generate tangents");

    /*
     * The following code is adapted from:
     * Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”.
     * Terathon Software 3D Graphics Library.
     *
     * https://fenix.tecnico.ulisboa.pt/downloadFile/845043405449073/Tangent%20Space%20Calculation.pdf
     *
     */

    // this code assumes that we have three indices per triangle
    assert(index.size() % 3 == 0);
    assert(position.size() == normal.size());
    const size_t triangleCount = index.size() / 3;
    const size_t vertexCount = position.size();

    // temporary buffers, will be destoyed at end of this scope
    std::vector<QVector3D> temp(vertexCount * 2);
    std::vector<QVector3D> tangent(vertexCount);
    std::vector<QVector3D> bitangent(vertexCount);

    // for details on calculation, see article referenced above
    for (size_t tri = 0; tri < triangleCount; tri++) {

        // get the indices of the three vertices belonging to a triangle
        size_t i1 = index[tri*3];
        size_t i2 = index[tri*3+1];
        size_t i3 = index[tri*3+2];

        // get the corresponding vertex position
        const QVector3D& v1 = position[i1];
        const QVector3D& v2 = position[i2];
        const QVector3D& v3 = position[i3];

        // get corresponding tex coords
        const QVector2D& w1 = texcoord[i1];
        const QVector2D& w2 = texcoord[i2];
        const QVector2D& w3 = texcoord[i3];

        float x1 = v2.x() - v1.x();
        float x2 = v3.x() - v1.x();
        float y1 = v2.y() - v1.y();
        float y2 = v3.y() - v1.y();
        float z1 = v2.z() - v1.z();
        float z2 = v3.z() - v1.z();

        float s1 = w2.x() - w1.x();
        float s2 = w3.x() - w1.x();
        float t1 = w2.y() - w1.y();
        float t2 = w3.y() - w1.y();

        float r = 1.0F / (s1 * t2 - s2 * t1);
        QVector3D sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                       (t2 * z1 - t1 * z2) * r);
        QVector3D tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                       (s1 * z2 - s2 * z1) * r);

        temp[i1] += sdir;
        temp[i2] += sdir;
        temp[i3] += sdir;

        temp[i1 + vertexCount] += tdir;
        temp[i2 + vertexCount] += tdir;
        temp[i3 + vertexCount] += tdir;

    }

    // for details on calculation, see article referenced above
    for (size_t vert = 0; vert < vertexCount; vert++) {

        const QVector3D& n = normal[vert];
        const QVector3D& t = temp[vert];

        // Gram-Schmidt orthogonalize
        QVector3D tan = (t - n * QVector3D::dotProduct(n, t)).normalized();

        // Calculate handedness so the bitangent will point to the rigth direction
        float w = (QVector3D::dotProduct(QVector3D::crossProduct(n, t), temp[vert+vertexCount]) < 0.0F) ? -1.0F : 1.0F;
        tan *= 1/w;

        // calcuate bitangent
        QVector3D bitan = QVector3D::crossProduct(normal[vert], tan);

        // write result
        tangent[vert]   = tan;
        bitangent[vert] = bitan;
    }

    // create actual vertex buffers and copy data
    // (ok, this could be optimized, but...)
    tangent_ = make_unique<VertexBuffer<QVector3D>>(tangent);
    bitangent_ = make_unique<VertexBuffer<QVector3D>>(bitangent);

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
             << index_->numElements() << "indices,"
             << (texcoord_->numElements() > 0 ? " and tex coords" : " no tex coords");
    qDebug() << "bbox: min=" << bbox_.minPoint() << ", max=" << bbox_.maxPoint();
    qDebug() << "";

    // generate tangents and bitangents
    if(texcoord_->numElements() > 0)
        generateTriangleTangents(loader.vertices(), loader.normals(),
                                 loader.textureCoordinates(), loader.indices());
}

