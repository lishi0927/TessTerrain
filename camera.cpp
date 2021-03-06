#include "camera.h"
// Include GLFW
#include <glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
	pos.x = pos.y = pos.z = 0;
	dir.x = dir.y = 0;
	dir.z = -1;
	up.x = up.z = 0;
	up.y = 1;
	horizontal = glm::normalize(glm::cross(dir, up));
	speed.x = speed.z = 50;
	speed.y = 0;
	yaw = 0;
	pitch = 0;
	rotateCoe = 0.25;
	modified = false;
}

Camera::~Camera() {}

void Camera::setPos(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

glm::vec3 Camera::getPos() const {
	return glm::vec3(pos);
}

glm::vec3 Camera::getUp() const {
	return glm::vec3(up);
}

glm::vec3 Camera::getHorizontal() const {
	return glm::vec3(horizontal);
}

glm::vec3 Camera::getDir() const {
	return glm::vec3(dir);
}

void Camera::move(bool keys[], float deltaTime, int width, int height) {
	glm::vec3 nPos(pos);
	speed *= deltaTime;
	if (keys[GLFW_KEY_W])
		pos += speed * dir;
	if (keys[GLFW_KEY_S])
		pos -= speed * dir;
	if (keys[GLFW_KEY_A])
		pos -= speed * horizontal;
	if (keys[GLFW_KEY_D])
		pos += speed * horizontal;
	if (keys[GLFW_KEY_SPACE])
		pos.y += speed.x * 10;
	if (keys[GLFW_KEY_C])
		pos.y -= speed.x * 10;
	speed /= deltaTime;
	if (pos.x > width || pos.x < 0 || pos.z > height || pos.z < 0)
		pos = nPos;
	//	static float remainX = 0, remainY = 0;
}

void Camera::rotate(float xpos, float ypos) {
	yaw = xpos;
	pitch -= ypos;
	modified = true;
}

float Camera::getPitch() const {
	return pitch;
}

float Camera::getYaw() const {
	return yaw;
}

void Camera::revise() {
	if (modified) {
		yaw /= 4;
		pitch = (pitch > 4 * 89) ? 4 * 89 : pitch;
		pitch = (pitch < -4 * 89) ? -4 * 89 : pitch;
		pitch /= 4;
		modified = false;

		yaw += 90;

		dir.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		dir.y = sin(glm::radians(pitch));
		dir.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		up.x = -sin(glm::radians(pitch)) * cos(glm::radians(yaw));
		up.y = cos(glm::radians(pitch));
		up.z = -sin(glm::radians(pitch)) * sin(glm::radians(yaw));

		horizontal = normalize(cross(dir, up));
		pitch *= 4;
	}
}

void Camera::setGrid(int grid) {
	this->grid = 1.0f / (float)grid;
}