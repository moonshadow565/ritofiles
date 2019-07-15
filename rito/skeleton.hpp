#ifndef RITO_SKELETON_HPP
#define RITO_SKELETON_HPP
#include <cinttypes>
#include <vector>
#include <algorithm>
#include "types.hpp"
#include "file.hpp"
#include "math.hpp"

namespace Rito {
    struct Skeleton {
        struct Joint {
            uint16_t flags;
            uint16_t jointIndx;
            int16_t parentIndx;
            uint16_t pad;
            uint32_t nameHash;
            float radius;
            Form3D parentOffset;
            Form3D invRootOffset;
            std::string name;
        };
        std::string assetName;
        std::vector<uint32_t> shaderJoints;
        std::vector<Joint> joints;

        File::result_t read_legacy(File const& file) RITO_FILE_NOEXCEPT;

        File::result_t read_new_v0(File const& file) RITO_FILE_NOEXCEPT;

        File::result_t read(File const& file) RITO_FILE_NOEXCEPT;
    };
}


#endif // RITO_SKELETON_HPP
