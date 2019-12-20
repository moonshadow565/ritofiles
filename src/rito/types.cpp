#include "types.hpp"

using namespace Rito;

float Vec4::length() const noexcept {
     return std::sqrt( x * x + y * y + z * z + w * w);
}

float Vec3::length() const noexcept {
     return std::sqrt( x * x + y * y + z * z);
}

float Vec2::length() const noexcept {
     return std::sqrt( x * x + y * y );
}

Mtx44 Mtx44::mul(const Mtx44 &r) const noexcept {
    auto const r00 = m[0][0] * r[0][0] + m[0][1] * r[1][0] + m[0][2] * r[2][0] + m[0][3] * r[3][0];
    auto const r01 = m[0][0] * r[0][1] + m[0][1] * r[1][1] + m[0][2] * r[2][1] + m[0][3] * r[3][1];
    auto const r02 = m[0][0] * r[0][2] + m[0][1] * r[1][2] + m[0][2] * r[2][2] + m[0][3] * r[3][2];
    auto const r03 = m[0][0] * r[0][3] + m[0][1] * r[1][3] + m[0][2] * r[2][3] + m[0][3] * r[3][3];
    auto const r10 = m[1][0] * r[0][0] + m[1][1] * r[1][0] + m[1][2] * r[2][0] + m[1][3] * r[3][0];
    auto const r11 = m[1][0] * r[0][1] + m[1][1] * r[1][1] + m[1][2] * r[2][1] + m[1][3] * r[3][1];
    auto const r12 = m[1][0] * r[0][2] + m[1][1] * r[1][2] + m[1][2] * r[2][2] + m[1][3] * r[3][2];
    auto const r13 = m[1][0] * r[0][3] + m[1][1] * r[1][3] + m[1][2] * r[2][3] + m[1][3] * r[3][3];
    auto const r20 = m[2][0] * r[0][0] + m[2][1] * r[1][0] + m[2][2] * r[2][0] + m[2][3] * r[3][0];
    auto const r21 = m[2][0] * r[0][1] + m[2][1] * r[1][1] + m[2][2] * r[2][1] + m[2][3] * r[3][1];
    auto const r22 = m[2][0] * r[0][2] + m[2][1] * r[1][2] + m[2][2] * r[2][2] + m[2][3] * r[3][2];
    auto const r23 = m[2][0] * r[0][3] + m[2][1] * r[1][3] + m[2][2] * r[2][3] + m[2][3] * r[3][3];
    auto const r30 = m[3][0] * r[0][0] + m[3][1] * r[1][0] + m[3][2] * r[2][0] + m[3][3] * r[3][0];
    auto const r31 = m[3][0] * r[0][1] + m[3][1] * r[1][1] + m[3][2] * r[2][1] + m[3][3] * r[3][1];
    auto const r32 = m[3][0] * r[0][2] + m[3][1] * r[1][2] + m[3][2] * r[2][2] + m[3][3] * r[3][2];
    auto const r33 = m[3][0] * r[0][3] + m[3][1] * r[1][3] + m[3][2] * r[2][3] + m[3][3] * r[3][3];

    return Mtx44 {{
            { r00, r01, r02, r03 },
            { r10, r11, r12, r13 },
            { r20, r21, r22, r23 },
            { r30, r31, r32, r33 }
        }};
}

float Mtx44::det() const noexcept {
    auto const d = 0.0f
            + m[0][0] * m[1][1] * m[2][2] * m[3][3]
            - m[0][0] * m[1][1] * m[2][3] * m[3][2]
            + m[0][0] * m[1][2] * m[2][3] * m[3][1]
            - m[0][0] * m[1][2] * m[2][1] * m[3][3]
            + m[0][0] * m[1][3] * m[2][1] * m[3][2]
            - m[0][0] * m[1][3] * m[2][2] * m[3][1]
            - m[0][1] * m[1][2] * m[2][3] * m[3][0]
            + m[0][1] * m[1][2] * m[2][0] * m[3][3]
            - m[0][1] * m[1][3] * m[2][0] * m[3][2]
            + m[0][1] * m[1][3] * m[2][2] * m[3][0]
            - m[0][1] * m[1][0] * m[2][2] * m[3][3]
            + m[0][1] * m[1][0] * m[2][3] * m[3][2]
            + m[0][2] * m[1][3] * m[2][0] * m[3][1]
            - m[0][2] * m[1][3] * m[2][1] * m[3][0]
            + m[0][2] * m[1][0] * m[2][1] * m[3][3]
            - m[0][2] * m[1][0] * m[2][3] * m[3][1]
            + m[0][2] * m[1][1] * m[2][3] * m[3][0]
            - m[0][2] * m[1][1] * m[2][0] * m[3][3]
            - m[0][3] * m[1][0] * m[2][1] * m[3][2]
            + m[0][3] * m[1][0] * m[2][2] * m[3][1]
            - m[0][3] * m[1][1] * m[2][2] * m[3][0]
            + m[0][3] * m[1][1] * m[2][0] * m[3][2]
            - m[0][3] * m[1][2] * m[2][0] * m[3][1]
            + m[0][3] * m[1][2] * m[2][1] * m[3][0];
    if (d == 0.0f) {
        return std::numeric_limits<float>::quiet_NaN();
    }
    return d;
}

