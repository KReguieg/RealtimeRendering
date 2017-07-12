#include "scene.h"

#include <iostream> // std::cout etc.
#include <assert.h> // assert()
#include <random>   // random number generation

#include "geometries/cube.h" // geom::Cube
#include "geometries/parametric.h" // geom::Sphere etc.
#include "cubemap.h"

#include <QtMath>
#include <QMessageBox>

using namespace std;

Scene::Scene(QWidget* parent, QOpenGLContext *context) :
    QOpenGLFunctions(context),
    parent_(parent),
    timer_(),
    firstDrawTime_(clock_.now()),
    lastDrawTime_(clock_.now())
{

    // check some OpenGL parameters
    {
        int minor, major;
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        cout << "OpenGL context version " << major << "." << minor << endl;

        int texunits_frag, texunits_vert;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texunits_frag);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &texunits_vert);
        cout << "texture units: " << texunits_frag << " (frag), "
             << texunits_vert << " (vert)" << endl;

        int texsize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texsize);
        cout << "max texture size: " << texsize << "x" << texsize << endl;
    }

    // construct map of nodes
    makeNodes();

    // from the nodes, construct a hierarchical scene (adding more nodes)
    makeScene();

    // initialize navigation
    navigator_ = std::make_unique<ModelTrackball>(nodes_["Scene"], nodes_["World"], nodes_["Camera"]);
    lightNavigator_ = std::make_unique<PositionNavigator>(nodes_["Light0"], nodes_["World"], nodes_["Camera"]);
    cameraNavigator_ = std::make_unique<RotateCameraY>(nullptr, nullptr, nodes_["Camera"]);

    // make sure we redraw when the timer hits
    connect(&timer_, SIGNAL(timeout()), this, SLOT(update()) );

}

