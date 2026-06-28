// Mapper.h - transfer nodal forces from the Nastran mesh onto a set of target
// (ADX surface) nodes, then export the result.
//
// Mapping modes:
//   SingleNearest  - original behaviour: full force to the single nearest
//                    target within the search distance (else lost).
//   WeightedKNearest - distribute each source force across the k nearest
//                    targets within the search distance, weighted by inverse
//                    distance. Per-source force is conserved when at least one
//                    target is in range.
//
// Options that improve global conservation:
//   fallbackNearest - sources with no target in range fall back to the global
//                    nearest target (nothing is lost to range).
//   conserveTotal   - after mapping, rescale the result so the total mapped
//                    force matches the total applied force (component-wise).
#pragma once

#include <string>
#include <vector>

#include "vm/AreaMap.h"
#include "vm/Nastran.h"
#include "vm/Vec3.h"

namespace vm {

enum class MapMode { SingleNearest, WeightedKNearest };

struct MapOptions {
    MapMode mode            = MapMode::SingleNearest;
    int     k               = 4;     // neighbours for WeightedKNearest
    double  idwPower        = 2.0;   // inverse-distance exponent
    bool    fallbackNearest = false; // assign out-of-range sources to global nearest
    bool    conserveTotal   = false; // rescale so total mapped == total applied
};

struct MappingResult {
    Vec3 appliedForce;  // total source force after the ratio
    Vec3 mappedForce;   // total force placed on targets (before conserveTotal)
    Vec3 lossForce;     // total force with no target (after fallback)
    int  mappedCount = 0;
    int  lossCount   = 0;
    int  fallbackCount = 0;
    double maxLossDistance = 0.0; // farthest unmatched source's nearest target
};

class Mapper {
public:
    Mapper(std::vector<SurfaceNode>& targets, double pitch = 0.0);

    MappingResult Map(const Nastran& nas, double searchDistance, const Vec3& ratio,
                      const MapOptions& opt = MapOptions{});

    int ExportAdxForce(const std::string& path, const std::string& processId) const;

    const std::vector<SurfaceNode>& targets() const { return targets_; }

private:
    SurfaceNode* GlobalNearest(const Vec3& p, double& distance);

    std::vector<SurfaceNode>& targets_;
    AreaMap                   areaMap_;
};

} // namespace vm
