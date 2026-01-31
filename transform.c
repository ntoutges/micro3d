#include "transform.h"

void m3_vec_add(m3_vec* dest, m3_vec src) {
    dest->x += src.x;
    dest->y += src.y;
    dest->z += src.z;
}

void m3_vec_sub(m3_vec* dest, m3_vec src) {
    dest->x -= src.x;
    dest->y -= src.y;
    dest->z -= src.z;
}

int16_t m3_vec_dot(m3_vec a, m3_vec b) {
    return a.x * b.x
        + a.y * b.y
        + a.z * b.z;
}

m3_vec m3_vec_cross(m3_vec a, m3_vec b) {
    m3_vec result;

    // Perform cross product math
    result.x = roundf((a.y * b.z - a.z * b.y) / 127.0);
    result.y = roundf((a.x * b.z - a.z * b.x) / 127.0);
    result.z = roundf((a.x * b.y - a.y * b.x) / 127.0);

    return result;
}

void m3_vec_normalize(m3_vec* vec) {
    // Compute square of total quat magnitude
    int16_t sq_magnitude = 0;
    sq_magnitude += vec->x * vec->x;
    sq_magnitude += vec->y * vec->y;
    sq_magnitude += vec->z * vec->z;

    // Invalid magnitude; Return default unit vector (0,0,127)
    if (sq_magnitude == 0) {
        vec->x = 0;
        vec->y = 0;
        vec->z = 127;
        return;
    }

    // Get inverse magnitude
    float inv_magnitude = 127 / sqrtf(sq_magnitude);

    // Multiply all components by inv_magnitude to obtain normalized quat
    vec->x *= inv_magnitude;
    vec->y *= inv_magnitude;
    vec->z *= inv_magnitude;
}

void m3_quat_normalize(m3_quat* quat) {
    // Compute square of total quat magnitude
    int16_t sq_magnitude = 0;
    sq_magnitude += quat->x * quat->x;
    sq_magnitude += quat->y * quat->y;
    sq_magnitude += quat->z * quat->z;
    sq_magnitude += quat->w * quat->w;

    // Invalid magnitude; Return default unit vector (0,0,0,127)
    if (sq_magnitude == 0) {
        quat->x = 0;
        quat->y = 0;
        quat->z = 0;
        quat->w = 127;
        return;
    }

    // Get inverse magnitude
    float inv_magnitude = 127 / sqrtf(sq_magnitude);

    // Multiply all components by inv_magnitude to obtain normalized quat
    quat->x *= inv_magnitude;
    quat->y *= inv_magnitude;
    quat->z *= inv_magnitude;
    quat->w *= inv_magnitude;
}

void m3_vec_rotate(m3_vec* vec, m3_quat quat) {
    m3_vec base = *vec; // Store copy of vec

    // Precompute reused values
    // Squares
    float x2 = quat.x * quat.x / (127.0 * 127.0);
    float y2 = quat.y * quat.y / (127.0 * 127.0);
    float z2 = quat.z * quat.z / (127.0 * 127.0);
    
    // Pariwise products
    float xy = quat.x * quat.y / (127.0 * 127.0);
    float xz = quat.x * quat.z / (127.0 * 127.0);
    float yz = quat.y * quat.z / (127.0 * 127.0);
    
    // Scalar products
    float xw = quat.x * quat.w / (127.0 * 127.0);
    float yw = quat.y * quat.w / (127.0 * 127.0);
    float zw = quat.z * quat.w / (127.0 * 127.0);
    
    // Perform vector rotation
    vec->x = roundf((1 - 2*y2 - 2*z2) * base.x + 2 * (xy - zw)     * base.y + 2 * (xz + yw)     * base.z);
    vec->y = roundf(2 * (xy + zw)     * base.x + (1 - 2*x2 - 2*z2) * base.y + 2 * (yz - xw)     * base.z);
    vec->z = roundf(2 * (xz - yw)     * base.x + 2 * (yz - xw)     * base.y + (1 - 2*x2 - 2*y2) * base.z);
}

