#ifndef RITO_MATH_HPP
#define RITO_MATH_HPP
#include "types.hpp"
#include <cmath>
#include <array>
#include <limits>
#include <numeric>

namespace Rito {
    inline constexpr float FloatInf = std::numeric_limits<float>::infinity();
    inline constexpr ColorF ColorFInf = { FloatInf, FloatInf, FloatInf, FloatInf };
    inline constexpr Vec4 Vec4Inf = { FloatInf, FloatInf, FloatInf, FloatInf };
    inline constexpr Vec3 Vec3Inf = { FloatInf, FloatInf, FloatInf };
    inline constexpr Vec2 Vec2Inf = { FloatInf, FloatInf };
    inline constexpr Mtx44 Mtx44Identity = {{
        { 1.f, 0.f, 0.f, 0.f },
        { 0.f, 1.f, 0.f, 0.f },
        { 0.f, 0.f, 1.f, 0.f },
        { 0.f, 0.f, 0.f, 1.f },
    }};

    inline auto Vec2_TransformNormal(Vec2 const& vec, Mtx44 const& mtx) {
        return Vec2 {
            mtx[0][0] * vec.x + mtx[1][0] * vec.y,
            mtx[0][1] * vec.x + mtx[1][1] * vec.y,
        };
    }

    inline auto Vec3_TransformNormal(Vec3 const& vec, Mtx44 const& mtx) {
        return Vec3 {
            mtx[0][0] * vec.x + mtx[1][0] * vec.y + mtx[2][0] * vec.z,
            mtx[0][1] * vec.x + mtx[1][1] * vec.y + mtx[2][1] * vec.z,
            mtx[0][2] * vec.x + mtx[1][2] * vec.y + mtx[2][2] * vec.z,
        };
    }

