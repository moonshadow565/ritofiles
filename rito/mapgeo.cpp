#include "mapgeo.hpp"

namespace Rito::MapGeoImpl {

}

using namespace Rito;

File::result_t Rito::MapGeo::read(File const& file) RITO_FILE_NOEXCEPT {
    using namespace Rito::MapGeoImpl;
    struct {
        std::array<char, 4> magic;
        uint32_t version;
    } header;
    uint8_t unkbool = 0;
    uint32_t vertexElemGroupCount = 0;
    uint32_t vertexBufferCount = 0;
    uint32_t indexBufferCount = 0;
    uint32_t meshInfoCount = 0;

    file_assert(file.read(header));
    file_assert((header.magic == std::array{'O', 'E', 'G', 'M'}));
    file_assert(header.version == 6u);

    file_assert(file.read(unkbool));

    file_assert(file.read(vertexElemGroupCount));
    file_assert(file.read(vertexElemGroups, vertexElemGroupCount));

    file_assert(file.read(vertexBufferCount));
    for(uint32_t i = 0; i < vertexBufferCount; i++) {
        uint32_t vertexBufferSize = 0;
        file_assert(file.read(vertexBufferSize));
        file_assert(file.read(vertexBuffers.emplace_back(), vertexBufferSize));
    }

    file_assert(file.read(indexBufferCount));
    for(uint32_t i = 0; i < indexBufferCount; i++) {
        uint32_t indexBufferSize = 0;
        file_assert(file.read(indexBufferSize));
        file_assert(file.read(indexBuffers.emplace_back(), indexBufferSize/2));
    }

    file_assert(file.read(meshInfoCount));

    for(uint32_t i = 0; i < meshInfoCount; i++) {
        auto& meshInfo = meshInfos.emplace_back();
        uint32_t meshNameLength = 0;
        uint32_t meshVtxBufferCount = 0;
        uint32_t subMeshCount = 0;
        uint32_t unkStrLength = {};

        file_assert(file.read(meshNameLength));
        file_assert(file.read(meshInfo.name, meshNameLength));
        file_assert(file.read(meshInfo.vertexCount));
        file_assert(file.read(meshVtxBufferCount));
        file_assert(file.read(meshInfo.vertexElemGroup));
        file_assert(file.read(meshInfo.vertexBuffers, meshVtxBufferCount));
        file_assert(file.read(meshInfo.indexCount));
        file_assert(file.read(meshInfo.indexBuffer));
        file_assert(file.read(subMeshCount));

        for(uint32_t c = 0; c < subMeshCount; c++) {
            auto& subMesh = meshInfo.subMeshes.emplace_back();
            uint32_t materialNameLength = {};
            file_assert(file.read(subMesh.unk0));
            file_assert(file.read(materialNameLength));
            file_assert(file.read(subMesh.materialName, materialNameLength));
            file_assert(file.read(subMesh.firstIndex));
            file_assert(file.read(subMesh.indexCount));
            file_assert(file.read(subMesh.unk1));
            file_assert(file.read(subMesh.unk2));
        }
        file_assert(file.read(meshInfo.meshUnkbool0));
        file_assert(file.read(meshInfo.boundingBox));
        file_assert(file.read(meshInfo.transformMatrix));
        file_assert(file.read(meshInfo.meshUnkbool1));
        if(unkbool) {
            file_assert(file.read(meshInfo.unkvec));
        }
        file_assert(file.read(meshInfo.unkdata));
        file_assert(file.read(unkStrLength));
        file_assert(file.read(meshInfo.unkstr, unkStrLength));
        file_assert(file.read(meshInfo.color));
    }

    return File::result_ok;
}
