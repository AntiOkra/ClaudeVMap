#include "vm/Mapper.h"

#include <cstdio>
#include <fstream>

namespace vm {

Mapper::Mapper(std::vector<SurfaceNode>& targets, double pitch)
    : targets_(targets) {
    const double p = (pitch > 0.0) ? pitch : 50.0;
    areaMap_.Build(targets_, p);
}

MappingResult Mapper::Map(const Nastran& nas, double searchDistance, const Vec3& ratio) {
    MappingResult res;
    for (const auto& n : nas.nodes()) {
        const Vec3 mapForce{
            n.force.x * ratio.x,
            n.force.y * ratio.y,
            n.force.z * ratio.z
        };

        double distance = 0.0;
        SurfaceNode* target = areaMap_.NearestNode(n.coord, searchDistance, distance);
        if (target != nullptr) {
            target->force += mapForce;
            res.mappedForce += mapForce;
            ++res.mappedCount;
        } else {
            res.lossForce += mapForce;
            ++res.lossCount;
        }
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
        // Output a node when ANY force component is non-zero (the original
        // checked only .x three times, dropping nodes with zero X but
        // non-zero Y/Z force).
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
