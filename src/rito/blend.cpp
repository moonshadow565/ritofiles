#include "blend.hpp"
#include "memory.hpp"

using namespace Rito;
namespace Rito::BlendImpl {
    namespace new_v1 {
        using namespace Rito::Mem;
        using Rito::Mem::FlexArr;
        using Rito::Mem::AbsPtr;
        using Rito::Mem::RelPtr;
        using Rito::Mem::AbsPtrArr;
        using Rito::Mem::RelPtrArr;

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
                enum class Type : uint32_t {
                    Particle = 0x0,
                    Sound = 0x1,
                    SubmeshVisibility = 0x2,
                    Fade = 0x3,
                    JointSnap = 0x4,
                    EnableLookAt = 0x5,
                };
                Type type;
                uint32_t flags;
                float frame;
                AbsPtr<char> name;
                union {
                    struct {
                        AbsPtr<char> effectName;
                        AbsPtr<char> boneName;
                        AbsPtr<char> targetBoneName;
                        float endFrame;
                    } particle;
                    struct {
                        AbsPtr<char> soundName;
                    } sound;
                    struct {
                        float endFrame;
                        uint32_t showSubmeshHash;
                        uint32_t hideSubmeshHash;
                    } submeshVisibility;
                    struct {
                        float timeToFade;
                        float targeAlpha;
                        float endFrame;
                    } fade;
                    struct {
                        float endFrame;
                        uint16_t jointToOverrideIndex;
                        uint16_t jointToSnapToIndex;
                    } jointSnap;
                    struct {
                        float endFrame;
                        uint32_t enableLookAt;
                        uint32_t lockCurrentValues;
                    } enableLookAt;
                } data;
            };
            uint32_t formatToken;
            uint32_t version;
            uint16_t flags;
            uint16_t numEvents;
            uint32_t uniqueID;
            AbsPtrArr<RawEventData> eventsData;
            AbsPtr<RawEventData> eventData;
            AbsPtr<RawEventHash> eventHashes;
            AbsPtr<RawEventFrame> eventFrames;
            AbsPtr<char> name;
            std::array<uint32_t, 2> extBuffer;
        };

        struct RawClip : BaseResource {
            struct RawUpdater : BaseResource {
                struct RawUpdaterData : BaseResource {
                    struct RawProcessor : BaseResource {
                        uint16_t type;
                        uint16_t pad;
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
                    uint8_t pad;
                    AbsPtr<RawProcessor> processor;
                };
                uint32_t version;
                uint16_t numUpdaters;
                uint16_t pad;
                AbsPtrArr<RawUpdaterData> updaters;
            };

            struct RawClipData {
                enum class Type : uint32_t {
                    Invalid,
                    Atomic,
                    Selector,
                    Sequencer,
                    Parallel,
                    MultiChildClip,
                    Parametric,
                    ConditionBool,
                    ConditionFloat,
                };
                Type type;
                union {
                    struct {
                        uint32_t startTick;
                        uint32_t endTick;
                        float tickDuration;
                        uint32_t animIndex;
                        RelPtr<RawEvent> event; // ptr
                        RelPtr<RawMask> mask; // ptr
                        RelPtr<RawTrack> track; // ptr
                        AbsPtr<RawUpdater> updater;
                        RelPtr<char> syncGroupName;
                        uint32_t syncGroup;
                        std::array<uint32_t, 2> extBuffer;
                    } atomic; // TODO 1
                    struct {
                        struct Entry {
                            uint32_t clipID;
                            float probability;
                        };
                        uint32_t trackIndex;
                        uint32_t numPairs;
                        FlexArr<Entry> entries;
                    } selector;
                    struct {
                        struct Entry {
                            uint32_t clipID; // children?
                        };
                        uint32_t trackIndex;
                        uint32_t numPairs;
                        FlexArr<Entry> entries;
                    } sequencer;
                    struct {
                        struct Entry {
                            uint32_t clipID; // children?
                        };
                        AbsPtr<uint32_t> clipFlags;
                        uint32_t numClips;
                        FlexArr<Entry> entries;
                    } parallel;
                    struct {
                    } multiChild;
                    struct {
                        struct Entry {
                            uint32_t clipID;
                            float value;
                        };
                        uint32_t numPairs;
                        uint32_t updaterType;
                        RelPtr<RawMask> mask;
                        RelPtr<RawTrack> track;
                        FlexArr<Entry> entries;
                    } parametric;
                    struct {
                        struct Entry {
                            uint32_t clipID;
                            bool value;
                            uint8_t pad[3];
                        };
                        uint32_t numPairs;
                        uint32_t updaterType;
                        bool changeAnimationMidPlay;
                        uint8_t pad[3];
                        FlexArr<Entry> entries;
                    } conditionBool;
                    struct {
                        struct Entry {
                            uint32_t clipID;
                            float value;
                            float holdAnimationToHigher;
                            float holdAnimationToLower;
                        };
                        uint32_t numPairs;
                        uint32_t updaterType;
                        bool changeAnimationMidPlay;
                        uint8_t pad[3];
                        FlexArr<Entry> entries;
                    } conditionFloat;
                } data;
            };
            uint16_t flags;
            uint16_t pad;
            uint32_t uniqueID;
            AbsPtr<char> name;
            AbsPtr<RawClipData> data;
        };

        struct Header : BaseResource {
            uint32_t formatToken;
            uint32_t version;
            uint32_t numClips;
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
            RelPtrArr<RawClip> clips;
            RelPtrArr<RawMask> masks;
            RelPtrArr<RawEvent> events;
            uint32_t animsData;
            uint32_t animNameCount;
            RelPtr<RawPath> animNamesOffset;
            RawPath skeleton;
            std::array<uint32_t, 1> extBuffer;
        };
    }
}

