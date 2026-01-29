#include "transform.h"
#include "stdio.h"

void m3_vec_add(m3_vec* dest, m3_vec src) {
    dest->x += src.x;
    dest->y += src.y;
    dest->z += src.z;
}

int16_t m3_vec_dot(m3_vec* a, m3_vec b) {
    return a->x * b.x
        + a->y * b.y
        + a->z * b.z;
}

m3_vec m3_vec_cross(m3_vec* a, m3_vec b) {
    m3_vec result;

    // Perform cross product math
    result.x = (a->y * b.z - a->z * b.y) / 127;
    result.y = (a->z * b.x - a->x * b.z) / 127;
    result.z = (a->x * b.y - a->y * b.x) / 127;

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
    m3_vec base = { vec->x, vec->y, vec->z }; // Store copy of vec

    // Precompute reused values
    // Squares
    int16_t x2 = quat.x * quat.x;
    int16_t y2 = quat.y * quat.y;
    int16_t z2 = quat.z * quat.z;
    
    // Pariwise products
    int16_t xy = quat.x * quat.y;
    int16_t xz = quat.x * quat.z;
    int16_t yz = quat.y * quat.z;
    
    // Scalar products
    int16_t xw = quat.x * quat.w;
    int16_t yw = quat.y * quat.w;
    int16_t zw = quat.z * quat.w;
    
    // Perform vector rotation
    vec->x = (1 - 2*y2 - 2*z2) * base.x + 2 * (xy - zw)     * base.y + 2 * (xz + yw)     * vec->z;
    vec->y = 2 * (xy + zw)     * base.x + (1 - 2*x2 - 2*z2) * base.y + 2 * (yz - xw)     * vec->z;
    vec->z = 2 * (xz - yw)     * base.x + 2 * (yz - xw)     * base.y + (1 - 2*x2 - 2*y2) * vec->z;
}

void m3_quat_rotate(m3_quat* dest, m3_quat src) {
    m3_quat left = { dest->x, dest->y, dest->z, dest->w }; // Store copy of dest

    // Perform quaternion composition
    dest->x = left.w * src.x + left.x * src.w + left.y * src.z - left.z * src.y;
    dest->y = left.w * src.y - left.x * src.z + left.y * src.w + left.z * src.x;
    dest->z = left.w * src.z + left.x * src.y - left.y * src.x + left.z * src.w;
    dest->w = left.w * src.w - left.x * src.x - left.y * src.y - left.z - src.z;
}

m3_quat m3_vec_to_quat(m3_vec dir, m3_vec _up) {

    // Create unit basis vector
    m3_vec orth = m3_vec_cross(&_up, dir);
    m3_vec_normalize(&orth);

    printf("dir: (%d, %d, %d)\n", dir.x, dir.y, dir.z);
    printf("orth: (%d, %d, %d)\n", orth.x, orth.y, orth.z);

    // Recompute "up" vector to garuntee orthogonality
    m3_vec up = m3_vec_cross(&dir, orth);

    printf("up: (%d, %d, %d)\n", up.x, up.y, up.z);

    // Important intermediates
    float trace = (dir.x + orth.y + up.z) / 127.0; // Used to apply rule of dir/up vector default positioning

    // @TODO: Handle case where (t < 0)

    float scale = sqrtf(trace + 1) * 2; // Used to maintain proper scale?

    // Create/return new quaternion
    return (m3_quat){
        (up.y - orth.z) / scale,
        (dir.z - up.x) / scale,
        (orth.x - dir.y) / scale,
        127.0 * scale / 4.0,
    };
}
