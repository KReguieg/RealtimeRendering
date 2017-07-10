
#pragma once

#include "mesh/geometrybuffers.h"
#include "material.h"

#include <memory> // std::shared_ptr

/*
 *  A mesh is geometry information combined with a surface material.
 *  Multiple mesh instances can share the same geometry information.
 *
 *  The Mesh creates an OpenGL Vertex Array Object (VAO) to
 *  represent the mapping of buffers to the material's uniform names.
 *
 */

class Mesh
{
public:

    /*
     * construct from existing geometry and material
     */
    Mesh(std::shared_ptr<GeometryBuffers> geometry, std::shared_ptr<Material> material);

    /*
     * convenience constructor: first creates a GeometryOBJ by loading geometry
     * data from a model file, then associates a material
     */
    Mesh(const std::string& filename, std::shared_ptr<Material> material);

    // Draw the mesh using the associated material
    void draw(unsigned int light_pass = 0);

    // access geometry
    std::shared_ptr<GeometryBuffers> geometry() const { return geometry_; }

    // access material
    std::shared_ptr<Material> material() const { return material_; }

    // replace material, fill VAO with new bindings
    void replaceMaterial(std::shared_ptr<Material> material);

    // do not copy meshes, please use constructor to generate copy
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

protected:

    // OpenGL vertex array object (VAO) representing the buffers' state
    QOpenGLVertexArrayObject vao_;

    // the actual geometry data
    std::shared_ptr<GeometryBuffers> geometry_;

    // surface Material to be used
    std::shared_ptr<Material> material_;

};