void Blend::read(File const& file) {
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

void Blend::read_v1(File const& file) {
    using namespace Rito::BlendImpl::new_v1;
    auto const oldPos = file.tell();
    file.seek_end(0);
    auto const dataSize = file.tell() - oldPos;
    file.seek_beg(oldPos);

    std::vector<uint8_t> data{};
    file.read(data, dataSize);
    Header const& header = *reinterpret_cast<Header const*>(data.data());
    file_assert(header.version == 0);

    if(auto skeletonPathPtr = header.skeleton.path.get(); skeletonPathPtr) {
       skeletonPath = skeletonPathPtr;
    }

    animationNames.reserve(header.animNameCount);
    for(uint32_t i = 0; i < header.animNameCount; i++) {
        animationNames.push_back(header.animNamesOffset.get(i)->path.get());
    }

    blendData.reserve(header.numBlends);
    for(uint32_t i = 0; i < header.numBlends; i++) {
        auto const& rawBlendData = *header.blendData.get(i);
        blendData.push_back(BlendData {
                                rawBlendData.fromAnimId,
                                rawBlendData.toAnimId,
                                rawBlendData.blendFlags,
                                rawBlendData.blendTime
                            });
    }

    transitionClips.reserve(header.numTransitionClips);
    for(uint32_t i = 0; i < header.numTransitionClips; i++) {
        auto const& raw = *header.transitionClips.get(i);
        auto& result = transitionClips.emplace_back();
        result.fromAnimID = raw.fromAnimID;
        result.transitions.resize(raw.numTransitions);
        for(uint32_t j = 0; j < raw.numTransitions; j++) {
            auto const& rawTransition = *raw.transitions.get(j);
            result.transitions[j] = {
                rawTransition.toAnimId,
                rawTransition.transitionAnimId
            };
        }
    }

    tracks.reserve(header.numTracks);
    for(uint32_t i = 0; i < header.numTracks; i++) {
        auto const& rawTrack = *header.blendTracks.get(i);
        tracks.push_back({
                             rawTrack.blendWeight,
                             rawTrack.blendMode,
                             rawTrack.index,
                             rawTrack.name.data()
                         });
    }

    masks.reserve(header.numMasks);
    for(uint32_t i = 0; i < header.numMasks; i++) {
        auto const& rawMask = *header.masks.get(i);
        auto& mask = masks.emplace_back();
        mask.flags = rawMask.flags;
        mask.uniqueID = rawMask.uniqueID;
        mask.joints.reserve(rawMask.numElements);
        for(uint32_t j = 0; j < rawMask.numElements; j++) {
            auto const& rawJointHash = *rawMask.jointHashes.get(rawMask, j);
            auto const& weight = *rawMask.weights.get(rawMask, j);
            mask.joints.push_back({
                                      rawJointHash.jointHash,
                                      weight
                                  });
        }
    }

    eventLists.reserve(header.numEvents);
    for(uint32_t i = 0; i < header.numEvents; i++) {
        auto const& rawEvent = *header.events.get(i);
        auto& eventList = eventLists.emplace_back();
        eventList.flags = rawEvent.flags;
        eventList.uniqueID = rawEvent.uniqueID;
        eventList.name = rawEvent.name.get(rawEvent);
        eventList.eventsData.reserve(rawEvent.numEvents);
        for(uint32_t j = 0; j < rawEvent.numEvents; j++) {
            auto const& raw = *rawEvent.eventsData.get(rawEvent, j);
            using Type = RawEvent::RawEventData::Type;
            auto eventBase = Event::EventBase {
                raw.flags,
                raw.frame,
                raw.name.get(raw)
            };
            switch(raw.type) {
            case Type::Particle:
                eventList.eventsData.push_back(Event::EventParticle {
                                                   eventBase,
                                                   raw.data.particle.effectName.get(raw),
                                                   raw.data.particle.boneName.get(raw),
                                                   raw.data.particle.targetBoneName.get(raw),
                                                   raw.data.particle.endFrame
                                               });
                break;
            case Type::Sound:
                eventList.eventsData.push_back(Event::EventSoundName {
                                                   eventBase,
                                                   raw.data.sound.soundName.get(raw),
                                               });
                break;
            case Type::SubmeshVisibility:
                eventList.eventsData.push_back(Event::EventSubmeshVisibility {
                                                   eventBase,
                                                   raw.data.submeshVisibility.endFrame,
                                                   raw.data.submeshVisibility.showSubmeshHash,
                                                   raw.data.submeshVisibility.hideSubmeshHash
                                               });
                break;
            case Type::Fade:
                eventList.eventsData.push_back(Event::EventFade {
                                                   eventBase,
                                                   raw.data.fade.timeToFade,
                                                   raw.data.fade.targeAlpha,
                                                   raw.data.fade.endFrame
                                               });
                break;
            case Type::JointSnap:
                eventList.eventsData.push_back(Event::EventJointSnap {
                                                   eventBase,
                                                   raw.data.jointSnap.endFrame,
                                                   raw.data.jointSnap.jointToOverrideIndex,
                                                   raw.data.jointSnap.jointToSnapToIndex
                                               });
                break;
            case Type::EnableLookAt:
                eventList.eventsData.push_back(Event::EventEnableLookAt {
                                                   eventBase,
                                                   raw.data.enableLookAt.endFrame,
                                                   raw.data.enableLookAt.enableLookAt,
                                                   raw.data.enableLookAt.lockCurrentValues
                                               });
                break;
            }
        }
    }

    clips.reserve(header.numClips);
    for(uint32_t i = 0; i < header.numClips; i++) {
        using Type = RawClip::RawClipData::Type;
        auto const& rawClip = *header.clips.get(i);
        auto const& rawData = *rawClip.data.get(rawClip);
        auto base = ClipBase {
            rawClip.flags,
            rawClip.uniqueID,
            rawClip.name.get(rawClip)
        };
        switch(rawData.type) {
        case Type::Invalid:
            break;
        case Type::Atomic:{
            auto const& raw = rawData.data.atomic;
            auto result = ClipAtomic {
                    base,
                    raw.startTick,
                    raw.endTick,
                    raw.tickDuration,
                    std::nullopt,
                    std::nullopt,
                    std::nullopt,
                    std::nullopt,
                    {},
                    {},
                    raw.syncGroup
            };
            if(raw.animIndex != 0x7FFFFFFF) {
                result.animIndex = raw.animIndex;
            }
            if(auto const event = raw.event.get(); event) {
                result.eventUniqueID = event->uniqueID;
            }
            if(auto const mask = raw.mask.get(); mask) {
                result.maskUniqueID = mask->uniqueID;
            }
            if(auto const track = raw.track.get(); track) {
                result.trackIndex = track->index;
            }
            if(auto const syncGroupName = raw.syncGroupName.get(); syncGroupName) {
                result.syncGroupName = syncGroupName;
            }
            if(auto const updaterList = raw.updater.get(rawData); updaterList) {
                result.updaters.reserve(updaterList->numUpdaters);
                for(uint32_t j = 0; j < updaterList->numUpdaters; j++) {
                    auto const updater = updaterList->updaters.get(*updaterList, j);
                    auto result2 = Updater {
                            updater->inputType,
                            updater->outputType,
                            {}
                    };
                    result2.processors.reserve(updater->numTransforms);
                    for(uint32_t k = 0; k < updater->numTransforms; k++) {
                        auto const proc = updater->processor.get(*updater, k);
                        auto result3base = BaseProcessor{};
                        auto result3 = LinerProcessor{
                                result3base,
                                proc->data.linearTransform.increment,
                                proc->data.linearTransform.nultiplier
                        };
                        result2.processors.push_back(result3);
                    }
                    result.updaters.emplace_back(result2);
                }
            }
            clips.emplace_back(result);
            break;
        }

        case Type::Selector: {
            auto const& raw = rawData.data.selector;
            auto result = ClipSelector {
                    base,
                    raw.trackIndex,
                    {}
            };
            result.entries.reserve(raw.numPairs);
            for(uint32_t j = 0; j < raw.numPairs; j++) {
                auto const& entry = *raw.entries.get(j);
                result.entries.push_back({
                                             entry.clipID,
                                             entry.probability
                                         });
            }
            clips.emplace_back(result);
            break;
        }
        case Type::Sequencer: {
            auto const& raw = rawData.data.sequencer;
            auto result = ClipSequencer {
                    base,
                    raw.trackIndex,
                    {}
            };
            result.entries.reserve(raw.numPairs);
            for(uint32_t j = 0; j < raw.numPairs; j++) {
                auto const& entry = *raw.entries.get(j);
                result.entries.push_back({
                                             entry.clipID,
                                         });
            }
            clips.emplace_back(result);
            break;
        }
        case Type::Parallel:{
            auto const& raw = rawData.data.parallel;
            auto result = ClipParallel {
                    base,
                    {},
                    {}
            };
            result.clipFlags.reserve(raw.numClips);
            if(raw.clipFlags.offset) {
                for(uint32_t j = 0; j < raw.numClips; j++) {
                    auto const& entry = *raw.clipFlags.get(rawData, j);
                    result.clipFlags.push_back(entry);
                }
            }
            result.entries.reserve(raw.numClips);
            for(uint32_t j = 0; j < raw.numClips; j++) {
                auto const& entry = *raw.entries.get(j);
                result.entries.push_back({
                                             entry.clipID,
                                         });
            }
            clips.emplace_back(result);
            break;
        }
        case Type::MultiChildClip: {
            // TODO: ???
            // auto const& raw = rawData.data.multiChild;
            auto result = ClipMultiChild{base};
            clips.emplace_back(result);
            break;
        }
        case Type::Parametric: {
            auto const& raw = rawData.data.parametric;
            auto result = ClipParametric {
                    base,
                    raw.updaterType,
                    std::nullopt,
                    std::nullopt,
                    {}
            };
            if(auto const mask = raw.mask.get(); mask) {
                result.maskUniqueID = mask->uniqueID;
            }
            if(auto const track = raw.track.get(); track) {
                result.trackIndex = track->index;
            }
            result.entries.reserve(raw.numPairs);
            for(uint32_t j = 0; j < raw.numPairs; j++) {
                auto const& entry = *raw.entries.get(j);
                result.entries.push_back({
                                             entry.clipID,
                                             entry.value
                                         });
            }
            clips.emplace_back(result);
            break;
        }
        case Type::ConditionBool: {
            auto const& raw = rawData.data.conditionBool;
            auto result = ClipConditionBool {
                    base,
                    raw.updaterType,
                    raw.changeAnimationMidPlay,
                    {},
            };
            result.entries.reserve(raw.numPairs);
            for(uint32_t j = 0; j < raw.numPairs; j++) {
                auto const& entry = *raw.entries.get(j);
                result.entries.push_back({
                                             entry.clipID,
                                             entry.value
                                         });
            }
            clips.emplace_back(result);
            break;
        }
        case Type::ConditionFloat: {
            auto const& raw = rawData.data.conditionFloat;
            auto result = ClipConditionFloat {
                    base,
                    raw.updaterType,
                    raw.changeAnimationMidPlay,
                    {},
            };
            result.entries.reserve(raw.numPairs);
            for(uint32_t j = 0; j < raw.numPairs; j++) {
                auto const& entry = *raw.entries.get(j);
                result.entries.push_back({
                                             entry.clipID,
                                             entry.value,
                                             entry.holdAnimationToHigher,
                                             entry.holdAnimationToLower
                                         });
            }
            clips.emplace_back(result);
            break;
        }
        }
    }
}
