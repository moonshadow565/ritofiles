#include "skeleton.hpp"
#include "memory.hpp"

using namespace Rito;
namespace  {
    namespace legacy {
        struct Header {
            std::array<char, 8> magic = {};
            uint32_t version = {};
            uint32_t skeletonID = {};
            uint32_t numBones = {};
        };
        struct RawJoint {
            std::array<char, 32> boneName;
            int32_t parentId;
            float boneLength;
            std::array<float, 4> col0;
            std::array<float, 4> col1;
            std::array<float, 4> col2;
            constexpr inline Mtx44 absPlacement() const noexcept {
                return Mtx44 {{
                        { col0[0], col1[0], col2[0], 0.f },
                        { col0[1], col1[1], col2[1], 0.f },
                        { col0[2], col1[2], col2[2], 0.f },
                        { col0[3], col1[3], col2[3], 1.f }
                    }};
            }
        };
    }

    namespace new_v0 {
    }
}


File::result_t Rito::Skeleton::read_legacy(File const& file) RITO_FILE_NOEXCEPT {
    using namespace legacy;
    Header header{};
    uint32_t numShaderBones = {};
    std::vector<RawJoint> rawJoints = {};

    file_assert(file.read(header));
    file_assert((header.magic == std::array{'r', '3', 'd', '2', 's', 'k', 'l', 't'}));
    file_assert(header.version > 0u && header.version < 3u);
    file_assert(file.read(rawJoints, header.numBones));

    if(header.version == 2) {
        file_assert(file.read(numShaderBones));
        file_assert(file.read(shaderJoints, numShaderBones));
    }

    joints.reserve(rawJoints.size());
    uint16_t idx = 0;
    for(auto& raw: rawJoints) {
        Mtx44 invParentMtx = Mtx44Identity;
        if(raw.parentId != -1) {
            auto const parentIdx = static_cast<size_t>(raw.parentId);
            auto& parent = rawJoints[parentIdx];
            invParentMtx = Mtx44_inverse(parent.absPlacement());
        }
        auto const absPlacement = raw.absPlacement();
        auto const relPlacement = Mtx44_Multiply(absPlacement, invParentMtx);
        joints.push_back({
                             0,
                             idx++,
                             static_cast<int16_t>(raw.parentId),
                             0,
                             ElfHash(&raw.boneName[0]),
                             raw.boneLength,
                             Mtx44_Decompose(absPlacement),
                             Mtx44_Decompose(relPlacement),
                             &raw.boneName[0]
                         });
    }

    return File::result_ok;
}

File::result_t Rito::Skeleton::read_new_v0(File const& file) RITO_FILE_NOEXCEPT {
    struct RawJointIndex {
        uint16_t jointIndex;
        uint16_t pad;
        uint32_t jointHash;
    };
    struct RawJoint {
        uint16_t flags;
        uint16_t jointNdx;
        int16_t parentIndx;
        uint16_t pad;
        uint32_t nameHash;
        float radius;
        Form3D parentOffset;
        Form3D invRootOffset;
        RelOffset<char> name;
    };
    struct Header : BaseResource {
        uint32_t formatToken;
        uint32_t version;
        uint16_t flags;
        uint16_t numJoints;
        uint32_t numShaderJoints;
        AbsOffset<RawJoint> joints;
        AbsOffset<RawJointIndex> jointIndices;
        AbsOffset<uint16_t> shaderJoints;
        AbsOffset<char> name;
        AbsOffset<char> assetName;
        uint32_t jointNamesOffset;
        uint32_t extBuffer[5];
    };

    uint32_t dataSize;
    std::vector<uint8_t> data;

    file_assert(file.read(dataSize));
    file_assert(file.seek_cur(-4));
    file_assert(dataSize >= sizeof(Header));
    file_assert(file.read(data, dataSize));

    auto const& header = *reinterpret_cast<Header const*>(data.data());

    file_assert(header.formatToken == 0x22FD4FC3u);
    file_assert(header.version == 0u);

    if(header.assetName) {
        assetName = header[header.assetName];
    }

    auto const sharedJointsArr = &header[header.shaderJoints];
    shaderJoints = { sharedJointsArr, sharedJointsArr + header.numShaderJoints };

    joints.reserve(header.numJoints);
    for(uint32_t i = 0; i < header.numJoints; i++) {
        auto const& raw = header[header.joints + i];
        joints.push_back({
                             raw.flags,
                             raw.jointNdx,
                             raw.parentIndx,
                             raw.pad,
                             raw.nameHash,
                             raw.radius,
                             raw.parentOffset,
                             raw.invRootOffset,
                             &raw.name[0]
                         });
    }
    return File::result_ok;
}

File::result_t Rito::Skeleton::read(File const& file) RITO_FILE_NOEXCEPT {
    struct {
        uint32_t magic0;
        uint32_t magic1;
        uint32_t version;
    } header;
    file_assert(file.read(header));
    file_assert(file.seek_cur(-static_cast<int64_t>(sizeof(header))));
    if(header.magic1 == 0x22FD4FC3u) {
        return read_new_v0(file);
    }
    return read_legacy(file);
}
