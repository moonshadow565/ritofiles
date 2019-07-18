#include "blend.hpp"
#include "memory.hpp"

using namespace Rito;
namespace Rito::BlendImpl {
    namespace new_v1 {
        struct RawPath {
            uint32_t hash;
            RelPtr<char> path;
        };

        struct RawBlendData {
            uint32_t fromAnimId;
            uint32_t toAnimId;
            uint32_t blendFlags;
            float blendTime;
        };

        struct RawTransitionClip {
            struct To {
                uint32_t toAnimId;
                uint32_t transitionAnimId;
            };
            uint32_t fromAnimID;
            uint32_t numTransitions;
            RelPtr<To> transitions;
        };

        struct RawTrack : BaseResource {
            float blendWeight;
            uint32_t blendMode;
            uint32_t index;
            std::array<char, 32> name;
        };

        struct RawMask : BaseResource {
            struct RawJointHash {
                int32_t weightID;
                uint32_t jointHash;
            };
            struct RawJointIndex {
                int32_t weightID;
                uint32_t jointHash;
            };
            uint32_t formatToken;
            uint32_t version;
            uint16_t flags;
            uint16_t numElements;
            uint32_t uniqueID;
            AbsPtr<float> weights;
            AbsPtr<RawJointHash> jointHashes;
            AbsPtr<RawJointIndex> jointIndices;
            std::array<uint32_t, 2> extBuffer;
        };

        struct RawEvent : BaseResource {
            struct RawEventHash {
                uint32_t dataID;
                uint32_t nameHash;
            };
            struct RawEventFrame {
                uint32_t dataID;
                float frame;
            };
            struct RawEventData : BaseResource {
                uint32_t type;
                uint32_t lags;
                float frame;
                AbsPtr<char> name;
                union {
                    struct {
                        AbsPtr<char> soundName;
                    } sound;
                    struct {
                        float endFrame;
                        uint32_t showSubmeshHash;
                        uint32_t hideSubmeshHash;
                    } submeshVisibility;
                    struct {
                        AbsPtr<char> effectName;
                        AbsPtr<char> boneName;
                        AbsPtr<char> targetBoneName;
                        float endFrame;
                    } particle;
                    struct {
                        float endFrame;
                        uint16_t jointToOverrideIndex;
                        uint16_t jointToSnapToIndex;
                    } jointSnap;
                    struct {
                        float timeToFade;
                        float targeAlpha;
                        float endFrame;
                    } fade;
                } data;
            };
            uint32_t formatToken;
            uint32_t version;
            uint16_t flags;
            uint16_t numEvents;
            uint32_t uniqueID;
            AbsPtr<RawEventData> eventsData;
            AbsPtr<RawEventData> eventData;
            AbsPtr<RawEventHash> eventHashes;
            AbsPtr<RawEventFrame> eventFrames;
            AbsPtr<char> name;
            std::array<uint32_t, 2> extBuffer;
        };

        struct RawClip : BaseResource {
            struct RawUpdater : BaseResource {
                struct RawData : BaseResource {
                    struct RawProcessor : BaseResource {
                        uint16_t type;
                        union {
                            struct {
                                float nultiplier;
                                float increment;
                            } linearTransform;
                        } data;
                    };
                    uint16_t inputType;
                    uint16_t outputType;
                    uint8_t numTransforms;
                    AbsPtr<AbsPtr<RawProcessor>> processor;
                };
                uint32_t version;
                uint16_t numUpdaters;
                AbsPtr<AbsPtr<RawData>> updaters;
            };

            struct RawClipData {
                uint32_t type;
                union {
                    struct {
                        uint32_t numPairs;
                        uint32_t updaterType;
                        uint32_t taskData; // ptr
                        uint32_t trackData; // ptr
                    } parametric;
                    struct {
                        uint32_t startTick;
                        uint32_t endTick;
                        float tickDuration;
                        uint32_t linkAnimData;
                        uint32_t eventData; // ptr
                        uint32_t maskData; // ptr
                        uint32_t trackData; // ptr
                        AbsPtr<RawUpdater> updaterData;
                        RelPtr<char> syncGroupName;
                        uint32_t syncGroup;
                        std::array<uint32_t, 2> extBuffer;
                    } atomic;
                    struct {
                        uint32_t numPairs;
                        uint32_t updaterType;
                        bool changeAnimationMidPlay;
                    } conditionBool;
                    struct {
                        uint32_t numPairs;
                        uint32_t updaterType;
                        bool changeAnimationMidPlay;
                    } conditionFloat;
                    struct {
                        uint32_t clipFlag; // ptr;
                        uint32_t numClips;
                    } parallel;
                    struct {
                        uint32_t trackIndex;
                        uint32_t numPairs;
                    } selector;
                    struct {
                        uint32_t trackIndex;
                        uint32_t numPairs;
                    } squencer;
                } data;
            };
            uint16_t flags;
            uint32_t uniqueID;
            AbsPtr<char> name;
            AbsPtr<RawClipData> data;
        };

