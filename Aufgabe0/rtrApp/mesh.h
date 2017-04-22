
#pragma once

#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "bbox.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>

#include <memory> // std::unique_ptr

/*
 *  A mesh is a collection of vextex data (position, normal, tex coords, indices)
 *  combined with an OpenGL shader program.
 *
 *  If a program is associated with the mesh through the constructor or
 *  via setProgram, vertex data will be bound to the following uniform names:
 *  vertex position     -> uniform vec3 position_MC
 *  vertex normal       -> uniform vec3 normal_MC
 *  texture coordinates -> uniform vec2 texcoord
 *
 *  The suffix _MC indicates model coordinates.
 *
 */

class Mesh
{
public:
    explicit Mesh();

    /*
     * construct by loading mesh data from a model file
     * and a program
     */
    Mesh(const std::string& filename,
         QOpenGLShaderProgram* prog);

    /*
     *  associate an OpenGL program with this mesh for rendering.
     *  regarding uniform bindings, see general remarks for this class
     */
    void setProgram(QOpenGLShaderProgram *prog);

    /*
     * bind / release the VAO and (if existing) shader program
     * for OpenGL commands to be called after this call
     */
    void bind();
    void release();

    /*
     *  Draw the mesh with its assoicated program by calling
     *  glDrawElements()
     */
    void draw();

    /*
     *  ask for bounding box (without considering transformations)
     */
    const BoundingBox &bbox() const;

protected:
    // mesh data stored one or multiple buffers
    std::unique_ptr<VertexBuffer<QVector3D>> position_;
    std::unique_ptr<VertexBuffer<QVector3D>> normal_;
    std::unique_ptr<VertexBuffer<QVector2D>> texcoord_;
    std::unique_ptr<IndexBuffer> index_;

    // OpenGL vertex array object (VAO) representing the buffers' state
    QOpenGLVertexArrayObject vao_;

    // associate an OpenGL program with this mesh for rendering
    QOpenGLShaderProgram* program_;

    // bbox
    BoundingBox bbox_;

};

