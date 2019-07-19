#ifndef RITO_MEMORY_HPP
#define RITO_MEMORY_HPP
#include <cinttypes>

namespace Rito {
    struct BaseResource;
    template<typename T>
    struct AbsPtr;
    template<typename T>
    struct AbsPtrArr;
    template<typename T>
    struct RelPtr;
    template<typename T>
    struct RelPtrArr;
    template<typename T>
    struct FlexArr;

    struct BaseResource {
        uint32_t resourceSize;
    };

    // Absolute pointer from begining of resource
    template<typename T>
    struct AbsPtr {
        int32_t offset;
        template<typename B>
        inline T const* get(B const& obj, size_t idx = 0) const noexcept {
            if(offset == 0 || offset == -1) {
                return nullptr;
            }
            return reinterpret_cast<T const*>(reinterpret_cast<intptr_t>(&obj) + offset) + idx;
        }
    };

    // Absolute pointer to array containt absolute pointers from beging or resources
    template<typename T>
    struct AbsPtrArr {
        int32_t offset;
        template<typename B>
        inline T const* get(B const& obj, size_t idx = 0) const noexcept {
            if(offset == 0 || offset == -1) {
                return nullptr;
            }
            auto const base = reinterpret_cast<intptr_t>(&obj);
            auto const ptr = *(reinterpret_cast<int32_t const*>(base + offset) + idx);
            if(ptr == 0 || ptr == -1) {
                return nullptr;
            }
            return reinterpret_cast<T const*>(base + ptr);
        }
    };

    // Relative pointer from this containing linear elements
    template<typename T>
    struct RelPtr {
        int32_t offset;
        inline T const* get(size_t idx = 0) const noexcept {
            if(offset == 0 || offset == -1) {
                return nullptr;
            }
            auto const base = reinterpret_cast<intptr_t>(&offset) + offset;
            return reinterpret_cast<T const*>(base) + idx;
        }
    };

    // Relative pointer from this containing relative pointers from begining of array
    template<typename T>
    struct RelPtrArr {
        int32_t offset;
        inline T const* get(size_t idx = 0) const noexcept {
            if(offset == 0 || offset == -1) {
                return nullptr;
            }
            auto const base = reinterpret_cast<intptr_t>(&offset) + offset;
            auto const ptr = *(reinterpret_cast<int32_t const*>(base) + idx);
            if(ptr == 0 || ptr == -1) {
                return nullptr;
            }
            return reinterpret_cast<T const*>(base + ptr);
        }
    };

    template<typename T>
    struct FlexArr {
        T first;
        inline T const* get(size_t idx) const noexcept {
            return &first + idx;
        }
    };
}

#endif // RITO_MEMORY_HPP
