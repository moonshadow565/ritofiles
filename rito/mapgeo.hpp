#ifndef RITO_MAPGEO_HPP
#define RITO_MAPGEO_HPP
#include "file.hpp"
#include "types.hpp"

namespace Rito {
    struct MapGeo {
        struct VertexElemGroup {
            enum class Usage : uint32_t {
                Static = 0x0,
                Dynamic = 0x1,
                Stream = 0x2
            };
            enum class Name : uint32_t {
                Position = 0x0,
                Normal = 0x1,
                FogCoord = 0x2,
                PrimaryColor = 0x3,
                SecondaryColor = 0x4,
                Texcoord0 = 0x5,
                Texcoord1 = 0x6,
                Texcoord2 = 0x7,
                Texcoord3 = 0x8,
                Texcoord4 = 0x9,
                Texcoord5 = 0xA,
                Texcoord6 = 0xB,
                Texcoord7 = 0xC,
            };
            enum class Format : uint32_t {
                X_Float32 = 0x0,
                XY_Float32 = 0x1,
                XYZ_Float32 = 0x2,
                XYZW_Float32 = 0x3,
                BGRA_Packed8888 = 0x4,
                ZYXW_Packed8888 = 0x4,
                RGBA_Packed8888 = 0x5,
                XYZW_Packed8888 = 0x5,
            };
            struct Elem {
                Name name = {};
                Format format = {};
            };
            Usage usage = {};
            uint32_t elemCount = {};
            std::array<Elem, 15> elems = {};
            inline constexpr size_t size() const noexcept {
                size_t s = 0;
                for(uint32_t i = 0; i < elemCount; i++) {
                    switch(elems[i].format) {
                    case Format::X_Float32:
                        s += 4 * 1;
                    break;
                    case Format::XY_Float32:
                        s += 4 * 2;
                    break;
                    case Format::XYZ_Float32:
                        s += 4 * 3;
                    break;
                    case Format::XYZW_Float32:
                        s += 4 * 4;
                    break;
                    case Format::BGRA_Packed8888:
                    case Format::RGBA_Packed8888:
                        s += 4;
                        break;
                    }
                }
                return s;
            }
        };
        struct SubMesh {
            uint32_t unk0;
            std::string materialName;
            uint32_t firstIndex;
            uint32_t indexCount;
            uint32_t unk1;
            uint32_t unk2;
        };

        struct MeshInfo {
            std::string name;
            uint32_t vertexElemGroup;
            uint32_t vertexCount;
            std::vector<uint32_t> vertexBuffers;
            uint32_t indexCount;
            uint32_t indexBuffer;
            std::vector<SubMesh> subMeshes;
            Box3D boundingBox;
            Mtx44 transformMatrix;
            uint8_t meshUnkbool0 = 0;
            uint8_t meshUnkbool1 = 0;
            Vec3 unkvec = {};
            std::array<uint8_t, 108> unkdata = {};
            std::string unkstr;

            ColorF color;
        };

        std::vector<VertexElemGroup> vertexElemGroups = {};
        std::vector<std::vector<uint8_t>> vertexBuffers = {};
        std::vector<std::vector<uint16_t>> indexBuffers = {};
        std::vector<MeshInfo> meshInfos = {};
        File::result_t read(File const& file) RITO_FILE_NOEXCEPT;
    };
}

#endif // RITO_MAPGEO_HPP