void m3_quat_rotate(m3_quat* dest, m3_quat src) {
    m3_quat left = { dest->x, dest->y, dest->z, dest->w }; // Store copy of dest

    // Perform quaternion composition
    dest->x = roundf((left.w * src.x + left.x * src.w + left.y * src.z - left.z * src.y) / 127.0);
    dest->y = roundf((left.w * src.y - left.x * src.z + left.y * src.w + left.z * src.x) / 127.0);
    dest->z = roundf((left.w * src.z + left.x * src.y - left.y * src.x + left.z * src.w) / 127.0);
    dest->w = roundf((left.w * src.w - left.x * src.x - left.y * src.y - left.z - src.z) / 127.0);
}

inline m3_quat m3_quat_conj(m3_quat src) {
    return (m3_quat){
        -src.x,
        -src.y,
        -src.z,
        src.w
    };
}

// OLD: uses fragile matrix math to transform vectors
// m3_quat m3_vec_to_quat(m3_vec dir, m3_vec _up) {

//     // Create unit basis vector
//     m3_vec orth = m3_vec_cross(_up, dir);
//     m3_vec_normalize(&orth);

//     // Recompute "up" vector to garuntee orthogonality
//     m3_vec up = m3_vec_cross(dir, orth);
//     m3_vec_normalize(&up);

//     // Important intermediates
//     float trace = (dir.x + orth.y + up.z) / 127.0; // Used to apply rule of dir/up vector default positioning

//     // @TODO: Handle case where (t < 0)

//     float scale = sqrtf(trace + 1) * 2; // Used to maintain proper scale

//     // Create/return new quaternion
//     m3_quat result = {
//         (orth.z - up.y) / scale,
//         (up.x - dir.z) / scale,
//         (dir.y - orth.x) / scale,
//         127.0 * scale / 4.0,
//     };
//     m3_quat_normalize(&result);

//     return result;
// }

// New: uses "2 stage" method, more resistant to 8-bit fixed-point errors
m3_quat m3_vec_to_quat(m3_vec dir, m3_vec up) {
    m3_vec_normalize(&up);

    // Edge case: desired axis exactly opposite +X
    if (dir.x == -127) {
        return (m3_quat){
            up.x,
            up.y,
            up.z,
            0
        };
    }

    // Calculate magnitude of q1 quat
    float mag1 = sqrtf(
        dir.z * dir.z
        + dir.y * dir.y
        + (dir.x + 127) * (dir.x + 127)
    ) / 127.0;

    // Rotate +X axis to align with 'dir'; Auto-normalize
    m3_quat q1 = {
        0,
        roundf(-dir.z / mag1),
        roundf(dir.y / mag1),
        roundf((dir.x + 127) / mag1)
    };

    // Rotate +Z axis to align with 'up'
    m3_vec u1 = { 0, 0, 127 };
    m3_vec_rotate(&u1, q1);

    // Project onto plane perpendicular to 'dir'
    int16_t d1 = m3_vec_dot(u1, dir);
    int16_t d2 = m3_vec_dot(up, dir);

    m3_vec u1p = {
        u1.x - (dir.x * d1) / 127,
        u1.y - (dir.y * d1) / 127,
        u1.z - (dir.z * d1) / 127
    };
    m3_vec u2p = {
        up.x - (dir.x * d2) / 127,
        up.y - (dir.y * d2) / 127,
        up.z - (dir.z * d2) / 127
    };

    m3_vec_normalize(&u1p);
    m3_vec_normalize(&u2p);

    // Roll correction
    int16_t dot = m3_vec_dot(u1p, u2p) / 127;
    m3_vec cross = m3_vec_cross(u1p, u2p);

    int8_t sign = m3_vec_dot(cross, dir) >= 0 ? 1 : -1;

    // Half angle terms
    int16_t w2 = sqrtf(127 + dot) * 127.0 / 2.0;
    float s2 = sqrtf(127 - dot) * 2.0;

    // Calculate magnitude of q2 quat
    float mag2 = sqrtf(
        s2*s2 * (dir.x*dir.x + dir.y*dir.y + dir.z*dir.z)
        + w2 * w2
    ) / 127.0;

    // Create result quat q2
    m3_quat q2 = {
        dir.x * s2 * sign / mag2,
        dir.y * s2 * sign / mag2,
        dir.z * s2 * sign / mag2,
        w2 / mag2
    };

    // q = q2 * q1
    m3_quat_rotate(&q2, q1);

    return q2;
}