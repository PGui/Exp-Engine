#include "Cube.h"

namespace Exp
{

	Cube::Cube()
	{
		positions = std::vector<glm::vec3>{
				glm::vec3(-0.5f, -0.5f, -0.5f),
				glm::vec3(0.5f,  0.5f, -0.5f),
				glm::vec3(0.5f, -0.5f, -0.5f),
				glm::vec3(0.5f,  0.5f, -0.5f),
				glm::vec3(-0.5f, -0.5f, -0.5f),
				glm::vec3(-0.5f,  0.5f, -0.5f),

				glm::vec3(-0.5f, -0.5f,  0.5f),
				glm::vec3(0.5f, -0.5f,  0.5f),
				glm::vec3(0.5f,  0.5f,  0.5f),
				glm::vec3(0.5f,  0.5f,  0.5f),
				glm::vec3(-0.5f,  0.5f,  0.5f),
				glm::vec3(-0.5f, -0.5f,  0.5f),

				glm::vec3(-0.5f,  0.5f,  0.5f),
				glm::vec3(-0.5f,  0.5f, -0.5f),
				glm::vec3(-0.5f, -0.5f, -0.5f),
				glm::vec3(-0.5f, -0.5f, -0.5f),
				glm::vec3(-0.5f, -0.5f,  0.5f),
				glm::vec3(-0.5f,  0.5f,  0.5f),

				glm::vec3(0.5f,  0.5f,  0.5f),
				glm::vec3(0.5f, -0.5f, -0.5f),
				glm::vec3(0.5f,  0.5f, -0.5f),
				glm::vec3(0.5f, -0.5f, -0.5f),
				glm::vec3(0.5f,  0.5f,  0.5f),
				glm::vec3(0.5f, -0.5f,  0.5f),

				glm::vec3(-0.5f, -0.5f, -0.5f),
				glm::vec3(0.5f, -0.5f, -0.5f),
				glm::vec3(0.5f, -0.5f,  0.5f),
				glm::vec3(0.5f, -0.5f,  0.5f),
				glm::vec3(-0.5f, -0.5f,  0.5f),
				glm::vec3(-0.5f, -0.5f, -0.5f),

				glm::vec3(-0.5f,  0.5f, -0.5f),
				glm::vec3(0.5f,  0.5f,  0.5f),
				glm::vec3(0.5f,  0.5f, -0.5f),
				glm::vec3(0.5f,  0.5f,  0.5f),
				glm::vec3(-0.5f,  0.5f, -0.5f),
				glm::vec3(-0.5f,  0.5f,  0.5f),
		};
		uv = std::vector<glm::vec2>{
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),

			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),

			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),

			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),

			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),

			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
		};
		normals = std::vector<glm::vec3>{
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),

			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),

			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),

			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),

			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),

			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
		};

		topology = TRIANGLES;
		Finalize(true);
	}

	Cube::~Cube()
	{
	}

}