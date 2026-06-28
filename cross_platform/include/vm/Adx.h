// Adx.h - portable port of CAdx: read an ADX mesh of 3D quadratic tetrahedra,
// build the surface topology, and extract surface nodes for selected element
// sets.
#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "vm/AreaMap.h"   // SurfaceNode
#include "vm/Vec3.h"

namespace vm {

enum class NodeType { Undefined, First, Second };

struct AdxNode {
    int      id = -1;
    Vec3     coord;
    NodeType type = NodeType::Undefined;
    std::vector<int> elementFaceIndex;  // faces whose min-vertex is this node
};

struct AdxElement {
    int id = -1;
    int nodeId[10]    = {0};
    int nodeIndex[10] = {0};
    int faceIndex[4]  = {-1, -1, -1, -1};
};

struct AdxElementFace {
    int nodeIndex[6]      = {0};
    int frontElementIndex = -1;
    int backElementIndex  = -1;
    int vertexSorted[3]   = {-1, -1, -1};  // ascending min vertices
};

struct AdxElementSet {
    int         id = 0;
    std::string nameAdx;
    std::string nameUser;
    std::vector<int> elementIndex;
};

class Adx {
public:
    int  Read(const std::string& path);
    int  Activate();
    bool empty() const { return nodes_.empty(); }

    // Surface faces (backElementIndex == -1) of a set, by ADX name.
    int  SurfaceExtract(const std::string& setName,
                        std::vector<int>& outNodes,
                        std::vector<int>& outFaces) const;

    // Build the mapping target set (surface nodes of the named element sets).
    int  ExtractSurfaceNodes(const std::vector<std::string>& setNames,
                             std::vector<SurfaceNode>& out) const;

    // Same, but also return the surface triangles (corner nodes only),
    // referencing the produced node array by index. Used for face projection.
    int  ExtractSurface(const std::vector<std::string>& setNames,
                        std::vector<SurfaceNode>& outNodes,
                        std::vector<TargetFace>& outFaces) const;

    const std::vector<AdxElementSet>& elementSets() const { return elementSets_; }

private:
    bool FaceExist(int a, int b, int c, int& faceIndex) const;

    std::vector<AdxNode>        nodes_;
    std::vector<AdxElement>     elements_;
    std::vector<AdxElementFace> faces_;
    std::vector<AdxElementSet>  elementSets_;

    std::unordered_map<int, int> nodeIdToIndex_;
    std::unordered_map<int, int> elemIdToIndex_;
};

} // namespace vm
