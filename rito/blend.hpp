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
        struct TransitionClip {
            struct To {
                uint32_t toAnimId;
                uint32_t transitionAnimId;
            };
            uint32_t fromAnimID;
            std::vector<To> transitions;
        };
        struct Track {
            float blendWeight;
            uint32_t blendMode;
            uint32_t index;
            std::string name;
        };
        struct Mask {
            struct JointHash {
                int32_t weightID;
                uint32_t hash;
            };
            struct JointIndex {
                int32_t weightID;
                int32_t index;
            };
            uint32_t uniqueID;
            uint32_t flags;
            std::vector<float> weights;
            std::vector<JointHash> jointHashes;
            std::vector<JointIndex> jointIndices;
            std::string name;
        };


        std::vector<BlendData> blendData;
        std::vector<TransitionClip> transitionClips;
        std::vector<Track> tracks;
        // TODO: classes
        std::vector<Mask> masks;

        std::string skeletonPath{};
        std::vector<std::string> animationNames;

        File::result_t read(File const& file) RITO_FILE_NOEXCEPT;
        File::result_t read_v1(File const& file) RITO_FILE_NOEXCEPT;
    };
}


#endif // RITO_BLEND_HPP
