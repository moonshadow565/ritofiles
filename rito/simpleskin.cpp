#include "simpleskin.hpp"

using namespace Rito;

File::result_t Rito::SimpleSkin::read(File const& file) RITO_FILE_NOEXCEPT {
    struct RawSubMesh {
        std::array<char, 64> name;
        uint32_t firstVertex;
        uint32_t vertexCount;
        uint32_t firstIndex;
        uint32_t indexCount;
    };
    struct RawVertexBasic {
        Vec3 position;
        std::array<uint8_t, 4> blendIndices;
        std::array<float, 4> blendWeights;
        Vec3 normal;
        Vec2 textureCoord;
    };
    struct RawVertexWithColor {
        Vec3 position;
        std::array<uint8_t, 4> blendIndices;
        std::array<float, 4> blendWeights;
        Vec3 normal;
        Vec2 textureCoord;
        ColorB color;
    };

    struct {
        uint32_t magic;
        uint32_t version;
    } header = {};
    uint32_t submeshCount = {};
    std::vector<RawSubMesh> rawSubMeshes = {};
    struct {
        uint32_t flags;
        struct {
            uint32_t numIndices;
            uint32_t numVertices;
        } old;
        uint32_t vertexSize = sizeof(RawVertexBasic);
        uint32_t vertexType = 0;
        Box3D boundingBox = {};
        Sphere boundingSphere = {};
    } geometry = {};
    std::vector<uint16_t> indicesRaw;
    std::vector<uint8_t> verticesRaw;
    Vec3 rawPivotPoint = {};

    file_assert(file.read(header));
    file_assert(header.magic == 0x00112233u);
    file_assert(header.version >= 0x10000u && header.version <= 0x10004u);
    if(header.version > 0x10000u) {
        file_assert(file.read(submeshCount));
        file_assert(file.read(rawSubMeshes, submeshCount));
    }
    if(header.version > 0x10003u) {
        file_assert(file.read(geometry));
    } else {
        file_assert(file.read(geometry.old));
    }
    file_assert(geometry.vertexType == 0 ?
                    geometry.vertexSize == sizeof(RawVertexBasic) :
                    geometry.vertexSize == sizeof(RawVertexWithColor));
    file_assert(file.read(indicesRaw, geometry.old.numIndices));
    file_assert(file.read(verticesRaw, geometry.old.numVertices * geometry.vertexSize));
    if(header.version > 0x10001u) {
        file_assert(file.read(rawPivotPoint));
    }

    submeshes.resize(submeshCount);
    std::transform(rawSubMeshes.begin(), rawSubMeshes.end(), submeshes.begin(),
                   [](RawSubMesh const& raw){
        return SubMesh {
            &raw.name[0],
                    raw.firstVertex >= 0x7FFFFFFF ? 0 : raw.firstVertex,
                    raw.vertexCount,
                    raw.firstIndex >= 0x7FFFFFFF ? 0 : raw.firstIndex,
                    raw.indexCount
        };
    });

    indices.resize(geometry.old.numIndices);
    std::transform(indicesRaw.begin(), indicesRaw.end(), indices.begin(),
                   [](uint16_t const& raw){
        return raw;
    });

    vertices.resize(geometry.old.numVertices);
    auto const transformVertices =  [](auto const& raw){
        using vertex_t = std::decay_t<decltype(raw)>;
        if constexpr(std::is_same_v<vertex_t, RawVertexWithColor>) {
            return Vertex {
                raw.position,
                        raw.blendIndices,
                        raw.blendWeights,
                        raw.normal,
                        raw.textureCoord,
                        raw.color
            };
        } else {
            return Vertex {
                raw.position,
                        raw.blendIndices,
                        raw.blendWeights,
                        raw.normal,
                        raw.textureCoord
            };
        }
    };

    if(geometry.vertexType == 0) {
        vertexColorCount = 0;
        auto const start = reinterpret_cast<RawVertexBasic const*>(verticesRaw.data());
        auto const end = start + geometry.old.numVertices;
        std::transform(start, end, vertices.begin(), transformVertices);
    } else {
        vertexColorCount = 1;
        auto const start = reinterpret_cast<RawVertexWithColor const*>(verticesRaw.data());
        auto const end = start + geometry.old.numVertices;
        std::transform(start, end, vertices.begin(), transformVertices);
    }

    // TODO: alternatively recalculate those
    pivotPoint = rawPivotPoint;
    boundingBox = geometry.boundingBox;
    boundingSphere = geometry.boundingSphere;
    flags = geometry.flags;

    return File::result_ok;
}
