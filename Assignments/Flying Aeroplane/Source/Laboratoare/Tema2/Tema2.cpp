#include "Tema2.h"
#include "Object.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>
#include <Core/GPU/Mesh.h>

using namespace std;
Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}
bool isFirstPerson = false;

void Tema2::Init()
{
	

	renderCameraTarget = false;

	firstpersonCamera = new Tema::Camera();
	firstpersonCamera->Set(glm::vec3(-6.8f + fallingFactorX, 3 + yMovement + fallingFactorY, -6), glm::vec3(1,1,-4.2f), glm::vec3(0,1,0));

	thirdpersonCamera = new Tema::Camera();
	thirdpersonCamera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	camera = new Tema::Camera();
	camera = thirdpersonCamera;

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* cylinder = CreateCylinder("cylinder", glm::vec3(0), 5, 12, 30, glm::vec3(1, 1, 1));
		AddMeshToList(cylinder);
	}

	{
		Mesh* life = Object::CreateCircle("life", glm::vec3(0), 0.5f, 30, glm::vec3(1, 1, 1), true);
		AddMeshToList(life);
	}

	{
		Mesh* remainingFuel = Object::CreateRectangle("remainingFuel", glm::vec3(0), 5, 1, glm::vec3(1, 1, 1), true);
		AddMeshToList(remainingFuel);
	}

	{
		Mesh* fuelBar = Object::CreateRectangle("fuelBar", glm::vec3(0), 5, 1, glm::vec3(0, 0, 0), false);
		AddMeshToList(fuelBar);
	}
	{
		Mesh* mesh = new Mesh("obstacle");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("fuel");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "oildrum.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

	// Create a simple cube
	{
		vector<VertexFormat> vertices
		{
			
			VertexFormat(glm::vec3(-1, -0.3,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
			VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
			VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),

			VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
			VertexFormat(glm::vec3(-1, -0.3, -1), glm::vec3(1, 1, 1), glm::vec3(0.3, 0.5, 0.4)),
			VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.2, 0.9)),

			VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
			VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
		};

		vector<unsigned short> indices =
		{
			0, 1, 2,		1, 3, 2,
			2, 3, 7,		2, 7, 6,
			1, 7, 3,		1, 5, 7,
			6, 7, 4,		7, 5, 4,
			0, 4, 1,		1, 4, 5,
			2, 6, 4,		0, 2, 4,
		};

		CreateMesh("cube", vertices, indices);
	} 

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("Tema2Shader");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(0, 3, 3);
		materialShininess = 50;
		materialKd = 0.5;
		materialKs = 0.5;
	}
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Create the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	// Set light position uniform
	GLint locLightPos = glGetUniformLocation(shader->program, "light_position");
	glUniform3fv(locLightPos, 1, glm::value_ptr(lightPosition));

	// Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	GLint locEyePos = glGetUniformLocation(shader->program, "eye_position");
	glUniform3fv(locEyePos, 1, glm::value_ptr(eyePosition));

	// Set material property uniforms (shininess, kd, ks, object color) 
	GLint locMaterial = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(locMaterial, materialShininess);

	GLint locMaterialKd = glGetUniformLocation(shader->program, "material_kd");  // diffuse light
	glUniform1f(locMaterialKd, materialKd);

	GLint locMaterialKs = glGetUniformLocation(shader->program, "material_ks");  // specular light
	glUniform1f(locMaterialKs, materialKs);

	GLint locObject = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(locObject, 1, glm::value_ptr(color));

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(1.204f, 0.859f, 0.929f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
	firstpersonCamera->Set(glm::vec3(-6.8f + fallingFactorX, 3 + yMovement + fallingFactorY, -6), glm::vec3(1, 1 + RADIANS(2 * yMovement), -4.2f), glm::vec3(0, 1, 0));
	//  marea
	{
		if (camera == thirdpersonCamera) {
			glm::mat4 modelCilindru = glm::mat4(1);
			modelCilindru = glm::translate(modelCilindru, glm::vec3(0, -5.7f, -3));
			modelCilindru = glm::rotate(modelCilindru, RADIANS(angularStep * 2), glm::vec3(0, 0, 1));
			
			RenderSimpleMesh(meshes["cylinder"], shaders["Tema2Shader"], modelCilindru, glm::vec3(0.26f, 0.6313f, 0.886f));
			modelCilindru = glm::mat4(1);
		}
		else {
			glm::mat4 modelCilindru = glm::mat4(1);
			modelCilindru = glm::translate(modelCilindru, glm::vec3(0, -5.7f, -10));
			modelCilindru = glm::rotate(modelCilindru, RADIANS(angularStep * 2), glm::vec3(0, 0, 1));

			RenderSimpleMesh(meshes["cylinder"], shaders["Tema2Shader"], modelCilindru, glm::vec3(0.26f, 0.6313f, 0.886f));
			modelCilindru = glm::mat4(1);
		}
	}
	if (yMovement < -2.9) {
		yMovement = -2.9;
	}
	glm::mat4 modelMatrix = glm::mat4(1);

	// vieti
	{
		glm::mat4 modelLives = glm::mat4(1);
		if (lives != 0 && (1 - fuelDecay) > 0) {
		
			if (camera == thirdpersonCamera) {
				for (int i = 1; i <= lives; ++i) {
					modelLives = glm::translate(modelLives, glm::vec3(-19 + 2 * i, 6, -17));
					RenderSimpleMesh(meshes["life"], shaders["Tema2Shader"], modelLives, glm::vec3(2, 0.5f, 0.3f));
					modelLives = glm::mat4(1);
				}
			}
			else {
				for (int i = 1; i <= lives; ++i) {
					modelLives = glm::translate(modelLives, glm::vec3(19 - 2 * i, 5, -17));
					RenderSimpleMesh(meshes["life"], shaders["Tema2Shader"], modelLives, glm::vec3(2, 0.5f, 0.3f));
					modelLives = glm::mat4(1);
				}
			}
		
		}
		else {
			fallingFactorY -= deltaTimeSeconds * 10;
			fallingFactorX += deltaTimeSeconds * 5;

			currentFuel = 0;
			fuelDecay = 1;
			// game over animation

			if (2 + yMovement + fallingFactorY < -40) {
				exit(1);
			}
		}

	}

	// motor
	{
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-5 + fallingFactorX, 2 + yMovement + fallingFactorY, -6));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f, 0.85f, 0.7f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(2 * yMovement), glm::vec3(0, 0, 1));
		RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelMatrix, glm::vec3(1, 1, 1));
		modelMatrix = glm::mat4(1);
	}

	// corp
	{
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-5.8f + fallingFactorX, 2 + yMovement + fallingFactorY, -6));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f, 0.5f, 0.5f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(2 * yMovement), glm::vec3(0, 0, 1));

		minCorner = modelMatrix * minCorner;
		maxCorner = modelMatrix * maxCorner;

		RenderSimpleMesh(meshes["cube"], shaders["Tema2Shader"], modelMatrix, glm::vec3(1, 0, 0));
		modelMatrix = glm::mat4(1);
	}

	// coada
	{
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-6.5f + fallingFactorX, 2.45f + yMovement + fallingFactorY, -6));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.78f, 0.35f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(2 * yMovement), glm::vec3(0, 0, 1));

		RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelMatrix, glm::vec3(1, 0, 0));
		modelMatrix = glm::mat4(1);
	}

	// aripi 
	{
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-5.6f + fallingFactorX, 2.3f + yMovement + fallingFactorY, -6));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f, 0.2f, 3.5f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(2 * yMovement), glm::vec3(0, 0, 1));

		RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelMatrix, glm::vec3(1, 0, 0));
		modelMatrix = glm::mat4(1);
	}

	// baza elice
	{
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.6f + fallingFactorX, 2 + yMovement + fallingFactorY, -6));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(2 * yMovement), glm::vec3(0, 0, 1));

		RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelMatrix, glm::vec3(0, 0, 0));
		modelMatrix = glm::mat4(1);

	}

	// elice 
	{
		glm::mat4 modelPropeller = glm::mat4(1);
		modelPropeller = glm::translate(modelPropeller, glm::vec3(-4.5f + fallingFactorX, 2 + yMovement + fallingFactorY, -6));
		modelPropeller = glm::rotate(modelPropeller, angularStep * 3, glm::vec3(-1, 0, 0));
		modelPropeller = glm::scale(modelPropeller, glm::vec3(0.1f, 0.175f, 1.3f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(2 * yMovement), glm::vec3(0, 0, 1));

		RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelPropeller, glm::vec3(0, 0, 0));
		modelPropeller = glm::mat4(1);
	}


	// Render the camera target. Useful for understanding where is the rotation point in Third-person camera movement
	if (renderCameraTarget)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}
	angularStep += deltaTimeSeconds * 10;;

	// nori
	glm::mat4 modelClouds = glm::mat4(1);
	modelClouds = glm::translate(modelClouds, glm::vec3(0, 0, 1));
	{
		for (int i = 0; i < 40; i++) {


			if (i % 4 == 0) {
				modelClouds = glm::rotate(modelClouds, RADIANS(randomSpin + angularStep * 5), glm::vec3(0, 0, 1));
				modelClouds = glm::translate(modelClouds, glm::vec3(-2, 30, -26));
				modelClouds = glm::scale(modelClouds, glm::vec3(0.3f));
			}
			else if (i % 4 == 1) {
				modelClouds = glm::rotate(modelClouds, RADIANS(randomSpin + (i * 10) % 20 + angularStep * 5), glm::vec3(0, 0, 1));
				modelClouds = glm::translate(modelClouds, glm::vec3(-2, 20, -26));
				modelClouds = glm::scale(modelClouds, glm::vec3(0.5f));

			}
			else if (i % 4 == 2) {
				modelClouds = glm::rotate(modelClouds, RADIANS(randomSpin + (i * 10) % 30 + angularStep * 5), glm::vec3(0, 0, 1));
				modelClouds = glm::translate(modelClouds, glm::vec3(-2, 39, -26));
				modelClouds = glm::scale(modelClouds, glm::vec3(0.75f));
			}
			else {
				modelClouds = glm::rotate(modelClouds, RADIANS(randomSpin - (i * 30) % 60 + angularStep * 5), glm::vec3(0, 0, 1));
				modelClouds = glm::translate(modelClouds, glm::vec3(-2, 20, -26));
				modelClouds = glm::scale(modelClouds, glm::vec3(0.69f));
			}

			modelClouds = glm::rotate(modelClouds, RADIANS(randomRotation + angularStep * 3), glm::vec3(1, 0, 0));
			RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelClouds, glm::vec3(1, 1, 1));
			modelClouds = glm::rotate(modelClouds, RADIANS(randomRotation + angularStep * 10), glm::vec3(-1, 0, 0));
			modelClouds = glm::translate(modelClouds, glm::vec3(1, 0, 0));
			modelClouds = glm::scale(modelClouds, glm::vec3(1.4f, 1.4f, 1.4f));
			RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelClouds, glm::vec3(1, 1, 1));
			modelClouds = glm::rotate(modelClouds, RADIANS(randomRotation + angularStep * 3), glm::vec3(1, 0, 0));
			modelClouds = glm::translate(modelClouds, glm::vec3(0.8f, 0, 0));
			modelClouds = glm::scale(modelClouds, glm::vec3(0.8f, 0.8f, 0.8f));
			RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelClouds, glm::vec3(1, 1, 1));
			modelClouds = glm::scale(modelClouds, glm::vec3(0.8f, 0.8f, 0.8f));
			modelClouds = glm::translate(modelClouds, glm::vec3(1, 0, 0));
			modelClouds = glm::rotate(modelClouds, RADIANS(randomRotation + angularStep * 2), glm::vec3(1, 0, 0));
			RenderSimpleMesh(meshes["box"], shaders["Tema2Shader"], modelClouds, glm::vec3(1, 1, 1));

			modelClouds = glm::mat4(1);
		}

	}

	// obstacole
	
	glm::mat4 modelObstacle = glm::mat4(1);
	{	
		// first obstacle
		{
			if (startingX1 - travelSpeed1 < -20) {

				startingX1 = 20;
				travelSpeed1 = 0;
				collided1 = false;

		}
		// collision detection
			if (!collided1) {
				modelObstacle = glm::translate(modelObstacle, glm::vec3(startingX1 - travelSpeed1, 1.5f, -6));
				modelObstacle = glm::scale(modelObstacle, glm::vec3(0.6f));
				RenderSimpleMesh(meshes["obstacle"], shaders["Tema2Shader"], modelObstacle, glm::vec3(2, 0.5f, 0.8f));
				obstacle1Pos = modelObstacle * obstacle1Pos;

		}

		if (intersect(obstacle1Pos)) {
			lives--;
			collided1 = true;
		}
		obstacle1Pos = glm::vec4(0, 0, 0, 1);
		modelObstacle = glm::mat4(1);
		}

		// second obstacle
		{
		if (startingX2 - travelSpeed2 < -18) {

			startingX2 = 18;
			travelSpeed2 = 0;
			collided2 = false;

		}
		// collision detection
		if (!collided2) {
			modelObstacle = glm::translate(modelObstacle, glm::vec3(startingX2 - travelSpeed2, 1.f, -6));
			modelObstacle = glm::scale(modelObstacle, glm::vec3(0.6f));
			RenderSimpleMesh(meshes["obstacle"], shaders["Tema2Shader"], modelObstacle, glm::vec3(2, 0.5f, 0.8f));

			obstacle2Pos = modelObstacle * obstacle2Pos;

		}

		if (intersect(obstacle2Pos)) {
			lives--;
			collided2 = true;
		}

		obstacle2Pos = glm::vec4(0, 0, 0, 1);
		modelObstacle = glm::mat4(1);
		}
		//third obstacle
		{

			if (startingX3 - travelSpeed3 < -22) {

				startingX3 = 22;
				travelSpeed3 = 0;
				collided3 = false;

			}
			// collision detection
			if (!collided3) {
				modelObstacle = glm::translate(modelObstacle, glm::vec3(startingX3 - travelSpeed3, 2.f, -6));
				modelObstacle = glm::scale(modelObstacle, glm::vec3(0.6f));
				RenderSimpleMesh(meshes["obstacle"], shaders["Tema2Shader"], modelObstacle, glm::vec3(2, 0.5f, 0.8f));

				obstacle3Pos = modelObstacle * obstacle3Pos;

			}

			if (intersect(obstacle3Pos)) {
				lives--;
				collided3 = true;
			}

			obstacle3Pos = glm::vec4(0, 0, 0, 1);
			modelObstacle = glm::mat4(1);
		}

		// fourth obstacle
		{
			if (startingX4 - travelSpeed4 < -20) {

				startingX4 = 20;
				travelSpeed4 = 0;
				collided4 = false;

			}
			// collision detection
			if (!collided4) {
				modelObstacle = glm::translate(modelObstacle, glm::vec3(startingX4 - travelSpeed4, 0.8f, -6));
				modelObstacle = glm::scale(modelObstacle, glm::vec3(0.6f));
				RenderSimpleMesh(meshes["obstacle"], shaders["Tema2Shader"], modelObstacle, glm::vec3(2, 0.5f, 0.8f));

				obstacle4Pos = modelObstacle * obstacle4Pos;

			}

			if (intersect(obstacle4Pos)) {
				lives--;
				collided4 = true;
			}
			obstacle4Pos = glm::vec4(0, 0, 0, 1);
			modelObstacle = glm::mat4(1);
		}
		travelSpeed1 += deltaTimeSeconds * 5;
		travelSpeed2 += deltaTimeSeconds * 2;
		travelSpeed3 += deltaTimeSeconds * 2.95f;
		travelSpeed4 += deltaTimeSeconds * 6;
	}

	// combustibil
	glm::mat4 modelFuel = glm::mat4(1);
	currentFuel = 1 - fuelDecay;
	{
		// fuel 1 
		{
			if (startingFuel1 - fuelSpeed1 < -20) {
				startingFuel1 = 20;
				fuelSpeed1 = 0;
				collidedFuel1 = false;
			}


			// collision detection
			if (!collidedFuel1) {
				modelFuel = glm::translate(modelFuel, glm::vec3(startingFuel1 - fuelSpeed1, 2, -6));
				modelFuel = glm::scale(modelFuel, glm::vec3(0.5f));
				modelFuel = glm::rotate(modelFuel, RADIANS(angularStep * 4), glm::vec3(0, 1, 1));
				RenderSimpleMesh(meshes["fuel"], shaders["Tema2Shader"], modelFuel, glm::vec3(0.9f, 0.7f, 1.8f));

				fuel1Pos = modelFuel * fuel1Pos;
			}

			if (intersect(fuel1Pos)) {
				fuelDecay -= 0.07f;
				collidedFuel1 = true;
			}
			fuel1Pos = glm::vec4(0, 0, 0, 1);
			modelFuel = glm::mat4(1);
		}

		// fuel 2
		{
			if (startingFuel2 - fuelSpeed2 < -19.3) {
				startingFuel2 = 20.7f;
				fuelSpeed2 = 0;
				collidedFuel2 = false;
			}
		
			// collision detection
			if (!collidedFuel2) {
				modelFuel = glm::translate(modelFuel, glm::vec3(startingFuel2 - fuelSpeed2, 2.2f, -6));
				modelFuel = glm::scale(modelFuel, glm::vec3(0.5f));
				modelFuel = glm::rotate(modelFuel, RADIANS(angularStep * 4), glm::vec3(0, 1, 1));
				RenderSimpleMesh(meshes["fuel"], shaders["Tema2Shader"], modelFuel, glm::vec3(0.9f, 0.7f, 1.8f));

				fuel2Pos = modelFuel * fuel2Pos;
			}

			if (intersect(fuel2Pos)) {
				fuelDecay -= 0.07f;
				collidedFuel2 = true;
			}
			fuel2Pos = glm::vec4(0, 0, 0, 1);
			modelFuel = glm::mat4(1);
		}

		// fuel 3
		{
			if (startingFuel3 - fuelSpeed3 < -18.7) {
				startingFuel3 = 21.4f;
				fuelSpeed3 = 0;
				collidedFuel3 = false;
			}

			// collision detection
			if (!collidedFuel3) {
				modelFuel = glm::translate(modelFuel, glm::vec3(startingFuel3 - fuelSpeed3, 2.4f, -6));
				modelFuel = glm::scale(modelFuel, glm::vec3(0.5f));
				modelFuel = glm::rotate(modelFuel, RADIANS(angularStep * 4), glm::vec3(0, 1, 1));
				RenderSimpleMesh(meshes["fuel"], shaders["Tema2Shader"], modelFuel, glm::vec3(0.9f, 0.7f, 1.8f));

				fuel3Pos = modelFuel * fuel2Pos;
			}

			if (intersect(fuel3Pos)) {
				fuelDecay -= 0.07f;
				collidedFuel3 = true;
			}
			fuel3Pos = glm::vec4(0, 0, 0, 1);
			modelFuel = glm::mat4(1);
		}

		// fuel 4
		{
			if (startingFuel4 - fuelSpeed4 < -18) {
				startingFuel4 = 22.1f;
				fuelSpeed4 = 0;
				collidedFuel4 = false;
			}

			// collision detection
			if (!collidedFuel4) {
				modelFuel = glm::translate(modelFuel, glm::vec3(startingFuel4 - fuelSpeed4, 2.4f, -6));
				modelFuel = glm::scale(modelFuel, glm::vec3(0.5f));
				modelFuel = glm::rotate(modelFuel, RADIANS(angularStep * 4), glm::vec3(0, 1, 1));
				RenderSimpleMesh(meshes["fuel"], shaders["Tema2Shader"], modelFuel, glm::vec3(0.9f, 0.7f, 1.8f));

				fuel4Pos = modelFuel * fuel4Pos;
			}

			if (intersect(fuel4Pos)) {
				fuelDecay -= 0.07f;
				collidedFuel4 = true;
			}
			fuel4Pos = glm::vec4(0, 0, 0, 1);
			modelFuel = glm::mat4(1);
		}
		
		// fuel 5
		{
			if (startingFuel5 - fuelSpeed5 < -17.3f) {
				startingFuel5 = 22.8f;
				fuelSpeed5 = 0;
				collidedFuel5 = false;
			}

			// collision detection
			if (!collidedFuel5) {
				modelFuel = glm::translate(modelFuel, glm::vec3(startingFuel5 - fuelSpeed5, 2.2f, -6));
				modelFuel = glm::scale(modelFuel, glm::vec3(0.5f));
				modelFuel = glm::rotate(modelFuel, RADIANS(angularStep * 4), glm::vec3(0, 1, 1));
				RenderSimpleMesh(meshes["fuel"], shaders["Tema2Shader"], modelFuel, glm::vec3(0.9f, 0.7f, 1.8f));

				fuel5Pos = modelFuel * fuel5Pos;
			}

			if (intersect(fuel5Pos)) {
				fuelDecay -= 0.07f;
				collidedFuel5 = true;
			}
			fuel5Pos = glm::vec4(0, 0, 0, 1);
			modelFuel = glm::mat4(1);
		}

		// fuel 6
		{
			if (startingFuel6 - fuelSpeed6 < -16.6f) {
				startingFuel6 = 23.5;
				fuelSpeed6 = 0;
				collidedFuel6 = false;
			}

			// collision detection
			if (!collidedFuel6) {
				modelFuel = glm::translate(modelFuel, glm::vec3(startingFuel6 - fuelSpeed6, 2, -6));
				modelFuel = glm::scale(modelFuel, glm::vec3(0.5f));
				modelFuel = glm::rotate(modelFuel, RADIANS(angularStep * 4), glm::vec3(0, 1, 1));
				RenderSimpleMesh(meshes["fuel"], shaders["Tema2Shader"], modelFuel, glm::vec3(0.9f, 0.7f, 1.8f));

				fuel6Pos = modelFuel * fuel6Pos;
			}

			if (intersect(fuel6Pos)) {
				fuelDecay -= 0.07f;
				collidedFuel6 = true;
			}
			fuel6Pos = glm::vec4(0, 0, 0, 1);
			modelFuel = glm::mat4(1);
		}

		// fuel 7
		{
			if (startingFuel7 - fuelSpeed7 < -15.9f) {
				startingFuel7 = 24.2;
				fuelSpeed7 = 0;
				collidedFuel7 = false;
			}

			// collision detection
			if (!collidedFuel7) {
				modelFuel = glm::translate(modelFuel, glm::vec3(startingFuel7 - fuelSpeed7, 1.8f, -6));
				modelFuel = glm::scale(modelFuel, glm::vec3(0.5f));
				modelFuel = glm::rotate(modelFuel, RADIANS(angularStep * 4), glm::vec3(0, 1, 1));
				RenderSimpleMesh(meshes["fuel"], shaders["Tema2Shader"], modelFuel, glm::vec3(0.9f, 0.7f, 1.8f));

				fuel7Pos = modelFuel * fuel7Pos;
			}

			if (intersect(fuel7Pos)) {
				fuelDecay -= 0.07f;
				collidedFuel7 = true;
			}
			fuel7Pos = glm::vec4(0, 0, 0, 1);
			modelFuel = glm::mat4(1);
		}
		fuelSpeed1 += deltaTimeSeconds * 4;
		fuelSpeed2 += deltaTimeSeconds * 4;
		fuelSpeed3 += deltaTimeSeconds * 4;
		fuelSpeed4 += deltaTimeSeconds * 4;
		fuelSpeed5 += deltaTimeSeconds * 4;
		fuelSpeed6 += deltaTimeSeconds * 4;
		fuelSpeed7 += deltaTimeSeconds * 4;
	}

	//bara combustibil
	glm::mat4 modelFuelBar = glm::mat4(1);
	{
		// fixed bar
		modelFuelBar = glm::translate(modelFuelBar, glm::vec3(12, 6, -17));
		RenderSimpleMesh(meshes["fuelBar"], shaders["Tema2Shader"], modelFuelBar, glm::vec3(0, 0, 0));

		modelFuelBar = glm::mat4(1);

		// make sure fuel stays at 0 till EndOFGame
		if (currentFuel < 0) {
			currentFuel = 0;
		}
		else if(currentFuel > 1){
			currentFuel = 1;
			fuelDecay = 0;
		}
		// remaining fuel
		modelFuelBar = glm::translate(modelFuelBar, glm::vec3(12, 6, -17));
		modelFuelBar = glm::scale(modelFuelBar, glm::vec3(currentFuel, 1, 1));
		RenderSimpleMesh(meshes["remainingFuel"], shaders["Tema2Shader"], modelFuelBar, glm::vec3(2, 2, 2));
		;

		fuelDecay += deltaTimeSeconds * 0.05f;

	}

	minCorner = glm::vec4(-1, -0.3, -1, 1);
	maxCorner = glm::vec4(1, 1, 1, 1);
}

