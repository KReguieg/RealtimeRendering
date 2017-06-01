#pragma once

#include <vector> // std::vector
#include <memory> // std::shared_ptr, std::unique_ptr

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

/*
 * convenience class for an OpenGL vertex buffer object (VBO)
 *
 */

template<typename T>
class VertexBuffer : protected QOpenGLFunctions {
public:

    // construct with name and from std::vector array data
    VertexBuffer(const std::vector<T>& data,
        QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::StaticDraw);

    // bind associated buffer
    void bind();

    // numer of data elements (of type T) in this buffer
    size_t numElements() const { return num_elements_; }

private:

    QOpenGLBuffer buffer_;
    size_t num_elements_;

};


// -----------------------------------------------------------------
// template implementation follows
// -----------------------------------------------------------------

template<typename T>
VertexBuffer<T>::VertexBuffer(const std::vector<T>& data,
                              QOpenGLBuffer::UsagePattern usage)
    : buffer_(QOpenGLBuffer::VertexBuffer),
      num_elements_(data.size())

{
    // don't create anything if there is no data
    if(data.size() == 0)
        return;

    // create new vertex buffer
    if(!buffer_.create())
        qFatal("Unable to create vertex buffer");

    // set usage pattern and copy data into buffer
    buffer_.bind();
    buffer_.setUsagePattern(usage);
    buffer_.allocate(data.data(), int(data.size() * sizeof(T)));
    buffer_.release();

}

template<typename T>
void
VertexBuffer<T>::bind()
{
    if(!buffer_.bind())
        qFatal("could not bind VBO");
}

