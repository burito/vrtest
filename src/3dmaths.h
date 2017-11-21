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

#ifndef __3DMATHS_H_
#define __3DMATHS_H_ 1


#ifdef WATCOM
#define sqrtf sqrt
#define sinf sin
#define cosf cos
#define strtof(x,y) atof(x)
#endif


typedef struct float4 {
	float x;
	float y;
	float z;
	float w;
} float4;

typedef struct int2 {
	int x, y;
} int2;

typedef struct int3 {
	int x, y, z;
} int3;

typedef struct byte4 {
	unsigned char x,y,z,w;
} byte4;


#define F2MAG(X) X.x*X.x+X.y*X.y
#define F3MAG(X) X.x*X.x+X.y*X.y+X.z*X.z
#define F3MAX(D) ((D.x>=D.y && D.x>=D.z)?D.x:(D.y>=D.z?D.y:D.z))


float finvsqrt(float x);

typedef union {
	struct { float x, y; };
	float f[2];
 } coord;

typedef union {
	struct { float x, y, z; };
	struct { coord xy; float fz; };
	struct { float fx; coord yz; };
	float f[3];
 } vect;

typedef union {
	float f[16];
	float m[4][4];
} mat4x4;


mat4x4 mat4x4_invert(mat4x4 m);
mat4x4 mat4x4_transpose(mat4x4 m);
mat4x4 mat4x4_identity(void);
mat4x4 mat4x4_rot_x(float t);
mat4x4 mat4x4_rot_y(float t);
mat4x4 mat4x4_rot_z(float t);
mat4x4 mat4x4_translate(vect v);
mat4x4 mat4x4_translate_float(float x, float y, float z);

vect vect_norm(vect v);
float vect_dot(vect l, vect r);
vect vect_cross(vect l, vect r);


/*
The following functions are to be called via the 
_Generic() macro's mul(), add() and sub()
*/

mat4x4 mat4x4_mul_mat4x4(mat4x4 l, mat4x4 r);
vect mat4x4_mul_vect(mat4x4 l, vect r);
mat4x4 mat4x4_add_mat4x4(mat4x4 l, mat4x4 r);
mat4x4 mat4x4_sub_mat4x4(mat4x4 l, mat4x4 r);

vect vect_mul_vect(vect l, vect r);
vect vect_mul_float(vect l, float r);
vect vect_add_vect(vect l, vect r);
vect vect_add_float(vect l, float r);
vect vect_sub_vect(vect l, vect r);

float float_mul(float l, float r);
float float_add(float l, float r);
float float_sub_float(float l, float r);
vect float_sub_vect(float l, vect r);

int int_mul(int l, int r);
int int_add(int l, int r);
int int_sub(int l, int r);


#define mul(X,Y) _Generic(X, \
	mat4x4: _Generic(Y, \
		mat4x4: mat4x4_mul_mat4x4, \
		default: mat4x4_mul_vect), \
	vect: _Generic(Y, \
		vect: vect_mul_vect,	\
		default: vect_mul_float), \
	float: float_mul, \
	default: int_mul \
	)(X,Y)

#define add(X,Y) _Generic(X, \
	mat4x4: mat4x4_add_mat4x4, \
	vect: _Generic(Y, \
		vect: vect_add_vect, \
		default:vect_add_float), \
	float: float_add, \
	default: int_add \
	)(X,Y)

#define sub(X,Y) _Generic(X, \
	mat4x4: mat4x4_sub_mat4x4, \
	vect: vect_sub_vect,	\
	float: _Generic(Y, \
		vect: float_sub_vect, \
		default:float_sub_float), \
	default: int_sub \
	)(X,Y)

#endif /* __3DMATHS_H_ */
