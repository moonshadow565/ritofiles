#include "blend.hpp"
#include "memory.hpp"

using namespace Rito;

File::result_t Blend::read(File const& file) RITO_FILE_NOEXCEPT {
    struct Header {
        std::array<char, 8> magic;
        uint32_t version;
    };

    Header header{};
    file.read(header);

    file_assert((header.magic == std::array{'r','3','d','2','b','l','n','d'}));
    file_assert(header.version == 1);

    return read_v1(file);
}

File::result_t Blend::read_v1(File const& file) RITO_FILE_NOEXCEPT {
    struct RawPath {
        uint32_t hash;
        RelOffset<char> path;
    };

    struct Header : BaseResource {
        uint32_t formatToken;
        uint32_t version;
        uint32_t numClasses;
        uint32_t numBlends;
        uint32_t numTransitionData;
        uint32_t numTracks;
        uint32_t numAnimData;
        uint32_t numMaskData;
        uint32_t numEventData;
        bool useCascadeBlend;
        uint8_t pad[3];
        float cascadeBlendValue;
        AbsOffset<BlendData> blendData;
        uint32_t transitionData;
        uint32_t blendTrackArr;
        uint32_t classAry;
        uint32_t maskDataArr;
        uint32_t eventDataArr;
        uint32_t animDataArr;
        uint32_t animNameCount;
        RelOffset<RawPath> animNamesOffset;
        RawPath skeleton;
        uint32_t extBuffer[1];
    };

    auto const oldPos = file.tell();
    file.seek_end(0);
    auto const dataSize = static_cast<uint32_t>(file.tell() - oldPos);
    file.seek_beg(oldPos);

    std::vector<uint8_t> data{};
    file_assert(file.read(data, dataSize));
    Header const& header = *reinterpret_cast<Header const*>(data.data());
    file_assert(header.version == 0);

    auto const blendDataArr = &header[header.blendData];
    blendData = { blendDataArr, blendDataArr + header.numBlends };

    skeletonPath = header.skeleton.path[0];
    animationNames.reserve(header.animNameCount);
    for(uint32_t i = 0; i < header.animNameCount; i++) {
        animationNames.push_back(&header.animNamesOffset[i].path[0]);
    }

    return File::result_ok;
}
