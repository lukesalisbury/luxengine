/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <SDL/SDL_opengl.h>
#ifndef M_PI
#define M_PI 3.1415926536
#endif

void gluLookAt(double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz);
static void frustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearval, GLdouble farval);
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

typedef struct {
	double X, Y, Z;
} VECTOR;

class glCamera {
	public:
		VECTOR focus;
		VECTOR position;
	private:
		float Transform[16];
		bool mode2d;
	public:
		glCamera(float x=0.0f, float y=0.0f, float z=0.0f);
		~glCamera();

		void ChangeFocus(float x = 0.0f, float z = 0.0f, bool relative = true, bool move_glCamera = true);
		void ChangePosition(float x = 0.0f, float z = 0.0f);
		void ZoomPosition(float speed = 0.2f);
		void Roll(float angle);
		void Yaw(float angle);
		void Pitch(float angle);

		void Set2DView(int w, int h);
		void Set3DView(int w, int h);
};

#endif
