#pragma once

#include "mesh/mesh.h"
#include "camera.h"
#include <QMatrix4x4>

/*
 *  A Node encapsulates a mesh together with a model transformation.
 *
 *  Furthermore, it can have a vector of child nodes.
 *
 *  When drawing a node, child nodes will be drawn as well
 *  (in depth-first order), and transformations will be
 *  accumulated from parent to all children, in such
 *  way that the child transformation is multiplied
 *  from the right to the parent transformation.
 *
 *
 */
class Node
{
public:

    // constructor
    Node(std::shared_ptr<Mesh> mesh,
         QMatrix4x4 transformation = QMatrix4x4());

    // mesh and transformation
    std::shared_ptr<Mesh> mesh;
    QMatrix4x4 transformation;

    // list of child nodes
    std::vector<std::shared_ptr<Node>> children;

    /*
     * draws the node by:
     * - calculating and setting the model matrix and camera-dependent transformation matrices
     * - drawing the mesh
     */
    void draw(const Camera& cam, unsigned int light_pass = 0,
              QMatrix4x4 parent_transform = QMatrix4x4());

    /*
     *  finds the node within the children of this node.
     *  and return list fo relative transformations
     *  from child coords to this node's parent's coords.
     *  Empty return vector means no child matches node.
     */
    QMatrix4x4 toWorldTransform(std::shared_ptr<Node> child) const;


protected:

    // recursive helper for toParent
    void gatherChildrenTransformations(const Node& node,
                        std::vector<QMatrix4x4>& result,
                        QMatrix4x4 transform) const;


};