bool Tema2::intersect(glm::vec4 obstaclePos)
{
	float x = fmax(minCorner.x, fmin(obstaclePos.x, maxCorner.x));
	float y = fmax(minCorner.y, fmin(obstaclePos.y, maxCorner.y));
	float z = fmax(minCorner.z, fmin(obstaclePos.z, maxCorner.z));

	float distance = sqrt((x - obstaclePos.x) * (x - obstaclePos.x) +
		(y - obstaclePos.y) * (y - obstaclePos.y) +
		(z - obstaclePos.z) * (z - obstaclePos.z));

	if (distance < 0.85f) {
		return true;
	}
	return false;
}

void Tema2::FrameEnd()
{
	//DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;
		if (window->GetSpecialKeyState() && GLFW_MOD_SHIFT) cameraSpeed = 0.3f;

		if (window->KeyHold(GLFW_KEY_W)) {
			// translate the camera forward
			camera->TranslateForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// translate the camera to the left
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// translate the camera backwards
			camera->TranslateForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// translate the camera to the right
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// translate the camera down
			camera->TranslateUpword(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// translate the camera up
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}

	}
}

void Tema2::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
	}

	if (key == GLFW_KEY_C) {
		if (camera == thirdpersonCamera) {
			camera = firstpersonCamera;
		}
		else {
			camera = thirdpersonCamera;
		}
		
	}

}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
	float sensivityOX = 0.001f;
	float sensivityOY = 0.001f;

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			// rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
			camera->RotateFirstPerson_OY(-deltaX * sensivityOY);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			// rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
		}
	}

	yMovement -= deltaY * 8 * sensivityOY;
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}

