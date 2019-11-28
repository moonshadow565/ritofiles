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
    };

    struct Vec3 {
        float x;
        float y;
        float z;
    };

    struct Vec2 {
        float x,y;
    };

    struct Mtx44 {
        using array_t = float[4];
        array_t m[4] = {};

        inline constexpr array_t& operator[](size_t index) & {
            return m[index];
        }

        inline constexpr array_t const& operator[](size_t index) const& {
            return m[index];
        }

        inline constexpr Mtx44 mul(Mtx44 const& r) const noexcept {
            return {{
                {
                    m[0][0] * r[0][0] + m[0][1] * r[1][0] + m[0][2] * r[2][0] + m[0][3] * r[3][0],
                    m[0][0] * r[0][1] + m[0][1] * r[1][1] + m[0][2] * r[2][1] + m[0][3] * r[3][1],
                    m[0][0] * r[0][2] + m[0][1] * r[1][2] + m[0][2] * r[2][2] + m[0][3] * r[3][2],
                    m[0][0] * r[0][3] + m[0][1] * r[1][3] + m[0][2] * r[2][3] + m[0][3] * r[3][3],
                },
                {
                    m[1][0] * r[0][0] + m[1][1] * r[1][0] + m[1][2] * r[2][0] + m[1][3] * r[3][0],
                    m[1][0] * r[0][1] + m[1][1] * r[1][1] + m[1][2] * r[2][1] + m[1][3] * r[3][1],
                    m[1][0] * r[0][2] + m[1][1] * r[1][2] + m[1][2] * r[2][2] + m[1][3] * r[3][2],
                    m[1][0] * r[0][3] + m[1][1] * r[1][3] + m[1][2] * r[2][3] + m[1][3] * r[3][3],
                },
                {
                    m[2][0] * r[0][0] + m[2][1] * r[1][0] + m[2][2] * r[2][0] + m[2][3] * r[3][0],
                    m[2][0] * r[0][1] + m[2][1] * r[1][1] + m[2][2] * r[2][1] + m[2][3] * r[3][1],
                    m[2][0] * r[0][2] + m[2][1] * r[1][2] + m[2][2] * r[2][2] + m[2][3] * r[3][2],
                    m[2][0] * r[0][3] + m[2][1] * r[1][3] + m[2][2] * r[2][3] + m[2][3] * r[3][3],
                },
                {
                    m[3][0] * r[0][0] + m[3][1] * r[1][0] + m[3][2] * r[2][0] + m[3][3] * r[3][0],
                    m[3][0] * r[0][1] + m[3][1] * r[1][1] + m[3][2] * r[2][1] + m[3][3] * r[3][1],
                    m[3][0] * r[0][2] + m[3][1] * r[1][2] + m[3][2] * r[2][2] + m[3][3] * r[3][2],
                    m[3][0] * r[0][3] + m[3][1] * r[1][3] + m[3][2] * r[2][3] + m[3][3] * r[3][3],
                },
            }};
        }
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
                    { m[0][0], m[0][1], m[0][2], m[0][3], },
                    { m[0][0], m[0][1], m[0][2], m[0][3], },
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
        float w;
        float x;
        float y;
        float z;

        inline constexpr Quat normalize() const noexcept {
            auto n = std::sqrt(w * w + x * x + y * y + z * z );
            return { w / n, x / n, y / n, z / n };
        }
    };

    struct QuantizedQuat {
        std::array<uint16_t, 3> v;

        // inline QuantizedQuat() noexcept = default;

        inline operator Quat() const noexcept {
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
        Vec3 pos;
        Vec3 scale;
        Quat rot;

        inline constexpr operator Mtx43() const noexcept {
            return {{
                   {
                        (1.0f - 2.0f * (rot.y * rot.y + rot.z * rot.z)) * scale.x,
                        (2.0f * (rot.x * rot.y - rot.z * rot.w)) * scale.x,
                        (2.0f * (rot.x * rot.z + rot.y * rot.w)) * scale.x,
                        pos.x,
                   },
                   {
                        (2.0f * (rot.x * rot.y + rot.z * rot.w)) * scale.y,
                        (1.0f - 2.0f * (rot.x * rot.x + rot.z * rot.z)) * scale.y,
                        (2.0f * (rot.y * rot.z - rot.x * rot.w)) * scale.y,
                        pos.y,
                   },
                   {
                        (2.0f * (rot.x * rot.z - rot.y * rot.w)) * scale.z,
                        (2.0f * (rot.y * rot.z + rot.x * rot.w)) * scale.z,
                        (1.0f - 2.0f * (rot.x * rot.x + rot.y * rot.y)) * scale.z,
                        pos.x,
                   },
                }};
        }

        inline constexpr operator Mtx44() const noexcept {
            return operator Mtx43();
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
