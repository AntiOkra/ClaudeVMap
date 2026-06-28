// cli_main.cpp - command-line driver for the portable VectorMapping core.
//
// Mirrors the GUI "Exec" pipeline:
//   read Nastran mesh -> read normal/tangent pressure -> ForceCalc
//   -> read ADX -> Activate -> extract surface nodes of selected sets
//   -> map forces (nearest node within search distance) -> export force file.
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "vm/Adx.h"
#include "vm/Mapper.h"
#include "vm/Nastran.h"

namespace {

void Usage(const char* prog) {
    std::cerr <<
        "Usage: " << prog << " [options]\n"
        "  --nastran   <file>     Nastran mesh file (required)\n"
        "  --normal    <file>     normal pressure vector file (required)\n"
        "  --tangent   <file>     tangent pressure vector file (required)\n"
        "  --adx       <file>     ADX mesh file (required)\n"
        "  --out       <file>     output ADX force file (required)\n"
        "  --set       <name>     element set to map (repeatable; default: all)\n"
        "  --distance  <value>    search distance (default 30)\n"
        "  --ratio     <value>    uniform mapping ratio (default 1)\n"
        "  --ratio-xyz <x> <y> <z>  per-axis mapping ratio\n"
        "  --process   <id>       process_id written to the force file\n";
}

std::string Arg(int& i, int argc, char** argv) {
    if (i + 1 >= argc) { std::cerr << "Missing value for " << argv[i] << "\n"; std::exit(2); }
    return argv[++i];
}

} // namespace

int main(int argc, char** argv) {
    std::string nastranPath, normalPath, tangentPath, adxPath, outPath;
    std::string processId = "AdvcStep-99999";
    std::vector<std::string> sets;
    double distance = 30.0;
    vm::Vec3 ratio{1.0, 1.0, 1.0};

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if      (a == "--nastran")  nastranPath = Arg(i, argc, argv);
        else if (a == "--normal")   normalPath  = Arg(i, argc, argv);
        else if (a == "--tangent")  tangentPath = Arg(i, argc, argv);
        else if (a == "--adx")      adxPath     = Arg(i, argc, argv);
        else if (a == "--out")      outPath     = Arg(i, argc, argv);
        else if (a == "--set")      sets.push_back(Arg(i, argc, argv));
        else if (a == "--distance") distance    = std::stod(Arg(i, argc, argv));
        else if (a == "--ratio")    { double r = std::stod(Arg(i, argc, argv)); ratio = {r, r, r}; }
        else if (a == "--ratio-xyz") {
            ratio.x = std::stod(Arg(i, argc, argv));
            ratio.y = std::stod(Arg(i, argc, argv));
            ratio.z = std::stod(Arg(i, argc, argv));
        }
        else if (a == "--process")  processId   = Arg(i, argc, argv);
        else if (a == "--help" || a == "-h") { Usage(argv[0]); return 0; }
        else { std::cerr << "Unknown option: " << a << "\n"; Usage(argv[0]); return 2; }
    }

    if (nastranPath.empty() || normalPath.empty() || tangentPath.empty() ||
        adxPath.empty() || outPath.empty()) {
        Usage(argv[0]);
        return 2;
    }

    std::string log;
    vm::Nastran nas;
    if (nas.ReadMeshFile(nastranPath, &log))       { std::cerr << log; return 1; }
    if (nas.ReadNormalVectorFile(normalPath, &log)) { std::cerr << log; return 1; }
    if (nas.ReadTangentVectorFile(tangentPath, &log)){ std::cerr << log; return 1; }
    nas.ForceCalc();

    vm::Adx adx;
    if (adx.Read(adxPath))  { std::cerr << "ERROR: cannot read ADX file\n"; return 1; }
    if (adx.Activate())     { std::cerr << "ERROR: ADX activate failed (unknown node ref)\n"; return 1; }

    if (sets.empty()) {
        for (const auto& es : adx.elementSets()) sets.push_back(es.nameAdx);
    }

    std::vector<vm::SurfaceNode> targets;
    adx.ExtractSurfaceNodes(sets, targets);
    if (targets.empty()) {
        std::cerr << "ERROR: no surface nodes extracted for the selected sets\n";
        return 1;
    }

    vm::Mapper mapper(targets, /*pitch=*/(distance > 50.0 ? distance : 50.0));
    vm::MappingResult r = mapper.Map(nas, distance, ratio);
    if (mapper.ExportAdxForce(outPath, processId)) {
        std::cerr << "ERROR: cannot write output force file\n";
        return 1;
    }

    const vm::Vec3 totalNas = nas.TotalForce();
    std::printf("Nastran nodes      : %zu\n", nas.nodes().size());
    std::printf("ADX surface nodes  : %zu\n", targets.size());
    std::printf("Mapped / Lost      : %d / %d\n", r.mappedCount, r.lossCount);
    std::printf("Total Nastran force: (%.6f, %.6f, %.6f)\n", totalNas.x, totalNas.y, totalNas.z);
    std::printf("Mapped force       : (%.6f, %.6f, %.6f)\n", r.mappedForce.x, r.mappedForce.y, r.mappedForce.z);
    std::printf("Lost force         : (%.6f, %.6f, %.6f)\n", r.lossForce.x, r.lossForce.y, r.lossForce.z);
    std::printf("Output             : %s\n", outPath.c_str());
    return 0;
}
