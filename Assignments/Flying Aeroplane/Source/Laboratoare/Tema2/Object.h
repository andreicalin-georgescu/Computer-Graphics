#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Object {
	Mesh* CreateCircle(std::string name, glm::vec3 center, float radius, int vericesNo, glm::vec3 color, bool fill = false);
	Mesh* CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);
	Mesh* CreateTriangle(std::string name, glm::vec3 corner, float size, glm::vec3 color, bool fill);
}