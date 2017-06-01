#include "node.h"

using namespace std;

Node::Node(shared_ptr<Mesh> mesh,
           QMatrix4x4 transformation)
    : mesh(mesh), transformation(transformation), children()
{

}


void
Node::draw(const Camera &cam, QMatrix4x4 parent_transform) {

    // chain this transformation with the parent's transformation
    QMatrix4x4 transform = parent_transform * transformation;

    // process children first
    for(auto child : children)
        child->draw(cam,transform);

    if(mesh) {
        // set uniforms for model matrix, modelview matrix, MVP matrix, normal matrix, etc.
        auto& prog = mesh->material()->program();
        cam.setMatrices(prog, transform);

        // issues actual draw call, draw mesh using current uniform values
        mesh->draw();
    }

}
