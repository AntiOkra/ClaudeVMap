// test_core.cpp - self-contained tests for the portable VectorMapping core.
// No external framework: a tiny CHECK macro that records failures.
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vm/Adx.h"
#include "vm/AreaMap.h"
#include "vm/Mapper.h"
#include "vm/Nastran.h"
#include "vm/StringUtil.h"
#include "vm/Vec3.h"

static int g_failures = 0;
static int g_checks   = 0;

#define CHECK(cond, msg)                                              \
    do {                                                             \
        ++g_checks;                                                  \
        if (!(cond)) {                                               \
            ++g_failures;                                            \
            std::printf("  FAIL: %s  (%s:%d)\n", msg, __FILE__, __LINE__); \
        }                                                            \
    } while (0)

static bool Near(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

static std::string TmpPath(const char* name) {
    return std::string("vm_test_") + name;
}

static void WriteFile(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

static std::string ReadFile(const std::string& path) {
    std::ifstream f(path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Fixed-column Nastran helpers matching the reader's expected layout:
//   GRID* : id at col 16 (w8), x at col 40 (w16), y at col 56 (w16);
//   line2 : z at col 8 (w16).
//   CTRIA3: id at col 8 (w8), nodes at cols 24/32/40 (w8).
static std::string GridLine(int id, double x, double y, double z) {
    char l1[160], l2[160];
    std::snprintf(l1, sizeof(l1), "GRID*%11s%-8d%-16s%-16.6f%-16.6f", "", id, "", x, y);
    std::snprintf(l2, sizeof(l2), "*%7s%-16.6f", "", z);
    return std::string(l1) + "\n" + std::string(l2) + "\n";
}
static std::string Ctria3Line(int id, int n0, int n1, int n2) {
    char l[160];
    std::snprintf(l, sizeof(l), "CTRIA3%2s%-8d%8s%-8d%-8d%-8d", "", id, "", n0, n1, n2);
    return std::string(l) + "\n";
}
static std::string PressureFile(const std::string& body) {
    std::string s;
    for (int i = 0; i < 9; ++i) s += "# header\n";
    return s + body;
}

// ---------------------------------------------------------------------------

static void TestGeometry() {
    std::printf("[geometry]\n");
    vm::Vec3 a{0, 0, 0}, b{3, 0, 0}, c{0, 4, 0};
    CHECK(Near(vm::AreaTriangle(a, b, c), 6.0), "3-4-5 triangle area == 6");

    vm::Vec3 q0{0, 0, 0}, q1{2, 0, 0}, q2{2, 2, 0}, q3{0, 2, 0};
    CHECK(Near(vm::AreaQuad(q0, q1, q2, q3), 4.0), "unit-ish quad area == 4");

    vm::Vec3 p{1, 2, 2};
    CHECK(Near(p.Length(), 3.0), "length(1,2,2) == 3");
    CHECK(Near(vm::Vec3{0,0,0}.Distance({0,0,5}), 5.0), "distance == 5");
}

// ---------------------------------------------------------------------------
// AreaMap: adaptive cell range must find a node further than one cell away
// when the search distance exceeds the grid pitch (robustness improvement).
static void TestAreaMapAdaptiveRange() {
    std::printf("[areamap adaptive range]\n");
    std::vector<vm::SurfaceNode> nodes;
    nodes.push_back({1, vm::Vec3{40.0, 0.0, 0.0}, vm::Vec3{}});  // 40 mm away

    vm::AreaMap map;
    map.Build(nodes, /*pitch=*/10.0);  // node sits ~4 cells from the origin

    double dist = 0.0;
    // Search distance 50 > pitch 10: must still find the node.
    vm::SurfaceNode* found = map.NearestNode(vm::Vec3{0, 0, 0}, 50.0, dist);
    CHECK(found != nullptr, "found node beyond one cell when limit>pitch");
    CHECK(found && found->id == 1, "found the right node");
    CHECK(Near(dist, 40.0), "distance reported == 40");

    // Search distance 30 < 40: node is out of range -> reject.
    vm::SurfaceNode* none = map.NearestNode(vm::Vec3{0, 0, 0}, 30.0, dist);
    CHECK(none == nullptr, "rejects node outside the search distance");
}

// ---------------------------------------------------------------------------
// AreaMap nearest selection matches a brute-force reference.
static void TestAreaMapNearestMatchesBrute() {
    std::printf("[areamap vs brute force]\n");
    std::vector<vm::SurfaceNode> nodes;
    for (int i = 0; i < 50; ++i) {
        double v = static_cast<double>(i);
        nodes.push_back({i, vm::Vec3{v * 1.7, std::fmod(v * 2.3, 31.0), v * 0.9}, vm::Vec3{}});
    }
    vm::AreaMap map;
    map.Build(nodes, 5.0);

    const vm::Vec3 q{20.0, 10.0, 8.0};
    const double limit = 100.0;

    // brute force
    int bestId = -1; double bestD2 = 1e100;
    for (auto& n : nodes) {
        double d2 = n.coord.DistanceSquared(q);
        if (d2 < bestD2) { bestD2 = d2; bestId = n.id; }
    }
    double dist = 0.0;
    vm::SurfaceNode* found = map.NearestNode(q, limit, dist);
    CHECK(found != nullptr, "grid found a node");
    CHECK(found && found->id == bestId, "grid nearest == brute-force nearest");
}

// ---------------------------------------------------------------------------
// Force calc on a single triangle with a uniform normal pressure.
static void TestForceCalc() {
    std::printf("[force calc]\n");
    const std::string mesh = TmpPath("mesh.nas");
    const std::string normal = TmpPath("normal.txt");
    const std::string tangent = TmpPath("tangent.txt");

    std::string m;
    m += GridLine(1, 0, 0, 0);
    m += GridLine(2, 3, 0, 0);
    m += GridLine(3, 0, 4, 0);
    m += Ctria3Line(100, 1, 2, 3);
    WriteFile(mesh, m);

    WriteFile(normal, PressureFile("1 0 0 1\n2 0 0 1\n3 0 0 1\n"));
    WriteFile(tangent, PressureFile("1 0 0 0\n2 0 0 0\n3 0 0 0\n"));

    std::string log;
    vm::Nastran nas;
    CHECK(nas.ReadMeshFile(mesh, &log) == 0, "read mesh ok");
    CHECK(nas.nodes().size() == 3, "3 nodes read");
    CHECK(nas.elements().size() == 1, "1 element read");
    CHECK(nas.ReadNormalVectorFile(normal, &log) == 0, "read normal ok");
    CHECK(nas.ReadTangentVectorFile(tangent, &log) == 0, "read tangent ok");
    nas.ForceCalc();

    // Triangle area = 6. Total Z force = pressure(1) * area(6) * 1000 = 6000.
    vm::Vec3 total = nas.TotalForce();
    CHECK(Near(total.z, 6000.0, 1e-3), "total Z force == 6000");
    CHECK(Near(total.x, 0.0) && Near(total.y, 0.0), "X,Y force == 0");
}

// ---------------------------------------------------------------------------
// Export filter: a node with zero X but non-zero Y/Z MUST be written.
// (This is the original SurfaceNode.cpp x||x||x bug.)
static void TestExportFilterBugfix() {
    std::printf("[export filter bugfix]\n");
    std::vector<vm::SurfaceNode> targets;
    targets.push_back({10, vm::Vec3{0,0,0}, vm::Vec3{0.0, 5.0, 0.0}});  // x==0, y!=0
    targets.push_back({11, vm::Vec3{0,0,0}, vm::Vec3{0.0, 0.0, 0.0}});  // all zero
    targets.push_back({12, vm::Vec3{0,0,0}, vm::Vec3{7.0, 0.0, 0.0}});  // x!=0

    vm::Mapper mapper(targets, 50.0);
    const std::string out = TmpPath("force.txt");
    CHECK(mapper.ExportAdxForce(out, "PROC-1") == 0, "export ok");

    const std::string content = ReadFile(out);
    CHECK(content.find("      10 ") != std::string::npos,
          "node 10 (x==0,y!=0) is exported");
    CHECK(content.find("      12 ") != std::string::npos,
          "node 12 (x!=0) is exported");
    CHECK(content.find("      11 ") == std::string::npos,
          "node 11 (all zero) is NOT exported");
}

// ---------------------------------------------------------------------------
// End-to-end: a single quadratic tet ADX + one Nastran node -> mapped force.
static void TestEndToEndAdx() {
    std::printf("[end-to-end ADX mapping]\n");

    // 10-node quadratic tetrahedron. Corner nodes 1..4, mid-edge 5..10.
    // Coordinates: corners of a unit-ish tet; mid nodes at edge midpoints.
    vm::Vec3 c1{0, 0, 0}, c2{10, 0, 0}, c3{0, 10, 0}, c4{0, 0, 10};
    auto mid = [](vm::Vec3 a, vm::Vec3 b) { return vm::Vec3{(a.x+b.x)/2,(a.y+b.y)/2,(a.z+b.z)/2}; };
    // Node order for the connectivity below: 1..4 corners, then mids.
    vm::Vec3 coords[10] = {
        c1, c2, c3, c4,
        mid(c1,c2), mid(c2,c3), mid(c1,c3), mid(c1,c4), mid(c2,c4), mid(c3,c4)
    };

    std::string adxText;
    adxText += "$Node\n";
    adxText += "node_set = NS1\n";
    for (int i = 0; i < 10; ++i) {
        char l[128];
        std::snprintf(l, sizeof(l), "%d %.6f %.6f %.6f\n",
                      i + 1, coords[i].x, coords[i].y, coords[i].z);
        adxText += l;
    }
    adxText += "$Element 3DQuadraticTetrahedron\n";
    adxText += "element_set = Body_1_e1  #Body_1\n";
    // element id then 10 node ids (1..10)
    adxText += "1 1 2 3 4 5 6 7 8 9 10\n";
    adxText += "$EOF\n";

    const std::string adxPath = TmpPath("mesh.adx");
    WriteFile(adxPath, adxText);

    vm::Adx adx;
    CHECK(adx.Read(adxPath) == 0, "adx read ok");
    CHECK(adx.Activate() == 0, "adx activate ok");
    CHECK(adx.elementSets().size() == 1, "one element set");
    CHECK(adx.elementSets()[0].nameAdx == "Body_1_e1", "element set name parsed");
    CHECK(adx.elementSets()[0].id == 1, "element set id parsed from name");

    std::vector<vm::SurfaceNode> targets;
    adx.ExtractSurfaceNodes({"Body_1_e1"}, targets);
    // A single tet: all 4 faces are surface faces -> all 10 nodes present.
    CHECK(targets.size() == 10, "all 10 surface nodes extracted");

    // Find target node closest to corner c1 (node id 1 at origin).
    // Build a Nastran node coincident with c1 carrying a known force, map it.
    // We bypass file IO by mapping directly through the Mapper using a Nastran
    // assembled in memory via a small mesh file.
    const std::string nasPath = TmpPath("e2e.nas");
    const std::string normPath = TmpPath("e2e_n.txt");
    const std::string tanPath = TmpPath("e2e_t.txt");

    // One triangle right at the c1 corner so the nearest ADX node is node 1.
    std::string m;
    m += GridLine(1, 0, 0, 0);
    m += GridLine(2, 1, 0, 0);
    m += GridLine(3, 0, 1, 0);
    m += Ctria3Line(100, 1, 2, 3);
    WriteFile(nasPath, m);

    // Uniform Z pressure of 1 -> total force concentrated near the corner.
    WriteFile(normPath, PressureFile("1 0 0 1\n2 0 0 1\n3 0 0 1\n"));
    WriteFile(tanPath, PressureFile("1 0 0 0\n2 0 0 0\n3 0 0 0\n"));

    std::string log;
    vm::Nastran nas;
    CHECK(nas.ReadMeshFile(nasPath, &log) == 0, "e2e mesh ok");
    CHECK(nas.ReadNormalVectorFile(normPath, &log) == 0, "e2e normal ok");
    CHECK(nas.ReadTangentVectorFile(tanPath, &log) == 0, "e2e tangent ok");
    nas.ForceCalc();
    vm::Vec3 nasTotal = nas.TotalForce();

    vm::Mapper mapper(targets, 50.0);
    vm::MappingResult r = mapper.Map(nas, /*distance=*/30.0, vm::Vec3{1,1,1});
    CHECK(r.lossCount == 0, "no lost forces (all within distance)");

    // Conservation: mapped total force == Nastran total force.
    CHECK(Near(r.mappedForce.z, nasTotal.z, 1e-3), "force conserved (Z)");

    // Sum of force over targets equals nastran total (force fully transferred).
    vm::Vec3 sum;
    for (auto& t : mapper.targets()) sum += t.force;
    CHECK(Near(sum.z, nasTotal.z, 1e-3), "sum of target forces == nastran total");
}

// ---------------------------------------------------------------------------
// Indexing robustness: a dangling element node reference is reported, not
// silently mapped to node index 0.
static void TestDanglingNodeRef() {
    std::printf("[dangling node ref]\n");
    const std::string mesh = TmpPath("bad.nas");
    std::string m;
    m += GridLine(1, 0, 0, 0);
    m += GridLine(2, 1, 0, 0);
    // CTRIA3 referencing node 999 which does not exist.
    m += Ctria3Line(100, 1, 2, 999);
    WriteFile(mesh, m);

    std::string log;
    vm::Nastran nas;
    int rc = nas.ReadMeshFile(mesh, &log);
    CHECK(rc != 0, "dangling node ref returns error");
    CHECK(log.find("unknown Node(999)") != std::string::npos, "error names node 999");
}

// ---------------------------------------------------------------------------

// Build an in-memory Nastran with explicit node coords/forces (bypasses file
// IO; the Mapper only reads coords and forces).
static vm::Nastran MakeNastran(const std::vector<std::pair<vm::Vec3, vm::Vec3>>& nodes) {
    vm::Nastran nas;
    int id = 1;
    for (const auto& nc : nodes) {
        vm::NastranNode n;
        n.id = id++;
        n.coord = nc.first;
        n.force = nc.second;
        nas.nodes().push_back(n);
    }
    return nas;
}

// Weighted mode distributes a source force across the k nearest targets;
// per-source force is conserved.
static void TestWeightedDistribution() {
    std::printf("[weighted distribution]\n");
    std::vector<vm::SurfaceNode> targets;
    targets.push_back({1, vm::Vec3{0, 0, 0},  vm::Vec3{}});
    targets.push_back({2, vm::Vec3{10, 0, 0}, vm::Vec3{}});

    // One source midway between the two targets.
    vm::Nastran nas = MakeNastran({{vm::Vec3{5, 0, 0}, vm::Vec3{0, 0, 100}}});

    vm::Mapper mapper(targets, 50.0);
    vm::MapOptions opt;
    opt.mode = vm::MapMode::WeightedKNearest;
    opt.k = 4;
    vm::MappingResult r = mapper.Map(nas, /*distance=*/20.0, vm::Vec3{1, 1, 1}, opt);

    CHECK(Near(mapper.targets()[0].force.z, 50.0), "equidistant: target1 gets half");
    CHECK(Near(mapper.targets()[1].force.z, 50.0), "equidistant: target2 gets half");
    CHECK(Near(r.mappedForce.z, 100.0), "weighted total conserved per source");
    CHECK(r.lossCount == 0, "no loss in weighted mode within range");
}

// conserveTotal rescales so the total on targets equals the applied total, even
// when some source force fell outside the search distance.
static void TestConservation() {
    std::printf("[conservation re-normalization]\n");
    std::vector<vm::SurfaceNode> targets;
    targets.push_back({1, vm::Vec3{0, 0, 0}, vm::Vec3{}});

    // One source in range, one far out of range.
    vm::Nastran nas = MakeNastran({
        {vm::Vec3{0, 0, 0},     vm::Vec3{0, 0, 100}},   // in range
        {vm::Vec3{1000, 0, 0},  vm::Vec3{0, 0, 100}},   // out of range -> lost
    });

    vm::Mapper mapper(targets, 50.0);
    vm::MapOptions opt;
    opt.conserveTotal = true;
    vm::MappingResult r = mapper.Map(nas, /*distance=*/20.0, vm::Vec3{1, 1, 1}, opt);

    CHECK(Near(r.appliedForce.z, 200.0), "applied total == 200");
    CHECK(Near(mapper.targets()[0].force.z, 200.0), "target rescaled to applied total");
    CHECK(Near(r.lossForce.z, 0.0), "conserveTotal reports zero loss");
}

// fallbackNearest assigns out-of-range sources to the global nearest target,
// so nothing is lost (without rescaling).
static void TestFallbackNoLoss() {
    std::printf("[fallback no-loss]\n");
    std::vector<vm::SurfaceNode> targets;
    targets.push_back({1, vm::Vec3{0, 0, 0}, vm::Vec3{}});

    vm::Nastran nas = MakeNastran({
        {vm::Vec3{0, 0, 0},    vm::Vec3{0, 0, 100}},
        {vm::Vec3{1000, 0, 0}, vm::Vec3{0, 0, 100}},
    });

    vm::Mapper mapper(targets, 50.0);
    vm::MapOptions opt;
    opt.fallbackNearest = true;
    vm::MappingResult r = mapper.Map(nas, /*distance=*/20.0, vm::Vec3{1, 1, 1}, opt);

    CHECK(r.lossCount == 0, "no lost sources with fallback");
    CHECK(r.fallbackCount == 1, "one source used the fallback");
    CHECK(Near(mapper.targets()[0].force.z, 200.0), "all force lands on the target");
    CHECK(r.maxLossDistance > 900.0, "max unmatched distance is reported");
}

int main() {
    TestGeometry();
    TestAreaMapAdaptiveRange();
    TestAreaMapNearestMatchesBrute();
    TestForceCalc();
    TestExportFilterBugfix();
    TestEndToEndAdx();
    TestDanglingNodeRef();
    TestWeightedDistribution();
    TestConservation();
    TestFallbackNoLoss();

    std::printf("\n%d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
