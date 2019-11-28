#include "skeleton.hpp"

using namespace Rito;

namespace Rito::SkeletonImpl::Legacy {
    struct Header {
        std::array<char, 8> magic = {};
        uint32_t version = {};
        uint32_t skeletonID = {};
    };

    struct Joint {
        FixedString<32> name;
        int32_t parentId;
        float boneLength;
        Mtx43 absPlacement;
    };

    inline void read(Skeleton& skl, File const& file) {
        auto const header = file.get<Header>();
        auto const joints = file.get<std::vector<Joint>>(size_prefix<int32_t>);
        file_assert((header.magic == std::array{'r', '3', 'd', '2', 's', 'k', 'l', 't'}));
        file_assert(header.version > 0u && header.version < 3u);
        if(header.version == 2) {
            file.read(skl.shaderBones, size_prefix<int32_t>);
        }

        skl.joints.reserve(joints.size());

        for(size_t i = 0; i != joints.size(); i++) {
            auto const& joint = joints[i];
            skl.joints.push_back({
                                     .jointIndx = static_cast<int32_t>(i),
                                     .parentIndx = joint.parentId,
                                     .nameHash = ElfHash(joint.name),
                                     .radius = joint.boneLength,
                                     .absPlacement = joint.absPlacement,
                                     .name = joint.name
                                 });
        }
    }
}

namespace Rito::SkeletonImpl::NewV0 {
    struct RawJointIndex {
        int16_t jointIndex;
        uint16_t pad;
        uint32_t jointHash;
    };

    struct RawJoint {
        uint16_t flags;
        int16_t jointNdx;
        int16_t parentIndx;
        uint16_t pad;
        uint32_t nameHash;
        float radius;
        Form3D parentOffset;
        Form3D invRootOffset;
        RelPtr<char> name;
    };

    struct Header : BaseResource {
        uint32_t formatToken;
        uint32_t version;
        uint16_t flags;
        int16_t numJoints;
        int32_t numShaderJoints;
        AbsPtr<RawJoint> joints;
        AbsPtr<RawJointIndex> jointIndices;
        AbsPtr<int16_t> shaderJoints;
        AbsPtr<char> name;
        AbsPtr<char> assetName;
        int32_t jointNamesOffset;
        int32_t extBuffer[5];
    };

    inline void read(Skeleton& skl, File const& file) {
        auto const header = file.get<WithOffset<Header>>();
        file_assert(header->formatToken == 0x22FD4FC3u);
        file_assert(header->version == 0u);


        auto const jointOffset = header->joints + header;
        for(int32_t i = 0; i < header->numJoints; i++) {
            auto const joint = file.get<RawJoint>(jointOffset[i]);
            std::string name = {};
            if(joint.name) {
                auto const nameOffset = joint.name + &RawJoint::name + jointOffset[i];
                name = file.get<std::string>(nameOffset, zero_terminated);
            }
            skl.joints.push_back({
                                     .flags = joint.flags,
                                     .jointIndx = joint.jointNdx,
                                     .parentIndx = joint.parentIndx,
                                     .nameHash = joint.nameHash,
                                     .radius = joint.radius,
                                     .absPlacement = joint.invRootOffset,
                                     .name = name
                             });
        }

        if (header->numShaderJoints > 0 && header->shaderJoints) {
            auto const shaderJointsOffset = header->shaderJoints + header->shaderJoints;
            auto const shaderJoints = file.get<std::vector<uint32_t>>(shaderJointsOffset,
                                                                      header->numShaderJoints);
            skl.shaderBones = { shaderJoints.begin(), shaderJoints.end() };
        }

        if (header->assetName) {
            auto const assetNameOffset = header->assetName + header.offset;
            skl.assetName = file.get<std::string>(assetNameOffset, zero_terminated);
        }
    }
}

Skeleton::Skeleton(File const& file) {
    file.seek_cur(4);
    auto const magic1 = file.get<uint32_t>();
    file.seek_cur(-8);
    if(magic1 == 0x22FD4FC3u) {
        Rito::SkeletonImpl::NewV0::read(*this, file);
    } else {
        Rito::SkeletonImpl::Legacy::read(*this, file);
    }
}
