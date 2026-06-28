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

bool Mapper::ProjectOntoFace(const Vec3& p, const Vec3& mapForce, const Vec3& srcDir,
                             double searchDistance, const MapOptions& opt) {
    if (faces_.empty()) return false;

    // Candidate faces: those whose centroid is within reach. A face can be in
    // range even if its centroid is up to maxFaceExtent_ farther than p.
    std::vector<std::pair<SurfaceNode*, double>> cands;
    centroidMap_.NodesInRange(p, searchDistance + maxFaceExtent_, cands);
    if (cands.empty()) return false;

    // Normal-alignment filter setup (thin shells: reject the opposite face).
    const bool useFilter = opt.normalFilter && srcDir.LengthSquared() > 1e-24;
    const double cosThresh = std::cos(opt.normalMaxAngleDeg * 3.14159265358979323846 / 180.0);
    Vec3 wantDir = srcDir;
    if (opt.normalFlip) wantDir = wantDir * (-1.0);

    int    bestFace = -1;
    double bestDist = 1e100;
    double bwa = 0, bwb = 0, bwc = 0;

    for (auto& c : cands) {
        const int fi = c.first->id;
        const TargetFace& f = faces_[fi];
        const Vec3& a = targets_[f.n[0]].coord;
        const Vec3& b = targets_[f.n[1]].coord;
        const Vec3& cc = targets_[f.n[2]].coord;

        if (useFilter) {
            Vec3 nf = (b - a).CrossProduct(cc - a);
            const double len = nf.Length();
            if (len > 1e-24) {
                nf = nf * (1.0 / len);
                const double cosA = Dot(nf, wantDir) / wantDir.Length();
                if (cosA < cosThresh) continue;  // face points the wrong way
            }
        }

        double wa, wb, wc;
        const Vec3 cp = ClosestPointOnTriangle(p, a, b, cc, wa, wb, wc);
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

Vec3 Mapper::TargetCentroid() const {
    Vec3 o;
    if (targets_.empty()) return o;
    for (const auto& t : targets_) o += t.coord;
    return o * (1.0 / static_cast<double>(targets_.size()));
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

void Mapper::PlaceForce(const Sample& s, double searchDistance,
                        const MapOptions& opt, MappingResult& res) {
    const Vec3& p = s.pos;
    const Vec3& force = s.force;
    bool placed = false;

    if (opt.mode == MapMode::FaceProjection || opt.mode == MapMode::SourceSampled) {
        placed = ProjectOntoFace(p, force, s.dir, searchDistance, opt);
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

namespace {
// Solve A x = b for a 3x3 A with a tiny Tikhonov regularisation so that rank
// deficiency (e.g. collinear targets) degrades gracefully to a least-norm-ish
// solution instead of blowing up.
bool Solve3x3(double A[3][3], const Vec3& b, Vec3& x) {
    const double reg = 1e-12 * (std::fabs(A[0][0]) + std::fabs(A[1][1]) +
                                std::fabs(A[2][2]) + 1.0);
    double m[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) m[i][j] = A[i][j] + (i == j ? reg : 0.0);

    const double det =
        m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    if (std::fabs(det) < 1e-30) return false;
    const double inv = 1.0 / det;

    double c[3][3];
    c[0][0] =  (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * inv;
    c[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) * inv;
    c[0][2] =  (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * inv;
    c[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) * inv;
    c[1][1] =  (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * inv;
    c[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) * inv;
    c[2][0] =  (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * inv;
    c[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) * inv;
    c[2][2] =  (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * inv;

    x.x = c[0][0] * b.x + c[0][1] * b.y + c[0][2] * b.z;
    x.y = c[1][0] * b.x + c[1][1] * b.y + c[1][2] * b.z;
    x.z = c[2][0] * b.x + c[2][1] * b.y + c[2][2] * b.z;
    return true;
}
} // namespace

MappingResult Mapper::Map(const Nastran& nas, double searchDistance,
                          const Vec3& ratio, const MapOptions& opt) {
    MappingResult res;

    const Vec3 o = TargetCentroid();  // moment reference point

    std::vector<Sample> samples;
    GenerateSamples(nas, ratio, opt, samples);

    for (const auto& s : samples) {
        res.appliedForce  += s.force;
        res.appliedMoment += (s.pos - o).CrossProduct(s.force);
        PlaceForce(s, searchDistance, opt, res);
    }

    if (opt.conserveTotal) {
        // Component-wise rescale so the total on the targets equals the total
        // applied force (recovers force that fell outside the search distance).
        auto factor = [](double applied, double mapped) {
            return (std::fabs(mapped) > 1e-30) ? applied / mapped : 1.0;
        };
        Vec3 mappedNow;
        for (auto& t : targets_) mappedNow += t.force;
        const double fx = factor(res.appliedForce.x, mappedNow.x);
        const double fy = factor(res.appliedForce.y, mappedNow.y);
        const double fz = factor(res.appliedForce.z, mappedNow.z);
        for (auto& t : targets_) {
            t.force.x *= fx; t.force.y *= fy; t.force.z *= fz;
        }
        res.mappedForce = res.appliedForce;
        res.lossForce = Vec3{0, 0, 0};
    }

    if (opt.conserveMoment) {
        // Add a self-equilibrated correction df_i = b x s_i (s_i = r_i - o) that
        // leaves the total force unchanged (sum df_i = b x sum s_i = 0 since o is
        // the centroid) and fixes the total moment. Solve A b = dM with
        // A = (sum|s_i|^2) I - sum s_i s_i^T.
        Vec3 mappedMoment;
        for (auto& t : targets_) mappedMoment += (t.coord - o).CrossProduct(t.force);
        const Vec3 dM = res.appliedMoment - mappedMoment;

        double A[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
        double sumSq = 0.0;
        for (auto& t : targets_) {
            const Vec3 s = t.coord - o;
            sumSq += s.LengthSquared();
            A[0][0] -= s.x * s.x; A[0][1] -= s.x * s.y; A[0][2] -= s.x * s.z;
            A[1][0] -= s.y * s.x; A[1][1] -= s.y * s.y; A[1][2] -= s.y * s.z;
            A[2][0] -= s.z * s.x; A[2][1] -= s.z * s.y; A[2][2] -= s.z * s.z;
        }
        A[0][0] += sumSq; A[1][1] += sumSq; A[2][2] += sumSq;

        Vec3 b;
        if (Solve3x3(A, dM, b)) {
            for (auto& t : targets_) t.force += b.CrossProduct(t.coord - o);
        }
    }

    // Report the final mapped moment about the same reference.
    Vec3 finalMoment;
    for (auto& t : targets_) finalMoment += (t.coord - o).CrossProduct(t.force);
    res.mappedMoment = finalMoment;

    return res;
}

void Mapper::GenerateSamples(const Nastran& nas, const Vec3& ratio,
                             const MapOptions& opt,
                             std::vector<Sample>& out) const {
    out.clear();
    const double kUnit = 1000.0;  // matches Nastran::ForceCalc unit convention

    auto unit = [](const Vec3& v) {
        const double L = v.Length();
        return (L > 1e-24) ? v * (1.0 / L) : Vec3{0, 0, 0};
    };

    if (opt.mode != MapMode::SourceSampled) {
        // One sample per source node, carrying its (already integrated) force.
        out.reserve(nas.nodes().size());
        for (const auto& n : nas.nodes()) {
            Sample s;
            s.pos = n.coord;
            s.force = Vec3{n.force.x * ratio.x, n.force.y * ratio.y, n.force.z * ratio.z};
            // Source normal direction for the alignment filter: prefer the
            // normal-pressure direction, fall back to the force direction.
            s.dir = unit(n.normalPressure.LengthSquared() > 0.0 ? n.normalPressure : n.force);
            out.push_back(s);
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
        const Vec3 faceDir = unit((C1 - C0).CrossProduct(C2 - C0));
        auto emit = [&](double b0, double b1, double b2) {
            const Vec3 pos{C0.x * b0 + C1.x * b1 + C2.x * b2,
                           C0.y * b0 + C1.y * b1 + C2.y * b2,
                           C0.z * b0 + C1.z * b1 + C2.z * b2};
            const Vec3 pr{P0.x * b0 + P1.x * b1 + P2.x * b2,
                          P0.y * b0 + P1.y * b1 + P2.y * b2,
                          P0.z * b0 + P1.z * b1 + P2.z * b2};
            const double s = subArea * kUnit;
            Sample smp;
            smp.pos = pos;
            smp.force = Vec3{pr.x * s * ratio.x, pr.y * s * ratio.y, pr.z * s * ratio.z};
            smp.dir = faceDir;
            out.push_back(smp);
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
                Sample smp;
                smp.pos = pos;
                smp.force = Vec3{pr.x * s * ratio.x, pr.y * s * ratio.y, pr.z * s * ratio.z};
                smp.dir = Vec3{0, 0, 0};  // a line has no surface normal
                out.push_back(smp);
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
