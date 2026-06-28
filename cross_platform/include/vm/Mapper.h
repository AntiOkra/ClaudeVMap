// Mapper.h - transfer nodal forces from the Nastran mesh onto a set of target
// (ADX surface) nodes via nearest-neighbour search, then export the result.
#pragma once

#include <string>
#include <vector>

#include "vm/AreaMap.h"
#include "vm/Nastran.h"
#include "vm/Vec3.h"

namespace vm {

struct MappingResult {
    Vec3 mappedForce;   // sum of forces successfully transferred
    Vec3 lossForce;     // sum of forces with no target node within the limit
    int  mappedCount = 0;
    int  lossCount   = 0;
};

class Mapper {
public:
    // targets: ADX surface nodes (forces are accumulated in place).
    // pitch: AreaMap cell size (defaults to max(searchDistance, 50) when <= 0).
    Mapper(std::vector<SurfaceNode>& targets, double pitch = 0.0);

    MappingResult Map(const Nastran& nas, double searchDistance, const Vec3& ratio);

    // Write the ADX force file (only nodes with a non-zero force vector).
    int ExportAdxForce(const std::string& path, const std::string& processId) const;

    const std::vector<SurfaceNode>& targets() const { return targets_; }

private:
    std::vector<SurfaceNode>& targets_;
    AreaMap                   areaMap_;
};

} // namespace vm
