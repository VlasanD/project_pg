#include "Camera.hpp"

namespace gps {

	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraUpDirection = cameraUp;

		this->cameraFrontDirection = glm::normalize(glm::vec3(cameraTarget - cameraPosition));
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, cameraUp));

		this->originalCameraUpDirection = this->cameraUpDirection;
		this->originalCameraFrontDirection = this->cameraFrontDirection;
	}

	glm::mat4 Camera::getViewMatrix() {
		return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
	}

	void Camera::move(MOVE_DIRECTION direction, float speed) {

		if (direction == MOVE_FORWARD)
			cameraPosition += cameraFrontDirection * speed;

		if (direction == MOVE_BACKWARD)
			cameraPosition -= cameraFrontDirection * speed;

		if (direction == MOVE_UP)
			cameraPosition += glm::vec3(0.0f, 1.0f, 0.0f) * speed;

		if (direction == MOVE_DOWN)
			cameraPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;

		if (direction == MOVE_LEFT)
			cameraPosition -= cameraRightDirection * speed;

		if (direction == MOVE_RIGHT)
			cameraPosition += cameraRightDirection * speed;

		cameraTarget = cameraPosition + cameraFrontDirection;
	}

	glm::mat3 E(float h, float p, float r)
	{
		return glm::rotate(glm::mat4(1.0f), glm::radians(h), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), glm::radians(p), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void Camera::rotate(float pitch, float yaw) {
		cameraFrontDirection = glm::normalize(E(yaw, pitch, 0.0f) * originalCameraFrontDirection);
		cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, originalCameraUpDirection));
		cameraUpDirection = glm::cross(cameraRightDirection, cameraFrontDirection);
		cameraTarget = cameraPosition + cameraFrontDirection;
	}

	glm::vec3 Camera::getCameraPosition() const { return this->cameraPosition; }
	glm::vec3 Camera::getCameraTarget() const { return this->cameraTarget; }
	glm::vec3 Camera::getCameraFrontDirection() const { return this->cameraFrontDirection; }
	glm::vec3 Camera::getCameraRightDirection() const { return this->cameraRightDirection; }
	glm::vec3 Camera::getCameraUpDirection() const { return this->cameraUpDirection; }

	glm::vec3 Camera::getOriginalCameraUpDirection() const { return this->originalCameraUpDirection; }
	glm::vec3 Camera::getOriginalCameraFrontDirection()const { return this->originalCameraFrontDirection; }


	void Camera::setCamera(
		glm::vec3 cameraPosition,
		glm::vec3 cameraTarget,
		glm::vec3 cameraFrontDirection,
		glm::vec3 cameraRightDirection,
		glm::vec3 cameraUpDirection,
		glm::vec3 originalCameraUpDirection,
		glm::vec3 originalCameraFrontDirection)
	{
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraFrontDirection = cameraFrontDirection;

		this->cameraRightDirection = cameraRightDirection;
		this->cameraUpDirection = cameraUpDirection;

		this->originalCameraUpDirection = originalCameraUpDirection;
		this->originalCameraFrontDirection = originalCameraFrontDirection;
	}
}