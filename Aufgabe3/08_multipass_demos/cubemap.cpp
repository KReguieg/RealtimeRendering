#include "cubemap.h"

using namespace std;


/*
 * Inspiration for the actual Qt-related code taken from here:
 * https://stackoverflow.com/questions/35137137/how-to-construct-a-cubemap-texture-using-qopengltexture
 */

std::shared_ptr<QOpenGLTexture>
makeCubeMap(string path_to_images, std::array<string, 6> sides)
{

    // load six images for the six sides of the cube
    std::vector<QImage> images;
    for(auto side : sides) {
        QString filename = (path_to_images + "/" + side).c_str();
        images.push_back( QImage(filename)./*mirrored().*/
                          convertToFormat(QImage::Format_RGBA8888) );
    }

    // create and allocate cube map texture
    std::shared_ptr<QOpenGLTexture> tex_;
    tex_ = std::make_shared<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);
    tex_->create();
    tex_->setSize(images[0].width(), images[0].height(), images[0].depth());
    tex_->setFormat(QOpenGLTexture::RGBA8_UNorm);
    tex_->allocateStorage();

    // the file names in array sides must match this order of cube faces (!)
    std::array<QOpenGLTexture::CubeMapFace, 6> faces =
        {{QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::CubeMapPositiveY,
          QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::CubeMapNegativeX,
          QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::CubeMapNegativeZ }};

    // set texture image data for each side
    for(auto i : {0,1,2,3,4,5})
        tex_->setData( 0, 0, faces[i],
                      QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                      (const void*)images[i].constBits(), 0 );

    // texture parameters and mip-map generation
    tex_->setWrapMode(QOpenGLTexture::ClampToEdge);
    tex_->setMagnificationFilter(QOpenGLTexture::Linear);
    tex_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    tex_->generateMipMaps();

    return tex_;
}

