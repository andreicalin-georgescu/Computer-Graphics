#include "Object.h"

#include <Core/Engine.h>


Mesh* Object::CreateTriangle(std::string name, glm::vec3 leftBottomCorner, float size, glm::vec3 color, bool fill) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner + glm::vec3(0.5f, 0.5f, 0.5f), color),
		VertexFormat(corner + glm::vec3(0.5f, size * 1.5f, 0.5f), color),
		VertexFormat(corner + glm::vec3(size, 0.5f, 0.5f), color)
	};

	Mesh* triangle = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2 };

	triangle->InitFromData(vertices, indices);
	return triangle;
}


Mesh* Object::CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill) {
	glm::vec3 corner = leftBottomCorner;
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(width, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};
	
	Mesh* rectangle = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };
	if (!fill) {
		rectangle->SetDrawMode(GL_LINE_LOOP);
	}
	else {

		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);

	}
	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

Mesh* Object::CreateCircle(std::string name, glm::vec3 center, float radius, int verticesNo, glm::vec3 color, bool fill) {

	std::vector<VertexFormat> vertices;
	double twopi = 6.283;
	vertices.push_back(VertexFormat(center, color));

	for (int i = 1; i <= verticesNo; i++) {
		glm::vec3 aux = center;
		aux.x = radius * cos(i * twopi / verticesNo) + center.x;
		aux.y = radius * sin(i * twopi / verticesNo) + center.y;
		vertices.push_back(VertexFormat(aux, color));
	}

	std::vector<unsigned short> indices(verticesNo + 1);
	for (int i = 0; i <= verticesNo; i++) {
		indices[i] = i;
	}


	Mesh* circle = new Mesh(name);
	circle->SetDrawMode(GL_TRIANGLE_FAN);
	circle->InitFromData(vertices, indices);
	return circle;
}