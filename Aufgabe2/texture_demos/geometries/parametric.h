#pragma once

#include <functional> // std::function
#include <math.h>

#include <QVector2D>
#include <QVector3D>

#include "mesh/geometrybuffers.h"

namespace geom {

class ParametricSurface : public GeometryBuffers
{
public:


protected:

    /* generate() has to be called from within the constructor of the derived class (!)
     *
     * from, to: parameter range
     * patches_u, patches_v: number of patches / cells in each direction
     * pos: function generating position vector
     *      input:   QVector2D with parameter point
     *      returns: vertex position as QVector3D
     * norm: function generating normal vector, see pos
     *
     */
    void generate(QVector2D from, QVector2D to,
                  size_t patches_u, size_t patches_v,
                  std::function<QVector3D(float,float)> pos,
                  std::function<QVector2D(float,float)> tex = nullptr);

}; // ParametricSurface

class Sphere : public ParametricSurface {
public:
    // make unit sphere (radius 0.5, poles are on Z axis)
    Sphere(size_t patches_u, size_t patches_v);
};

class Planet : public ParametricSurface {
public:
    // unit sphere with tex coordinates for planetary textures
    Planet(size_t patches_u, size_t patches_v);
};

// torus
class Torus : public ParametricSurface {
public:

    // make torus specified by two radii, winds around Z axis
    Torus(float r1, float r2, size_t patches_u, size_t patches_v);

};

// rectangle
class Rect : public ParametricSurface {
public:

    // make unit rectangle in X-Z plane
    Rect(size_t patches_u, size_t patches_v);

};

} // namespace geom
