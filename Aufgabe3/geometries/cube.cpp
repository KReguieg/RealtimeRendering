#include "cube.h"

#include <vector>

using namespace std;

namespace geom {

/*
* The Cube is centered in the origin, all sides are axis-aligned,
* and each edge has length 1.
*
*                   H              G
*                   .--------------.
*                  /              /|
*                 / |            / |
*                /              /  |
*              D/   |         C/   |
*    y         .--------------.    |
*    |         |    |         |    |
*    |         |    .- - - - -|----.
*    |         |    E         |   /F
*    0-----x   |  /           |  /
*   /          |              | /
*  /           |/             |/
* z            .--------------.
*              A              B
*
*
* We use a right-handed coordinate system with Z pointing towards the
* viewer. For example, vertex A (front bottom left) has the coordinates
* ( x = -0.5, y = -0.5, z = 0.5 ) .
*
* The cube only consists of eight different vertex positions; however
* for various reasons (e.g. different normal directions) these vertices
* are "cloned" for each face of the cube. There will be 3 instances
* of each vertex, since each vertex belongs to three different faces.
*/

Cube::Cube()
{

    std::vector<QVector3D> positions = {
            // front
            QVector3D{ -0.5, -0.5,  0.5} ,  // A: index 0
            QVector3D{  0.5, -0.5,  0.5} ,  // B: index 1
            QVector3D{  0.5,  0.5,  0.5} ,  // C: index 2
            QVector3D{ -0.5,  0.5,  0.5} ,  // D: index 3

            // back
            QVector3D{-0.5, -0.5, -0.5} ,  // E: index 4
            QVector3D{ 0.5, -0.5, -0.5} ,  // F: index 5
            QVector3D{ 0.5,  0.5, -0.5} ,  // G: index 6
            QVector3D{-0.5,  0.5, -0.5} ,  // H: index 7

            // left
            QVector3D{-0.5, -0.5,  0.5} ,  // A': index 8
            QVector3D{-0.5,  0.5,  0.5} ,  // D': index 9
            QVector3D{-0.5,  0.5, -0.5} ,  // H': index 10
            QVector3D{-0.5, -0.5, -0.5} ,  // E': index 11

            // right
            QVector3D{ 0.5, -0.5,  0.5} ,  // B': index 12
            QVector3D{ 0.5, -0.5, -0.5} ,  // F': index 13
            QVector3D{ 0.5,  0.5, -0.5} ,  // G': index 14
            QVector3D{ 0.5,  0.5,  0.5} ,  // C': index 15

            // top
            QVector3D{-0.5,  0.5,  0.5} ,  // D'': index 16
            QVector3D{ 0.5,  0.5,  0.5} ,  // C'': index 17
            QVector3D{ 0.5,  0.5, -0.5} ,  // G'': index 18
            QVector3D{-0.5,  0.5, -0.5} ,  // H'': index 19

            // bottom
            QVector3D{-0.5, -0.5,  0.5 },  // A'': index 20
            QVector3D{-0.5, -0.5, -0.5 },  // E'': index 21
            QVector3D{ 0.5, -0.5, -0.5 },  // F'': index 22
            QVector3D{ 0.5, -0.5,  0.5 }   // B'': index 23
        };

    std::vector<QVector3D> normals = {
        // front
        QVector3D{  0,  0,  1 } ,
        QVector3D{  0,  0,  1 } ,
        QVector3D{  0,  0,  1 } ,
        QVector3D{  0,  0,  1 } ,

        // back
        QVector3D{  0,  0, -1 } ,
        QVector3D{  0,  0, -1 } ,
        QVector3D{  0,  0, -1 } ,
        QVector3D{  0,  0, -1 } ,

        // left
        QVector3D{ -1,  0,  0 } ,
        QVector3D{ -1,  0,  0 } ,
        QVector3D{ -1,  0,  0 } ,
        QVector3D{ -1,  0,  0 } ,

        // right
        QVector3D{  1,  0,  0 } ,
        QVector3D{  1,  0,  0 } ,
        QVector3D{  1,  0,  0 } ,
        QVector3D{  1,  0,  0 } ,

        // top
        QVector3D{  0,  1,  0 } ,
        QVector3D{  0,  1,  0 } ,
        QVector3D{  0,  1,  0 } ,
        QVector3D{  0,  1,  0 } ,

        // bottom
        QVector3D{  0,  -1,  0 } ,
        QVector3D{  0,  -1,  0 } ,
        QVector3D{  0,  -1,  0 } ,
        QVector3D{  0,  -1,  0 }
    };

    // tex coords: each side gets the full range [-1 : 1]
    std::vector<QVector2D> texcoords = {
        // front
        QVector2D{ 0, 0 } ,  // A: index 0
        QVector2D{  1, 0 } ,  // B: index 1
        QVector2D{  1,  1 } ,  // C: index 2
        QVector2D{ 0,  1 } ,  // D: index 3

        // back
        QVector2D{ 1, 0 } ,  // E: index 4
        QVector2D{0, 0 } ,  // F: index 5
        QVector2D{0,  1 } ,  // G: index 6
        QVector2D{ 1,  1 } ,  // H: index 7

        // left
        QVector2D{ 1, 0 } ,  // A': index 8
        QVector2D{ 1,  1 } ,  // D': index 9
        QVector2D{0,  1 } ,  // H': index 10
        QVector2D{0, 0 } ,  // E': index 11

        // right
        QVector2D{0, 0 } ,  // B': index 12
        QVector2D{ 1, 0 } ,  // F': index 13
        QVector2D{ 1,  1 } ,  // G': index 14
        QVector2D{0,  1 } ,  // C': index 15

        // top
        QVector2D{0,  1} ,  // D'': index 16
        QVector2D{ 1,  1} ,  // C'': index 17
        QVector2D{ 1, 0} ,  // G'': index 18
        QVector2D{0, 0} ,  // H'': index 19

        // bottom
        QVector2D{ 1,  1 },  // A'': index 20
        QVector2D{ 1, 0 },  // E'': index 21
        QVector2D{0, 0 },  // F'': index 22
        QVector2D{0,  1 }   // B'': index 23
    };

    std::vector<unsigned int> indices = {
        0,  1,  2,  0,  2,  3,  // front: ABC, ACD
        6,  5,  4,  7,  6,  4,  // back
        8,  9, 10,  8, 10, 11,  // left
        12, 13, 14, 12, 14, 15, // right
        16, 17, 18, 16, 18, 19, // top
        20, 21, 22, 20, 22, 23  // bot
    };

    // create OpenGL vertex buffer objects (VBOs) from geometry data
    position_ = make_unique<VertexBuffer<QVector3D>>(positions);
    normal_   = make_unique<VertexBuffer<QVector3D>>(normals);
    texcoord_ = make_unique<VertexBuffer<QVector2D>>(texcoords);
    index_    = make_unique<IndexBuffer>(indices);

    // calculate bounding box from extreme vertices
    bbox_ = BoundingBox({ positions[4] , positions[2] });

    // generate tangents and bitangents
    if(texcoord_->numElements() > 0)
        generateTriangleTangents(positions, normals, texcoords, indices);

} // Cube()

} // geom::
