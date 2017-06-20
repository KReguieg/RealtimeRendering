#include "parametric.h"

#include <vector>
#include <cassert>

const float pi = 3.14159265f;
using namespace std;

namespace geom {

void
ParametricSurface::generate(QVector2D from,
                            QVector2D to,
                            size_t patches_u,
                            size_t patches_v,
                            std::function<QVector3D (float, float)> pos,
                            std::function<QVector2D (float, float)> tex)
{

    // replace dummy tex func by actual lambda
    if(!tex)
        tex = [from,to](float s, float t) {
            return (QVector2D(s,t)-from)/(to-from);
        };

    auto numVertices = (patches_u+1) * (patches_v+1);
    auto numPatches  = patches_u * patches_v;

    // store position, normal, and tex coord for each vertex
    vector<QVector3D> positions, normals, tangents, bitangents;
    vector<QVector2D> texcoords;
    positions.reserve(numVertices);
    normals.reserve(numVertices);
    texcoords.reserve(numVertices);

    // each patch is made out of two triangles
    vector<unsigned int> indices;
    indices.reserve(numPatches*2);

    // step size
    QVector2D step = (to-from) / QVector2D(patches_u, patches_v);

    // loop over u and v to define per-vertex attributes
    for(size_t j=0; j<=patches_v; j++) {
        for(size_t i=0; i<=patches_u; i++) {

            QVector2D ij(i,j);

            // current position (u,v) on the surface
            QVector2D st = from + ij*step;

            // current index into the vertex buffers
            auto vindex = j*(patches_u+1) + i;

            // calculate and store vertex attributes using provided functions
            assert(vindex == positions.size());
            positions.push_back( pos(st.x(), st.y()));
            texcoords.push_back( tex(st.x(), st.y()));
            // normals.push_back(  norm(st.x(), st.y()));

            // indices for drawing two triangles per patch (but not at border)
            if(i<patches_u && j<patches_v) {
                indices.push_back((unsigned int) (vindex) );
                indices.push_back((unsigned int) (vindex+(patches_u+1)) );
                indices.push_back((unsigned int) (vindex+(patches_u+1)+1) );
                indices.push_back((unsigned int) (vindex+(patches_u+1)+1) );
                indices.push_back((unsigned int) (vindex+1) );
                indices.push_back((unsigned int) (vindex) );
            }

            // calculate tangent and bitangent using central differences
            auto next = st+step, prev = st-step; // not correct, tex coords can be different!
            QVector3D t = (pos(next.x(), st.y()) - pos (prev.x(), st.y())).normalized();
            QVector3D b = (pos(st.x(), next.y()) - pos (st.x(), prev.y())).normalized();

            // normal is cros product of tangent
            QVector3D n = QVector3D::crossProduct(t,b).normalized();
            normals.push_back(n);

        } // for j
    } // for i

    // create OpenGL vertex buffer objects (VBOs) from geometry data
    position_ = make_unique<VertexBuffer<QVector3D>>(positions);
    texcoord_ = make_unique<VertexBuffer<QVector2D>>(texcoords);
    normal_   = make_unique<VertexBuffer<QVector3D>>(normals);
    index_    = make_unique<IndexBuffer>(indices);

    // calculate bounding box from extreme vertices
    bbox_ = BoundingBox(positions);

    // generate tangents and bitangents based on tex coordinates
    if(texcoord_->numElements() > 0)
        generateTriangleTangents(positions, normals, texcoords, indices);

}

Sphere::Sphere(size_t patches_u, size_t patches_v)
{
    auto pos = [](float s, float t) -> QVector3D {
        const float r = 0.5;
        return QVector3D(r * sin(s) * cos(t),
                         r * sin(s) * sin(t),
                         r * cos(s) );
    };

    generate(QVector2D(0,0),
             QVector2D(pi, 2.0f*pi),
             patches_u, patches_v,
             pos);

}

Planet::Planet(size_t patches_u, size_t patches_v)
{
    QVector2D from(0,0);
    QVector2D to(pi, 2.0f*pi);

    auto pos = [](float s, float t) -> QVector3D {
        const float r = 0.5;
        return QVector3D(r * sin(s) * cos(t),
                         r * sin(s) * sin(t),
                         r * cos(s) );
    };

    auto tex = [=](float s, float t) -> QVector2D {
        auto st = (QVector2D(s,t)-from)/(to-from);
        return QVector2D(st.y(), 1.0 - st.x());
    };

    generate(from, to,
             patches_u, patches_v,
             pos, tex);

}

Torus::Torus(float r1, float r2, size_t patches_u, size_t patches_v)
{

    auto pos = [r1,r2](float s, float t) -> QVector3D {
        return QVector3D((r1 + r2 * cos(t)) * cos(s),
                         (r1 + r2 * cos(t)) * sin(s),
                         r2 * sin(t) );
    };

    generate(QVector2D(0,0),
             QVector2D(2*pi, 2*pi),
             patches_u, patches_v,
             pos);

}

Rect::Rect(size_t patches_u, size_t patches_v)
{

    auto pos = [](float s, float t) -> QVector3D {
        return QVector3D(-0.5 + s, 0, 0.5 - t );
    };

    generate(QVector2D(0,0),
             QVector2D(1,1),
             patches_u, patches_v,
             pos);

}

RectXY::RectXY(size_t patches_u, size_t patches_v)
{

    auto pos = [](float s, float t) -> QVector3D {
        return QVector3D(-1 + s, -1 + t, 0 );
    };

    generate(QVector2D(0,0),
             QVector2D(2,2),
             patches_u, patches_v,
             pos);

}


} // namespace geom
