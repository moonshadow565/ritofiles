#ifndef RITO_BLEND_HPP
#define RITO_BLEND_HPP
#include "types.hpp"
#include "file.hpp"

namespace Rito {
    struct Blend {
        struct BlendData {
            uint32_t fromAnimId;
            uint32_t toAnimId;
            uint32_t blendFlags;
            float blendTime;
        };

        std::vector<BlendData> blendData;
        std::string skeletonPath{};
        std::vector<std::string> animationNames;

        File::result_t read(File const& file) RITO_FILE_NOEXCEPT;
        File::result_t read_v1(File const& file) RITO_FILE_NOEXCEPT;
    };
}


#endif // RITO_BLEND_HPP
