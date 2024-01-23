//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp" 


#include <iostream>


int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;

// view mode
bool wireframeView = false;
bool pointframeView = false;

// fog 
bool fog = false;
GLfloat fogDensityLoc;
float fogDensity;

// snow
struct Snowflake {
	float x;        // X position of the snowflake
	float y;        // Y position of the snowflake
	float z;        // Z position of the snowflake
	float velocity; // Speed factor or velocity of the snowflake in the Y direction
	float scale;    // Scaling factor for the snowflake

	Snowflake(float _x, float _y, float _z, float _velocity, float _scale)
		: x(_x), y(_y), z(_z), velocity(_velocity), scale(_scale) {}
};
std::vector<Snowflake> snowflakes;
float snow = false;


glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

//point light
bool lowLight = false;
glm::vec3 pointLight = glm::vec3(-14.4391f, 0.329104f, 2.67753f);
GLuint pointLightLoc;
glm::vec3 pointLightColor = glm::vec3(1.0f, 1.0f, 0.929f);
GLuint pointLightColorLoc;
GLuint intensityLoc;
float intensity = 5.0f;
bool rising = true;

glm::vec3 pointLight1 = glm::vec3(14.7718f, 0.141703f, 14.8373f);
GLuint pointLightLoc1;
glm::vec3 pointLightColor1 = glm::vec3(1.0f, 1.0f, 0.929f);
GLuint pointLightColorLoc1;

glm::vec3 pointLight2 = glm::vec3(-2.52071f, 0.141703f, 23.575f);
GLuint pointLightLoc2;
glm::vec3 pointLightColor2 = glm::vec3(1.0f, 1.0f, 0.929f);
GLuint pointLightColorLoc2;

glm::vec3 pointLight3 = glm::vec3(-6.65209f, 0.141703f, 11.7328f);
GLuint pointLightLoc3;
glm::vec3 pointLightColor3 = glm::vec3(1.0f, 1.0f, 0.929f);
GLuint pointLightColorLoc3;

glm::vec3 pointLight4 = glm::vec3(1.97752f, 0.141703f, 5.39967f);
GLuint pointLightLoc4;
glm::vec3 pointLightColor4 = glm::vec3(1.0f, 1.0f, 0.929f);
GLuint pointLightColorLoc4;

glm::vec3 pointLight5 = glm::vec3(7.71059f, 0.141703f, 5.51511f);
GLuint pointLightLoc5;
glm::vec3 pointLightColor5 = glm::vec3(1.0f, 1.0f, 0.929f);
GLuint pointLightColorLoc5;

glm::vec3 pointLight6 = glm::vec3(7.71059f, 0.141703f, 5.51511f);
GLuint pointLightLoc6;
glm::vec3 pointLightColor6 = glm::vec3(1.0f, 1.0f, 0.929f);
GLuint pointLightColorLoc6;

int night;
GLuint nightLoc;

