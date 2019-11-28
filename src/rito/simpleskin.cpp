#include <cstring>
#include "simpleskin.hpp"

using namespace Rito;

namespace Rito::SimpleSkinImpl {
    struct SubMesh {
        FixedString<64> name;
        int32_t firstVertex;
        int32_t vertexCount;
        int32_t firstIndex;
        int32_t indexCount;
    };

    struct VertexBasic {
        Vec3 position;
        std::array<uint8_t, 4> blendIndices;
        std::array<float, 4> blendWeights;
        Vec3 normal;
        Vec2 textureCoord;
    };

    struct VertexColor : VertexBasic{
        ColorB color;
    };

    struct Header {
        uint32_t magic;
        uint32_t version;
    };

    struct Geometry {
        struct Old {
            int32_t numIndices;
            int32_t numVertices;
        };
        uint32_t flags;
        Old old;
        int32_t vertexSize = sizeof(VertexBasic);
        uint32_t vertexType;
        Box3D boundingBox;
        Sphere boundingSphere;
    };

    inline void read(SimpleSkin& skn, File const& file) {
        auto const header = file.get<Header>();
        file_assert(header.magic == 0x00112233u);
        file_assert(header.version >= 0x10000u && header.version <= 0x10004u);


        if(header.version > 0x10000u) {
            auto submeshes = file.get<std::vector<SubMesh>>(size_prefix<int32_t>);
            skn.submeshes.reserve(submeshes.size());
            for(auto const& from: submeshes) {
                auto [name, firstVertex, vertexCount, firstIndex, indexCount] = from;
                auto& submesh = skn.submeshes.emplace_back();
                submesh.name = name;
                submesh.firstVertex = firstVertex >= 0 && vertexCount > 0 ? firstVertex : 0;
                submesh.vertexCount = firstVertex >= 0 && vertexCount > 0 ? vertexCount : 0;
                submesh.firstIndex = firstIndex >= 0 && indexCount > 0 ? firstIndex : 0;
                submesh.indexCount = firstIndex >= 0 && indexCount > 0 ? indexCount : 0;
            }
        }

        auto geometry = Geometry{};
        if (header.version > 0x10003u) {
            file.read(geometry);
        } else {
            file.read(geometry.old);
        }

        file_assert(geometry.vertexType == 0
                    ? geometry.vertexSize == sizeof(VertexBasic)
                    : geometry.vertexSize == sizeof(VertexColor));

        auto const indices = file.get<std::vector<uint16_t>>(geometry.old.numIndices);
        skn.indices = { indices.begin(), indices.end() };

        auto const vertices = file.get<std::vector<uint8_t>>(geometry.old.numVertices * geometry.vertexSize);
        skn.vtxPositions.reserve(static_cast<size_t>(geometry.old.numVertices));
        skn.vtxBlendIndices.reserve(static_cast<size_t>(geometry.old.numVertices));
        skn.vtxBlendWeights.reserve(static_cast<size_t>(geometry.old.numVertices));
        skn.vtxNormals.reserve(static_cast<size_t>(geometry.old.numVertices));
        skn.vtxUVs.reserve(static_cast<size_t>(geometry.old.numVertices));
        if(geometry.vertexType == 1) {
            skn.vtxColors.reserve(static_cast<size_t>(geometry.old.numVertices));
        }

        for(auto i = vertices.data();
            i != vertices.data() + vertices.size();
            i += geometry.vertexSize) {
            VertexColor vtx;
            memcpy(&vtx, i, static_cast<size_t>(geometry.vertexSize));
            skn.vtxPositions.push_back(vtx.position);
            skn.vtxBlendIndices.push_back(vtx.blendIndices);
            skn.vtxBlendWeights.push_back(vtx.blendWeights);
            skn.vtxNormals.push_back(vtx.normal);
            skn.vtxUVs.push_back(vtx.textureCoord);
            if(geometry.vertexType == 1) {
                skn.vtxColors.push_back(vtx.color);
            }
        }

        skn.pivotPoint = file.get<Vec3>();
    }
}

SimpleSkin::SimpleSkin(File const& file) {
    Rito::SimpleSkinImpl::read(*this, file);
}
