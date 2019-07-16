#include "animation.hpp"
#include <unordered_map>
#include <cstdio>

using namespace Rito;

File::result_t Rito::Animation::read(File const& file) RITO_FILE_NOEXCEPT {
    struct Header {
        std::array<char, 8> magic;
        uint32_t version;
    };
    Header header{};
    file_assert(file.read(header));
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
    struct RawFrame {
        uint32_t boneHash;
        uint16_t posIndx;
        uint16_t scaleIndx;
        uint16_t quatIndx;
        uint16_t pad;
    };
    struct Header {
        uint32_t resourceSize;
        uint32_t formatToken;
        uint32_t version;
        uint32_t flags;
        uint32_t numTracks;
        uint32_t numFrames;
        float tickDuration;
        uint32_t tracksOffset;
        uint32_t assetNameOffset;
        uint32_t timeOffset;
        uint32_t vectorPaletteOffset;
        uint32_t quatPaletteOffset;
        uint32_t frameDataOffset;
        uint32_t extBuffer[3];
    };
    uint32_t dataSize;
    file_assert(file.read(dataSize));
    file_assert(file.seek_cur(-4));
    file_assert(dataSize >= sizeof(Header));

    std::vector<uint8_t> data{};
    file_assert(file.read(data, dataSize));
    Header const& header = *reinterpret_cast<Header const*>(data.data());

    file_assert(header.vectorPaletteOffset >= sizeof(Header));
    file_assert(header.vectorPaletteOffset < header.resourceSize);
    file_assert(header.quatPaletteOffset >= sizeof(Header));
    file_assert(header.quatPaletteOffset < header.resourceSize);
    file_assert(header.frameDataOffset >= sizeof(Header));
    file_assert(header.frameDataOffset < header.resourceSize);

    tickDuration = header.tickDuration;
    if(header.assetNameOffset != 0u && header.assetNameOffset != 0xFFFFFFFFu) {
        assetName = reinterpret_cast<char const*>(data.data() + header.assetNameOffset);
        file_assert(header.assetNameOffset < header.resourceSize);
    }

    auto const frameStartAdr = data.data() + header.frameDataOffset;
    auto const frameEndAdr = frameStartAdr + sizeof(RawFrame) * header.numFrames * header.numTracks;
    file_assert(frameEndAdr <= (data.data() + data.size()));

    auto const vecStartAdr = data.data() + header.vectorPaletteOffset;
    auto const quatStartAdr = data.data() + header.quatPaletteOffset;

    tracks.resize(header.numTracks);
    for(auto& track: tracks) {
        track.frames.resize(header.numFrames);
    }

    for(uint32_t t = 0; t < header.numTracks; t++) {
        for(uint32_t f = 0; f < header.numFrames; f++) {
            auto const frameIdx = f * header.numTracks + t;
            auto const frameAdr = frameStartAdr + frameIdx * sizeof(RawFrame);
            file_assert(frameAdr < (data.data() + data.size()));
            auto const& raw = *reinterpret_cast<RawFrame const*>(frameAdr);

            auto const posAdr = vecStartAdr + raw.posIndx * sizeof(Vec3);
            file_assert(posAdr < (data.data() + data.size()));
            auto const scaleAdr = vecStartAdr + raw.scaleIndx * sizeof(Vec3);
            file_assert(scaleAdr < (data.data() + data.size()));
            auto const quatAdr = quatStartAdr + raw.quatIndx * sizeof(Quat);
            file_assert(quatAdr < (data.data() + data.size()));

            tracks[t].boneHash = raw.boneHash;
            tracks[t].frames[f] = {
                *reinterpret_cast<Vec3 const*>(posAdr),
                *reinterpret_cast<Vec3 const*>(scaleAdr),
                Quat_normalize(*reinterpret_cast<Quat const*>(quatAdr))
            };
        }
    }

    return File::result_ok;
}