void Scene::makeNodes()
{
    // load 2D textures
    auto day       = std::make_shared<QOpenGLTexture>(QImage("qrc:/assets/textures/earth_day.jpg").mirrored());
    auto night     = std::make_shared<QOpenGLTexture>(QImage(":/assets/textures/earth_at_night_2048.jpg").mirrored());
    auto gloss     = std::make_shared<QOpenGLTexture>(QImage(":/assets/textures/earth_bathymetry_2048.jpg").mirrored());
    auto disp      = std::make_shared<QOpenGLTexture>(QImage(":/assets/textures/earth_topography_2048.jpg").mirrored());
    auto bumps     = std::make_shared<QOpenGLTexture>(QImage(":/assets/textures/earth_topography_2048_NRM.png").mirrored());
    auto rtrsuper  = std::make_shared<QOpenGLTexture>(QImage(":/assets/textures/RTR-ist-super-4-3.png").mirrored());

    auto wallTex      = std::make_shared<QOpenGLTexture>(QImage(":/assets/wall.jpg").mirrored());

    // load cube textures
    std::shared_ptr<QOpenGLTexture> cubetex = makeCubeMap(":/assets/textures/bridge2048");

    // load shader source files and compile them into OpenGL program objects
    auto phong_prog = createProgram(":/assets/shaders/textured_phong.vert", ":/assets/shaders/textured_phong.frag");

    // make multiple instances of (non-) textured Phong material
    materials_["red"] = std::make_shared<TexturedPhongMaterial>(phong_prog,1);
    materials_["red"]->phong.k_diffuse = QVector3D(0.8f,0.1f,0.1f);
    materials_["red"]->phong.k_ambient = materials_["red"]->phong.k_diffuse * 0.3f;
    materials_["red"]->phong.shininess = 80;
    materials_["red"]->diffuseTexture = wallTex;
    materials_["red"]->tex.useDiffuseTexture = true;
    materials_["red"]->envmap.useEnvironmentTexture = true;
    materials_["red"]->environmentTexture = cubetex;
    materials_["red_original"] = std::make_shared<TexturedPhongMaterial>(*materials_["red"]);
    auto std = materials_["red"];

    // make multiple instances of (non-) textured Phong material
    materials_["green"] = std::make_shared<TexturedPhongMaterial>(phong_prog,1);
    materials_["green"]->phong.k_diffuse = QVector3D(0.1f,0.8f,0.1f);
    materials_["green"]->phong.k_ambient = materials_["green"]->phong.k_diffuse * 0.3f;
    materials_["green"]->phong.shininess = 80;
    materials_["green"]->envmap.useEnvironmentTexture = true;
    materials_["green"]->environmentTexture = cubetex;
    materials_["green"] = std::make_shared<TexturedPhongMaterial>(*materials_["green"]);
    auto std1 = materials_["green"];

    materials_["wall"] = std::make_shared<TexturedPhongMaterial>(phong_prog,1);
    materials_["wall"]->phong.k_diffuse = QVector3D(0.1f,0.8f,0.1f);
    materials_["wall"]->phong.k_ambient = materials_["wall"]->phong.k_diffuse * 0.3f;
    materials_["wall"]->phong.shininess = 80;
    materials_["wall"]->diffuseTexture = wallTex;
    materials_["wall"]->envmap.useEnvironmentTexture = true;
    materials_["wall"]->environmentTexture = cubetex;
    materials_["wall"] = std::make_shared<TexturedPhongMaterial>(*materials_["wall"]);
    auto wall = materials_["wall"];

    // post processing stuff, in separate tex units 10-12
    auto orig = createProgram(":/assets/shaders/post.vert",
                              ":/assets/shaders/original.frag");
    post_materials_["original"] = make_shared<PostMaterial>(orig, 10);

    // depth of field program
    auto depth_of_field = createProgram(":assets/shaders/post.vert", ":/assets/shaders/depth_of_field.frag");
    post_materials_["depth_of_field"] = make_shared<PostMaterial>(depth_of_field, 13);

    auto blur = createProgram(":/assets/shaders/post.vert",
                              ":/assets/shaders/blur.frag");
    post_materials_["blur"] = make_shared<PostMaterial>(blur, 11);

    auto gaussA = createProgram(":/assets/shaders/post.vert",
                                ":/assets/shaders/gauss_9x9_passA.frag");
    auto gaussB = createProgram(":/assets/shaders/post.vert",
                                ":/assets/shaders/gauss_9x9_passB.frag");
    post_materials_["gauss_1"] = make_shared<PostMaterial>(gaussA,11);
    post_materials_["gauss_2"] = make_shared<PostMaterial>(gaussB,12);

    // load meshes from .obj files and assign shader programs to them
    meshes_["Duck"]    = std::make_shared<Mesh>(":/assets/models/duck/duck.obj", std);
    meshes_["Teapot"]  = std::make_shared<Mesh>(":/assets/models/teapot/teapot.obj", std);

    // add meshes of some procedural geometry objects (not loaded from OBJ files)
    meshes_["Cube"]         = std::make_shared<Mesh>(make_shared<geom::Cube>(), std);
    meshes_["Cube1"]        = std::make_shared<Mesh>(make_shared<geom::Cube>(), std1);
    meshes_["Cube2"]        = std::make_shared<Mesh>(make_shared<geom::Cube>(), std1);
    meshes_["FloorRect"]    = std::make_shared<Mesh>(make_shared<geom::Rect>(500,500), wall);
    meshes_["LeftRect"]     = std::make_shared<Mesh>(make_shared<geom::Rect>(500,500), wall);
    meshes_["RightRect"]    = std::make_shared<Mesh>(make_shared<geom::Rect>(500,500), wall);
    meshes_["Sphere"]       = std::make_shared<Mesh>(make_shared<geom::Sphere>(80,80), std);
    meshes_["Torus"]        = std::make_shared<Mesh>(make_shared<geom::Torus>(4, 2, 80,20), std);

    // full-screen rectangles for post processing
    meshes_["original"]  = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1),
                                                  post_materials_["original"]);
    nodes_["original"]   = createNode(meshes_["original"], false);

    meshes_["blur"]      = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1),
                                                  post_materials_["blur"]);
    nodes_["blur"]       = createNode(meshes_["blur"], false);

    meshes_["depth_of_field"]   = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1), post_materials_["depth_of_field"]);
    nodes_["depth_of_field"]    = createNode(meshes_["depth_of_field"], false);

    meshes_["gauss_1"]   = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1), post_materials_["gauss_1"]);
    nodes_["gauss_1"]    = createNode(meshes_["gauss_1"], false);
    meshes_["gauss_2"]   = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1), post_materials_["gauss_2"]);
    nodes_["gauss_2"]    = createNode(meshes_["gauss_2"], false);

    // initial state of post processing phases
    nodes_["post_pass_1"] = nodes_["depth_of_field"];
    nodes_["post_pass_2"] = nullptr;

    // pack each mesh into a scene node, along with a transform that scales
    // it to standard size [1,1,1]
    nodes_["Cube"]    = createNode(meshes_["Cube"], true);
    nodes_["Cube1"]    = createNode(meshes_["Cube1"], true);
    nodes_["Cube2"]    = createNode(meshes_["Cube2"], true);

    nodes_["Sphere"]  = createNode(meshes_["Sphere"], true);
    nodes_["Torus"]   = createNode(meshes_["Torus"], true);
    nodes_["Duck"]    = createNode(meshes_["Duck"], true);
    nodes_["Teapot"]  = createNode(meshes_["Teapot"], true);

}

