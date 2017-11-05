/*
Copyright (c) 2012 Daniel Burke

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include <math.h>

#include "3dmaths.h"

/* Fast Inverse Square Root
 * http://www.beyond3d.com/content/articles/8/
 */
float finvsqrt(float x)
{
	union finv { int i; float f; } u;
	u.f = x;

	float xhalf = 0.5f*x;
	u.i = 0x5f3759df - (u.i>>1);
	x = u.f;
	x = x*(1.5f - xhalf*x*x); /* repeat this line to improve accuracy */
	return x;
}

/*
 * Vector manipulation functions
 */

void vect_sdivide(float3 *result, const float3 *vect, const float scalar)
{
	result->x = vect->x / scalar;
	result->y = vect->y / scalar;
	result->z = vect->z / scalar;
}


void vect_madd(float3 *result, const float scalar,
		const float3 *left, const float3 *right)
{
	result->x = scalar * left->x + right->x;
	result->y = scalar * left->y + right->y;
	result->z = scalar * left->z + right->z;
}


void vect_smul(float3 *result, const float3 *left, const float right)
{
	result->x = left->x * right;
	result->y = left->y * right;
	result->z = left->z * right;
}


void vect_mul(float3 *result, const float3 *left, const float3 *right)
{
	result->x = left->x * right->x;
	result->y = left->y * right->y;
	result->z = left->z * right->z;
}


void vect_add(float3 *result, const float3 *left, const float3 *right)
{
	result->x = left->x + right->x;
	result->y = left->y + right->y;
	result->z = left->z + right->z;
}


void vect_sadd(float3 *result, const float3 *left, const float right)
{
	result->x = left->x + right;
	result->y = left->y + right;
	result->z = left->z + right;
}


void vect_sub(float3 *result, const float3 *left, const float3 *right)
{
	result->x = left->x - right->x;
	result->y = left->y - right->y;
	result->z = left->z - right->z;
}


float vect_magnitude(const float3 *vect)
{
	return vect->x * vect->x + vect->y * vect->y + vect->z * vect->z;
}


void vect_norm(float3 * result, const float3 *vect)
{
	float len = sqrt(vect_magnitude(vect));
	float scale = 1.0f / len;
	result->x = vect->x * scale;
	result->y = vect->y * scale;
	result->z = vect->z * scale;
}


/* inner product (dot product) of two vectors */
float vect_dot(const float3 *left, const float3 *right)
{
	return left->x * right->x + left->y * right->y + left->z * right->z;
}


/* outer product (cross product) of two vectors */
void vect_cross(float3 *result, const float3 *left, const float3 *right)
{
	result->x = left->y * right->z - left->z * right->y;
	result->y = left->z * right->x - left->x * right->z;
	result->z = left->x * right->y - left->y * right->x;
}



/*
 * Matrix functions
 */

void mat_mul(float ret[16], const float left[16], const float right[16])
{
	for (unsigned int i = 0; i < 16; i += 4)
	for (unsigned int j = 0; j < 4; ++j)
		ret[i + j] = (right[i + 0] * left[j +  0])
			+ (right[i + 1] * left[j +  4])
			+ (right[i + 2] * left[j +  8])
			+ (right[i + 3] * left[j + 12]);
}


void mat_identity(float ret[16])
{
	ret[0*4+0] = 1.0f; ret[1*4+0] = 0.0f; ret[2*4+0] = 0.0f; ret[3*4+0] = 0.0f;
	ret[0*4+1] = 0.0f; ret[1*4+1] = 1.0f; ret[2*4+1] = 0.0f; ret[3*4+1] = 0.0f;
	ret[0*4+2] = 0.0f; ret[1*4+2] = 0.0f; ret[2*4+2] = 1.0f; ret[3*4+2] = 0.0f;
	ret[0*4+3] = 0.0f; ret[1*4+3] = 0.0f; ret[2*4+3] = 0.0f; ret[3*4+3] = 1.0f;
}


void mat_trans(float ret[16], const float x, const float y, const float z)
{
	ret[0*4+0] = 1.0f; ret[1*4+0] = 0.0f; ret[2*4+0] = 0.0f; ret[3*4+0] = x;
	ret[0*4+1] = 0.0f; ret[1*4+1] = 1.0f; ret[2*4+1] = 0.0f; ret[3*4+1] = y;
	ret[0*4+2] = 0.0f; ret[1*4+2] = 0.0f; ret[2*4+2] = 1.0f; ret[3*4+2] = z;
	ret[0*4+3] = 0.0f; ret[1*4+3] = 0.0f; ret[2*4+3] = 0.0f; ret[3*4+3] = 1.0f;
}