File::result_t Rito::Animation::read_v5(File const& file) RITO_FILE_NOEXCEPT {
    struct RawFrame {
        uint16_t posIndx;
        uint16_t scaleIndx;
        uint16_t quatIndx;
    };
    struct Header {
        uint32_t resourceSize;
        uint32_t formatToken;
        uint32_t version;
        uint32_t flags;
        uint32_t numTracks;
        uint32_t numFrames;
        float tickDuration;
        uint32_t joinNameHashesOffsets;
        uint32_t assetNameOffset;
        uint32_t timeOffset;
        uint32_t vectorPaletteOffset;
        uint32_t quatPaletteOffset;
        uint32_t frameDataOffset;
        uint32_t extBuffer[3];
    };
    uint32_t dataSize;
    file_assert(file.read(dataSize));
    file_assert(file.seek_cur(-4));
    file_assert(dataSize >= sizeof(Header));

    std::vector<uint8_t> data{};
    file_assert(file.read(data, dataSize));
    Header const& header = *reinterpret_cast<Header const*>(data.data());

    file_assert(header.joinNameHashesOffsets >= sizeof(Header));
    file_assert(header.joinNameHashesOffsets < header.resourceSize);
    file_assert(header.vectorPaletteOffset >= sizeof(Header));
    file_assert(header.vectorPaletteOffset < header.resourceSize);
    file_assert(header.quatPaletteOffset >= sizeof(Header));
    file_assert(header.quatPaletteOffset < header.resourceSize);
    file_assert(header.frameDataOffset >= sizeof(Header));
    file_assert(header.frameDataOffset < header.resourceSize);

    tickDuration = header.tickDuration;
    if(header.assetNameOffset != 0u && header.assetNameOffset != 0xFFFFFFFFu) {
        assetName = reinterpret_cast<char const*>(data.data() + header.assetNameOffset);
        file_assert(header.assetNameOffset < header.resourceSize);
    }

    auto const frameStartAdr = data.data() + header.frameDataOffset;
    auto const frameEndAdr = frameStartAdr + sizeof(RawFrame) * header.numFrames * header.numTracks;
    file_assert(frameEndAdr <= (data.data() + data.size()));

    auto const vecStartAdr = data.data() + header.vectorPaletteOffset;
    auto const quatStartAdr = data.data() + header.quatPaletteOffset;
    auto const hashStartAdr = data.data() + header.joinNameHashesOffsets;

    tracks.resize(header.numTracks);
    for(auto& track: tracks) {
        track.frames.resize(header.numFrames);
    }

    for(uint32_t t = 0; t < header.numTracks; t++) {
        auto const hashAdr = hashStartAdr + t * sizeof(uint32_t);
        file_assert(hashAdr < (data.data() + data.size()));
        tracks[t].boneHash = *reinterpret_cast<uint32_t const*>(hashAdr);
        for(uint32_t f = 0; f < header.numFrames; f++) {
            auto const frameIdx = f * header.numTracks + t;
            auto const frameAdr = frameStartAdr + frameIdx * sizeof(RawFrame);
            file_assert(frameAdr < (data.data() + data.size()));
            auto const& raw = *reinterpret_cast<RawFrame const*>(frameAdr);

            auto const posAdr = vecStartAdr + raw.posIndx * sizeof(Vec3);
            file_assert(posAdr < (data.data() + data.size()));
            auto const scaleAdr = vecStartAdr + raw.scaleIndx * sizeof(Vec3);
            file_assert(scaleAdr < (data.data() + data.size()));
            auto const quatAdr = quatStartAdr + raw.quatIndx * sizeof(QuantizedQuat);
            file_assert(quatAdr < (data.data() + data.size()));
            tracks[t].frames[f] = {
                *reinterpret_cast<Vec3 const*>(posAdr),
                *reinterpret_cast<Vec3 const*>(scaleAdr),
                *reinterpret_cast<QuantizedQuat const*>(quatAdr)
            };
        }
    }

    return File::result_ok;
}

File::result_t Rito::Animation::read_compressed(File const&) RITO_FILE_NOEXCEPT {
    return File::result_ok;
}