gps::Camera myCamera(
	glm::vec3(0.0f, 2.0f, 5.5f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.1f;
float lastX = 0, lastY = 0;
float deltaX;
float deltaY;
float pitch;
float yaw;
bool firstTimeInFrame = true;
float sensitivity = 0.05f;
float cameraAnimation = false;

std::ifstream inputFile;
std::string filePath = "presentation.txt";

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D snowflake;
gps::Model3D cube;
gps::Model3D terrain1;
gps::Model3D wings;
gps::Model3D ship;
gps::Model3D cannon;
gps::Model3D fireball;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

//animation
float amountToRotate = 0.0f;
glm::vec3 wingsCoord = glm::vec3(16.0449, 7.54809, -0.131407);

glm::vec3 boatCoord = glm::vec3(-34.315f, 0.313049f, -63.5075f);
glm::vec3 boatDest = glm::vec3(8.53091f, 0.313049f, -17.7294f);

glm::vec3 boatDirection = boatDest - boatCoord;
float amount = 0.1f;
float sign = 1.0f;

glm::vec3 fireballInitialCoord = glm::vec3(14.1472f, 1.11067f, -9.23627f);
glm::vec3 fireballCurrentPosition = glm::vec3(14.1472f, 1.11067f, -9.23627f);
glm::vec3 fireballDirection;
float ammountForFireball = 0.001f;
int shoot = 1;


GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

bool CheckCollision(const gps::Model3D& model1, const gps::Model3D& model2) {
	gps::BoundingBox bbox1 = model1.GetBoundingBox();
	gps::BoundingBox bbox2 = model2.GetBoundingBox();

	// Check for collision on x, y and z axes
	bool collisionX = bbox1.maxCoords.x >= bbox2.minCoords.x && bbox2.maxCoords.x >= bbox1.minCoords.x;
	bool collisionY = bbox1.maxCoords.y >= bbox2.minCoords.y && bbox2.maxCoords.y >= bbox1.minCoords.y;
	bool collisionZ = bbox1.maxCoords.z >= bbox2.minCoords.z && bbox2.maxCoords.z >= bbox1.minCoords.z;

	// If collisions occurred on all three axes, then the bounding boxes are intersecting
	return collisionX && collisionY && collisionZ;
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key == GLFW_KEY_O && action == GLFW_PRESS) {

		wireframeView = !wireframeView;

		if (wireframeView) {

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		fog = !fog;

		fogDensity;

		if (fog)
			fogDensity = 0.01f;
		else
			fogDensity = 0.0f;

	}
	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		snow = !snow;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {

		pointframeView = !pointframeView;

		if (pointframeView) {

			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else {

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		lowLight = !lowLight;
		std::vector<const GLchar*> faces;
		if (!lowLight) {
			faces.push_back("skybox/day/right.tga");
			faces.push_back("skybox/day/left.tga");
			faces.push_back("skybox/day/top.tga");
			faces.push_back("skybox/day/bottom.tga");
			faces.push_back("skybox/day/back.tga");
			faces.push_back("skybox/day/front.tga");
		}
		else {
			faces.push_back("skybox/night/right.tga");
			faces.push_back("skybox/night/left.tga");
			faces.push_back("skybox/night/top.tga");
			faces.push_back("skybox/night/bottom.tga");
			faces.push_back("skybox/night/back.tga");
			faces.push_back("skybox/night/front.tga");
		}
		mySkyBox.Load(faces);

		faces.clear();
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		cameraAnimation = !cameraAnimation;
	}

	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		shoot = 1;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void clampAngle(float& angle, float lower, float upper)
{
	angle = angle > upper ? upper : angle;
	angle = angle < lower ? lower : angle;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstTimeInFrame)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstTimeInFrame = false;
	}
	deltaX = (float)xpos - lastX;
	deltaY = (float)ypos - lastY;

	lastX = (float)xpos;
	lastY = (float)ypos;

	yaw -= deltaX * sensitivity;
	pitch -= deltaY * sensitivity;

	clampAngle(pitch, -70.0f, 89.0f);

	myCamera.rotate(pitch, yaw);
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_SPACE]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}

	amountToRotate += 0.001f;

	amount += (sign * 0.001f);
	if (amount > 1.0f || amount < 0.0f) {
		sign *= (-1.0f);
	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void generateSnowflakes() {
	for (int i = 0; i < 450; i++) {
		float x = static_cast<float>(rand() % 455 - 275);
		float z = static_cast<float>(rand() % 455 - 275);
		float velocity = (static_cast<float>(rand() % 45 + 5) / 100.0f) + 0.01f;
		float scale = static_cast<float>(rand() % 50);

		snowflakes.push_back(Snowflake(x, 100.0f, z, velocity, scale));
	}
}

void initObjects() {

	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");

	snowflake.LoadModel("objects/snowflake/snowflake.obj");

	terrain1.LoadModel("objects/terrain1/terrain1.obj");

	wings.LoadModel("objects/terrain1/wings.obj");

	ship.LoadModel("objects/terrain1/ship.obj");

	cannon.LoadModel("objects/terrain1/cannon.obj");

	fireball.LoadModel("objects/terrain1/fireball.obj");

	generateSnowflakes();
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(10.0f, 10.0f, 10.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	if (lowLight) {
		lightColor = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	else {
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	}
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//point coord
	pointLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight");
	glUniform3fv(pointLightLoc, 1, glm::value_ptr(pointLight));

	//set point light color
	pointLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor");
	glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));

	//point coord
	pointLightLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight1");
	glUniform3fv(pointLightLoc1, 1, glm::value_ptr(pointLight1));

	//set point light color
	pointLightColorLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor1");
	glUniform3fv(pointLightColorLoc1, 1, glm::value_ptr(pointLightColor1));

	//point coord
	pointLightLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight2");
	glUniform3fv(pointLightLoc2, 1, glm::value_ptr(pointLight2));

	//set point light color
	pointLightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor2");
	glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor2));

	//point coord
	pointLightLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight3");
	glUniform3fv(pointLightLoc3, 1, glm::value_ptr(pointLight3));

	//set point light color
	pointLightColorLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor3");
	glUniform3fv(pointLightColorLoc3, 1, glm::value_ptr(pointLightColor3));

	//point coord
	pointLightLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight4");
	glUniform3fv(pointLightLoc4, 1, glm::value_ptr(pointLight4));

	//set point light color
	pointLightColorLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor4");
	glUniform3fv(pointLightColorLoc4, 1, glm::value_ptr(pointLightColor4));

	//point coord
	pointLightLoc5 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight5");
	glUniform3fv(pointLightLoc5, 1, glm::value_ptr(pointLight5));

	//set point light color
	pointLightColorLoc5 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor5");
	glUniform3fv(pointLightColorLoc5, 1, glm::value_ptr(pointLightColor5));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initSkyBox() {
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/day/right.tga");
	faces.push_back("skybox/day/left.tga");
	faces.push_back("skybox/day/top.tga");
	faces.push_back("skybox/day/bottom.tga");
	faces.push_back("skybox/day/back.tga");
	faces.push_back("skybox/day/front.tga");
	mySkyBox.Load(faces);
}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir * 2.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	const GLfloat near_plane = 0.1f, far_plane = 99.0f;
	glm::mat4 lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);

	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void generateSnowfall(gps::Shader shader) {
	for (int i = 0; i < 400; i++) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(snowflakes[i].x, snowflakes[i].y, snowflakes[i].z));
		model = glm::scale(model, glm::vec3(snowflakes[i].scale, snowflakes[i].scale, snowflakes[i].scale));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		snowflake.Draw(shader);

		if (snowflakes[i].y > -5)
			snowflakes[i].y -= snowflakes[i].velocity;
		else
			snowflakes[i].y = 100.0f;
	}
}

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	if (snow) {
		generateSnowfall(shader);
	}

	fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, fogDensity);

	model = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	terrain1.Draw(shader);


	model = glm::mat4(1.0f);
	model = glm::translate(model, boatDirection * amount);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	ship.Draw(shader);


	model = glm::translate(glm::mat4(1.0f), wingsCoord);
	model = glm::rotate(model, glm::radians(360.0f) * amountToRotate, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, -wingsCoord);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	pointLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight");
	glUniform3fv(pointLightLoc, 1, glm::value_ptr(pointLight));

	//set point light color
	pointLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor");
	glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));

	wings.Draw(shader);


	model = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	cannon.Draw(shader);


	if (shoot) {

		fireballDirection = boatCoord + boatDirection * amount - fireballInitialCoord;

		ammountForFireball += 0.002f;

		model = glm::translate(glm::mat4(1.0f), fireballDirection * ammountForFireball);

		fireballCurrentPosition = fireballInitialCoord + fireballDirection * ammountForFireball;

		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}

		fireball.Draw(shader);

		myCustomShader.useShaderProgram();

		if (ammountForFireball > 1.0f) {
			shoot = 0;
			ammountForFireball = 0.0f;
		}

		pointLight6 = fireballCurrentPosition;
		pointLightLoc6 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight6");
		glUniform3fv(pointLightLoc6, 1, glm::value_ptr(pointLight6));


		pointLightColor6 = shoot ? glm::vec3(1.0f, 1.0f, 0.929f) : glm::vec3(0.0f, 0.0f, 0.f);
		//set point light color
		pointLightColorLoc6 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor6");
		glUniform3fv(pointLightColorLoc6, 1, glm::value_ptr(pointLightColor6));

	}

}

