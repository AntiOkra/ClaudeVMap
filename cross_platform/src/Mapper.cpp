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

void Mapper::PlaceForce(const Vec3& p, const Vec3& force, double searchDistance,
                        const MapOptions& opt, MappingResult& res) {
    bool placed = false;

    if (opt.mode == MapMode::FaceProjection || opt.mode == MapMode::SourceSampled) {
        placed = ProjectOntoFace(p, force, searchDistance);
    } else if (opt.mode == MapMode::WeightedKNearest) {
        std::vector<std::pair<SurfaceNode*, double>> candidates;
        areaMap_.NodesInRange(p, searchDistance, candidates);
        if (!candidates.empty()) {
            const int k = (opt.k > 0) ? opt.k : 1;
            if (static_cast<int>(candidates.size()) > k) {
                std::partial_sort(
                    candidates.begin(), candidates.begin() + k, candidates.end(),
                    [](const auto& a, const auto& b) { return a.second < b.second; });
                candidates.resize(k);
            }
            bool exactHit = false;
            for (auto& c : candidates) if (c.second <= 1e-12) { exactHit = true; break; }
            if (exactHit) {
                for (auto& c : candidates)
                    if (c.second <= 1e-12) { c.first->force += force; break; }
            } else {
                double wsum = 0.0;
                std::vector<double> w(candidates.size());
                for (std::size_t i = 0; i < candidates.size(); ++i) {
                    w[i] = 1.0 / std::pow(candidates[i].second, opt.idwPower);
                    wsum += w[i];
                }
                for (std::size_t i = 0; i < candidates.size(); ++i)
                    candidates[i].first->force += force * (w[i] / wsum);
            }
            placed = true;
        }
    } else {
        double distance = 0.0;
        SurfaceNode* target = areaMap_.NearestNode(p, searchDistance, distance);
        if (target != nullptr) { target->force += force; placed = true; }
    }

    if (placed) {
        res.mappedForce += force;
        ++res.mappedCount;
        return;
    }

    double gdist = 0.0;
    SurfaceNode* g = GlobalNearest(p, gdist);
    if (gdist > res.maxLossDistance) res.maxLossDistance = gdist;
    if (opt.fallbackNearest && g != nullptr) {
        g->force += force;
        res.mappedForce += force;
        ++res.fallbackCount;
    } else {
        res.lossForce += force;
        ++res.lossCount;
    }
}

MappingResult Mapper::Map(const Nastran& nas, double searchDistance,
                          const Vec3& ratio, const MapOptions& opt) {
    MappingResult res;

    std::vector<std::pair<Vec3, Vec3>> samples;
    GenerateSamples(nas, ratio, opt, samples);

    for (const auto& s : samples) {
        res.appliedForce += s.second;
        PlaceForce(s.first, s.second, searchDistance, opt, res);
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

void Mapper::GenerateSamples(const Nastran& nas, const Vec3& ratio,
                             const MapOptions& opt,
                             std::vector<std::pair<Vec3, Vec3>>& out) const {
    out.clear();
    const double kUnit = 1000.0;  // matches Nastran::ForceCalc unit convention

    if (opt.mode != MapMode::SourceSampled) {
        // One sample per source node, carrying its (already integrated) force.
        out.reserve(nas.nodes().size());
        for (const auto& n : nas.nodes()) {
            out.emplace_back(n.coord, Vec3{n.force.x * ratio.x,
                                           n.force.y * ratio.y,
                                           n.force.z * ratio.z});
        }
        return;
    }

    const int k = (opt.sampleLevel > 0) ? opt.sampleLevel : 1;
    const auto& nodes = nas.nodes();

    // Subdivide a triangle into k*k congruent sub-triangles and emit one sample
    // (pressure * sub-area * unit * ratio) at each sub-triangle centroid.
    auto addTriangle = [&](const Vec3& C0, const Vec3& C1, const Vec3& C2,
                           const Vec3& P0, const Vec3& P1, const Vec3& P2) {
        const double area = AreaTriangle(C0, C1, C2);
        if (area <= 0.0) return;
        const double subArea = area / (k * k);
        auto emit = [&](double b0, double b1, double b2) {
            const Vec3 pos{C0.x * b0 + C1.x * b1 + C2.x * b2,
                           C0.y * b0 + C1.y * b1 + C2.y * b2,
                           C0.z * b0 + C1.z * b1 + C2.z * b2};
            const Vec3 pr{P0.x * b0 + P1.x * b1 + P2.x * b2,
                          P0.y * b0 + P1.y * b1 + P2.y * b2,
                          P0.z * b0 + P1.z * b1 + P2.z * b2};
            const double s = subArea * kUnit;
            out.emplace_back(pos, Vec3{pr.x * s * ratio.x,
                                       pr.y * s * ratio.y,
                                       pr.z * s * ratio.z});
        };
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k - i; ++j) {
                double b1 = (i + 1.0 / 3.0) / k, b2 = (j + 1.0 / 3.0) / k;
                emit(1.0 - b1 - b2, b1, b2);                 // "up" sub-triangle
                if (i + j <= k - 2) {
                    b1 = (i + 2.0 / 3.0) / k; b2 = (j + 2.0 / 3.0) / k;
                    emit(1.0 - b1 - b2, b1, b2);             // "down" sub-triangle
                }
            }
        }
    };

    auto pressureOf = [&](int idx) {
        return nodes[idx].normalPressure + nodes[idx].tangentPressure;
    };

    for (const auto& e : nas.elements()) {
        if (e.type == ElemType::CTRIA3) {
            addTriangle(nodes[e.nodeIndex[0]].coord, nodes[e.nodeIndex[1]].coord,
                        nodes[e.nodeIndex[2]].coord,
                        pressureOf(e.nodeIndex[0]), pressureOf(e.nodeIndex[1]),
                        pressureOf(e.nodeIndex[2]));
        } else if (e.type == ElemType::CQUAD4) {
            addTriangle(nodes[e.nodeIndex[0]].coord, nodes[e.nodeIndex[1]].coord,
                        nodes[e.nodeIndex[2]].coord,
                        pressureOf(e.nodeIndex[0]), pressureOf(e.nodeIndex[1]),
                        pressureOf(e.nodeIndex[2]));
            addTriangle(nodes[e.nodeIndex[2]].coord, nodes[e.nodeIndex[3]].coord,
                        nodes[e.nodeIndex[0]].coord,
                        pressureOf(e.nodeIndex[2]), pressureOf(e.nodeIndex[3]),
                        pressureOf(e.nodeIndex[0]));
        } else { // CBEAM: sample midpoints of k equal segments along the line.
            const Vec3& A = nodes[e.nodeIndex[0]].coord;
            const Vec3& B = nodes[e.nodeIndex[1]].coord;
            const Vec3  Pa = pressureOf(e.nodeIndex[0]);
            const Vec3  Pb = pressureOf(e.nodeIndex[1]);
            const double len = A.Distance(B);
            if (len <= 0.0) continue;
            const double seg = len / k;
            for (int i = 0; i < k; ++i) {
                const double t = (i + 0.5) / k;
                const Vec3 pos{A.x + (B.x - A.x) * t, A.y + (B.y - A.y) * t,
                               A.z + (B.z - A.z) * t};
                const Vec3 pr{Pa.x + (Pb.x - Pa.x) * t, Pa.y + (Pb.y - Pa.y) * t,
                              Pa.z + (Pb.z - Pa.z) * t};
                const double s = seg * kUnit;
                out.emplace_back(pos, Vec3{pr.x * s * ratio.x, pr.y * s * ratio.y,
                                           pr.z * s * ratio.z});
            }
        }
    }
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
