#include "vm/Nastran.h"
#include "vm/StringUtil.h"

#include <fstream>

namespace vm {

namespace {
void Log(std::string* log, const std::string& msg) {
    if (log) *log += msg;
}
}

int Nastran::ReadMeshFile(const std::string& path, std::string* log) {
    nodes_.clear();
    elements_.clear();
    nodeIdToIndex_.clear();
    elemIdToIndex_.clear();

    std::ifstream in(path);
    if (!in) {
        Log(log, "ERROR: cannot open mesh file: " + path + "\n");
        return 1;
    }

    std::string line;
    int line_count = 0;
    while (std::getline(in, line)) {
        ++line_count;

        if (StartsWith(line, "GRID*")) {
            std::string line2;
            if (!std::getline(in, line2)) {
                Log(log, "ERROR: GRID* missing continuation line near line " +
                         std::to_string(line_count) + "\n");
                return 1;
            }
            ++line_count;
            NastranNode n;
            // Fixed-column layout, identical to CNastranNode::Read.
            n.id      = ParseInt   (SafeMid(line, 16, 8));
            n.coord.x = ParseDouble(SafeMid(line, 40, 16));
            n.coord.y = ParseDouble(SafeMid(line, 56, 16));
            n.coord.z = ParseDouble(SafeMid(line2, 8, 16));
            nodes_.push_back(n);
        } else if (StartsWith(line, "CTRIA3") || StartsWith(line, "CQUAD4") ||
                   StartsWith(line, "CBEAM")) {
            NastranElement e;
            const std::string type = Trim(SafeMid(line, 0, 6));
            e.id = ParseInt(SafeMid(line, 8, 8));
            if (type == "CTRIA3") {
                e.type = ElemType::CTRIA3;
                e.nodeId[0] = ParseInt(SafeMid(line, 24, 8));
                e.nodeId[1] = ParseInt(SafeMid(line, 32, 8));
                e.nodeId[2] = ParseInt(SafeMid(line, 40, 8));
                e.nodeId[3] = -1;
            } else if (type == "CQUAD4") {
                e.type = ElemType::CQUAD4;
                e.nodeId[0] = ParseInt(SafeMid(line, 24, 8));
                e.nodeId[1] = ParseInt(SafeMid(line, 32, 8));
                e.nodeId[2] = ParseInt(SafeMid(line, 40, 8));
                e.nodeId[3] = ParseInt(SafeMid(line, 48, 8));
            } else { // CBEAM
                e.type = ElemType::CBEAM;
                e.nodeId[0] = ParseInt(SafeMid(line, 24, 8));
                e.nodeId[1] = ParseInt(SafeMid(line, 32, 8));
                e.nodeId[2] = -1;
                e.nodeId[3] = -1;
            }
            elements_.push_back(e);
        }
    }

    return Indexing(log);
}

int Nastran::Indexing(std::string* log) {
    nodeIdToIndex_.clear();
    for (int i = 0; i < static_cast<int>(nodes_.size()); ++i) {
        nodeIdToIndex_[nodes_[i].id] = i;
    }
    elemIdToIndex_.clear();
    for (int i = 0; i < static_cast<int>(elements_.size()); ++i) {
        elemIdToIndex_[elements_[i].id] = i;
    }

    for (auto& e : elements_) {
        int node_cnt = 4;
        if (e.type == ElemType::CTRIA3)      node_cnt = 3;
        else if (e.type == ElemType::CBEAM)  node_cnt = 2;

        for (int j = 0; j < node_cnt; ++j) {
            auto it = nodeIdToIndex_.find(e.nodeId[j]);
            if (it == nodeIdToIndex_.end()) {
                Log(log, "ERROR: Element(" + std::to_string(e.id) +
                         ") refers to unknown Node(" + std::to_string(e.nodeId[j]) + ")\n");
                return 1;
            }
            e.nodeIndex[j] = it->second;
        }
    }
    return 0;
}

int Nastran::ReadNormalVectorFile(const std::string& path, std::string* log) {
    return ReadVectorFile(path, /*tangent=*/false, log);
}

int Nastran::ReadTangentVectorFile(const std::string& path, std::string* log) {
    return ReadVectorFile(path, /*tangent=*/true, log);
}

int Nastran::ReadVectorFile(const std::string& path, bool tangent, std::string* log) {
    std::ifstream in(path);
    if (!in) {
        Log(log, "ERROR: cannot open pressure file: " + path + "\n");
        return 1;
    }

    std::string line;
    int line_count = 0;
    // Skip 9 header lines (matches the original).
    for (int i = 0; i < 9; ++i) {
        ++line_count;
        if (!std::getline(in, line)) {
            Log(log, "ERROR: pressure file too short (header) near line " +
                     std::to_string(line_count) + "\n");
            return 1;
        }
    }

    while (std::getline(in, line)) {
        ++line_count;
        auto words = Split(line, " ");
        if (words.size() != 4) continue;

        const int id = ParseInt(words[0]);
        auto it = nodeIdToIndex_.find(id);
        if (it == nodeIdToIndex_.end()) {
            Log(log, "ERROR: pressure refers to unknown Node(" + std::to_string(id) +
                     ") at line " + std::to_string(line_count) + "\n");
            return 1;
        }
        Vec3& target = tangent ? nodes_[it->second].tangentPressure
                               : nodes_[it->second].normalPressure;
        target.x = ParseDouble(words[1]);
        target.y = ParseDouble(words[2]);
        target.z = ParseDouble(words[3]);
    }
    return 0;
}

int Nastran::ForceCalc() {
    for (auto& e : elements_) {
        if (e.type == ElemType::CTRIA3) {
            NastranNode& n0 = nodes_[e.nodeIndex[0]];
            NastranNode& n1 = nodes_[e.nodeIndex[1]];
            NastranNode& n2 = nodes_[e.nodeIndex[2]];
            e.area = AreaTriangle(n0.coord, n1.coord, n2.coord);
            const double a = e.area / 3.0;
            n0.area += a; n1.area += a; n2.area += a;
        } else if (e.type == ElemType::CQUAD4) {
            NastranNode& n0 = nodes_[e.nodeIndex[0]];
            NastranNode& n1 = nodes_[e.nodeIndex[1]];
            NastranNode& n2 = nodes_[e.nodeIndex[2]];
            NastranNode& n3 = nodes_[e.nodeIndex[3]];
            e.area = AreaQuad(n0.coord, n1.coord, n2.coord, n3.coord);
            const double a = e.area / 4.0;
            n0.area += a; n1.area += a; n2.area += a; n3.area += a;
        } else { // CBEAM
            NastranNode& n0 = nodes_[e.nodeIndex[0]];
            NastranNode& n1 = nodes_[e.nodeIndex[1]];
            e.area = n0.coord.Distance(n1.coord);
            const double a = e.area / 2.0;
            n0.area += a; n1.area += a;
        }
    }

    for (auto& n : nodes_) {
        const Vec3 pressure = n.normalPressure + n.tangentPressure;
        n.force = pressure * (n.area * 1000.0); // unit: N
    }
    return 0;
}

Vec3 Nastran::TotalForce() const {
    Vec3 total;
    for (const auto& n : nodes_) total += n.force;
    return total;
}

} // namespace vm
