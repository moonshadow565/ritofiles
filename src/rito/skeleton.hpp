#ifndef RITO_SKELETON_HPP
#define RITO_SKELETON_HPP
#include <cinttypes>
#include <vector>
#include <algorithm>
#include <optional>
#include "types.hpp"
#include "file.hpp"

namespace Rito {
    struct Skeleton {
        struct Joint {
            uint32_t flags;
            int32_t jointIndx;
            int32_t parentIndx;
            uint32_t nameHash;
            float radius;
            Mtx44 parentOffset;
            Mtx44 invRootOffset;
            std::string name;
        };
        std::string assetName;
        std::vector<Joint> joints;
        std::vector<int32_t> shaderBones;

        Skeleton(File const& file);
    };
}


#endif // RITO_SKELETON_HPP
