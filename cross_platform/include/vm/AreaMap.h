// AreaMap.h - spatial grid for nearest-neighbour queries.
//
// Portable rework of the original CAreaMap. Two deliberate changes vs. the
// MFC version:
//   * Sparse storage (hash map of occupied cells) instead of a dense
//     ixno*iyno*izno array, so a thin/large surface no longer allocates a
//     huge mostly-empty grid.
//   * The query scans a cell radius derived from the search distance, so it
//     stays correct even when the search distance exceeds the cell pitch.
#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "vm/Vec3.h"

namespace vm {

// A node as seen by the mapper: position plus an accumulated force vector.
struct SurfaceNode {
    int  id = -1;
    Vec3 coord;
    Vec3 force;
};

class AreaMap {
public:
    // Build the grid over the given nodes. The pitch defaults to a sensible
    // value but is clamped to be positive.
    void Build(std::vector<SurfaceNode>& nodes, double pitch);

    // Find the nearest node to p within upr_limit.
    // Returns the node pointer (or nullptr) and the distance found.
    SurfaceNode* NearestNode(const Vec3& p, double upr_limit, double& distance);

    double pitch() const { return pitch_; }
    bool   empty() const { return cells_.empty(); }

private:
    struct CellKey {
        int ix, iy, iz;
        bool operator==(const CellKey& o) const {
            return ix == o.ix && iy == o.iy && iz == o.iz;
        }
    };
    struct CellHash {
        std::size_t operator()(const CellKey& k) const {
            // Simple mix of the three indices.
            std::uint64_t h = 1469598103934665603ull;
            auto mix = [&](int v) {
                h ^= static_cast<std::uint64_t>(static_cast<std::uint32_t>(v));
                h *= 1099511628211ull;
            };
            mix(k.ix); mix(k.iy); mix(k.iz);
            return static_cast<std::size_t>(h);
        }
    };

    CellKey IndexOf(const Vec3& p) const;

    double pitch_ = 1.0;
    std::unordered_map<CellKey, std::vector<SurfaceNode*>, CellHash> cells_;
};

} // namespace vm