// once the nodes_ map is filled, construct a hierarchical scene from it
void Scene::makeScene()
{
    // world contains the scene plus the camera
    nodes_["World"] = createNode(nullptr, false);

    // scene means everything but the camera
    nodes_["Scene"] = createNode(nullptr, false);
    nodes_["World"]->children.push_back(nodes_["Scene"]);

    // initial model to be shown in the scene
    nodes_["Scene"]->children.push_back(nodes_["Cube"]);
    nodes_["Scene"]->children.push_back(nodes_["Cube1"]);
    nodes_["Scene"]->children.push_back(nodes_["Cube2"]);

    // add camera node
    nodes_["Camera"] = createNode(nullptr, false);
    nodes_["Camera"]->transformation.translate(QVector3D(0,0.5,3)); // move camera back and up a bit
    nodes_["Camera"]->transformation.rotate(-7.5, QVector3D(1,0,0)); // look down on scene
    nodes_["World"]->children.push_back(nodes_["Camera"]);

    // add a light relative to the world
    nodes_["Light0"] = createNode(nullptr, false);
    nodes_["World"]->children.push_back(nodes_["Light0"]);
    lightNodes_.push_back(nodes_["Light0"]);
    nodes_["Light0"]->transformation.translate(QVector3D(-0.55f, 0.68f, 4.34f)); // above camera

    // translate new cubes into the background
    nodes_["Cube1"]->transformation.translate(QVector3D(-1.0f, 0.0f, -5.0f));
    nodes_["Cube2"]->transformation.translate(QVector3D(1.0f, 0.0f, -2.0f));
}


// helper to load shaders and create programs
shared_ptr<QOpenGLShaderProgram>
Scene::createProgram(const string& vertex, const string& fragment, const string& geom)
{
    auto p = make_shared<QOpenGLShaderProgram>();
    if(!p->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex.c_str()))
        qFatal("could not add vertex shader");
    if(!p->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment.c_str()))
        qFatal("could not add fragment shader");
    if(!geom.empty()) {
        if(!p->addShaderFromSourceFile(QOpenGLShader::Geometry, geom.c_str()))
            qFatal("could not add geometryshader");
    }
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
    QMatrix4x4 transform;
    if(scale_to_1) {
        float r = mesh->geometry()->bbox().maxExtent();
        transform.scale(QVector3D(1.0/r,1.0/r,1.0/r));
    }

    return make_shared<Node>(mesh,transform);
}


void Scene::toggleAnimation(bool flag)
{
    if(flag) {
        timer_.start(1000.0 / 60.0); // update *roughly* every 60 ms
    } else {
        timer_.stop();
    }
}

void Scene::setSceneNode(QString node)
{
    auto n = nodes_[node];
    assert(n);

    nodes_["Scene"]->children.clear();
    nodes_["Scene"]->children.push_back(n);

    update();
}

