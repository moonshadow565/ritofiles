#include "mapgeo.hpp"

using namespace Rito;

namespace Rito::MapGeoImpl {
    struct Header {
        std::array<char, 4> magic;
        uint32_t version;
    };

    void read(MapGeo& map, File const& file) {
        auto const header = file.get<Header>();
        file_assert((header.magic == std::array{'O', 'E', 'G', 'M'}));
        file_assert(header.version == 6u);

        auto const unkbool = file.get<bool>();
        file.read(map.vertexElemGroups, size_prefix<int32_t>);

        auto const vertexBufferCount = file.get<int32_t>();
        for(int32_t i = 0; i < vertexBufferCount; i++) {
            file.read(map.vertexBuffers.emplace_back(), size_prefix<int32_t>);
        }

        auto const indexBufferCount = file.get<int32_t>();
        for(int32_t i = 0; i < indexBufferCount; i++) {
            auto const indexBufferSize = file.get<int32_t>();
            file.read(map.indexBuffers.emplace_back(), indexBufferSize / 2);
        }

        auto const meshInfoCount = file.get<int32_t>();
        for(int32_t i = 0; i < meshInfoCount; i++) {
            auto& meshInfo = map.meshInfos.emplace_back();
            file.read(meshInfo.name, size_prefix<int32_t>);
            file.read(meshInfo.vertexCount);
            auto const meshVtxBufferCount = file.get<int32_t>();
            file.read(meshInfo.vertexElemGroup);
            file.read(meshInfo.vertexBuffers, meshVtxBufferCount);
            file.read(meshInfo.indexCount);
            file.read(meshInfo.indexBuffer);

            auto const subMeshCount = file.get<int32_t>();
            for(int32_t c = 0; c < subMeshCount; c++) {
                auto& subMesh = meshInfo.subMeshes.emplace_back();
                file.read(subMesh.unk0);
                file.read(subMesh.materialName, size_prefix<int32_t>);
                file.read(subMesh.firstIndex);
                file.read(subMesh.indexCount);
                file.read(subMesh.unk1);
                file.read(subMesh.unk2);
            }

            file.read(meshInfo.meshUnkbool0);
            file.read(meshInfo.boundingBox);
            file.read(meshInfo.transformMatrix);
            file.read(meshInfo.meshUnkbool1);

            if(unkbool) {
                file.read(meshInfo.unkvec);
            }

            file.read(meshInfo.unkdata);
            file.read(meshInfo.unkstr, size_prefix<int32_t>);
            file.read(meshInfo.color);
        }
    }
}

Rito::MapGeo::MapGeo(File const& file) {
    Rito::MapGeoImpl::read(*this, file);
}

