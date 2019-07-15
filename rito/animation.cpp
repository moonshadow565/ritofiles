#include "animation.hpp"

using namespace Rito;

File::result_t Rito::Animation::read(File const& file) RITO_FILE_NOEXCEPT {
    struct {
        uint32_t magic0;
        uint32_t magic1;
        uint32_t version;
    } header;
    file_assert(file.read(header));
    file_assert(file.seek_cur(-static_cast<int64_t>(sizeof(header))));
    if(header.magic1 == 0x2892Cu) {
        return read_new_v0(file);
    }
    if(header.magic0 == 0x32643372u && header.magic1 == 0x646D6E61u && header.version == 4u) {
        return read_v4(file);
    }
    return read_legacy(file);
}

File::result_t Rito::Animation::read_legacy(File const& file) RITO_FILE_NOEXCEPT {
    struct Header {
        std::array<char, 8> magic;
        uint32_t version;
        uint32_t skeletonID;
        uint32_t numTracks;
        uint32_t numFrames;
        int32_t frameRate;
    };
    struct TrackRaw {
        std::array<char, 32> name;
        uint32_t flags;
    };
    struct FrameRaw {
        Quat quaternion;
        Vec3 point;
    };

    Header header = {};
    file_assert(file.read(header));
    file_assert((header.magic == std::array{'r', '3', 'd', '2', 'a', 'n', 'm', 'd'}));
    file_assert(header.version == 3u);
    tickDuration = 1.f / header.frameRate;

    for(uint32_t i = 0; header.numTracks; i++) {
        TrackRaw trackRaw{};
        std::vector<FrameRaw> framesRaw{};
        file_assert(file.read(trackRaw));

        auto& channel = channels.emplace_back();
        file_assert(file.read(framesRaw, header.numFrames));
        channel.ticks.reserve(framesRaw.size());
        channel.name = &trackRaw.name[0];
        for(auto const& rawFrame: framesRaw) {
            auto& tick = channel.ticks.emplace_back();
            tick.boneHash = 0u;
            tick.quat = rawFrame.quaternion;
            tick.position = rawFrame.point;
        }
    }

    return File::result_ok;
}

File::result_t Rito::Animation::read_v4(File const& file) RITO_FILE_NOEXCEPT {
    struct RawTick {
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
        uint32_t numChannels;
        uint32_t numTicks;
        float tickDuration;
        uint32_t channelsOffset;
        uint32_t assetNameOffset;
        uint32_t timeOffset;
        uint32_t vectorPaletteOffset;
        uint32_t quatPaletteOffset;
        uint32_t tickDataOffset;
        uint32_t extBuffer[3];
    };
    struct {
        std::array<char, 8> magic;
        uint32_t version;
    } legacyHeader = {};

    file_assert(file.read(legacyHeader));
    file_assert((legacyHeader.magic == std::array{'r', '3', 'd', '2', 'a', 'n', 'm', 'd'}));
    file_assert(legacyHeader.version == 4u);

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
    file_assert(header.tickDataOffset >= sizeof(Header));
    file_assert(header.tickDataOffset < header.resourceSize);

    if(header.assetNameOffset != 0u && header.assetNameOffset != 0xFFFFFFFFu) {
        assetName = reinterpret_cast<char const*>(data.data() + header.assetNameOffset);
        file_assert(header.assetNameOffset < header.resourceSize);
    }

    auto const tickStartAdr = data.data() + header.tickDataOffset;
    auto const tickEndAdr = tickStartAdr + sizeof(RawTick) * header.numTicks * header.numChannels;
    file_assert(tickEndAdr <= (data.data() + data.size()));

    auto const vecStartAdr = data.data() + header.vectorPaletteOffset;
    auto const quatStartAdr = data.data() + header.quatPaletteOffset;

    for(uint32_t t = 0; t < header.numTicks; t++) {
        auto& channel = channels.emplace_back();
        auto& ticks = channel.ticks;
        ticks.reserve(header.numTicks);
        auto const channelStart = t * header.numChannels;
        for(uint32_t c = 0; c < header.numChannels; c++) {
            auto const tickAdr = tickStartAdr + (channelStart + c) * sizeof(RawTick);
            file_assert(tickAdr < (data.data() + data.size()));

            auto const& raw = *reinterpret_cast<RawTick const*>(tickAdr);

            auto const posAdr = vecStartAdr + raw.posIndx * sizeof(Vec3);
            file_assert(posAdr < (data.data() + data.size()));
            auto const scaleAdr = vecStartAdr + raw.scaleIndx * sizeof(Vec3);
            file_assert(scaleAdr < (data.data() + data.size()));
            auto const quatAdr = quatStartAdr + raw.quatIndx * sizeof(Vec3);
            file_assert(quatAdr < (data.data() + data.size()));

            ticks.push_back(Channel::Tick {
                                raw.boneHash,
                                *reinterpret_cast<Vec3 const*>(posAdr),
                                *reinterpret_cast<Vec3 const*>(scaleAdr),
                                *reinterpret_cast<Quat const*>(quatAdr)
                            });
        }
    }

    return File::result_ok;
}

File::result_t Rito::Animation::read_new_v0(File const& file) RITO_FILE_NOEXCEPT {
    return File::result_ok;
}
