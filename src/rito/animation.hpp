#ifndef RITO_ANIMATION_HPP
#define RITO_ANIMATION_HPP
#include <cinttypes>
#include "types.hpp"
#include "file.hpp"

namespace Rito {
    struct Animation {
        struct Track {
            std::vector<Vec3> positions;
            std::vector<Vec3> scales;
            std::vector<Quat> rotations;
            std::string name;
            uint32_t boneHash;
        };
        std::vector<Track> tracks;
        float tickDuration;
        std::string assetName;

        Animation(File const&);
    };
}

#endif // RITO_ANIMATION_HPP
