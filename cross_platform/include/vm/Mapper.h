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

enum class MapMode { SingleNearest, WeightedKNearest, FaceProjection, SourceSampled };

struct MapOptions {
    MapMode mode            = MapMode::SingleNearest;
    int     k               = 4;     // neighbours for WeightedKNearest
    double  idwPower        = 2.0;   // inverse-distance exponent
    bool    fallbackNearest = false; // assign out-of-range sources to global nearest
    bool    conserveTotal   = false; // rescale so total mapped == total applied
    int     sampleLevel     = 3;     // SourceSampled: edge subdivisions per element
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

    // Supply the surface triangles required by MapMode::FaceProjection.
    // Builds an internal grid over the face centroids.
    void SetFaces(std::vector<TargetFace> faces);

    MappingResult Map(const Nastran& nas, double searchDistance, const Vec3& ratio,
                      const MapOptions& opt = MapOptions{});

    int ExportAdxForce(const std::string& path, const std::string& processId) const;

    const std::vector<SurfaceNode>& targets() const { return targets_; }

private:
    SurfaceNode* GlobalNearest(const Vec3& p, double& distance);
    // Distribute mapForce onto the nearest face within searchDistance.
    // Returns true if a face was found.
    bool ProjectOntoFace(const Vec3& p, const Vec3& mapForce, double searchDistance);
    // Place one (point, force) sample using the mode's placement rule, updating
    // the running result (counters and accumulated forces).
    void PlaceForce(const Vec3& p, const Vec3& force, double searchDistance,
                    const MapOptions& opt, MappingResult& res);
    // Build the (point, force) samples to distribute. For SourceSampled this
    // subdivides each source element; otherwise it is one sample per source node.
    void GenerateSamples(const Nastran& nas, const Vec3& ratio, const MapOptions& opt,
                         std::vector<std::pair<Vec3, Vec3>>& out) const;

    std::vector<SurfaceNode>& targets_;
    AreaMap                   areaMap_;

    std::vector<TargetFace>   faces_;
    std::vector<SurfaceNode>  centroids_;   // one per face (id = face index)
    AreaMap                   centroidMap_;
    double                    maxFaceExtent_ = 0.0;
};

} // namespace vm
