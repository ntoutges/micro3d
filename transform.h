#ifndef _MICROTRANSFORM_H
#define _MICROTRANSFORM_H

// Note: All functions in this file assume the input vector/quaternions exist

#include "math.h"

#include "./types.h"

/**
 * Add the `src` vector contents to the `dest` vector
 * @param dest The vector to store the result in
 * @param src The vector to add from
 */
void m3_vec_add(m3_vec* dest, m3_vec src);

/**
 * Get the dot product of two input vectors
 * @param a A vector to dot
 * @param b A vector to dot
 * @returns The result of a . b
 */
int16_t m3_vec_dot(m3_vec* a, m3_vec b);

/**
 * Get the cross product of two input vectors.
 * Note that this works using the compressed fixed-point format (1 = 0x7F), to avoid overflowing the output vector
 * @param a A vector to cross
 * @param b A vector to cross
 * @returns The result of a x b
 */
m3_vec m3_vec_cross(m3_vec* a, m3_vec b);

/**
 * Normalize a vector s.t. it has unit magnitude (127, due to packed non-float format)
 * If the vector cannot be normalized (has magnitude 0), the vec is set to a default (0,0,127)
 * @param quat The quaternion to normalize.
 */
void m3_vec_normalize(m3_vec* vec);

/**
 * Normalize a quaternion s.t. it has unit magnitude (127, due to packed non-float format)
 * If the quat cannot be normalized (has magnitude 0), the quat is set to a default (0,0,0,127)
 * @param quat The quaternion to normalize.
 */
void m3_quat_normalize(m3_quat* quat);

/**
 * Rotate a vector by a quaternion
 * @param dest The vector to rotate/store the result in
 * @param quat The quaternion to rotate the vector by. Note that this quat _must_ be a normalized
 */
void m3_vec_rotate(m3_vec* vec, m3_quat quat);

/**
 * Rotate the `dest` quaternion by the `src` quaternion
 * @param dest The quaternion to store the result in.
 * @param src The quaternion to add from. Note that this quat _must_ be a normalized
 */
void m3_quat_rotate(m3_quat* dest, m3_quat src);

/**
 * Create a unit quaternion from two vectors.
 * Note: Default (non-rotated) quat produced by vectors (dir -> +X), (up -> +Z)
 * @param dir   The main vector indicating the direction the quaternion must point in. Note that this vec _must_ be normalized
 * @param up    The vector indicating the "up" direction of the resulting quaternion
 */
m3_quat m3_vec_to_quat(m3_vec dir, m3_vec up);

#endif