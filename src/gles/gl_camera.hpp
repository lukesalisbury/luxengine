#ifndef GL_CAMERA_HPP
#define GL_CAMERA_HPP

#include "gles.hpp"
#include "gl_platform.hpp"

#ifndef M_PI
#define M_PI 3.1415926536
#endif

void gluLookAt(double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz);
void frustum(double left, double right, double bottom, double top, double nearval, double farval);
void gluPerspective(double fovy, double aspect, double zNear, double zFar);


class glCamera {
	public:
		LuxVertex focus;
		LuxVertex position;
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

#endif // GL_CAMERA_HPP
