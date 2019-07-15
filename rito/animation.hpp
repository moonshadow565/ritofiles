#ifndef RITO_ANIMATION_HPP
#define RITO_ANIMATION_HPP
#include <cinttypes>
#include "types.hpp"
#include "file.hpp"
#include "math.hpp"

namespace Rito {
    struct Animation {
        struct Channel {
            struct Tick {
                uint32_t boneHash;
                Vec3 position;
                Vec3 scale;
                Quat quat;
            };
            std::vector<Tick> ticks;
            std::string name;
        };
        std::vector<Channel> channels;
        float tickDuration;
        std::string assetName;

        File::result_t read(File const&) RITO_FILE_NOEXCEPT;
        File::result_t read_legacy(File const&) RITO_FILE_NOEXCEPT;
        File::result_t read_v4(File const&) RITO_FILE_NOEXCEPT;
        File::result_t read_new_v0(File const&) RITO_FILE_NOEXCEPT;
    };
}

#endif // RITO_ANIMATION_HPP
