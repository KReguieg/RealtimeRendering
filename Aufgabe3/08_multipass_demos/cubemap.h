#pragma once

#include <memory> // std::shared_ptr
#include <string>
#include <array>
#include <QOpenGLTexture>


/*
 *  Generate an OpenGL Cube Map texture object
 *  from a set of six images in a common subdirectory.
 *  Author:      Hartmut Schirmacher
 *
 *  Designed for cube map images in the format like
 *  it is used here:
 *  http://www.humus.name/index.php?page=Textures
 *
 *  If you choose non-default file names for the six sides,
 *  please ensure that you keep the following order:
 *  +X, +Y, +Z, -X, -Y, -Z
 *
 */
std::shared_ptr<QOpenGLTexture>
makeCubeMap(std::string path_to_images, std::array<std::string,6> sides =
        {{"posx.jpg", "posy.jpg", "posz.jpg", "negx.jpg", "negy.jpg", "negz.jpg"}});



