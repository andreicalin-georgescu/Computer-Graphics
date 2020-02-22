#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include "LabCamera.h"
#include <Core/GPU/Mesh.h>
#include <vector>

class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;
	Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));
	Mesh* CreateCylinder(std::string name, glm::vec3 center, float radius, float length, int verticesNo, glm::vec3 color);

	void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	bool intersect(glm::vec4 obstaclePos);

	glm::vec3 lightPosition;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;

	Tema::Camera* camera;
	Tema::Camera* firstpersonCamera;
	Tema::Camera* thirdpersonCamera;

	glm::mat4 projectionMatrix;
	bool renderCameraTarget;

	int lives = 3;
	float yMovement = 0;

	float angularStep = 0.f;
	float randomRotation = rand();
	float randomSpin = rand();

	float travelSpeed1 = 0;
	float startingX1 = 20;

	float startingX2 = 18;
	float travelSpeed2 = travelSpeed1;

	float startingX3 = 22;
	float travelSpeed3 = travelSpeed1;

	float startingX4 = 20;
	float travelSpeed4 = travelSpeed1;

	bool collided1 = false;
	bool collided2 = false;
	bool collided3 = false;
	bool collided4 = false;

	float fallingFactorY = 0;
	float fallingFactorX = 0;

	glm::vec4 minCorner = glm::vec4(-1, -0.3, -1, 1);
	glm::vec4 maxCorner = glm::vec4(1, 1, 1, 1);

	glm::vec4 obstacle1Pos = glm::vec4(0, 0, 0, 1);
	glm::vec4 obstacle2Pos = glm::vec4(0, 0, 0, 1);
	glm::vec4 obstacle3Pos = glm::vec4(0, 0, 0, 1);
	glm::vec4 obstacle4Pos = glm::vec4(0, 0, 0, 1);

	float startingFuel1 = 20;
	bool collidedFuel1 = false;
	glm::vec4 fuel1Pos = glm::vec4(0, 0, 0, 1);
	float fuelSpeed1 = 0;

	float startingFuel2 = 20.7;
	bool collidedFuel2 = false;
	glm::vec4 fuel2Pos = glm::vec4(0, 0, 0, 1);
	float fuelSpeed2 = 0;

	float startingFuel3 = 21.4;
	bool collidedFuel3 = false;
	glm::vec4 fuel3Pos = glm::vec4(0, 0, 0, 1);
	float fuelSpeed3 = 0;

	float startingFuel4 = 22.1;
	bool collidedFuel4 = false;
	glm::vec4 fuel4Pos = glm::vec4(0, 0, 0, 1);
	float fuelSpeed4 = 0;

	float startingFuel5 = 22.8;
	bool collidedFuel5 = false;
	glm::vec4 fuel5Pos = glm::vec4(0, 0, 0, 1);
	float fuelSpeed5 = 0;

	float startingFuel6 = 23.5;
	bool collidedFuel6 = false;
	glm::vec4 fuel6Pos = glm::vec4(0, 0, 0, 1);
	float fuelSpeed6 = 0;

	float startingFuel7 = 24.2;
	bool collidedFuel7 = false;
	glm::vec4 fuel7Pos = glm::vec4(0, 0, 0, 1);
	float fuelSpeed7 = 0;

	float fuelDecay = 0;
	float currentFuel;
	

};
