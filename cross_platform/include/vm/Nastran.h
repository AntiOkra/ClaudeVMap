// Nastran.h - portable port of CNastran: read a Nastran-ish mesh plus the
// normal/tangent pressure files, then compute nodal force vectors.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "vm/Vec3.h"

namespace vm {

enum class ElemType { CTRIA3, CQUAD4, CBEAM };

struct NastranNode {
    int  id = -1;
    Vec3 coord;
    Vec3 normalPressure;
    Vec3 tangentPressure;
    Vec3 force;     // (normal + tangent) * area * 1000
    double area = 0.0;
};

struct NastranElement {
    int      id = -1;
    ElemType type = ElemType::CTRIA3;
    int      nodeId[4]    = {-1, -1, -1, -1};
    int      nodeIndex[4] = {-1, -1, -1, -1};
    double   area = 0.0;
};

class Nastran {
public:
    // Each call appends to the provided log (human-readable diagnostics).
    int ReadMeshFile(const std::string& path, std::string* log = nullptr);
    int ReadNormalVectorFile(const std::string& path, std::string* log = nullptr);
    int ReadTangentVectorFile(const std::string& path, std::string* log = nullptr);
    int ForceCalc();

    bool empty() const { return nodes_.empty(); }
    Vec3 TotalForce() const;

    std::vector<NastranNode>&       nodes()    { return nodes_; }
    std::vector<NastranElement>&    elements() { return elements_; }
    const std::vector<NastranNode>& nodes() const { return nodes_; }

private:
    int  Indexing(std::string* log);
    int  ReadVectorFile(const std::string& path, bool tangent, std::string* log);

    std::vector<NastranNode>           nodes_;
    std::vector<NastranElement>        elements_;
    std::unordered_map<int, int>       nodeIdToIndex_;
    std::unordered_map<int, int>       elemIdToIndex_;
};

} // namespace vm
