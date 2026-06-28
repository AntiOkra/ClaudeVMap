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

    // Helper: build an element from already-split fields (free or small field).
    // fields[0]=card, [1]=id, [2]=pid, [3..]=node ids.
    auto elementFromFields = [&](const std::string& type,
                                 const std::vector<std::string>& f) -> bool {
        if (f.size() < 4) return false;
        NastranElement e;
        e.id = ParseInt(f[1]);
        auto fid = [&](std::size_t i) { return (i < f.size()) ? ParseInt(f[i]) : -1; };
        if (type == "CTRIA3") {
            e.type = ElemType::CTRIA3;
            e.nodeId[0] = fid(3); e.nodeId[1] = fid(4); e.nodeId[2] = fid(5); e.nodeId[3] = -1;
        } else if (type == "CQUAD4") {
            if (f.size() < 7) return false;
            e.type = ElemType::CQUAD4;
            e.nodeId[0] = fid(3); e.nodeId[1] = fid(4); e.nodeId[2] = fid(5); e.nodeId[3] = fid(6);
        } else { // CBEAM
            e.type = ElemType::CBEAM;
            e.nodeId[0] = fid(3); e.nodeId[1] = fid(4); e.nodeId[2] = -1; e.nodeId[3] = -1;
        }
        elements_.push_back(e);
        return true;
    };

    std::string line;
    int line_count = 0;
    int skipped = 0;
    while (std::getline(in, line)) {
        ++line_count;

        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '$') continue;  // blank / comment

        const bool freeFormat = line.find(',') != std::string::npos;

        if (freeFormat) {
            // Free format: comma-separated fields (GRID/elements, short form).
            std::vector<std::string> f = Split(line, ",");
            for (auto& s : f) s = Trim(s);
            if (f.empty()) { ++skipped; continue; }
            const std::string card = f[0];
            if (card == "GRID" && f.size() >= 6) {
                NastranNode n;
                n.id = ParseInt(f[1]);
                n.coord.x = ParseDouble(f[3]);
                n.coord.y = ParseDouble(f[4]);
                n.coord.z = ParseDouble(f[5]);
                nodes_.push_back(n);
            } else if (card == "CTRIA3" || card == "CQUAD4" || card == "CBEAM") {
                if (!elementFromFields(card, f)) ++skipped;
            } else {
                ++skipped;
            }
            continue;
        }

        if (StartsWith(line, "GRID*")) {
            std::string line2;
            if (!std::getline(in, line2)) {
                Log(log, "ERROR: GRID* missing continuation line near line " +
                         std::to_string(line_count) + "\n");
                return 1;
            }
            ++line_count;
            NastranNode n;
            // Fixed-column long-format layout, identical to CNastranNode::Read.
            n.id      = ParseInt   (SafeMid(line, 16, 8));
            n.coord.x = ParseDouble(SafeMid(line, 40, 16));
            n.coord.y = ParseDouble(SafeMid(line, 56, 16));
            n.coord.z = ParseDouble(SafeMid(line2, 8, 16));
            nodes_.push_back(n);
        } else if (StartsWith(line, "GRID")) {
            // Small-field (8-column) GRID: id at 8, x/y/z at 24/32/40.
            NastranNode n;
            n.id      = ParseInt   (SafeMid(line, 8, 8));
            n.coord.x = ParseDouble(SafeMid(line, 24, 8));
            n.coord.y = ParseDouble(SafeMid(line, 32, 8));
            n.coord.z = ParseDouble(SafeMid(line, 40, 8));
            nodes_.push_back(n);
        } else if (StartsWith(line, "CTRIA3") || StartsWith(line, "CQUAD4") ||
                   StartsWith(line, "CBEAM")) {
            // Small-field (8-column) element layout.
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
        } else {
            ++skipped;  // unrecognised non-comment card
        }
    }

    if (skipped > 0) {
        Log(log, "INFO: skipped " + std::to_string(skipped) +
                 " unrecognised mesh line(s)\n");
    }

    return Indexing(log);
}

int Nastran::Indexing(std::string* log) {
    nodeIdToIndex_.clear();
    int dupNodes = 0;
    for (int i = 0; i < static_cast<int>(nodes_.size()); ++i) {
        auto res = nodeIdToIndex_.insert({nodes_[i].id, i});
        if (!res.second) {
            ++dupNodes;
            res.first->second = i;  // keep the last definition, but flag it
        }
    }
    if (dupNodes > 0) {
        Log(log, "WARNING: " + std::to_string(dupNodes) +
                 " duplicate node ID(s); the last definition wins\n");
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

    // The original skipped a fixed 9-line header. Instead, auto-detect data
    // lines: a data line is exactly "<int> <num> <num> <num>". Anything else
    // (headers of any length, blanks, comments) is skipped. This removes the
    // magic header count and is backward compatible.
    std::string line;
    int line_count = 0;
    int applied = 0;
    while (std::getline(in, line)) {
        ++line_count;
        auto words = Split(line, " \t");
        if (words.size() != 4) continue;
        if (!IsInteger(words[0]) || !IsNumber(words[1]) ||
            !IsNumber(words[2]) || !IsNumber(words[3])) {
            continue;  // header or non-data line
        }

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
        ++applied;
    }
    if (applied == 0) {
        Log(log, "WARNING: no pressure data lines found in " + path + "\n");
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
