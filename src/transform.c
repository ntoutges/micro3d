#include "transform.h"
#include "debug.h"

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

void m3_quat_rotate_by(m3_quat dest, m3_quat* src) {
    m3_quat_rotate(&dest, *src);

    src->x = dest.x;
    src->y = dest.y;
    src->z = dest.z;
    src->w = dest.w;
}

void m3_quat_rotate(m3_quat* dest, m3_quat src) {
    m3_quat left = { dest->x, dest->y, dest->z, dest->w }; // Store copy of dest

    // Perform quaternion composition
    dest->x = roundf(((int16_t) left.w * src.x + (int16_t) left.x * src.w + (int16_t) left.y * src.z - (int16_t) left.z * src.y) / 127.0);
    dest->y = roundf(((int16_t) left.w * src.y - (int16_t) left.x * src.z + (int16_t) left.y * src.w + (int16_t) left.z * src.x) / 127.0);
    dest->z = roundf(((int16_t) left.w * src.z + (int16_t) left.x * src.y - (int16_t) left.y * src.x + (int16_t) left.z * src.w) / 127.0);
    dest->w = roundf(((int16_t) left.w * src.w - (int16_t) left.x * src.x - (int16_t) left.y * src.y - (int16_t) left.z * src.z) / 127.0);
}

inline m3_quat m3_quat_conj(m3_quat src) {
    return (m3_quat){
        -src.x,
        -src.y,
        -src.z,
        src.w
    };
}

m3_quat m3_vec_to_quat(m3_vec dir, m3_vec _up) {
    // --- Step 1: Build orthonormal basis ---

    // right = up × dir
    m3_vec right = m3_vec_cross(_up, dir);
    m3_vec_normalize(&right);

    // recompute up = dir × right
    m3_vec up = m3_vec_cross(dir, right);
    m3_vec_normalize(&up);

    // --- Step 2: Convert to float [-1, 1] ---

    float rx = right.x / 127.0f;
    float ry = right.y / 127.0f;
    float rz = right.z / 127.0f;

    float ux = up.x / 127.0f;
    float uy = up.y / 127.0f;
    float uz = up.z / 127.0f;

    float fx = dir.x / 127.0f;
    float fy = dir.y / 127.0f;
    float fz = dir.z / 127.0f;

    // --- Step 3: Rotation matrix (column-major) ---
    // [ rx  ux  fx ]
    // [ ry  uy  fy ]
    // [ rz  uz  fz ]

    float m00 = rx, m01 = ux, m02 = fx;
    float m10 = ry, m11 = uy, m12 = fy;
    float m20 = rz, m21 = uz, m22 = fz;

    // --- Step 4: Matrix → quaternion (stable) ---

    float qw, qx, qy, qz;
    float trace = m00 + m11 + m22;

    if (trace > 0.0f) {
        float s = sqrtf(trace + 1.0f) * 2.0f;
        qw = 0.25f * s;
        qx = (m21 - m12) / s;
        qy = (m02 - m20) / s;
        qz = (m10 - m01) / s;
    } else if (m00 > m11 && m00 > m22) {
        float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
        qw = (m21 - m12) / s;
        qx = 0.25f * s;
        qy = (m01 + m10) / s;
        qz = (m02 + m20) / s;
    } else if (m11 > m22) {
        float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
        qw = (m02 - m20) / s;
        qx = (m01 + m10) / s;
        qy = 0.25f * s;
        qz = (m12 + m21) / s;
    } else {
        float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
        qw = (m10 - m01) / s;
        qx = (m02 + m20) / s;
        qy = (m12 + m21) / s;
        qz = 0.25f * s;
    }

    // --- Step 5: Normalize quaternion (important after numeric error) ---

    float mag = sqrtf(qx*qx + qy*qy + qz*qz + qw*qw);
    float inv = (mag > 0.0f) ? (1.0f / mag) : 1.0f;

    qx *= inv;
    qy *= inv;
    qz *= inv;
    qw *= inv;

    // --- Step 6: Convert back to fixed-point (127 = 1.0) ---

    m3_quat out = {
        (int16_t)roundf(qx * 127.0f),
        (int16_t)roundf(qy * 127.0f),
        (int16_t)roundf(qz * 127.0f),
        (int16_t)roundf(qw * 127.0f)
    };

    return out;
}