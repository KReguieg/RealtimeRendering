#include "node.h"
#include <assert.h>

using namespace std;

Node::Node(shared_ptr<Mesh> mesh,
           QMatrix4x4 transformation)
    : mesh(mesh), transformation(transformation), children()
{

}


void
Node::draw(const Camera &cam, unsigned int light_pass, QMatrix4x4 parent_transform) {

    // chain this transformation with the parent's transformation
    QMatrix4x4 transform = parent_transform * transformation;

    // process children first
    for(auto child : children)
        child->draw(cam, light_pass, transform);

    if(mesh) {
        // set uniforms for model matrix, modelview matrix, MVP matrix, normal matrix, etc.
        cam.setMatrices(*mesh->material(), transform);

        // issues actual draw call, draw mesh using current uniform values
        mesh->draw(light_pass);
    }

}

void
Node::gatherChildrenTransformations(const Node& node,
                                    std::vector<QMatrix4x4> &result,
                                    QMatrix4x4 transform) const
{

    // apply local transformation
    transform *= this->transformation;

    // found desired node?
    if(this == &node)
        result.push_back(transform);

    // continue search for all children, adding this nodes's transform
    for(auto child : children)
        child->gatherChildrenTransformations(node,result,transform);
}

QMatrix4x4 Node::toWorldTransform(std::shared_ptr<Node> child) const
{
    std::vector<QMatrix4x4> result;

    // gather transformation information recursively
    gatherChildrenTransformations(*child, result, QMatrix4x4());

    // if this assertion fails, the child node was found multiple times
    assert(result.size() == 1);

    return result[0];
}
