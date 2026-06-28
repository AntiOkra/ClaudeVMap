#include "vm/AreaMap.h"

#include <cmath>

namespace vm {

AreaMap::CellKey AreaMap::IndexOf(const Vec3& p) const {
    return CellKey{
        static_cast<int>(std::floor(p.x / pitch_)),
        static_cast<int>(std::floor(p.y / pitch_)),
        static_cast<int>(std::floor(p.z / pitch_))
    };
}

void AreaMap::Build(std::vector<SurfaceNode>& nodes, double pitch) {
    cells_.clear();
    pitch_ = (pitch > 0.0) ? pitch : 1.0;
    for (auto& n : nodes) {
        cells_[IndexOf(n.coord)].push_back(&n);
    }
}

SurfaceNode* AreaMap::NearestNode(const Vec3& p, double upr_limit, double& distance) {
    distance = 1e100;
    if (cells_.empty()) return nullptr;

    const CellKey c = IndexOf(p);

    // Scan enough cells to fully cover upr_limit regardless of pitch.
    int cell_range = 1;
    if (pitch_ > 0.0) {
        cell_range = static_cast<int>(std::ceil(upr_limit / pitch_));
        if (cell_range < 1) cell_range = 1;
    }

    SurfaceNode* nearest = nullptr;
    double min_dst2 = 1e100;

    for (int ix = c.ix - cell_range; ix <= c.ix + cell_range; ++ix) {
        for (int iy = c.iy - cell_range; iy <= c.iy + cell_range; ++iy) {
            for (int iz = c.iz - cell_range; iz <= c.iz + cell_range; ++iz) {
                auto it = cells_.find(CellKey{ix, iy, iz});
                if (it == cells_.end()) continue;
                for (SurfaceNode* n : it->second) {
                    const double d2 = n->coord.DistanceSquared(p);
                    // Track the global nearest; the distance limit is checked
                    // once, afterwards. A nearer-but-out-of-limit node must not
                    // hide a valid within-limit node.
                    if (d2 < min_dst2) {
                        min_dst2 = d2;
                        nearest = n;
                    }
                }
            }
        }
    }

    if (nearest != nullptr) {
        const double d = std::sqrt(min_dst2);
        distance = d;
        if (d < upr_limit) {
            return nearest;
        }
    }
    return nullptr;
}

void AreaMap::NodesInRange(const Vec3& p, double upr_limit,
                           std::vector<std::pair<SurfaceNode*, double>>& out) {
    out.clear();
    if (cells_.empty()) return;

    const CellKey c = IndexOf(p);
    int cell_range = 1;
    if (pitch_ > 0.0) {
        cell_range = static_cast<int>(std::ceil(upr_limit / pitch_));
        if (cell_range < 1) cell_range = 1;
    }
    const double limit2 = upr_limit * upr_limit;

    for (int ix = c.ix - cell_range; ix <= c.ix + cell_range; ++ix) {
        for (int iy = c.iy - cell_range; iy <= c.iy + cell_range; ++iy) {
            for (int iz = c.iz - cell_range; iz <= c.iz + cell_range; ++iz) {
                auto it = cells_.find(CellKey{ix, iy, iz});
                if (it == cells_.end()) continue;
                for (SurfaceNode* n : it->second) {
                    const double d2 = n->coord.DistanceSquared(p);
                    if (d2 < limit2) {
                        out.emplace_back(n, std::sqrt(d2));
                    }
                }
            }
        }
    }
}

} // namespace vm