int readCoordsAndAnimate();

void changeLightIntensity() {
	float change = 0.1f;
	if (rising && intensity > 25.0f) {
		rising = false;
		intensity -= change;
	}
	else if (!rising && intensity < 0.1f) {
		rising = true;
		intensity += change;
	}
	else if (rising) {
		intensity += change;
	}
	else {
		intensity -= change;
	}
	myCustomShader.useShaderProgram();
	intensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "intensity");
	glUniform1f(intensityLoc, intensity);
}

void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map

	myCustomShader.useShaderProgram();
	if (lowLight) {
		lightColor = glm::vec3(0.0f, 0.0f, 0.0f);
		night = 1;
	}
	else {
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
		night = 0;
	}
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	nightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "night");
	glUniform1i(nightLoc, night);

	// render depth map on screen - toggled with the M key
	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		if (cameraAnimation) {
			readCoordsAndAnimate();
		}

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		if (!lowLight) {
			//bind the shadow map
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, depthMapTexture);
			glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
		}
		else {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		// draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		/*lightCube.Draw(lightShader);*/

		// skybox

		mySkyBox.Draw(skyboxShader, view, projection);

		changeLightIntensity();
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int writeCoords() {

	std::ofstream outputFile(filePath, std::ios::app);

	if (!outputFile.is_open()) {
		return 1;
	}
	outputFile
		<< myCamera.getCameraPosition().x << std::endl
		<< myCamera.getCameraPosition().y << std::endl
		<< myCamera.getCameraPosition().z << std::endl
		<< myCamera.getCameraTarget().x << std::endl
		<< myCamera.getCameraTarget().y << std::endl
		<< myCamera.getCameraTarget().z << std::endl
		<< myCamera.getCameraFrontDirection().x << std::endl
		<< myCamera.getCameraFrontDirection().y << std::endl
		<< myCamera.getCameraFrontDirection().z << std::endl
		<< myCamera.getCameraRightDirection().x << std::endl
		<< myCamera.getCameraRightDirection().y << std::endl
		<< myCamera.getCameraRightDirection().z << std::endl
		<< myCamera.getCameraUpDirection().x << std::endl
		<< myCamera.getCameraUpDirection().y << std::endl
		<< myCamera.getCameraUpDirection().z << std::endl
		<< myCamera.getOriginalCameraUpDirection().x << std::endl
		<< myCamera.getOriginalCameraUpDirection().y << std::endl
		<< myCamera.getOriginalCameraUpDirection().z << std::endl
		<< myCamera.getOriginalCameraFrontDirection().x << std::endl
		<< myCamera.getOriginalCameraFrontDirection().y << std::endl
		<< myCamera.getOriginalCameraFrontDirection().z << std::endl;
	outputFile.close();

	return 0;
}

int readCoordsAndAnimate() {
	glm::vec3 cameraPosition;
	glm::vec3 cameraTarget;
	glm::vec3 cameraFrontDirection;
	glm::vec3 cameraRightDirection;
	glm::vec3 cameraUpDirection;

	glm::vec3 originalCameraUpDirection;
	glm::vec3 originalCameraFrontDirection;


	if (!inputFile.is_open()) {
		inputFile.open(filePath);
	}

	if (!(inputFile
		>> cameraPosition.x
		>> cameraPosition.y
		>> cameraPosition.z
		>> cameraTarget.x
		>> cameraTarget.y
		>> cameraTarget.z
		>> cameraFrontDirection.x
		>> cameraFrontDirection.y
		>> cameraFrontDirection.z
		>> cameraRightDirection.x
		>> cameraRightDirection.y
		>> cameraRightDirection.z
		>> cameraUpDirection.x
		>> cameraUpDirection.y
		>> cameraUpDirection.z
		>> originalCameraUpDirection.x
		>> originalCameraUpDirection.y
		>> originalCameraUpDirection.z
		>> originalCameraFrontDirection.x
		>> originalCameraFrontDirection.y
		>> originalCameraFrontDirection.z)) 
	{
		cameraAnimation = false;
		
		inputFile.close();
	}
	else {
		myCamera.setCamera(
			cameraPosition,
			cameraTarget,
			cameraFrontDirection,
			cameraRightDirection,
			cameraUpDirection,
			originalCameraUpDirection,
			originalCameraFrontDirection
		);
	}

	return 0;
}

int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	initSkyBox();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		//writeCoords();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
