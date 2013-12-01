/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "camera_opengl.h"
#include "core.h"
#include "display.h"


#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926536
#endif

void gluLookAt(double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz)
{
	double m[16];
	double x[3], y[3], z[3];
	double mag;

	/* Make rotation matrix */

	/* Z vector */
	z[0] = eyex - centerx;
	z[1] = eyey - centery;
	z[2] = eyez - centerz;
	mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	if (mag) {			/* mpichler, 19950515 */
		z[0] /= mag;
		z[1] /= mag;
		z[2] /= mag;
	}

	/* Y vector */
	y[0] = upx;
	y[1] = upy;
	y[2] = upz;

	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];

	/* mpichler, 19950515 */
	/* cross product gives area of parallelogram, which is < 1.0 for
	 * non-perpendicular unit-length vectors; so normalize x, y here
	 */

	mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	if (mag) {
		x[0] /= mag;
		x[1] /= mag;
		x[2] /= mag;
	}

	mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
	if (mag) {
		y[0] /= mag;
		y[1] /= mag;
		y[2] /= mag;
	}

	#define M(row,col)  m[col*4+row]
		M(0, 0) = x[0];
		M(0, 1) = x[1];
		M(0, 2) = x[2];
		M(0, 3) = 0.0;
		M(1, 0) = y[0];
		M(1, 1) = y[1];
		M(1, 2) = y[2];
		M(1, 3) = 0.0;
		M(2, 0) = z[0];
		M(2, 1) = z[1];
		M(2, 2) = z[2];
		M(2, 3) = 0.0;
		M(3, 0) = 0.0;
		M(3, 1) = 0.0;
		M(3, 2) = 0.0;
		M(3, 3) = 1.0;
	#undef M
	glMultMatrixd(m);

	/* Translate Eye to Origin */
	glTranslated(-eyex, -eyey, -eyez);
}

void frustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,  GLdouble nearval, GLdouble farval)
{
	GLdouble x, y, a, b, c, d;
	GLdouble m[16];

	x = (2.0 * nearval) / (right - left);
	y = (2.0 * nearval) / (top - bottom);
	a = (right + left) / (right - left);
	b = (top + bottom) / (top - bottom);
	c = -(farval + nearval) / ( farval - nearval);
	d = -(2.0 * farval * nearval) / (farval - nearval);

	#define M(row,col)  m[col*4+row]
		M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
		M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
		M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
		M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
	#undef M

	glMultMatrixd(m);
}

void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	double xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	/* don't call glFrustum() because of error semantics (covglu) */
	frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}



