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
    result.y = roundf((a.z * b.x - a.x * b.z) / 127.0);
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
m3_quat m3_vec_to_quat(m3_vec dir, m3_vec _up) {

    // Create unit basis vector
    m3_vec orth = m3_vec_cross(_up, dir);
    m3_vec_normalize(&orth);

    // Recompute "up" vector to garuntee orthogonality
    m3_vec up = m3_vec_cross(dir, orth);
    m3_vec_normalize(&up);

    // Important intermediates
    float trace = (dir.x + orth.y + up.z) / 127.0; // Used to apply rule of dir/up vector default positioning

    // @TODO: Handle case where (t < 0)

    float scale = sqrtf(trace + 1) * 2; // Used to maintain proper scale

    int16_t rx = roundf((orth.z - up.y) / scale);
    int16_t ry = roundf((up.x - dir.z) / scale);
    int16_t rz = roundf((dir.y - orth.x) / scale);
    int16_t rw = roundf(127.0 * scale / 4.0);

    // Normalize rx/ry/rz/wx
    float inv_mag = 127.0 / sqrtf(rx*rx + ry*ry + rz*rz + rw*rw);

    // Create/return new quaternion
    return (m3_quat){
        rx * inv_mag,
        ry * inv_mag,
        rz * inv_mag,
        rw * inv_mag
    };
}
