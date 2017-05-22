#include "scene.h"

#include <iostream> // std::cout etc.
#include <assert.h> // assert()

#include "geometries/cube.h" // geom::Cube

using namespace std;

Scene::Scene(QWidget* parent, QOpenGLContext *context) :
    QOpenGLFunctions(context),
    parent_(parent),
    currentNode_(nullptr)
{

    // load shader source files and compile them into OpenGL program objects
    auto phong_prog = createProgram(":/assets/shaders/cel.vert", ":/assets/shaders/cel.frag");
    auto dot_prog = createProgram(":/assets/shaders/dot.vert", ":/assets/shaders/dot.frag");

    // create required materials
    dotMaterial_ = std::make_shared<DotMaterial>(dot_prog);
    material_ = std::make_shared<PhongMaterial>(phong_prog);

    // store materials in map container
    materials_["dot_prog"] = dotMaterial_;
    materials_["phong_prog"] = material_;

    // load meshes from .obj files and assign shader programs to them
    meshes_["Duck"] = std::make_shared<Mesh>(":/assets/models/duck/duck.obj", material_);
    meshes_["Trefoil"] = std::make_shared<Mesh>(":/assets/models/trefoil.obj", material_);
    meshes_["Cube"] = std::make_shared<Mesh>(make_shared<geom::Cube>(), dotMaterial_);
    meshes_["Bunny"] = std::make_shared<Mesh>(":/assets/models/bunny.obj", material_);
    meshes_["Teddy"] = std::make_shared<Mesh>(":/assets/models/teddy.obj", material_);
    meshes_["Teapot"] = std::make_shared<Mesh>(":/assets/models/teapot/teapot.obj", dotMaterial_);

    // add meshes of some procedural geometry objects (not loaded from OBJ files) 

    // pack each mesh into a scene node, along with a transform that scales
    // it to standard size [1,1,1]
    nodes_["Duck"]    = createNode(meshes_["Duck"], true);
    nodes_["Trefoil"] = createNode(meshes_["Trefoil"], true);
    nodes_["Cube"]    = createNode(meshes_["Cube"], true);
    nodes_["Bunny"]   = createNode(meshes_["Bunny"], true);
    nodes_["Teddy"]   = createNode(meshes_["Teddy"], true);
    nodes_["Teapot"]   = createNode(meshes_["Teapot"], true);

    // make the duck the current model
    changeModel("Duck");

    // create default camera (0,0,4) -> (0,0,0), 45°
    float aspect = float(parent->width())/float(parent->height());
    camera_ = std::make_shared<Camera>(
                QVector3D(0,0.3f,3), // look from
                QVector3D(0,0,0), // look to
                QVector3D(0,1,0), // this way is up
                30.0,   // field of view in up direction
                aspect, // aspect ratio
                0.01,   // near plane
                10.0    // far plane
                );

}

// helper to load shaders and create programs
shared_ptr<QOpenGLShaderProgram>
Scene::createProgram(const string& vertex, const string& fragment)
{
    auto p = make_shared<QOpenGLShaderProgram>();
    if(!p->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex.c_str()))
        qFatal("could not add vertex shader");
    if(!p->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment.c_str()))
        qFatal("could not add fragment shader");
    if(!p->link())
        qFatal("could not link shader program");

    return p;
}

// helper to make a node from a mesh, and
// scale the mesh to standard size 1 of desired
shared_ptr<Node>
Scene::createNode(shared_ptr<Mesh> mesh,
                  bool scale_to_1)
{
    float r = mesh->geometry()->bbox().maxExtent();
    QMatrix4x4 transform;
    if(scale_to_1)
        transform.scale(QVector3D(1.0/r,1.0/r,1.0/r));

    return make_shared<Node>(mesh,transform);
}

void Scene::changeModel(const QString &txt)
{
    currentNode_ = nodes_[txt];
    if(!currentNode_)
        qFatal("scene: desired mesh/node not found");

    update();

}

void Scene::changeShadingLevel(const float &shadingLevels)
{
    std::cout << "In changeShadingLevel= " << shadingLevels << endl;
    material_->shadingLevels = shadingLevels;

    update();
}

void Scene::changeDotRadius(const float &dotRadius) {
    std::cout << "In changeDotRadius= " << dotRadius << endl;
    dotMaterial_->radius = dotRadius;
    update();
}

void Scene::changeDotDensity(const float &dotDensity) {
    std::cout << "In changeDotDensity= " << dotDensity << endl;
    dotMaterial_->density = 100.0/dotDensity;
    update();
}

void Scene::changeDotColor(const QVector4D &dotColor) {
    std::cout << "In changeDotColor= " << dotColor.x() << dotColor.y() << dotColor.z() << endl;
    dotMaterial_->color = dotColor;
    update();
}

void Scene::draw()
{
    assert(currentNode_);
    assert(camera_);

    // clear background
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // draw selected node, apply current world transformation
    currentNode_->draw(*camera_, worldTransform_);

}

void Scene::updateViewport(size_t width, size_t height)
{
    camera_->setAspectRatio(float(width)/float(height));
    glViewport(0,0,GLint(width),GLint(height));
}

