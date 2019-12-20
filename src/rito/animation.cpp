#include <unordered_map>
#include <cstdio>
#include "animation.hpp"

using namespace Rito;

namespace Rito::AnimationImpl::Legacy {
    struct Header {
        uint32_t skeletonID;
        int32_t numTracks;
        int32_t numFrames;
        int32_t frameRate;
    };

    struct Track {
        FixedString<32> name;
        uint32_t flags;
    };

    struct Frame {
        Quat quaternion;
        Vec3 position;
    };

    inline void read(Animation& anm, File const& file) {
        auto const header = file.get<Header>();
        anm.tickDuration = 1.f / header.frameRate;

        auto const tracks = file.get<std::vector<Track>>(header.numTracks);
        for(auto const& rawTrack: tracks) {
            auto frames = file.get<std::vector<Frame>>(header.numFrames);
            auto& track = anm.tracks.emplace_back();
            track.positions.reserve(frames.size());
            track.scales.reserve(frames.size());
            track.rotations.reserve(frames.size());
            track.name = rawTrack.name;
            track.boneHash = ElfHash(rawTrack.name);
            for(auto const& frame: frames) {
                track.positions.push_back(frame.position);
                track.rotations.push_back(frame.quaternion);
                track.scales.push_back(Vec3{1.0f, 1.0f, 1.0f});
            }
        }
    }
}

namespace Rito::AnimationImpl::NewV4 {
    struct Frame {
        uint32_t boneHash;
        uint16_t posIndx;
        uint16_t scaleIndx;
        uint16_t quatIndx;
        uint16_t pad;
    };

    struct Header : BaseResource {
        uint32_t formatToken;
        uint32_t version;
        uint32_t flags;
        int32_t numTracks;
        int32_t numFrames;
        float tickDuration;
        int32_t tracksOffset;
        AbsPtr<char> assetName;
        int32_t timeOffset;
        AbsPtr<Vec3> vectors;
        AbsPtr<Quat> quats;
        AbsPtr<Frame> frames;
        int32_t extBuffer[3];
    };

    inline void read(Animation& anm, File const& file) {
        auto const header = file.get<WithOffset<Header>>();
        anm.tickDuration = header->tickDuration;
        auto const framesOffset = header->frames + header;
        auto const vectorsOffset = header->vectors + header;
        auto const quatsOffset = header->quats + header;
        for(int32_t t = 0; t < header->numTracks; t++) {
            auto& track = anm.tracks.emplace_back();
            track.positions.reserve(static_cast<size_t>(header->numFrames));
            track.scales.reserve(static_cast<size_t>(header->numFrames));
            track.rotations.reserve(static_cast<size_t>(header->numFrames));
            for(int32_t f = 0; f < header->numFrames; f++) {
                auto const frameIdx = f * header->numTracks + t;
                auto const frameOffset = framesOffset[frameIdx];
                auto const frame = file.get<Frame>(frameOffset);
                track.boneHash = frame.boneHash;
                track.boneHash = frame.boneHash;
                track.positions.push_back(file.get<Vec3>(vectorsOffset[frame.posIndx]));
                track.scales.push_back(file.get<Vec3>(vectorsOffset[frame.scaleIndx]));
                track.rotations.push_back(file.get<Quat>(quatsOffset[frame.quatIndx]).normalize());
            }
        }

        if (header->assetName) {
            auto const assetNameOffset = header->assetName + header.offset;
            anm.assetName = file.get<std::string>(assetNameOffset, zero_terminated);
        }
    }
}

namespace Rito::AnimationImpl::NewV5 {
    struct Frame  {
        uint16_t posIndx;
        uint16_t scaleIndx;
        uint16_t quatIndx;
    };

    struct Header : BaseResource {
        uint32_t formatToken;
        uint32_t version;
        uint32_t flags;
        int32_t numTracks;
        int32_t numFrames;
        float tickDuration;
        AbsPtr<uint32_t> jointHashes;
        AbsPtr<char> assetName;
        int32_t timeOffset;
        AbsPtr<Vec3> vectors;
        AbsPtr<QuantizedQuat> quats;
        AbsPtr<Frame> frames;
        int32_t extBuffer[3];
    };

    inline void read(Animation& anm, File const& file) {
        auto const header = file.get<WithOffset<Header>>();
        anm.tickDuration = header->tickDuration;

        auto const hashesOffset = header->jointHashes + header;
        auto const framesOffset = header->frames + header;
        auto const vectorsOffset = header->vectors + header;
        auto const quatsOffset = header->quats + header;
        for(int32_t t = 0; t < header->numTracks; t++) {
            auto track = anm.tracks.emplace_back();
            track.positions.reserve(static_cast<size_t>(header->numFrames));
            track.scales.reserve(static_cast<size_t>(header->numFrames));
            track.rotations.reserve(static_cast<size_t>(header->numFrames));
            for(int32_t f = 0; f < header->numFrames; f++) {
                auto const frameIdx = f * header->numTracks + t;
                auto const frameOffset = framesOffset[frameIdx];
                auto const frame = file.get<Frame>(frameOffset);
                track.boneHash = file.get<uint32_t>(hashesOffset[frameIdx]);
                track.positions.push_back(file.get<Vec3>(vectorsOffset[frame.posIndx]));
                track.scales.push_back(file.get<Vec3>(vectorsOffset[frame.scaleIndx]));
                auto const quat_quantized = file.get<QuantizedQuat>(quatsOffset[frame.quatIndx]);
                auto const quat = static_cast<Quat>(quat_quantized);
                track.rotations.push_back(quat.normalize());
            }
        }

        if (header->assetName) {
            auto const assetNameOffset = header->assetName + header.offset;
            anm.assetName = file.get<std::string>(assetNameOffset, zero_terminated);
        }
    }
}

namespace Rito::AnimationImpl::NewCompressed {
    struct Frame {
      uint16_t keyTime;
      uint16_t jointIndex;
      std::array<uint16_t, 3> v;
    };

    struct Header : BaseResource {
        uint32_t formatToken;
        uint32_t flags;
        int32_t jointCount;
        int32_t frameCount;
        int32_t jumpCacheCount;
        float duration;
        float fps;
        float rotErrorMargin;
        float rotDiscontinuityThreshold;
        float traErrorMargin;
        float traDiscontinuityThreshold;
        float scaErrorMargin;
        float scaDiscontinuityThreshold;
        Vec3 translationMin;
        Vec3 translationMax;
        Vec3 scaleMin;
        Vec3 scaleMax;
        int32_t framesOffset;
        int32_t jumpCachesOffset;
        int32_t jointNameHashesOffset;
    };

    [[noreturn]] inline void read(Animation&, File const&) {
        file_assert(("TODO" && false));
    }
}

Rito::Animation::Animation(File const& file) {
    struct Header {
        std::array<char, 8> magic;
        uint32_t version;
    };
    Header header{};
    file.read(header);
    if(header.magic == std::array{'r', '3', 'd', '2', 'c', 'a', 'n', 'm'}) {
        file_assert(header.version == 1);
        Rito::AnimationImpl::NewCompressed::read(*this, file);
    } else {
        file_assert((header.magic == std::array{'r', '3', 'd', '2', 'a', 'n', 'm', 'd'}));
        if(header.version == 4u) {
            Rito::AnimationImpl::NewV4::read(*this, file);
        } else if(header.version == 5u) {
            Rito::AnimationImpl::NewV5::read(*this, file);
        } else {
            file_assert(header.version == 3u);
            Rito::AnimationImpl::Legacy::read(*this, file);
        }
    }
}
