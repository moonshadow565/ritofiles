#include "skeleton.hpp"

using namespace Rito;

File::result_t Rito::Skeleton::read_legacy(File const& file) RITO_FILE_NOEXCEPT {
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

    std::array<char, 8> magic = {};
    uint32_t version = {};
    uint32_t skeletonID = {};
    uint32_t numBones = {};
    uint32_t numShaderBones = {};
    std::vector<RawJoint> rawJoints = {};

    file_assert(file.read(magic));
    file_assert((magic == std::array{'r', '3', 'd', '2', 's', 'k', 'l', 't'}));
    file_assert(version > 0u && version < 3u);
    file_assert(file.read(skeletonID));
    file_assert(file.read(numBones));
    file_assert(file.read(rawJoints, numBones));
    if(version == 2) {
        file_assert(file.read(numShaderBones));
        file_assert(file.read(shaderJoints, numShaderBones));
    }
    file_assert(file.read(rawJoints, numBones));

    joints.reserve(rawJoints.size());
    uint16_t idx = 0;
    for(auto& raw: rawJoints) {
        Mtx44 invParentMtx = Mtx44Identity;
        if(raw.parentId != -1) {
            auto const parentIdx = static_cast<size_t>(raw.parentId);
            file_assert(parentIdx < rawJoints.size());
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
        uint32_t name;
    };
    struct Header {
        uint32_t resourceSize;
        uint32_t formatToken;
        uint32_t version;
        uint16_t flags;
        uint16_t numJoints;
        uint32_t numShaderJoints;
        uint32_t jointsOffset;
        uint32_t jointIndicesOffset;
        uint32_t shaderJointOffset;
        uint32_t nameOffset;
        uint32_t assetNameOffset;
        uint32_t jointNamesOffset;
        uint32_t extBuffer[5];
        inline constexpr auto jointsOffsetEnd() const noexcept {
            return jointsOffset + numJoints * sizeof(RawJoint);
        }
        inline constexpr auto jointIndicesOffsetEnd() const noexcept {
            return jointIndicesOffset + numJoints * sizeof(RawJointIndex);
        }
        inline constexpr auto shaderJointsOffsetEnd() const noexcept {
            return shaderJointOffset + numShaderJoints * sizeof(uint32_t);
        }
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

    file_assert(header.jointsOffset < header.resourceSize);
    file_assert(header.jointsOffsetEnd() <= header.resourceSize);

    file_assert(header.jointIndicesOffset < header.resourceSize);
    file_assert(header.jointIndicesOffsetEnd() <= header.resourceSize);

    file_assert(header.shaderJointOffset < header.resourceSize);
    file_assert(header.shaderJointsOffsetEnd() <= header.resourceSize);

    file_assert(header.jointNamesOffset < header.resourceSize);

    if(header.nameOffset != 0u && header.nameOffset != 0xFFFFFFFFu) {
        assetName = reinterpret_cast<char const*>(data.data() + header.nameOffset);
        file_assert(header.nameOffset < header.resourceSize);
    }

    auto const rawShaderJointAddr = data.data() + header.jointsOffset;
    auto const rawShaderJointBeg = reinterpret_cast<uint32_t const*>(rawShaderJointAddr);
    auto const rawShaderJointEnd = rawShaderJointBeg + header.numShaderJoints;
    shaderJoints.reserve(header.numShaderJoints);
    shaderJoints = { rawShaderJointBeg, rawShaderJointEnd };

    auto const rawJointAddr = data.data() + header.jointsOffset;
    auto const rawJointBeg = reinterpret_cast<RawJoint const*>(rawJointAddr);
    auto const rawJointEnd = rawJointBeg + header.numJoints;
    joints.reserve(header.numJoints);
    for(auto raw = rawJointBeg; raw != rawJointEnd; raw++) {
        auto const nameAddr = reinterpret_cast<uint8_t const*>(&raw->name) + raw->name;
        file_assert(nameAddr > data.data());
        file_assert(nameAddr < data.data() + data.size());
        joints.push_back({
                             raw->flags,
                             raw->jointNdx,
                             raw->parentIndx,
                             raw->pad,
                             raw->nameHash,
                             raw->radius,
                             raw->parentOffset,
                             raw->invRootOffset,
                             reinterpret_cast<char const*>(nameAddr)
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
