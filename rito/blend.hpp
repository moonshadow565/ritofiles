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
            struct Joint {
                uint32_t hash;
                float weight;
            };
            uint32_t uniqueID;
            uint32_t flags;
            std::vector<Joint> joints;
        };

        struct Event {
            struct EventBase {
                uint32_t flags;
                float frame;
                std::string name;
            };
            struct EventParticle : EventBase {
                std::string effectName;
                std::string boneName;
                std::string targetBoneName;
                float endFrame;
            };
            struct EventSoundName : EventBase {
                std::string soundName;
            };
            struct EventSubmeshVisibility : EventBase {
                float endFrame;
                uint32_t showSubmeshHash;
                uint32_t hideSubmeshHash;
            };
            struct EventFade : EventBase {
                float timeToFade;
                float targeAlpha;
                float endFrame;
            };
            struct EventJointSnap : EventBase {
                float endFrame;
                uint16_t jointToOverrideIndex;
                uint16_t jointToSnapToIndex;
            };
            struct EventEnableLookAt : EventBase {
                float endFrame;
                uint32_t enableLookAt;
                uint32_t lockCurrentValues;
            };
            using EventData = std::variant<
               EventParticle,
               EventSoundName,
               EventSubmeshVisibility,
               EventFade,
               EventJointSnap,
               EventEnableLookAt
            >;
            uint32_t flags;
            uint32_t uniqueID;
            std::string name;
            std::vector<EventData> eventsData;
        };

        struct BaseProcessor { };
        struct LinerProcessor : BaseProcessor {
            float multiplier;
            float increment;
        };

        using Processor = std::variant<
            LinerProcessor
        >;

        struct Updater {
            uint16_t inputType;
            uint16_t outputType;
            std::vector<Processor> processors;
        };

        struct ClipBase {
            uint32_t flags;
            uint32_t uniqueID;
            std::string name;
        };

        struct ClipAtomic : ClipBase {
            uint32_t startTick;
            uint32_t endTick;
            float tickDuration;
            std::optional<uint32_t> animIndex;
            std::optional<uint32_t> eventUniqueID;
            std::optional<uint32_t> maskUniqueID;
            std::optional<uint32_t> trackIndex;
            std::vector<Updater> updaters;
            std::string syncGroupName;
            uint32_t syncGroup;
        };

        struct ClipSelector : ClipBase {
            struct Entry {
                uint32_t clipID;
                float probability;
            };
            uint32_t trackIndex;
            std::vector<Entry> entries;
        };

        struct ClipSequencer : ClipBase {
            struct Entry {
                uint32_t clipID; // children?
            };
            uint32_t trackIndex;
            std::vector<Entry> entries;
        };

        struct ClipParallel : ClipBase {
            struct Entry {
                uint32_t clipID; // children?
            };
            std::vector<uint32_t> clipFlags; // optional
            std::vector<Entry> entries;
        };

        struct ClipMultiChild : ClipBase {
            // TODO: figure out what this is...
        };

        struct ClipParametric : ClipBase {
            struct Entry {
                uint32_t clipID;
                float value;
            };
            uint32_t updaterType;
            std::optional<uint32_t> maskUniqueID;
            std::optional<uint32_t> trackIndex;
            std::vector<Entry> entries;
        };

        struct ClipConditionBool : ClipBase {
            struct Entry {
                uint32_t clipID;
                bool value;
            };
            uint32_t updaterType;
            bool changeAnimationMidPlay;
            std::vector<Entry> entries;
        };

        struct ClipConditionFloat : ClipBase {
            struct Entry {
                uint32_t clipID;
                float value;
                float holdAnimationToHigher;
                float holdAnimationToLower;
            };
            uint32_t updaterType;
            bool changeAnimationMidPlay;
            std::vector<Entry> entries;
        };

        using Clip = std::variant<
            ClipAtomic,
            ClipSelector,
            ClipSequencer,
            ClipParallel,
            ClipMultiChild,
            ClipParametric,
            ClipConditionBool,
            ClipConditionFloat
        >;

        std::vector<BlendData> blendData;
        std::vector<TransitionClip> transitionClips;
        std::vector<Track> tracks;
        std::vector<Clip> clips;
        std::vector<Mask> masks;
        std::vector<Event> eventLists;

        std::string skeletonPath{};
        std::vector<std::string> animationNames;

        File::result_t read(File const& file) RITO_FILE_NOEXCEPT;
        File::result_t read_v1(File const& file) RITO_FILE_NOEXCEPT;
    };
}


#endif // RITO_BLEND_HPP