/*
void mat_trans(float ret[16], const float x, const float y, const float z)
{
	ret[0*4+0] = 1.0f; ret[0*4+1] = 0.0f; ret[0*4+2] = 0.0f; ret[0*4+3] = x;
	ret[1*4+0] = 0.0f; ret[1*4+1] = 1.0f; ret[1*4+2] = 0.0f; ret[1*4+3] = y;
	ret[2*4+0] = 0.0f; ret[2*4+1] = 0.0f; ret[2*4+2] = 1.0f; ret[2*4+3] = z;
	ret[3*4+0] = 0.0f; ret[3*4+1] = 0.0f; ret[3*4+2] = 0.0f; ret[3*4+3] = 1.0f;
}
*/

void mat_rotX(float ret[16], float theta)
{
	ret[0*4+0] = 1.0f; ret[1*4+0] = 0.0f; ret[2*4+0] = 0.0f; ret[3*4+0] = 0.0f;
	ret[0*4+1] = 0.0f; ret[1*4+1] = cos(theta); ret[2*4+1] = -sin(theta); ret[3*4+1] = 0.0f;
	ret[0*4+2] = 0.0f; ret[1*4+2] = sin(theta); ret[2*4+2] = cos(theta); ret[3*4+2] = 0.0f;
	ret[0*4+3] = 0.0f; ret[1*4+3] = 0.0f; ret[2*4+3] = 0.0f; ret[3*4+3] = 1.0f;
}

void mat_rotY(float ret[16], float theta)
{
	ret[0*4+0] = cos(theta); ret[1*4+0] = 0.0f; ret[2*4+0] = sin(theta); ret[3*4+0] = 0.0f;
	ret[0*4+1] = 0.0f; ret[1*4+1] = 1.0f; ret[2*4+1] = 0.0f; ret[3*4+1] = 0.0f;
	ret[0*4+2] = -sin(theta); ret[1*4+2] = 0.0f; ret[2*4+2] = cos(theta); ret[3*4+2] = 0.0f;
	ret[0*4+3] = 0.0f; ret[1*4+3] = 0.0f; ret[2*4+3] = 0.0f; ret[3*4+3] = 1.0f;
}

void mat_rotZ(float ret[16], float theta)
{
	ret[0*4+0] = cos(theta); ret[1*4+0] = -sin(theta); ret[2*4+0] = 0.0f; ret[3*4+0] = 0.0f;
	ret[0*4+1] = sin(theta); ret[1*4+1] = cos(theta); ret[2*4+1] = 0.0f; ret[3*4+1] = 0.0f;
	ret[0*4+2] = 0.0f; ret[1*4+2] = 0.0f; ret[2*4+2] = 1.0f; ret[3*4+2] = 0.0f;
	ret[0*4+3] = 0.0f; ret[1*4+3] = 0.0f; ret[2*4+3] = 0.0f; ret[3*4+3] = 1.0f;
}

// https://github.com/niswegmann/small-matrix-inverse


/*
// https://stackoverflow.com/questions/18499971/efficient-4x4-matrix-multiplication-c-vs-assembly
asm("    .text
.align 32                           # 1. function entry alignment
.globl matrixMultiplyASM            #    (for a faster call)
.type matrixMultiplyASM, @function
matrixMultiplyASM:
movaps   (%rdi), %xmm0
movaps 16(%rdi), %xmm1
movaps 32(%rdi), %xmm2
movaps 48(%rdi), %xmm3
movq $48, %rcx                      # 2. loop reversal
1:                                      #    (for simpler exit condition)
movss (%rsi, %rcx), %xmm4           # 3. extended address operands
shufps $0, %xmm4, %xmm4             #    (faster than pointer calculation)
mulps %xmm0, %xmm4
movaps %xmm4, %xmm5
movss 4(%rsi, %rcx), %xmm4
shufps $0, %xmm4, %xmm4
mulps %xmm1, %xmm4
addps %xmm4, %xmm5
movss 8(%rsi, %rcx), %xmm4
shufps $0, %xmm4, %xmm4
mulps %xmm2, %xmm4
addps %xmm4, %xmm5
movss 12(%rsi, %rcx), %xmm4
shufps $0, %xmm4, %xmm4
mulps %xmm3, %xmm4
addps %xmm4, %xmm5
movaps %xmm5, (%rdx, %rcx)
subq $16, %rcx                      # one 'sub' (vs 'add' & 'cmp')
jge 1b                              # SF=OF, idiom: jump if positive
ret")
*/