Mtx44 Mtx44::inv() const noexcept {
    auto const id = 1.0f / det();
    auto const r00 = 0.0f
            + m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
            + m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3])
            + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]);
    auto const r01 = 0.0f
            + m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
            + m[0][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3])
            + m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]);
    auto const r02 = 0.0f
            + m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2])
            + m[0][2] * (m[1][3] * m[3][1] - m[1][1] * m[3][3])
            + m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]);
    auto const r03 = 0.0f
            + m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2])
            + m[0][2] * (m[1][3] * m[2][1] - m[1][1] * m[2][3])
            + m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]);
    auto const r10 = 0.0f
            + m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
            + m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3])
            + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]);
    auto const r11 = 0.0f
            + m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
            + m[0][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3])
            + m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]);
    auto const r12 = 0.0f
            + m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2])
            + m[0][2] * (m[1][3] * m[3][0] - m[1][0] * m[3][3])
            + m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]);
    auto const r13 = 0.0f
            + m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2])
            + m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3])
            + m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]);
    auto const r20 = 0.0f
            + m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1])
            + m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3])
            + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
    auto const r21 = 0.0f
            + m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1])
            + m[0][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3])
            + m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
    auto const r22 = 0.0f
            + m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1])
            + m[0][1] * (m[1][3] * m[3][0] - m[1][0] * m[3][3])
            + m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]);
    auto const r23 = 0.0f
            + m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1])
            + m[0][1] * (m[1][3] * m[2][0] - m[1][0] * m[2][3])
            + m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    auto const r30 = 0.0f
            + m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
            + m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2])
            + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
    auto const r31 = 0.0f
            + m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
            + m[0][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2])
            + m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
    auto const r32 = 0.0f
            + m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1])
            + m[0][1] * (m[1][2] * m[3][0] - m[1][0] * m[3][2])
            + m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]);
    auto const r33 = 0.0f
            + m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
            + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2])
            + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    return Mtx44 {{
            { r00 * +id, r01 * -id, r02 * +id, r03 * -id },
            { r10 * -id, r11 * +id, r12 * -id, r13 * +id },
            { r20 * +id, r21 * -id, r22 * +id, r23 * -id },
            { r30 * -id, r31 * +id, r32 * -id, r33 * +id },
        }};
}

QuantizedQuat::operator Quat() const noexcept {
    uint64_t const bits = uint64_t{v[0]} | uint64_t{v[1]} << 16 | uint64_t{v[2]} << 32;
    uint16_t const maxIndex = static_cast<uint16_t>((bits >> 45) & 0x0003u);
    uint16_t const v_a = static_cast<uint16_t>((bits >> 30) & 0x7FFFu);
    uint16_t const v_b = static_cast<uint16_t>((bits >> 15) & 0x7FFFu);
    uint16_t const v_c = static_cast<uint16_t>(bits & 0x7FFFu);

    auto const a = (v_a / 32767.0f) * std::sqrt(2.f) - 1 / std::sqrt(2.f);
    auto const b = (v_b / 32767.0f) * std::sqrt(2.f) - 1 / std::sqrt(2.f);
    auto const c = (v_c / 32767.0f) * std::sqrt(2.f) - 1 / std::sqrt(2.f);
    auto const sub = std::max(0.f, 1.f - (a * a + b * b + c * c));
    auto const d = std::sqrt(sub);
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

Quat Quat::normalize() const noexcept {
    auto n = std::sqrt(w * w + x * x + y * y + z * z );
    return { x / n, y / n, z / n, w / n };
}

Form3D::operator Mtx43() const noexcept {
    auto const r00 = (1.0f - 2.0f * (rot.y * rot.y + rot.z * rot.z)) * scale.x;
    auto const r01 = (2.0f * (rot.x * rot.y - rot.z * rot.w)) * scale.x;
    auto const r02 = (2.0f * (rot.x * rot.z + rot.y * rot.w)) * scale.x;
    auto const r03 = pos.x;
    auto const r10 = (2.0f * (rot.x * rot.y + rot.z * rot.w)) * scale.y;
    auto const r11 = (1.0f - 2.0f * (rot.x * rot.x + rot.z * rot.z)) * scale.y;
    auto const r12 = (2.0f * (rot.y * rot.z - rot.x * rot.w)) * scale.y;
    auto const r13 = pos.y;
    auto const r20 = (2.0f * (rot.x * rot.z - rot.y * rot.w)) * scale.z;
    auto const r21 = (2.0f * (rot.y * rot.z + rot.x * rot.w)) * scale.z;
    auto const r22 = (1.0f - 2.0f * (rot.x * rot.x + rot.y * rot.y)) * scale.z;
    auto const r23 = pos.z;
    return Mtx43 {{
            { r00, r01, r02, r03 },
            { r10, r11, r12, r13 },
            { r20, r21, r22, r23 },
        }};
}
