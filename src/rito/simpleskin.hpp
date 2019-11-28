#ifndef RITO_SIMPLESKIN_HPP
#define RITO_SIMPLESKIN_HPP
#include "file.hpp"
#include "types.hpp"
#include <vector>
#include <algorithm>

namespace Rito {
    struct SimpleSkin {
        struct SubMesh {
            std::string name;
            int32_t firstVertex;
            int32_t vertexCount;
            int32_t firstIndex;
            int32_t indexCount;
        };
        std::vector<SubMesh> submeshes;
        std::vector<uint32_t> indices;
        std::vector<Vec3> vtxPositions;
        std::vector<std::array<uint8_t, 4>> vtxBlendIndices;
        std::vector<std::array<float, 4>> vtxBlendWeights;
        std::vector<Vec3> vtxNormals;
        std::vector<Vec2> vtxUVs;
        std::vector<ColorB> vtxColors;
        Vec3 pivotPoint;

        SimpleSkin() noexcept = default;
        SimpleSkin(File const& file);
    };
}



#endif // RITO_SIMPLESKIN_HPP
