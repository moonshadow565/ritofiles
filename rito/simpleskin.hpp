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
            uint32_t firstVertex;
            uint32_t vertexCount;
            uint32_t firstIndex;
            uint32_t indexCount;
        };

        struct Vertex {
            Vec3 position;
            std::array<uint8_t, 4> blendIndices;
            std::array<float, 4> blendWeights;
            Vec3 normal;
            Vec2 textureCoord;
            ColorB color;
        };

        std::vector<SubMesh> submeshes = {};
        std::vector<uint32_t> indices = {};
        std::vector<Vertex> vertices = {};

        Vec3 pivotPoint = {};
        Box3D boundingBox = {};
        Sphere boundingSphere = {};
        uint32_t flags = {};
        uint32_t vertexColorCount = {};

        File::result_t read(File const& file) RITO_FILE_NOEXCEPT;
    };
}

#endif // RITO_SIMPLESKIN_HPP
