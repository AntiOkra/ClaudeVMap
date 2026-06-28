#include "vm/Adx.h"
#include "vm/StringUtil.h"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace vm {

namespace {

// Face -> local node indices for a 3D quadratic (10-node) tetrahedron.
// Matches cElementFaceIndex in the original AdxElement.h.
const int kFaceIndex[4][6] = {
    {1, 2, 3, 7, 8, 9},
    {0, 3, 2, 6, 8, 5},
    {0, 1, 3, 4, 9, 6},
    {0, 2, 1, 5, 7, 4}
};

void Sort3(int a, int b, int c, int& s0, int& s1, int& s2) {
    int v[3] = {a, b, c};
    for (int i = 0; i < 2; ++i)
        for (int j = i + 1; j < 3; ++j)
            if (v[j] < v[i]) std::swap(v[i], v[j]);
    s0 = v[0]; s1 = v[1]; s2 = v[2];
}

// Resolve an $Include path relative to the parent file's directory.
std::string IncludePath(const std::string& parent, const std::string& include) {
    const auto pos = parent.find_last_of("/\\");
    if (pos == std::string::npos) return include;
    return parent.substr(0, pos + 1) + include;
}

// First run of digits in a string -> integer, else 0.
int FirstNumber(const std::string& s) {
    std::size_t i = 0;
    while (i < s.size() && !std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
    if (i >= s.size()) return 0;
    std::size_t j = i;
    while (j < s.size() && std::isdigit(static_cast<unsigned char>(s[j]))) ++j;
    return ParseInt(s.substr(i, j - i));
}

} // namespace

int Adx::Read(const std::string& path) {
    std::ifstream in(path);
    if (!in) return 1;

    enum class State { None, Node, Element };
    State state = State::None;
    const std::string delims = " =:";

    AdxElementSet currentElementSet;
    bool haveElementSet = false;

    std::string line;
    bool eof = false;
    bool reparse = false;

    while (!eof) {
        if (reparse) {
            reparse = false;
        } else if (!std::getline(in, line)) {
            eof = true;
            line = "$EOF";
        }

        if (StartsWith(line, "#")) continue;
        if (Trim(line).empty()) continue;

        switch (state) {
        case State::None: {
            if (StartsWith(line, "$")) {
                auto words = Split(line, delims);
                if (words.empty()) break;
                if (words[0] == "$Include" && words.size() >= 2) {
                    Read(IncludePath(path, words[1]));
                } else if (words[0] == "$Node") {
                    state = State::Node;
                } else if (words[0] == "$Element" && words.size() >= 2 &&
                           words[1] == "3DQuadraticTetrahedron") {
                    state = State::Element;
                }
            }
            break;
        }
        case State::Node: {
            if (StartsWith(line, "$")) {
                state = State::None;
                reparse = true;
                continue;
            }
            auto words = Split(line, delims);
            if (words.empty()) continue;
            if (words[0] == "node_set" || words[0] == "dimension" ||
                words[0] == "index_format" || words[0] == "format") {
                continue;
            }
            if (words.size() >= 4) {
                AdxNode n;
                n.id      = ParseInt(words[0]);
                n.coord.x = ParseDouble(words[1]);
                n.coord.y = ParseDouble(words[2]);
                n.coord.z = ParseDouble(words[3]);
                nodes_.push_back(n);
            }
            break;
        }
        case State::Element: {
            if (StartsWith(line, "$")) {
                if (haveElementSet) {
                    elementSets_.push_back(currentElementSet);
                    haveElementSet = false;
                }
                state = State::None;
                reparse = true;
                continue;
            }
            auto words = Split(line, delims);
            if (words.empty()) continue;
            if (words[0] == "element_set") {
                if (haveElementSet) {
                    elementSets_.push_back(currentElementSet);
                }
                currentElementSet = AdxElementSet{};
                haveElementSet = true;
                if (words.size() >= 2) {
                    currentElementSet.nameAdx = words[1];
                    currentElementSet.id = FirstNumber(words[1]);
                }
                if (words.size() >= 3) {
                    std::string c = words[2];
                    if (!c.empty() && c[0] == '#') c = c.substr(1);
                    currentElementSet.nameUser = c;
                }
                continue;
            }
            if (words[0] == "num_nodes_per_element" ||
                words[0] == "index_format" || words[0] == "format") {
                continue;
            }
            if (words.size() >= 11) {
                AdxElement e;
                e.id = ParseInt(words[0]);
                for (int i = 0; i < 10; ++i) e.nodeId[i] = ParseInt(words[i + 1]);
                elements_.push_back(e);
                if (haveElementSet) {
                    currentElementSet.elementIndex.push_back(
                        static_cast<int>(elements_.size()) - 1);
                }
            }
            break;
        }
        }
    }
    return 0;
}

bool Adx::FaceExist(int a, int b, int c, int& faceIndex) const {
    faceIndex = -1;
    int v0, v1, v2;
    Sort3(a, b, c, v0, v1, v2);
    const AdxNode& vertex = nodes_[v0];
    for (int fi : vertex.elementFaceIndex) {
        const AdxElementFace& f = faces_[fi];
        if (f.vertexSorted[0] == v0 && f.vertexSorted[1] == v1 && f.vertexSorted[2] == v2) {
            faceIndex = fi;
            return true;
        }
    }
    return false;
}

int Adx::Activate() {
    nodeIdToIndex_.clear();
    for (int i = 0; i < static_cast<int>(nodes_.size()); ++i)
        nodeIdToIndex_[nodes_[i].id] = i;

    elemIdToIndex_.clear();
    for (int i = 0; i < static_cast<int>(elements_.size()); ++i)
        elemIdToIndex_[elements_[i].id] = i;

    for (auto& e : elements_) {
        for (int j = 0; j < 10; ++j) {
            auto it = nodeIdToIndex_.find(e.nodeId[j]);
            if (it == nodeIdToIndex_.end()) return 1;
            e.nodeIndex[j] = it->second;
        }
        for (int j = 0; j < 4; ++j)  nodes_[e.nodeIndex[j]].type = NodeType::First;
        for (int j = 4; j < 10; ++j) nodes_[e.nodeIndex[j]].type = NodeType::Second;
    }

    // Build faces; shared faces get a back element and become interior.
    for (int i = 0; i < static_cast<int>(elements_.size()); ++i) {
        AdxElement& e = elements_[i];
        for (int j = 0; j < 4; ++j) {
            int ni[6];
            for (int k = 0; k < 6; ++k) ni[k] = e.nodeIndex[kFaceIndex[j][k]];

            int faceIndex = -1;
            if (FaceExist(ni[0], ni[1], ni[2], faceIndex)) {
                faces_[faceIndex].backElementIndex = i;
                e.faceIndex[j] = faceIndex;
            } else {
                AdxElementFace f;
                for (int k = 0; k < 6; ++k) f.nodeIndex[k] = ni[k];
                f.frontElementIndex = i;
                f.backElementIndex = -1;
                Sort3(ni[0], ni[1], ni[2], f.vertexSorted[0], f.vertexSorted[1], f.vertexSorted[2]);
                faces_.push_back(f);
                const int newIndex = static_cast<int>(faces_.size()) - 1;
                e.faceIndex[j] = newIndex;
                nodes_[f.vertexSorted[0]].elementFaceIndex.push_back(newIndex);
            }
        }
    }
    return 0;
}

int Adx::SurfaceExtract(const std::string& setName,
                        std::vector<int>& outNodes,
                        std::vector<int>& outFaces) const {
    outNodes.clear();
    outFaces.clear();

    int index = -1;
    for (int i = 0; i < static_cast<int>(elementSets_.size()); ++i) {
        if (elementSets_[i].nameAdx == setName) { index = i; break; }
    }
    if (index == -1) return 1;

    const AdxElementSet& es = elementSets_[index];
    for (int ei : es.elementIndex) {
        const AdxElement& e = elements_[ei];
        for (int k = 0; k < 4; ++k) {
            const AdxElementFace& f = faces_[e.faceIndex[k]];
            if (f.backElementIndex == -1) outFaces.push_back(e.faceIndex[k]);
        }
    }

    std::map<int, int> nodeMap;
    for (int fi : outFaces) {
        const AdxElementFace& f = faces_[fi];
        for (int j = 0; j < 6; ++j) nodeMap[f.nodeIndex[j]] = fi;
    }
    for (const auto& kv : nodeMap) outNodes.push_back(kv.first);
    return 0;
}

int Adx::ExtractSurfaceNodes(const std::vector<std::string>& setNames,
                             std::vector<SurfaceNode>& out) const {
    std::map<int, int> nodeMap;
    for (const auto& name : setNames) {
        std::vector<int> vnode, vface;
        SurfaceExtract(name, vnode, vface);
        for (int idx : vnode) nodeMap[idx] = idx;
    }
    out.clear();
    out.reserve(nodeMap.size());
    for (const auto& kv : nodeMap) {
        const AdxNode& n = nodes_[kv.first];
        SurfaceNode s;
        s.id = n.id;
        s.coord = n.coord;
        s.force = Vec3{0.0, 0.0, 0.0};
        out.push_back(s);
    }
    return 0;
}

} // namespace vm