// change background color
void Scene::setBackgroundColor(QVector3D rgb) {
    bgcolor_ = rgb; update();
}
// methods to change common material parameters
void Scene::setLightIntensity(size_t i, float v)
{
    if(i>=lightNodes_.size())
        return;
    for(auto mat : materials_)
        mat.second->lights[i].intensity = v; update();
}
void Scene::setAmbientScale(float v)
{
    materials_["red"]->phong.k_ambient = materials_["red_original"]->phong.k_ambient * v;
    update();
}
void Scene::setDiffuseScale(float v)
{
    materials_["red"]->phong.k_diffuse = materials_["red_original"]->phong.k_diffuse * v;
    update();
}
void Scene::setSpecularScale(float v)
{
    materials_["red"]->phong.k_specular = materials_["red_original"]->phong.k_specular * v;
    update();
}
void Scene::setShininess(float v)
{
    materials_["red"]->phong.shininess = v;
    update();
}

// change kernel size of al post processing filters
void Scene::setPostFilterKernelSize(int n) {
    for(auto mat : post_materials_)
        mat.second->kernel_size = QSize(n,n);
    update();
}

// change post processing filter
void Scene::useDepthOfField() {
    nodes_["post_pass_1"] = nodes_["depth_of_field"];
    nodes_["post_pass_2"] = nullptr;
    update();
}
void Scene::useTwoPassGauss() {
    nodes_["post_pass_1"] = nodes_["gauss_1"];
    nodes_["post_pass_2"] = nodes_["gauss_2"];
    update();
}
void Scene::toggleJittering(bool value)
{
    for(auto m : post_materials_) m.second->use_jitter = value;
    update();
}
void Scene::toggleSplitDisplay(bool value)
{
    split_display_ = value;
    update();
}
void Scene::toggleFBODisplay(bool value)
{
    show_FBOs_ = value;
    update();
}

// pass key/mouse events on to navigator objects
void Scene::keyPressEvent(QKeyEvent *event) {

    // if Alt is pressed, pass on to light navigator, else camera navigator
    if(event->modifiers() & Qt::AltModifier) {
        lightNavigator_->keyPressEvent(event);
    } else {
        cameraNavigator_->keyPressEvent(event);
    }
    update();

}
// mouse press events all processed by trackball navigator
void Scene::mousePressEvent(QMouseEvent *event)
{
    navigator_->mousePressEvent(event); update();
}
void Scene::mouseMoveEvent(QMouseEvent *event)
{
    navigator_->mouseMoveEvent(event); update();
}
void Scene::mouseReleaseEvent(QMouseEvent *event)
{
    navigator_->mouseReleaseEvent(event); update();
}
void Scene::wheelEvent(QWheelEvent *event) {
    navigator_->wheelEvent(event); update();
}

// trigger a redraw of the widget through this method
void Scene::update()
{
    parent_->update();
}

void Scene::updateViewport(size_t width, size_t height)
{
    glViewport(0,0,GLint(width),GLint(height));

    // discard existing FBOs; need to re-create with new size
    fbo1_.reset();
    fbo2_.reset();
}

void Scene::draw()
{
    // calculate animation time
    chrono::milliseconds millisec_since_first_draw;
    chrono::milliseconds millisec_since_last_draw;

    // calculate total elapsed time and time since last draw call
    auto current = clock_.now();
    millisec_since_first_draw = chrono::duration_cast<chrono::milliseconds>(current - firstDrawTime_);
    millisec_since_last_draw = chrono::duration_cast<chrono::milliseconds>(current - lastDrawTime_);
    lastDrawTime_ = current;

    // set time uniform in animated shader(s)
    float t = millisec_since_first_draw.count() / 1000.0f;
    for(auto mat : materials_)
        mat.second->time = t;

    // create an FBO to render the scene into
    if(!fbo1_) {

        // for high-res Retina displays
        auto pixel_scale = parent_->devicePixelRatio();

        // what kind of FBO do we want?
        auto fbo_format = QOpenGLFramebufferObjectFormat();
        fbo_format.setAttachment(QOpenGLFramebufferObject::Depth);

        // create some FBOs for post processing
        fbo1_ = std::make_shared<QOpenGLFramebufferObject>(parent_->width()*pixel_scale,
                                                          parent_->height()*pixel_scale,
                                                          fbo_format);
        fbo2_ = std::make_shared<QOpenGLFramebufferObject>(parent_->width()*pixel_scale,
                                                           parent_->height()*pixel_scale,
                                                           fbo_format);
        // qDebug() << "FBO size =" << fbo_->size();
    }

    // draw the actual scene into fbo1
    fbo1_->bind();
    draw_scene_();
    fbo1_->release();
    auto fbo_to_be_rendered = fbo1_;
    auto node_to_be_rendered = nodes_["post_pass_1"];

    // second pass?
    if(nodes_["post_pass_2"]) {
        fbo2_->bind();
        post_draw_full_(*fbo_to_be_rendered, *node_to_be_rendered);
        fbo2_->release();
        fbo_to_be_rendered = fbo2_;
        node_to_be_rendered = nodes_["post_pass_2"];
    }

    // final rendering pass, into visible framebuffer (object)
    if(split_display_) {
        post_draw_split_(*fbo1_, *nodes_["original"],
                         *fbo_to_be_rendered, *node_to_be_rendered);
    } else {
        post_draw_full_(*fbo_to_be_rendered, *node_to_be_rendered);
    }

    // extract FBI image and display in the UI, every 20 frames
    static size_t framecount=20-2; // initially will render twice
    if(show_FBOs_) {
        if(++framecount % 20 == 0) {
            emit displayBufferContents(0, "rendered scene", fbo1_->toImage());
            if(nodes_["post_pass_2"])
                emit displayBufferContents(1, "post pass 1", fbo2_->toImage());
        }
    }
}

