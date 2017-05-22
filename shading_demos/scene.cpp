#include "scene.h"

#include <iostream> // std::cout etc.
#include <assert.h> // assert()
#include <random>   // random number generation

#include "geometries/cube.h" // geom::Cube

using namespace std;



Scene::Scene(QWidget* parent, QOpenGLContext *context) :
    QOpenGLFunctions(context),
    parent_(parent),
    timer_(),
    firstDrawTime_(clock_.now()),
    lastDrawTime_(clock_.now()),
    currentNode_(nullptr)
{

    // load shader source files and compile them into OpenGL program objects
    auto phong_prog = createProgram(":/assets/shaders/myphong.vert", ":/assets/shaders/myphong.frag");
    phongMaterial_ = std::make_shared<PhongMaterial>(phong_prog);

    std::shared_ptr<QOpenGLShaderProgram> prog1, prog2, prog3, prog4, prog5, prog6, prog7;

    prog1 = createProgram(":/assets/shaders/shader-1.vert", ":/assets/shaders/shader-1.frag");
    minimalMaterial_ = std::make_shared<DummyMaterial>(prog1);

    prog2 = createProgram(":/assets/shaders/shader-2.vert", ":/assets/shaders/shader-2.frag");
    uniformMaterial_ = std::make_shared<UniformMaterial>(prog2);
    uniformMaterial_->myUniformColor = QVector3D(0,0,1);

    prog3 = createProgram(":/assets/shaders/shader-3.vert", ":/assets/shaders/shader-3.frag");
    normalsMaterial_ = std::make_shared<DummyMaterial>(prog3);

    prog4 = createProgram(":/assets/shaders/shader-4.vert", ":/assets/shaders/shader-4.frag");
    deform1Material_ = std::make_shared<DummyMaterial>(prog4);

    prog5 = createProgram(":/assets/shaders/shader-5.vert", ":/assets/shaders/shader-5.frag");
    deform2Material_ = std::make_shared<DummyMaterial>(prog5);

    prog6 = createProgram(":/assets/shaders/shader-6.vert", ":/assets/shaders/shader-6.frag");
    animationMaterial_ = std::make_shared<TimeMaterial>(prog6);

    prog7 = createProgram(":/assets/shaders/shader-7.vert", ":/assets/shaders/shader-7.frag");
    texcoordMaterial_ = std::make_shared<DummyMaterial>(prog7);

    // load meshes from .obj files and assign shader programs to them
    auto std = minimalMaterial_;
    meshes_["Duck"] = std::make_shared<Mesh>(":/assets/models/duck/duck.obj", std);
    meshes_["Trefoil"] = std::make_shared<Mesh>(":/assets/models/trefoil.obj", std);

    // add meshes of some procedural geometry objects (not loaded from OBJ files)
    meshes_["Cube"] = std::make_shared<Mesh>(make_shared<geom::Cube>(), std);

    // pack each mesh into a scene node, along with a transform that scales
    // it to standard size [1,1,1]
    nodes_["Duck"]    = createNode(meshes_["Duck"], true);
    nodes_["Trefoil"] = createNode(meshes_["Trefoil"], true);
    nodes_["Cube"]    = createNode(meshes_["Cube"], true);

    // make the duck the current model
    changeModel("Cube");

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

    // make sure we redraw when the timer hits
    connect(&timer_, SIGNAL(timeout()), this, SLOT(update()) );

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

void Scene::changeShader(const QString &txt)
{
    std::shared_ptr<Material> material;
    if(txt == "minimal")
        material = minimalMaterial_;
    else if(txt == "user-defined color")
        material = uniformMaterial_;
    else if(txt == "normal as color")
        material = normalsMaterial_;
    else if(txt == "deform 1")
        material = deform1Material_;
    else if(txt == "deform 2")
        material = deform2Material_;
    else if(txt == "animated")
        material = animationMaterial_;
    else if(txt == "texcoord")
        material = texcoordMaterial_;
    else if(txt == "Phong")
        material = phongMaterial_;

    // detect if the UI did not send one of the expected strings
    assert(material);

    // replace material in all meshes
    for (auto& element : meshes_) {
        auto mesh = element.second;
        mesh->replaceMaterial(material);
    }

    update();

}

void Scene::setNewRandomColor()
{
    // random numbers for random colors
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> random(0.0, 1.0);

    // pick random color
    QVector3D color(random(mt), random(mt), random(mt));

    // set uniform variable in the respective material
    uniformMaterial_->myUniformColor = color;

    // do not forget to trigger redraw
    update();
}

void Scene::toggleAnimation(bool flag)
{
    if(flag) {
        timer_.start(1000.0 / 60.0); // update *roughly* every 60 ms
    } else {
        timer_.stop();
    }
}

void Scene::draw()
{
    assert(currentNode_);
    assert(camera_);

    // calculate animation time
    chrono::milliseconds millisec_since_first_draw;

    if(timer_.isActive()) {
        // calculate total elapsed time and time since last draw call
        auto current = clock_.now();
        millisec_since_first_draw = chrono::duration_cast<chrono::milliseconds>(current - firstDrawTime_);
        lastDrawTime_ = current;
    } else {
        millisec_since_first_draw = chrono::duration_cast<chrono::milliseconds>(lastDrawTime_ - firstDrawTime_);
    }

    // set time uniform in animated shader(s)
    float t = millisec_since_first_draw.count() / 1000.0f;
    animationMaterial_->time = t;

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

