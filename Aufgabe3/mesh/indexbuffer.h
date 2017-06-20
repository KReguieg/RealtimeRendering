#pragma once

#include <vector> // std::vector
#include <memory> // std::shared_ptr, std::unique_ptr
#include <type_traits> // is_integral etc.

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

/*
 *  convenience class for an OpenGL index buffer
 *  This is a vertex buffer, interpreted differently by OpenGL.
 *  It holds indices (unsigned int) connecting vertices to describe
 *  primitives (lines or triangles).
 *
 */

class IndexBuffer : protected QOpenGLFunctions {
public:

    using T = unsigned int;

    // construct with name and from std::vector array data
    IndexBuffer(const std::vector<T>& data,
        QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::StaticDraw);

    // bind associated buffer
    void bind();

    // numer of data elements (of type T) in this buffer
    size_t numElements() const { return num_elements_; }

private:

    QOpenGLBuffer buffer_;
    size_t num_elements_;

};


