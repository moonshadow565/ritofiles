#ifndef RITO_TYPES_HPP
#define RITO_TYPES_HPP
#include <cinttypes>
#include <array>
#include <vector>
#include <variant>
#include <optional>
#include <string_view>
#include <cmath>

namespace Rito {
    struct ColorF {
        float r;
        float g;
        float b;
        float a;
    };

    struct ColorB {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    struct Vec4 {
        float x;
        float y;
        float z;
        float w;

        float length() const noexcept;
    };

    struct Vec3 {
        float x;
        float y;
        float z;

        float length() const noexcept;
    };

    struct Vec2 {
        float x,y;


        float length() const noexcept;
    };

    struct Mtx44 {
        using array_t = float[4];
        array_t m[4] = {};

        static inline constexpr Mtx44 identity() noexcept {
            return {{
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f },
            }};
        }

        inline constexpr array_t& operator[](size_t index) & {
            return m[index];
        }

        inline constexpr array_t const& operator[](size_t index) const& {
            return m[index];
        }

        Mtx44 mul(Mtx44 const& r) const noexcept;

        float det() const noexcept;

        Mtx44 inv() const noexcept;
    };

    struct Mtx43 {
        using array_t = float[4];
        array_t m[3] = {};

        inline constexpr array_t& operator[](size_t index) & {
            return m[index];
        }

        inline constexpr array_t const& operator[](size_t index) const& {
            return m[index];
        }

        inline constexpr operator Mtx44() const noexcept {
            return Mtx44 {{
                    { m[0][0], m[0][1], m[0][2], m[0][3], },
                    { m[1][0], m[1][1], m[1][2], m[1][3], },
                    { m[2][0], m[2][1], m[2][2], m[2][3], },
                    { 0.0f, 0.0f, 0.0f, 1.0f, },
                }};
        }
    };

    struct Box3D {
        Vec3 min;
        Vec3 max;
    };

    struct Sphere {
        Vec3 centerPoint;
        float radius;
    };

    struct Quat {
        float x;
        float y;
        float z;
        float w;

        Quat normalize() const noexcept;
    };

    struct QuantizedQuat {
        std::array<uint16_t, 3> v;

        operator Quat() const noexcept;
    };

    struct Form3D {
        Vec3 pos;
        Vec3 scale;
        Quat rot;

        operator Mtx43() const noexcept;
        inline constexpr operator Mtx44() const noexcept {
            return static_cast<Mtx44>(static_cast<Mtx43>(*this));
        }
    };

    inline constexpr uint32_t ElfHash (std::string_view view) noexcept {
        uint32_t h = 0, high = 0;
        for(auto const& c: view) {
            h = (h << 4) + static_cast<uint8_t>(::tolower(c));
            if((high = h & 0xF0000000) != 0) {
                h ^= high >> 24;
            }
            h &= ~high;
        }
        return h;
    }
}

#endif // RITO_TYPES_HPP
