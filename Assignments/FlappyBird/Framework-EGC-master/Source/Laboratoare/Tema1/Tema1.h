#pragma once

#include <Component/SimpleScene.h>
#include <Core/Engine.h>

class Tema1 : public SimpleScene {
public:
	Tema1();
	~Tema1();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

protected:
	glm::mat3 modelMatrix;
	float translateX, translateY;
	float scaleX, scaleY;
	float angularStep;
	bool isJumping;
	float fBirdPosition;
	float fBirdVelocity;
	float fBirdAcceleration;
	float Gravity;

	float pipes[5];
	bool collision;
	float collisionCircleRadius;
	glm::vec2 collisionCircleCentre;
	int closestPipePair;
	
	int score;
};