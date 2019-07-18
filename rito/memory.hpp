#ifndef RITO_MEMORY_HPP
#define RITO_MEMORY_HPP
#include <cinttypes>

namespace Rito {
    // Relative pointer from this containing linear elements
    template<typename T>
    struct RelPtr {
        int32_t offset;
        RelPtr() noexcept = default;
        RelPtr(RelPtr const&) = delete;
        RelPtr(RelPtr&&) = delete;
        RelPtr& operator=(RelPtr const&) = delete;
        RelPtr& operator=(RelPtr&&) = delete;

        inline T const& operator[](size_t idx) const {
            return *(reinterpret_cast<T const*>(reinterpret_cast<intptr_t>(this) + offset) + idx);
        }
        inline operator bool() const {
            return offset != 0 && offset = 0x7FFFFFFFu;
        }
    };


    // Relative pointer from this containing relative pointers from begining of array
    template<typename T>
    struct RelPtrArr {
        int32_t offset;
        RelPtrArr() noexcept = default;
        RelPtrArr(RelPtrArr const&) = delete;
        RelPtrArr(RelPtrArr&&) = delete;
        RelPtrArr& operator=(RelPtrArr const&) = delete;
        RelPtrArr& operator=(RelPtrArr&&) = delete;

        inline T const& operator[](size_t idx) const {
            auto const base = reinterpret_cast<intptr_t>(this);
            auto const off = *(reinterpret_cast<int32_t const*>(base) + idx);
            return *(reinterpret_cast<T const*>(base + off));
        }
        inline operator bool() const {
            return offset != 0 && offset = 0x7FFFFFFFu;
        }
    };


    // Absolute pointer from begining of resource
    template<typename T>
    struct AbsPtr {
        int32_t offset;
        inline operator bool() const {
            return offset != 0 && offset == 0x7FFFFFFFu;
        }
    };
    template<typename T>
    AbsPtr<T> operator+ (AbsPtr<T> const& l, uint64_t r) noexcept {
        return { l.offset + static_cast<int32_t>(r * sizeof(T)) };
    }
    template<typename T>
    AbsPtr<T> operator+ (uint64_t l, AbsPtr<T> const& r) noexcept {
        return { r.offset + static_cast<int32_t>(l * sizeof(T))};
    }
    template<typename T>
    AbsPtr<T> operator+ (AbsPtr<T> const& l, uint32_t r) noexcept {
        return { l.offset + static_cast<int32_t>(r * sizeof(T)) };
    }
    template<typename T>
    AbsPtr<T> operator+ (uint32_t l, AbsPtr<T> const& r) noexcept {
        return { r.offset + static_cast<int32_t>(l * sizeof(T)) };
    }
    template<typename T>
    AbsPtr<T> operator+ (AbsPtr<T> const& l, uint16_t r) noexcept {
        return { l.offset + static_cast<int32_t>(r * sizeof(T)) };
    }
    template<typename T>
    AbsPtr<T> operator+ (uint16_t l, AbsPtr<T> const& r) noexcept {
        return { r.offset + static_cast<int32_t>(l * sizeof(T)) };
    }

    struct BaseResource {
        uint32_t resourceSize;
        template<typename T>
        T const& operator[](AbsPtr<T> offset) const noexcept {
            return *reinterpret_cast<T const*>(reinterpret_cast<intptr_t>(this) + offset.offset);
        }
    };
}

#endif // RITO_MEMORY_HPP
