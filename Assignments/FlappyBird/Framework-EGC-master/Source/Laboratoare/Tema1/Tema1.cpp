#include "Tema1.h"
#include "Object.h"
#include "Transform2D.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>


using namespace std;

Tema1::Tema1() {

}

Tema1::~Tema1() {

}

void Tema1::Init() {

	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);


	translateX = 0;
	translateY = 0;

	scaleX = 1;
	scaleY = 1;

	angularStep = 0;

	fBirdAcceleration = 0.0f;
	fBirdPosition = 500.0f;
	fBirdVelocity = 0.0f;

	closestPipePair = 0;
	collision = false;
	collisionCircleRadius = 26;

	Gravity = 200.0f;
	isJumping = false;

	int score = 0;

	for (int i = 0; i < 5; i++) {
		pipes[i] = resolution.x - i * 267;
	}

	glm::vec3 corner = glm::vec3(0, 0, 0);

	Mesh* body = Object::CreateCircle("body", corner, 27, 1000, glm::vec3(1, 1, 0), true);
	AddMeshToList(body);

	Mesh* eye = Object::CreateCircle("eye", corner, 10, 1000, glm::vec3(1, 1, 1), true);
	AddMeshToList(eye);

	Mesh* pupil = Object::CreateCircle("pupil", corner, 5, 1000, glm::vec3(0, 0, 0), true);
	AddMeshToList(pupil);

	Mesh* wing = Object::CreateTriangle("wing", corner, 20, glm::vec3(1, 0, 0), true);
	AddMeshToList(wing);

	Mesh* obstacle = Object::CreateRectangle("obstacle", corner, 55, 450, glm::vec3(0, 1, 1), true);
	AddMeshToList(obstacle);

}

void Tema1::FrameStart() {
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds) {
	collision = false;

	glm::ivec2 resolution = window->GetResolution();

	modelMatrix = glm::mat3(1);
	translateX = 160;
	translateY = fBirdPosition + 15;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["pupil"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	translateX = 160;
	translateY = fBirdPosition + 15;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["eye"], shaders["VertexColor"], modelMatrix);

	translateX = 150;
	translateY = fBirdPosition; 
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(translateX, translateY); 
	RenderMesh2D(meshes["body"], shaders["VertexColor"], modelMatrix);

	collisionCircleCentre.x = 150;
	collisionCircleCentre.y = fBirdPosition;

	modelMatrix = glm::mat3(1);
	angularStep = 48;
	translateX = 170;
	translateY = fBirdPosition + 16;
	scaleX = 0.7f;
	scaleY = 0.7f;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	modelMatrix *= Transform2D::Rotate(angularStep);
	modelMatrix *= Transform2D::Scale(scaleX, scaleX);
	RenderMesh2D(meshes["wing"], shaders["VertexColor"], modelMatrix);
	// wing flap

	if (fBirdVelocity < 0) {
		
		modelMatrix = glm::mat3(1);
		angularStep = 90;
		translateX = 151;
		translateY = fBirdPosition - 12;
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		modelMatrix *= Transform2D::Rotate(angularStep);
		RenderMesh2D(meshes["wing"], shaders["VertexColor"], modelMatrix);
	}
	else {
		modelMatrix = glm::mat3(1);
		angularStep = 45;
		translateX = 151;
		translateY = fBirdPosition - 12;
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		modelMatrix *= Transform2D::Rotate(angularStep);
		RenderMesh2D(meshes["wing"], shaders["VertexColor"], modelMatrix);
	}
	// jumping physics

	if (isJumping && fBirdVelocity >= Gravity / 10.0f) {
		fBirdAcceleration = 0.0f;
		fBirdVelocity = -1.2f*Gravity / 2.0f;
	}
	else {
		fBirdAcceleration += Gravity * deltaTimeSeconds;
	}

	if (fBirdAcceleration >= Gravity) {
		fBirdAcceleration = Gravity;
	}

	fBirdVelocity += fBirdAcceleration * deltaTimeSeconds;
	fBirdPosition -= fBirdVelocity * deltaTimeSeconds;

	glm::vec2 upperPipeCorner;
	glm::vec2 lowerPipeCorner;
	// draw pipes

	for (int i = 0; i < 5; i++) {
		// compute closest pipe pair corners for collisions
		if (closestPipePair == 0) {
			upperPipeCorner.x = pipes[4 - closestPipePair];
			upperPipeCorner.y = 400;
			lowerPipeCorner.x = pipes[4 - closestPipePair];
			lowerPipeCorner.y = upperPipeCorner.y - 200;
		}
		else if (closestPipePair == 1) {
			upperPipeCorner.x = pipes[4 - closestPipePair];
			upperPipeCorner.y = 390;
			lowerPipeCorner.x = pipes[4 - closestPipePair];
			lowerPipeCorner.y = upperPipeCorner.y - 200;
		}
		else if (closestPipePair == 2) {
			upperPipeCorner.x = pipes[4 - closestPipePair];
			upperPipeCorner.y = 500;
			lowerPipeCorner.x = pipes[4 - closestPipePair];
			lowerPipeCorner.y = upperPipeCorner.y - 200;
		}
		else if (closestPipePair == 3) {
			upperPipeCorner.x = pipes[4 - closestPipePair];
			upperPipeCorner.y = 390;
			lowerPipeCorner.x = pipes[4 - closestPipePair];
			lowerPipeCorner.y = upperPipeCorner.y - 200;
		}
		else {
			upperPipeCorner.x = pipes[4 - closestPipePair];
			upperPipeCorner.y = 300;
			lowerPipeCorner.x = pipes[4 - closestPipePair];
			lowerPipeCorner.y = upperPipeCorner.y - 200;
		}
		
	
		if (pipes[i] < -55) {
			pipes[i] = resolution.x;
			closestPipePair++;

			if (closestPipePair == 5) {
				closestPipePair = 0;
			}
			score++;
		}
		
		if (i == 0) {
			modelMatrix = glm::mat3(1);
			translateY = 300;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			translateY = -350;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

		} 
		else if(i == 1){
			modelMatrix = glm::mat3(1);
			translateY = 390;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			translateY = -260;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
		}
		else if (i == 2) {
			modelMatrix = glm::mat3(1);
			translateY = 500;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			translateY = -150;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
		}
		else if (i == 3) {
			modelMatrix = glm::mat3(1);
			translateY = 390;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			translateY = -260;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
		}
		else {
			modelMatrix = glm::mat3(1);
			translateY = 400;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			translateY = -250;
			modelMatrix *= Transform2D::Translate(pipes[i], translateY);
			RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
		}

		pipes[i]-= 1.5f;
	}
	
	// collision check
	if (collisionCircleCentre.x < upperPipeCorner.x) {
		if ((collisionCircleCentre.y > upperPipeCorner.y ||
			collisionCircleCentre.y < lowerPipeCorner.y) &&
			collisionCircleCentre.x + collisionCircleRadius > upperPipeCorner.x) {
			collision = true;
		}
	}
	else if (collisionCircleCentre.x <= upperPipeCorner.x + 55) {
		if (collisionCircleCentre.y + collisionCircleRadius > upperPipeCorner.y ||
			collisionCircleCentre.y - collisionCircleRadius < lowerPipeCorner.y) {
			collision = true;
		}
	} 
	
	if (collision == true) {
		cout << "Game Over:  Scorul final este: " << score << endl;
		exit(1);
	}

}

void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	// add user input event
}

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_SPACE) {
		isJumping = true;
	}
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
	if (key == GLFW_KEY_SPACE) {
		isJumping = false;
	}
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}


