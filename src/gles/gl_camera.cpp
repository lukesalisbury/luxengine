#include "gl_camera.hpp"
#include "gl_platform.hpp"
#include <math.h>

void gluLookAt(double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz)
{
	NATIVEFLOATTYPE m[16];
	NATIVEFLOATTYPE x[3], y[3], z[3];
	NATIVEFLOATTYPE mag;

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
	glMultMatrixf(m);

	/* Translate Eye to Origin */
	glTranslatef(-eyex, -eyey, -eyez);
}

void frustum(double left, double right, double bottom, double top, double nearval, double farval)
{
	NATIVEFLOATTYPE x, y, a, b, c, d;
	NATIVEFLOATTYPE m[16];

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

	glMultMatrixf(m);
}

void gluPerspective(double fovy, double aspect, double zNear, double zFar)
{
	double xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	/* don't call glFrustum() because of error semantics (covglu) */
	frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}


glCamera::glCamera(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	focus.x = 0.0;
	focus.y = 0.0;
	focus.z = 0.0;

	mode2d = false;
}

glCamera::~glCamera()
{

}

void glCamera::ChangeFocus(float x, float z, bool relative, bool move_glCamera)
{
	if ( relative )
	{
		focus.x += x;
		focus.z += z;
		if ( move_glCamera )
			this->ChangePosition(x,z);
	}
	else
	{
		focus.x = x;
		focus.z = z;
	}
}

void glCamera::ChangePosition(float x, float z)
{
	position.x += x;
	position.z += z;
}

void glCamera::ZoomPosition(float speed)
{
	position.y += speed;
	position.z += speed;
}

void glCamera::Roll(float angle)
{
	/*
	float cs = cos(PI/180 * angle);
	float sn = sin(PI/180 * angle);
	Vector3 t = u;  // remember u
	u.set(cs*t.x - sn*v.x, cs*t.y - sn*v.y, cs*t.z - sn*v.z);
	v.set(sn*t.x + cs*v.x, sn*t.y + cs*v.y, sn*t.z + cs*v.z);
	setModelViewMatrix(); // tell OpenGL
	*/
}

void glCamera::Pitch(float angle){
	/*
	float cs = cos(angle);
	float sn = sin(angle);
	Vector3 t = v;  // remember u
	v.set(cs*t.x - sn*n.x, cs*t.y - sn*n.y, cs*t.z - sn*n.z);
	n.set(sn*t.x + cs*n.x, sn*t.y + cs*n.y, sn*t.z + cs*n.z);
	setModelViewMatrix(); // tell OpenGL
	*/
}

void glCamera::Yaw(float angle) {
	/*
	float cs = cos(PI/180 * angle);
	float sn = sin(PI/180 * angle);
	Vector3 t = n;  // remember u
	n.set(cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z);
	u.set(sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z);
	setModelViewMatrix(); // tell OpenGL
	*/
}

void glCamera::Set2DView(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

#if USING_GLES
	glOrthof(0, w, h, 0, -10.0f, 10.0f);
#else
	glOrtho(0, w, h, 0, -10.0f, 10.0f);
#endif
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	this->mode2d = true;
}


void glCamera::Set3DView(int w, int h)
{
	focus.x = (float)w/2;
	focus.y = (float)h/2;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(-45.0, 1.3, -5.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
/*
	if ( lux::core->GetInput(KEYBOARD, 0, 304) )
	{
		if (lux::core->GetInput(KEYBOARD, 0, 127))
			focus.X += 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 277))
			focus.X -= 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 278))
			focus.Y += 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 279))
			focus.Y -= 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 280))
			focus.Z += 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 281))
			focus.Z -= 1.0f;
	}
	else
	{

		if (lux::core->GetInput(KEYBOARD, 0, 277))
			position.X -= 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 127))
			position.X += 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 277))
			position.X -= 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 278))
			position.Y += 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 279))
			position.Y -= 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 280))
			position.Z += 1.0f;
		if (lux::core->GetInput(KEYBOARD, 0, 281))
			position.Z -= 1.0f;
	}
*/
	gluLookAt(position.x, position.y, position.z, focus.x, focus.y, focus.z, 0, 1, 0);

	this->mode2d = false;

}