    inline float Vec4_length(Vec4 const& v) noexcept {
        return std::sqrt( v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    }

    inline float Vec3_length(Vec3 const& v) noexcept {
        return std::sqrt( v.x * v.x + v.y * v.y + v.z * v.z);
    }

    inline float Vec2_length(Vec2 const& v) noexcept {
        return std::sqrt( v.x * v.x + v.y * v.y);
    }

    inline Vec4 Vec4_cross(Vec4 const& v1, Vec4 const& v2, Vec4 const& v3) noexcept {
        float const x = + ( + v1.y * (v2.z * v3.w - v3.z * v2.w)
                            - v1.z * (v2.y * v3.w - v3.y * v2.w)
                            + v1.w * (v2.y * v3.z - v2.z * v3.y));
        float const y = - ( + v1.x * (v2.z * v3.w - v3.z * v2.w)
                            - v1.z * (v2.x * v3.w - v3.x * v2.w)
                            + v1.w * (v2.x * v3.z - v3.x * v2.z));
        float const z = + ( + v1.x * (v2.y * v3.w - v3.y * v2.w)
                            - v1.y * (v2.x * v3.w - v3.x * v2.w)
                            + v1.w * (v2.x * v3.y - v3.x * v2.y));
        float const w = - ( + v1.x * (v2.y * v3.z - v3.y * v2.z)
                            - v1.y * (v2.x * v3.z - v3.x * v2.z)
                            + v1.z * (v2.x * v3.y - v3.x * v2.y));
        return Vec4 { x, y, z, w };
    }

    inline float Mtx44_determinant(Mtx44 const& mtx) noexcept {
        Vec4 const v1 = { mtx[0][0], mtx[1][0], mtx[2][0], mtx[3][0] };
        Vec4 const v2 = { mtx[0][1], mtx[1][1], mtx[2][1], mtx[3][1] };
        Vec4 const v3 = { mtx[0][2], mtx[1][2], mtx[2][2], mtx[3][2] };
        Vec4 const vm = Vec4_cross(v1, v2, v3);
        return - (mtx[0][3] * vm.x + mtx[1][3] * vm.y + mtx[2][3] * vm.z + mtx[3][3] * vm.w);
    }

    inline Mtx44 Mtx44_inverse(Mtx44 const& mtx) noexcept {
        auto const det = Mtx44_determinant(mtx);
        Mtx44 result;
        for(size_t i = 0; i < 4; i++) {
            std::array<Vec4, 3> vec{};
            for(size_t j = 0; j < 4; j++) {
                if(j == i) {
                    continue;
                }
                auto const indx = j > i ? j - 1 : j;
                vec[indx] = {  mtx[j][0], mtx[j][1], mtx[j][2], mtx[j][3] };
            }
            auto const cross = Vec4_cross(vec[0], vec[1], vec[2]);
            for(size_t j = 0; j < 4; j++) {
                result[j][i] = std::pow(-1.0f, static_cast<float>(i)) * cross[j] / det;
            }
        }
        return result;
    }

    inline Mtx44 Mtx44_Multiply(Mtx44 const& l, Mtx44 const& r) noexcept {
        return {{
            {
                l[0][0] * r[0][0] + l[0][1] * r[1][0] + l[0][2] * r[2][0] + l[0][3] * r[3][0],
                l[0][0] * r[0][1] + l[0][1] * r[1][1] + l[0][2] * r[2][1] + l[0][3] * r[3][1],
                l[0][0] * r[0][2] + l[0][1] * r[1][2] + l[0][2] * r[2][2] + l[0][3] * r[3][2],
                l[0][0] * r[0][3] + l[0][1] * r[1][3] + l[0][2] * r[2][3] + l[0][3] * r[3][3],
            },
            {
                l[1][0] * r[0][0] + l[1][1] * r[1][0] + l[1][2] * r[2][0] + l[1][3] * r[3][0],
                l[1][0] * r[0][1] + l[1][1] * r[1][1] + l[1][2] * r[2][1] + l[1][3] * r[3][1],
                l[1][0] * r[0][2] + l[1][1] * r[1][2] + l[1][2] * r[2][2] + l[1][3] * r[3][2],
                l[1][0] * r[0][3] + l[1][1] * r[1][3] + l[1][2] * r[2][3] + l[1][3] * r[3][3],
            },
            {
                l[2][0] * r[0][0] + l[2][1] * r[1][0] + l[2][2] * r[2][0] + l[2][3] * r[3][0],
                l[2][0] * r[0][1] + l[2][1] * r[1][1] + l[2][2] * r[2][1] + l[2][3] * r[3][1],
                l[2][0] * r[0][2] + l[2][1] * r[1][2] + l[2][2] * r[2][2] + l[2][3] * r[3][2],
                l[2][0] * r[0][3] + l[2][1] * r[1][3] + l[2][2] * r[2][3] + l[2][3] * r[3][3],
            },
            {
                l[3][0] * r[0][0] + l[3][1] * r[1][0] + l[3][2] * r[2][0] + l[3][3] * r[3][0],
                l[3][0] * r[0][1] + l[3][1] * r[1][1] + l[3][2] * r[2][1] + l[3][3] * r[3][1],
                l[3][0] * r[0][2] + l[3][1] * r[1][2] + l[3][2] * r[2][2] + l[3][3] * r[3][2],
                l[3][0] * r[0][3] + l[3][1] * r[1][3] + l[3][2] * r[2][3] + l[3][3] * r[3][3],
            },
        }};
    }

    // TODO
    inline Form3D Mtx44_Decompose(Mtx44 const& mtx) noexcept {
        return {};
    }

    inline constexpr uint32_t ElfHash (char const * s) noexcept {
        uint32_t h = 0, high = 0;
        while(*s) {
            h = (h << 4) + static_cast<uint8_t>(::tolower(*s++));
            if((high = h & 0xF0000000) != 0) {
                h ^= high >> 24;
            }
            h &= ~high;
        }
        return h;
    }
}


#endif // RITO_MATH_HPP
