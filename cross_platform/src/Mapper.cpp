#include "vm/Mapper.h"
#include "vm/Geometry.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <utility>
#include <vector>

namespace vm {

Mapper::Mapper(std::vector<SurfaceNode>& targets, double pitch)
    : targets_(targets) {
    const double p = (pitch > 0.0) ? pitch : 50.0;
    areaMap_.Build(targets_, p);
}

void Mapper::SetFaces(std::vector<TargetFace> faces) {
    faces_ = std::move(faces);
    centroids_.clear();
    centroids_.reserve(faces_.size());
    maxFaceExtent_ = 0.0;

    for (std::size_t fi = 0; fi < faces_.size(); ++fi) {
        const TargetFace& f = faces_[fi];
        const Vec3& a = targets_[f.n[0]].coord;
        const Vec3& b = targets_[f.n[1]].coord;
        const Vec3& c = targets_[f.n[2]].coord;
        Vec3 centroid{(a.x + b.x + c.x) / 3.0,
                      (a.y + b.y + c.y) / 3.0,
                      (a.z + b.z + c.z) / 3.0};
        // Track the largest centroid-to-corner distance to size the query.
        const double r = std::max({centroid.Distance(a), centroid.Distance(b),
                                   centroid.Distance(c)});
        if (r > maxFaceExtent_) maxFaceExtent_ = r;

        SurfaceNode cn;
        cn.id = static_cast<int>(fi);
        cn.coord = centroid;
        centroids_.push_back(cn);
    }

    double pitch = areaMap_.pitch();
    if (pitch <= 0.0) pitch = 50.0;
    centroidMap_.Build(centroids_, pitch);
}

bool Mapper::ProjectOntoFace(const Vec3& p, const Vec3& mapForce, double searchDistance) {
    if (faces_.empty()) return false;

    // Candidate faces: those whose centroid is within reach. A face can be in
    // range even if its centroid is up to maxFaceExtent_ farther than p.
    std::vector<std::pair<SurfaceNode*, double>> cands;
    centroidMap_.NodesInRange(p, searchDistance + maxFaceExtent_, cands);
    if (cands.empty()) return false;

    int    bestFace = -1;
    double bestDist = 1e100;
    double bwa = 0, bwb = 0, bwc = 0;

    for (auto& c : cands) {
        const int fi = c.first->id;
        const TargetFace& f = faces_[fi];
        double wa, wb, wc;
        const Vec3 cp = ClosestPointOnTriangle(
            p, targets_[f.n[0]].coord, targets_[f.n[1]].coord, targets_[f.n[2]].coord,
            wa, wb, wc);
        const double d = p.Distance(cp);
        if (d < bestDist) {
            bestDist = d; bestFace = fi; bwa = wa; bwb = wb; bwc = wc;
        }
    }

    if (bestFace < 0 || bestDist >= searchDistance) return false;

    const TargetFace& f = faces_[bestFace];
    targets_[f.n[0]].force += mapForce * bwa;
    targets_[f.n[1]].force += mapForce * bwb;
    targets_[f.n[2]].force += mapForce * bwc;
    return true;
}

SurfaceNode* Mapper::GlobalNearest(const Vec3& p, double& distance) {
    SurfaceNode* best = nullptr;
    double best2 = 1e100;
    for (auto& t : targets_) {
        const double d2 = t.coord.DistanceSquared(p);
        if (d2 < best2) { best2 = d2; best = &t; }
    }
    distance = (best != nullptr) ? std::sqrt(best2) : 1e100;
    return best;
}

MappingResult Mapper::Map(const Nastran& nas, double searchDistance,
                          const Vec3& ratio, const MapOptions& opt) {
    MappingResult res;

    std::vector<std::pair<SurfaceNode*, double>> candidates;

    for (const auto& n : nas.nodes()) {
        const Vec3 mapForce{
            n.force.x * ratio.x,
            n.force.y * ratio.y,
            n.force.z * ratio.z
        };
        res.appliedForce += mapForce;

        bool placed = false;

        if (opt.mode == MapMode::FaceProjection) {
            if (ProjectOntoFace(n.coord, mapForce, searchDistance)) {
                res.mappedForce += mapForce;
                ++res.mappedCount;
                placed = true;
            }
        } else if (opt.mode == MapMode::WeightedKNearest) {
            areaMap_.NodesInRange(n.coord, searchDistance, candidates);
            if (!candidates.empty()) {
                // Keep the k nearest.
                const int k = (opt.k > 0) ? opt.k : 1;
                if (static_cast<int>(candidates.size()) > k) {
                    std::partial_sort(
                        candidates.begin(), candidates.begin() + k, candidates.end(),
                        [](const auto& a, const auto& b) { return a.second < b.second; });
                    candidates.resize(k);
                }

                // Inverse-distance weights (exact-hit collapses to that node).
                double wsum = 0.0;
                bool exactHit = false;
                for (auto& c : candidates) {
                    if (c.second <= 1e-12) { exactHit = true; break; }
                }
                if (exactHit) {
                    for (auto& c : candidates) {
                        if (c.second <= 1e-12) { c.first->force += mapForce; break; }
                    }
                } else {
                    std::vector<double> w(candidates.size());
                    for (std::size_t i = 0; i < candidates.size(); ++i) {
                        w[i] = 1.0 / std::pow(candidates[i].second, opt.idwPower);
                        wsum += w[i];
                    }
                    for (std::size_t i = 0; i < candidates.size(); ++i) {
                        candidates[i].first->force += mapForce * (w[i] / wsum);
                    }
                }
                res.mappedForce += mapForce;
                ++res.mappedCount;
                placed = true;
            }
        } else {
            double distance = 0.0;
            SurfaceNode* target = areaMap_.NearestNode(n.coord, searchDistance, distance);
            if (target != nullptr) {
                target->force += mapForce;
                res.mappedForce += mapForce;
                ++res.mappedCount;
                placed = true;
            }
        }

        if (!placed) {
            double gdist = 0.0;
            SurfaceNode* g = GlobalNearest(n.coord, gdist);
            if (gdist > res.maxLossDistance) res.maxLossDistance = gdist;
            if (opt.fallbackNearest && g != nullptr) {
                g->force += mapForce;
                res.mappedForce += mapForce;
                ++res.fallbackCount;
            } else {
                res.lossForce += mapForce;
                ++res.lossCount;
            }
        }
    }

    if (opt.conserveTotal) {
        // Component-wise rescale so the total on the targets equals the total
        // applied force (recovers force that fell outside the search distance).
        auto scaleAxis = [](double applied, double mapped, double& factorOut) {
            if (std::fabs(mapped) > 1e-30) { factorOut = applied / mapped; return true; }
            return false;
        };
        double fx = 1.0, fy = 1.0, fz = 1.0;
        Vec3 mappedNow;
        for (auto& t : targets_) mappedNow += t.force;
        scaleAxis(res.appliedForce.x, mappedNow.x, fx);
        scaleAxis(res.appliedForce.y, mappedNow.y, fy);
        scaleAxis(res.appliedForce.z, mappedNow.z, fz);
        for (auto& t : targets_) {
            t.force.x *= fx;
            t.force.y *= fy;
            t.force.z *= fz;
        }
        res.mappedForce = res.appliedForce;
        res.lossForce = Vec3{0, 0, 0};
    }

    return res;
}

int Mapper::ExportAdxForce(const std::string& path, const std::string& processId) const {
    std::ofstream out(path);
    if (!out) return 1;

    const std::string bar(88, '#');
    out << bar << "\n$Load\n" << bar << "\n";
    out << "process_id = " << processId << "\n";

    char buf[64];
    for (const auto& n : targets_) {
        if (n.force.x != 0.0 || n.force.y != 0.0 || n.force.z != 0.0) {
            std::snprintf(buf, sizeof(buf), "%8d 0 %12.6f\n", n.id, n.force.x);
            out << buf;
            std::snprintf(buf, sizeof(buf), "%8d 1 %12.6f\n", n.id, n.force.y);
            out << buf;
            std::snprintf(buf, sizeof(buf), "%8d 2 %12.6f\n", n.id, n.force.z);
            out << buf;
        }
    }
    return 0;
}

} // namespace vm
