#ifndef RITO_ANIMATION_HPP
#define RITO_ANIMATION_HPP
#include <cinttypes>
#include "types.hpp"
#include "file.hpp"
#include "math.hpp"

namespace Rito {
    struct Animation {
        struct Track {
            struct Frame {
                Vec3 position;
                Vec3 scale;
                Quat quat;
            };
            std::vector<Frame> frames;
            std::string name;
            uint32_t boneHash;
        };
        std::vector<Track> tracks;
        float tickDuration;
        std::string assetName;

        File::result_t read(File const&) RITO_FILE_NOEXCEPT;
    private:
        File::result_t read_legacy(File const&) RITO_FILE_NOEXCEPT;
        File::result_t read_v4(File const&) RITO_FILE_NOEXCEPT;
        File::result_t read_v5(File const&) RITO_FILE_NOEXCEPT;
        File::result_t read_compressed(File const&) RITO_FILE_NOEXCEPT;
    };
}

#endif // RITO_ANIMATION_HPP
