#pragma once

#include "mesh/geometrybuffers.h"

namespace geom {

/*
 *   1x1x1 Cube Geometry.
 *   Each side uses a separate set of vertices
 *   (composition of six independent rectangles).
 *   Tex Coordinates: [0,0] to [1,1] on each side.
 *
 */

class Cube : public GeometryBuffers
{
public:

    // no params, cube coordinates are from -0.5 to 0.5 in each dimension.
    Cube();
};

} // geom::

