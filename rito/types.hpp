#ifndef RITO_TYPES_HPP
#define RITO_TYPES_HPP
#include <cinttypes>
#include <array>
#include <vector>
#include <variant>
#include <optional>

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
        Vec3 org;
        Vec3 size;
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

    struct Form3D {
        Vec3 position;
        Vec3 scale;
        Quat orientation;
    };
}

#endif // RITO_TYPES_HPP