void Scene::draw_scene_()
{

    // set camera based on node in scene graph
    QMatrix4x4 camToWorld = nodes_["World"]->toWorldTransform(nodes_["Camera"]);
    float aspect = float(parent_->width())/float(parent_->height());
    LookAtCamera camera(camToWorld*QVector3D(0,0,0), // look from
                        camToWorld*QVector3D(0,0,-1), // look along -Z
                        camToWorld*QVector3D(0,1,0), // this way is up
                        30.0f,   // field of view in up direction
                        aspect, // aspect ratio
                        0.01f,   // near plane
                        10.0f    // far plane
                        );

    // clear buffer
    glClearColor(bgcolor_[0], bgcolor_[1], bgcolor_[2], 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // first light pass: standard depth test, no blending
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    // draw one pass for each light
    for(unsigned int i=0; i<lightNodes_.size(); i++) {

        // determine current light position and set it in material
        QMatrix4x4 lightToWorld = nodes_["World"]->toWorldTransform(lightNodes_[i]);
        for(auto mat : materials_)
            mat.second->lights[i].position_WC = lightToWorld * QVector3D(0,0,0);

        // draw light pass i
        nodes_["World"]->draw(camera, i);

        // settings for i>0 (add light contributions using alpha blending)
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_ONE);
        glDepthFunc(GL_EQUAL);
    }
}

void Scene::post_draw_full_(QOpenGLFramebufferObject &fbo, Node& node)
{
    // set up transformation matrices
    PostProcessingCamera camera;

    // width and height of FBO / viewport
    int w = fbo.size().width();
    int h = fbo.size().height();

    // use the texture from the FBO during rendering
    for(auto mat : post_materials_) {
        mat.second->post_texture_id = fbo.texture();
        mat.second->image_size = QSize(w,h);
    }

    // initial state for drawing full-viewport rectangles
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // draw single full screen rectangle with post processing material
    node.draw(camera);
}

void Scene::post_draw_split_(QOpenGLFramebufferObject &fbo1, Node& node1,
                             QOpenGLFramebufferObject &fbo2, Node& node2)
{
    // set up transformation matrices
    PostProcessingCamera camera;

    // width and height of FBO / viewport
    int w = fbo1.size().width();
    int h = fbo1.size().height();
    int halfw = w/2;

    // initial state for drawing full-viewport rectangles
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // left half of node1

    // use texture from fbo1 during rendering
    for(auto mat : post_materials_) {
        mat.second->post_texture_id = fbo1.texture();
        mat.second->image_size = QSize(w,h);
    }
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,halfw,h);
    node1.draw(camera);

    // right half of node2

    // use texture from fbo2 during rendering
    for(auto mat : post_materials_) {
        mat.second->post_texture_id = fbo2.texture();
        mat.second->image_size = QSize(w,h);
    }

    glScissor(halfw,0,w-halfw,h);
    node2.draw(camera);
    glDisable(GL_SCISSOR_TEST);
}