        struct Header : BaseResource {
            uint32_t formatToken;
            uint32_t version;
            uint32_t numClasses;
            uint32_t numBlends;
            uint32_t numTransitionClips;
            uint32_t numTracks;
            uint32_t numAnimData;
            uint32_t numMasks;
            uint32_t numEvents;
            bool useCascadeBlend;
            uint8_t pad[3];
            float cascadeBlendValue;
            RelPtr<RawBlendData> blendData;
            RelPtr<RawTransitionClip> transitionClips;
            RelPtr<RawTrack> blendTracks;
            uint32_t classes;
            RelPtr<RelPtrArr<RawMask>> masks;
            RelPtr<RelPtrArr<RawEvent>> events;
            uint32_t animsData;
            uint32_t animNameCount;
            RelPtr<RawPath> animNamesOffset;
            RawPath skeleton;
            std::array<uint32_t, 1> extBuffer;
        };
    }
}

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
    using namespace Rito::BlendImpl::new_v1;
    auto const oldPos = file.tell();
    file.seek_end(0);
    auto const dataSize = static_cast<uint32_t>(file.tell() - oldPos);
    file.seek_beg(oldPos);

    std::vector<uint8_t> data{};
    file_assert(file.read(data, dataSize));
    Header const& header = *reinterpret_cast<Header const*>(data.data());
    file_assert(header.version == 0);

    auto const rawBlendData = &header.blendData[0];
    blendData.resize(header.numBlends);
    std::transform(rawBlendData, rawBlendData + header.numBlends,
                   blendData.begin(), [](RawBlendData const& raw) {
        return BlendData {
            raw.fromAnimId,
            raw.toAnimId,
            raw.blendFlags,
            raw.blendTime
        };
    });

    transitionClips.reserve(header.numTransitionClips);
    for(uint32_t i = 0; i < header.numTransitionClips; i++) {
        auto const& raw = header.transitionClips[i];
        auto& result = transitionClips.emplace_back();
        result.fromAnimID = raw.fromAnimID;
        result.transitions.resize(raw.numTransitions);
        for(uint32_t j = 0; j < raw.numTransitions; j++) {
            result.transitions[j] = {
                raw.transitions[j].toAnimId,
                raw.transitions[j].transitionAnimId
            };
        }
    }

    tracks.reserve(header.numTracks);
    for(uint32_t i = 0; i < header.numTracks; i++) {
        auto const& rawTrack = header.blendTracks[i];
        tracks.push_back({
                             rawTrack.blendWeight,
                             rawTrack.blendMode,
                             rawTrack.index,
                             rawTrack.name.data()
                         });
    }

    // TODO: classes
    // TODO: masks
    /*
    for(uint32_t i = 0; i < header.numMasks; i++) {
        auto const& rawMask = header.masks[0][i];
        for(uint32_t j = 0; j < rawMask.numElements; j++) {
            auto const& rawJointHash = rawMask[rawMask.jointHashes + j];
            auto const& rawJointIndex = rawMask[rawMask.jointIndices + j];
            auto const& weight = rawMask[rawMask.weights +j];
        }
    }
    */
    // TODO: events
    for(uint32_t i = 0; i < header.numEvents; i++) {
        auto const& rawEvent = header.events[0][i];
        std::string name = &rawEvent[rawEvent.name];
        puts("Event!");
    }

    skeletonPath = &header.skeleton.path[0];
    animationNames.reserve(header.animNameCount);
    for(uint32_t i = 0; i < header.animNameCount; i++) {
        animationNames.push_back(&header.animNamesOffset[i].path[0]);
    }

    return File::result_ok;
}