Mesh* Tema2::CreateCylinder(std::string name, glm::vec3 center, float radius, float length, int verticesNo, glm::vec3 color) {
	std::vector<VertexFormat> vertices;
	double twopi = 6.283;
	vertices.push_back(VertexFormat(center, color));
	
	float randomOffset;

	for (int i = 1; i <= verticesNo; i++) {
		glm::vec3 aux = center;
		randomOffset = rand() % 10;
		aux.x = (radius + randomOffset / 10) * cos(i * twopi / verticesNo) + center.x;
		aux.y = (radius + randomOffset / 10) * sin(i * twopi / verticesNo) + center.y;
		vertices.push_back(VertexFormat(aux, color));
	}
	
	glm::vec3 centru2 = glm::vec3(center.x, center.y, center.z + length);
	vertices.push_back(VertexFormat(centru2, color));

	for (int i = 1; i <= verticesNo; i++) {
		glm::vec3 aux = centru2;
		randomOffset = rand() % 5;

		aux.x = (radius + randomOffset / 10) * cos(i * twopi / verticesNo) + centru2.x;
		aux.y = (radius + randomOffset / 10) * sin(i * twopi / verticesNo) + centru2.y;
		vertices.push_back(VertexFormat(aux, color));
	}

	glm::vec3 centru3 = glm::vec3(center.x, center.y, center.z + length / 2);
	vertices.push_back(VertexFormat(centru3, color));

	for (int i = 1; i <= verticesNo; i++) {
		glm::vec3 aux = centru3;
		randomOffset = rand() % 8;

		aux.x = (radius + randomOffset / 10) * cos(i * twopi / verticesNo) + centru3.x;
		aux.y = (radius + randomOffset / 10) * sin(i * twopi / verticesNo) + centru3.y;
		vertices.push_back(VertexFormat(aux, color));
	}

	glm::vec3 centru4 = glm::vec3(center.x, center.y, center.z + length / 4);
	vertices.push_back(VertexFormat(centru4, color));

	for (int i = 1; i <= verticesNo; i++) {
		glm::vec3 aux = centru4;
		randomOffset = rand() % 13;

		aux.x = (radius + randomOffset / 10) * cos(i * twopi / verticesNo) + centru4.x;
		aux.y = (radius + randomOffset / 10) * sin(i * twopi / verticesNo) + centru4.y;
		vertices.push_back(VertexFormat(aux, color));
	}
	
	glm::vec3 centru5 = glm::vec3(center.x, center.y, center.z + 3 * length / 4);
	vertices.push_back(VertexFormat(centru5, color));

	for (int i = 1; i <= verticesNo; i++) {
		glm::vec3 aux = centru5;
		randomOffset = rand() % 11;

		aux.x = (radius + randomOffset / 10) * cos(i * twopi / verticesNo) + centru5.x;
		aux.y = (radius + randomOffset / 10) * sin(i * twopi / verticesNo) + centru5.y;
		vertices.push_back(VertexFormat(aux, color));
	}

	
	std::vector<unsigned short> indices(13 * 3 * verticesNo);

	// first circle
	int k = 1;
	int t = 2;
	for (int i = 0; i < 3 * verticesNo; i++) {
		if (i % 3 == 0) {
			indices[i] = 0;
		}
		if (i % 3 == 1) {
			indices[i] = k;
			k++;
			if (k > verticesNo) {
				k = 1;
			}
		}

		if (i % 3 == 2) {
			indices[i] = t;
			t++;
			if (t > verticesNo) {
				t = 1;
			}
		}
	}

	// second circle
	k = verticesNo + 2;
    t = verticesNo + 3;

	for (int i = 3 * verticesNo; i < 2 * 3 * verticesNo; i++) {
		if (i % 3 == 0) {
			indices[i] = verticesNo + 1;
		}
		if (i % 3 == 1) {
			indices[i] = k;
			k++;
			if (k > 2 * verticesNo + 1) {
				k = verticesNo + 2;
			}
		}

		if (i % 3 == 2) {
			indices[i] = t;
			t++;
			if (t > 2 * verticesNo + 1) {
				t = verticesNo + 2;
			}
		}
	}

	// third circle
	k = 2 * verticesNo + 3;
	t = 2 * verticesNo + 4;

	for (int i = 2 * 3 * verticesNo; i < 3 * 3 * verticesNo; i++) {
		if (i % 3 == 0) {
			indices[i] = 2 * verticesNo + 2;
		}
		if (i % 3 == 1) {
			indices[i] = k;
			k++;
			if (k > 3 * verticesNo + 3) {
				k = 2 * verticesNo + 3;
			}
		}

		if (i % 3 == 2) {
			indices[i] = t;
			t++;
			if (t > 3 * verticesNo + 2) {
				t = 2 * verticesNo + 3;
			}
		}
	}

	// fourth circle
	k = 3 * verticesNo + 4;
	t = 4 * verticesNo - (verticesNo - 5);

	for (int i = 3 * 3 * verticesNo; i < 4 * 3 * verticesNo; i++) {
		if (i % 3 == 0) {
			indices[i] = 3 * verticesNo + 3;
		}
		if (i % 3 == 1) {
			indices[i] = k;
			k++;
			if (k > 4 * verticesNo + 3) {
				k = 3 * verticesNo + 4;
			}
		}

		if (i % 3 == 2) {
			indices[i] = t;
			t++;
			if (t > 4 * verticesNo + 3) {
				t = 3 * verticesNo + 4;
			}
		}
	}


	// fifth circle
	k = 5 * verticesNo - (verticesNo - 5);
	t = 5 * verticesNo + 1 - (verticesNo - 5);
	for (int i = 4 * 3 * verticesNo; i < 5 * 3 * verticesNo; i++) {
		if (i % 3 == 0) {
			indices[i] = 5 * verticesNo + 4 - verticesNo;
		}
		if (i % 3 == 1) {
			indices[i] = k;
			k++;
			if (k > 5 * verticesNo + 4) {
				k = 5 * verticesNo - (verticesNo - 5);
			}
		}

		if (i % 3 == 2) {
			indices[i] = t;
			t++;
			if (t > 5 * verticesNo + 4) {
				t = 5 * verticesNo - (verticesNo - 5);
			}
		}
	}

	// first set of triangles
	{
		int stepFirst = verticesNo - 1 ;
		int stepSecond = 4 * verticesNo + 2;
		int stepThird = 4 * verticesNo + 3;

		for (int i = 1; i <= 3 * verticesNo; i++) {
			if ((i - 1) % 3 == 0) {
				indices[5 * 3 * verticesNo + i - 1] = stepFirst;
				stepFirst++;
				if (stepFirst > verticesNo) {
					stepFirst = 1;
				}
			}

			if ((i - 1) % 3 == 1) {
				indices[5 * 3 * verticesNo + i - 1] = stepSecond;
				stepSecond++;
			if (stepSecond > 4 * verticesNo + 3) {
					stepSecond = 3 * verticesNo + 4;
				}
			}

		if ((i - 1) % 3 == 2) {
			indices[5 * 3 * verticesNo + i - 1] = stepThird;
			stepThird++;
			if (stepSecond > 4 * verticesNo + 2) {
				stepThird = 3 * verticesNo + 4;
			}
		}
	}

}

	// second set of triangles
	{
		int stepFirst = verticesNo - 3;
	int stepSecond = 4 * verticesNo;
	int stepThird = verticesNo - 4;

	for (int i = 1; i <= 3 * verticesNo; i++) {
		if ((i - 1) % 3 == 0) {
			indices[6 * 3 * verticesNo + i - 1] = stepFirst;
			stepFirst++;
			if (stepFirst > verticesNo) {
				stepFirst = 1;
			}
		}

		if ((i - 1) % 3 == 1) {
			indices[6 * 3 * verticesNo + i - 1] = stepSecond;
			stepSecond++;
			if (stepSecond > 4 * verticesNo + 3) {
				stepSecond = 3 * verticesNo + 4;
			}
		}

		if ((i - 1) % 3 == 2) {
			indices[6 * 3 * verticesNo + i - 1 ] = stepThird;
			stepThird++;
			if (stepThird > verticesNo) {
				stepThird = 1;
			}
		}
	}
	
}

	// thrid set of triangles
	{
		int stepFirst = verticesNo * 3 + 5;
		int stepSecond = verticesNo * 3 + 4;
		int stepThird = verticesNo * 2 + 4;

		for (int i = 1; i <= 3 * verticesNo; i++) {
			if ((i - 1) % 3 == 0) {
				indices[7 * 3 * verticesNo + i - 1] = stepFirst;
				stepFirst++;
				if (stepFirst > 4 * verticesNo + 3) {
					stepFirst = verticesNo * 3 + 4;
				}
			}

			if ((i - 1) % 3 == 1) {
				indices[7 * 3 * verticesNo + i - 1] = stepSecond;
				stepSecond++;
				if (stepSecond > 4 * verticesNo + 3) {
					stepSecond = verticesNo * 3 + 4;
				}
			}

			if ((i - 1) % 3 == 2) {
				indices[7 * 3 * verticesNo + i - 1] = stepThird;
				stepThird++;
				if (stepThird > 3 * verticesNo + 2) {
					stepThird = verticesNo * 2 + 3;
				}
			}
		}	
	}

	// fourth set of triangles
	{
		int stepFirst = verticesNo * 3 + 4;
		int stepSecond = verticesNo * 2 + 4;
		int stepThird = verticesNo * 2 + 3;

		for (int i = 1; i <= 3 * verticesNo; i++) {
			if ((i - 1) % 3 == 0) {
				indices[8 * 3 * verticesNo + i - 1] = stepFirst;
				stepFirst++;
				if (stepFirst > 4 * verticesNo + 3) {
					stepFirst = verticesNo * 3 + 4;
				}
			}

			if ((i - 1) % 3 == 1) {
				indices[8 * 3 * verticesNo + i - 1] = stepSecond;
				stepSecond++;
				if (stepSecond > 3 * verticesNo + 2) {
					stepSecond = verticesNo * 2 + 3;
				}
			}

			if ((i - 1) % 3 == 2) {
				indices[8 * 3 * verticesNo + i - 1] = stepThird;
				stepThird++;
				if (stepThird > 3 * verticesNo + 2) {
					stepThird = verticesNo * 2 + 3;
				}
			}
		}
	}

	// fifth set of triangles
	{
		int stepFirst = verticesNo * 2 + 3;
		int stepSecond = verticesNo * 4 + 5;
		int stepThird = verticesNo * 2  + 4;

		for (int i = 1; i <= 3 * verticesNo; i++) {
			if ((i - 1) % 3 == 0) {
				indices[9 * 3 * verticesNo + i - 1] = stepFirst;
				stepFirst++;
				if (stepFirst > 3 * verticesNo + 2) {
					stepFirst = verticesNo * 2 + 3;
				}
			}

			if ((i - 1) % 3 == 1) {
				indices[9 * 3 * verticesNo + i - 1] = stepSecond;
				stepSecond++;
				if (stepSecond > 5 * verticesNo + 4) {
					stepSecond = verticesNo * 4 + 5;
				}
			}

			if ((i - 1) % 3 == 2) {
				indices[9 * 3 * verticesNo + i - 1] = stepThird;
				stepThird++;
				if (stepThird > 3 * verticesNo + 2) {
					stepThird = verticesNo * 2 + 3;
				}
			}
		}
	}
	
	// sixth set of triangles
	{
		int stepFirst = verticesNo * 2 + 4;
		int stepSecond = verticesNo * 4 + 5;
		int stepThird = verticesNo * 4 + 6;

		for (int i = 1; i <= 3 * verticesNo; i++) {
			if ((i - 1) % 3 == 0) {
				indices[10 * 3 * verticesNo + i - 1] = stepFirst;
				stepFirst++;
				if (stepFirst > 3 * verticesNo + 2) {
					stepFirst = verticesNo * 2 + 3;
				}
			}

			if ((i - 1) % 3 == 1) {
				indices[10 * 3 * verticesNo + i - 1] = stepSecond;
				stepSecond++;
				if (stepSecond > 5 * verticesNo + 4) {
					stepSecond = verticesNo * 4 + 5;
				}
			}

			if ((i - 1) % 3 == 2) {
				indices[10 * 3 * verticesNo + i - 1] = stepThird;
				stepThird++;
				if (stepThird > 5 * verticesNo + 4) {
					stepThird = verticesNo * 4 + 5;
				}
			}

		}
	}
	
	// seventh set of triangles
	{
		int stepFirst = verticesNo * 4 + 5;
		int stepSecond = verticesNo + 2;
		int stepThird = verticesNo + 3;

		for (int i = 1; i <= 3 * verticesNo; i++) {
			if ((i - 1) % 3 == 0) {
				indices[11 * 3 * verticesNo + i - 1] = stepFirst;
				stepFirst++;
				if (stepFirst > 5 * verticesNo + 4) {
					stepFirst = verticesNo * 4 + 5;
				}
			}

		if ((i - 1) % 3 == 1) {
				indices[11 * 3 * verticesNo + i - 1] = stepSecond;
				stepSecond++;
				if (stepSecond > 2 * verticesNo + 2) {
					stepSecond = verticesNo + 2;
				}
			}

			if ((i - 1) % 3 == 2) {
				indices[11 * 3 * verticesNo + i - 1] = stepThird;
			stepThird++;
			if (stepThird > 2 * verticesNo + 1) {
					stepThird = verticesNo + 2;
				}
			}
		}
	}
	// eigth set of triangles
	{
		int stepFirst = verticesNo * 4 + 5;
		int stepSecond = verticesNo + 3;
		int stepThird = verticesNo * 4 + 6;

		for (int i = 1; i <= 3 * verticesNo; i++) {
		if ((i - 1) % 3 == 0) {
			indices[12 * 3 * verticesNo + i - 1] = stepFirst;
				stepFirst++;
				if (stepFirst > 5 * verticesNo + 4) {
					stepFirst = verticesNo * 4 + 5;
				}
			}

			if ((i - 1) % 3 == 1) {
				indices[12 * 3 * verticesNo + i - 1] = stepSecond;
				stepSecond++;
				if (stepSecond > 2 * verticesNo + 1) {
					stepSecond = verticesNo + 2;
				}
			}

			if ((i - 1) % 3 == 2) {
				indices[12 * 3 * verticesNo + i - 1] = stepThird;
				stepThird++;
				if (stepThird > 5 * verticesNo + 4) {
					stepThird = verticesNo * 4 + 5;
				}
			}
		}
	}

	Mesh* cylinder = CreateMesh("cylinder", vertices, indices);

	return cylinder;
}
