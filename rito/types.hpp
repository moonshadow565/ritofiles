#ifndef RITO_TYPES_HPP
#define RITO_TYPES_HPP
#include <cinttypes>
#include <array>
#include <vector>
#include <variant>
#include <optional>
#include <cmath>

namespace Rito {
    struct ColorF;
    struct Vec4;
    struct Vec3;
    struct Vec2;
    struct Mtx44;
    struct Box3D;


    struct ColorF {
        float r,g,b,a;
        inline constexpr static size_t size = 4;
        inline float &operator[](size_t idx) & {
            return *(reinterpret_cast<float *>(this) + idx);
        }
        inline float const &operator[](size_t idx) const & {
            return *(reinterpret_cast<float const *>(this) + idx);
        }
    };
    struct ColorB {
        uint8_t r,g,b,a;
        inline constexpr static size_t size = 4;
        inline uint8_t &operator[](size_t idx) & {
            return *(reinterpret_cast<uint8_t *>(this) + idx);
        }
        inline uint8_t const &operator[](size_t idx) const & {
            return *(reinterpret_cast<uint8_t const *>(this) + idx);
        }
    };
    struct Vec4 {
        float x,y,z,w;
        inline constexpr static size_t size = 4;
        inline float &operator[](size_t idx) & {
            return *(reinterpret_cast<float *>(this) + idx);
        }
        inline float const &operator[](size_t idx) const & {
            return *(reinterpret_cast<float const *>(this) + idx);
        }
    };
    struct Vec3 {
        float x,y,z;
        inline constexpr static size_t size = 3;
        inline float &operator[](size_t idx) & {
            return *(reinterpret_cast<float *>(this) + idx);
        }
        inline float const &operator[](size_t idx) const & {
            return *(reinterpret_cast<float const *>(this) + idx);
        }
    };
    struct Vec2 {
        float x,y;
        inline constexpr static size_t size = 2;
        inline float &operator[](size_t idx) & {
            return *(reinterpret_cast<float *>(this) + idx);
        }
        inline float const &operator[](size_t idx) const & {
            return *(reinterpret_cast<float const *>(this) + idx);
        }
    };

    struct Mtx44 {
        using array_t = float[4];
        float m[4][4] = {};
        inline array_t& operator[](size_t index) & {
            return m[index];
        }
        inline array_t const& operator[](size_t index) const& {
            return m[index];
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
        float x,y,z,w;
        inline float &operator[](size_t idx) & {
            return *(reinterpret_cast<float *>(this) + idx);
        }
        inline float const &operator[](size_t idx) const & {
            return *(reinterpret_cast<float const *>(this) + idx);
        }
    };

    struct QuantizedQuat {
        std::array<uint16_t, 3> v;
        operator Quat() const noexcept {
            uint64_t const bits = uint64_t{v[0]} | uint64_t{v[1]} << 16 | uint64_t{v[2]} << 32;
            uint16_t const maxIndex = static_cast<uint16_t>((bits >> 45) & 0x0003u);
            uint16_t const v_a = static_cast<uint16_t>((bits >> 30) & 0x7FFFu);
            uint16_t const v_b = static_cast<uint16_t>((bits >> 15) & 0x7FFFu);
            uint16_t const v_c = static_cast<uint16_t>(bits & 0x7FFFu);

            auto const a = (v_a / 32767.0f) * std::sqrt(2.f) - 1 / std::sqrt(2.f);
            auto const b = (v_b / 32767.0f) * std::sqrt(2.f) - 1 / std::sqrt(2.f);
            auto const c = (v_c / 32767.0f) * std::sqrt(2.f) - 1 / std::sqrt(2.f);
            auto const sub = std::max(0.f, 1.f - (a * a + b * b + c * c));
            auto const inv = 1 / std::sqrt(sub);
            auto const d = sub == 0.f ? 0.f : ((3.f - sub * inv * inv) * (inv * 0.5f)) * sub;
            switch(maxIndex) {
            case 0:
                return {d, a, b, c};
            case 1:
                return {a, d, b, c};
            case 2:
                return {a, b, d, c};
            default:
                return {a, b, c, d};
            }
        }
    };

    struct Form3D {
        Vec3 position;
        Vec3 scale;
        Quat orientation;
    };
}

#endif // RITO_TYPES_HPP
