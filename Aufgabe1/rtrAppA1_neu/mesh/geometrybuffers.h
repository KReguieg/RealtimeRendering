#pragma once

#include "mesh/vertexbuffer.h"
#include "indexbuffer.h"
#include "bbox.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>

#include <memory> // std::unique_ptr, std::shared_ptr

/*
 *  GeometryBuffers is an interface that represents
 *  geometry information stored in vertex buffer objects
 *  (VBO) and an element buffer.
 *
 *  When using the bind() method, vertex data will be bound
 *  to the following uniform names:
 *  vertex position     -> uniform vec3 position_MC
 *  vertex normal       -> uniform vec3 normal_MC
 *  texture coordinates -> uniform vec2 texcoord
 *
 *  The suffix _MC indicates model coordinates.
 *
 *  GeometryBuffers does not store a program/material.
 *  The Mesh class combines GeometryBuffers with Material.
 *  One GeometryBuffers object can be shared among
 *  multiple Mesh instances.
 *
 */

class GeometryBuffers {

public:

    /*
     *  ask for bounding box (without considering transformations)
     */
    const BoundingBox &bbox() const;

    // query number of indices in index buffer
    size_t numIndices() const { return index_? (size_t) index_->numElements() : 0; }

    /*
     *  bind buffer objects to uniforms in a program. bindings are recorded in specified VAO.
     */
    virtual void bind(QOpenGLVertexArrayObject& vao, QOpenGLShaderProgram& prog) const;

protected:

    // geometry data stored in one or multiple buffers
    std::unique_ptr<VertexBuffer<QVector3D>> position_;
    std::unique_ptr<VertexBuffer<QVector3D>> normal_;
    std::unique_ptr<VertexBuffer<QVector2D>> texcoord_;
    std::unique_ptr<IndexBuffer> index_;

    // bbox
    BoundingBox bbox_;
};

class GeometryOBJ :public GeometryBuffers {

public:
    /*
     * load geometry information from an OBJ model file
     */
    GeometryOBJ(const std::string& filename);

};
