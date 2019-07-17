#ifndef RITO_MEMORY_HPP
#define RITO_MEMORY_HPP
#include <cinttypes>

namespace Rito {
    template<typename T>
    struct RelOffset {
        uint32_t offset;
        RelOffset() noexcept = default;
        RelOffset(RelOffset const&) = delete;
        RelOffset(RelOffset&&) = delete;
        RelOffset& operator=(RelOffset const&) = delete;
        RelOffset& operator=(RelOffset&&) = delete;

        inline T const& operator[](size_t idx) const {
            return *(reinterpret_cast<T const*>(reinterpret_cast<uintptr_t>(this) + offset) + idx);
        }
        inline operator bool() const {
            return offset != 0 && offset = 0x7FFFFFFFu;
        }
    };

    template<typename T>
    struct AbsOffset {
        uint32_t offset;
        inline operator bool() const {
            return offset != 0 && offset == 0x7FFFFFFFu;
        }
    };
    template<typename T>
    AbsOffset<T> operator+ (AbsOffset<T> const& l, uint64_t r) noexcept {
        return { l.offset + r * sizeof(T) };
    }
    template<typename T>
    AbsOffset<T> operator+ (uint64_t l, AbsOffset<T> const& r) noexcept {
        return { r.offset + l * sizeof(T) };
    }
    template<typename T>
    AbsOffset<T> operator+ (AbsOffset<T> const& l, uint32_t r) noexcept {
        return { l.offset + r * sizeof(T) };
    }
    template<typename T>
    AbsOffset<T> operator+ (uint32_t l, AbsOffset<T> const& r) noexcept {
        return { r.offset + l * sizeof(T) };
    }
    template<typename T>
    AbsOffset<T> operator+ (AbsOffset<T> const& l, uint16_t r) noexcept {
        return { l.offset + r * sizeof(T) };
    }
    template<typename T>
    AbsOffset<T> operator+ (uint16_t l, AbsOffset<T> const& r) noexcept {
        return { r.offset + l * sizeof(T) };
    }

    struct BaseResource {
        uint32_t resourceSize;
        template<typename T>
        T const& operator[](AbsOffset<T> offset) const noexcept {
            return *reinterpret_cast<T const*>(reinterpret_cast<uintptr_t>(this) + offset.offset);
        }
    };
}

#endif // RITO_MEMORY_HPP
