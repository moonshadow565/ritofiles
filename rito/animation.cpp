#include "animation.hpp"
#include <unordered_map>
#include <cstdio>
#include "memory.hpp"

using namespace Rito;

namespace Rito::AnimationImpl{
    namespace legacy {
        struct Header {
            uint32_t skeletonID;
            uint32_t numTracks;
            uint32_t numFrames;
            int32_t frameRate;
        };
        struct RawTrack {
            std::array<char, 32> name;
            uint32_t flags;
        };
        struct RawFrame {
            Quat quaternion;
            Vec3 point;
        };
    }
    namespace new_v4 {
        struct RawFrame {
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
            uint32_t numTracks;
            uint32_t numFrames;
            float tickDuration;
            uint32_t tracksOffset;
            AbsPtr<char> assetName;
            uint32_t timeOffset;
            AbsPtr<Vec3> vectors;
            AbsPtr<Quat> quats;
            AbsPtr<RawFrame> frames;
            uint32_t extBuffer[3];
        };
    }
    namespace new_v5 {
        struct RawFrame {
            uint16_t posIndx;
            uint16_t scaleIndx;
            uint16_t quatIndx;
        };
        struct Header : BaseResource {
            uint32_t formatToken;
            uint32_t version;
            uint32_t flags;
            uint32_t numTracks;
            uint32_t numFrames;
            float tickDuration;
            AbsPtr<uint32_t> jointHashes;
            AbsPtr<char> assetName;
            uint32_t timeOffset;
            AbsPtr<Vec3> vectors;
            AbsPtr<QuantizedQuat> quats;
            AbsPtr<RawFrame> frames;
            uint32_t extBuffer[3];
        };
    }
    namespace new_compressed {
        struct Header {
            uint32_t resourceSize;
            uint32_t formatToken;
            uint32_t flags;
            uint32_t jointCount;
            uint32_t frameCount;
            uint32_t jumpCacheCount;
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
            uint32_t framesOffset;
            uint32_t jumpCachesOffset;
            uint32_t jointNameHashesOffset;
        };
        struct RawFrame {
          uint16_t keyTime;
          uint16_t jointIndex;
          std::array<uint16_t, 3> v;
        };
    }
}

File::result_t Rito::Animation::read(File const& file) RITO_FILE_NOEXCEPT {
    struct Header {
        std::array<char, 8> magic;
        uint32_t version;
    };
    Header header{};
    file_assert(file.read(header));
    if(header.magic == std::array{'r', '3', 'd', '2', 'c', 'a', 'n', 'm'}) {
        file_assert(header.version == 1);
        return read_compressed(file);
    }
    file_assert((header.magic == std::array{'r', '3', 'd', '2', 'a', 'n', 'm', 'd'}));
    if(header.version == 4u) {
        return read_v4(file);
    }
    if(header.version == 5u) {
        return read_v5(file);
    }
    file_assert(header.version == 3u);
    return read_legacy(file);
}

File::result_t Rito::Animation::read_legacy(File const& file) RITO_FILE_NOEXCEPT {
    using namespace Rito::AnimationImpl::legacy;
    Header header = {};
    file_assert(file.read(header));
    tickDuration = 1.f / header.frameRate;

    for(uint32_t i = 0; header.numTracks; i++) {
        RawTrack rawTrack{};
        std::vector<RawFrame> rawFrames{};
        file_assert(file.read(rawTrack));

        auto& track = tracks.emplace_back();
        file_assert(file.read(rawFrames, header.numFrames));
        track.frames.reserve(rawFrames.size());
        track.name = &rawTrack.name[0];
        track.boneHash = ElfHash(&rawTrack.name[0]);
        for(auto const& rawFrame: rawFrames) {
            auto& frame = track.frames.emplace_back();
            frame.quat = Quat_normalize(rawFrame.quaternion);
            frame.position = rawFrame.point;
        }
    }
    return File::result_ok;
}

File::result_t Rito::Animation::read_v4(File const& file) RITO_FILE_NOEXCEPT {
    using namespace Rito::AnimationImpl::new_v4;
    uint32_t dataSize;
    file_assert(file.read(dataSize));
    file_assert(file.seek_cur(-4));
    file_assert(dataSize >= sizeof(Header));

    std::vector<uint8_t> data{};
    file_assert(file.read(data, dataSize));
    Header const& header = *reinterpret_cast<Header const*>(data.data());

    tickDuration = header.tickDuration;
    if(auto assetNamePtr = header.assetName.get(header); assetNamePtr) {
        assetName = assetNamePtr;
    }

    tracks.resize(header.numTracks);
    for(auto& track: tracks) {
        track.frames.resize(header.numFrames);
    }

    for(uint32_t t = 0; t < header.numTracks; t++) {
        for(uint32_t f = 0; f < header.numFrames; f++) {
            auto const raw = *header.frames.get(header, f * header.numTracks + t);
            tracks[t].boneHash = raw.boneHash;
            tracks[t].frames[f] = {
                *header.vectors.get(header, raw.posIndx),
                *header.vectors.get(header, raw.scaleIndx),
                Quat_normalize(*header.quats.get(header, raw.quatIndx))
            };
        }
    }

    return File::result_ok;
}

File::result_t Rito::Animation::read_v5(File const& file) RITO_FILE_NOEXCEPT {
    using namespace Rito::AnimationImpl::new_v5;
    uint32_t dataSize;
    file_assert(file.read(dataSize));
    file_assert(file.seek_cur(-4));
    file_assert(dataSize >= sizeof(Header));

    std::vector<uint8_t> data{};
    file_assert(file.read(data, dataSize));
    Header const& header = *reinterpret_cast<Header const*>(data.data());

    tickDuration = header.tickDuration;
    if(auto assetNamePtr = header.assetName.get(header); assetNamePtr) {
        assetName = assetNamePtr;
    }

    tracks.resize(header.numTracks);
    for(auto& track: tracks) {
        track.frames.resize(header.numFrames);
    }

    for(uint32_t t = 0; t < header.numTracks; t++) {
        tracks[t].boneHash = *header.jointHashes.get(header, t);
        for(uint32_t f = 0; f < header.numFrames; f++) {
            auto const raw = *header.frames.get(header, f * header.numTracks + t);
            tracks[t].frames[f] = {
                *header.vectors.get(header, raw.posIndx),
                *header.vectors.get(header, raw.scaleIndx),
                Quat_normalize(*header.quats.get(header, raw.quatIndx))
            };
        }
    }

    return File::result_ok;
}

File::result_t Rito::Animation::read_compressed(File const& file) RITO_FILE_NOEXCEPT {
    using namespace Rito::AnimationImpl::new_compressed;
    uint32_t dataSize;
    file_assert(file.read(dataSize));
    file_assert(file.seek_cur(-4));
    file_assert(dataSize >= sizeof(Header));

    std::vector<uint8_t> data{};
    file_assert(file.read(data, dataSize));
    Header const& header = *reinterpret_cast<Header const*>(data.data());
    file_assert(("TODO" && false));
    return File::result_ok;
}
