#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "../glm/glm.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Exp
{

	class Mesh
	{
	public:
		enum TOPOLOGY
		{
			POINTS,
			LINES,
			LINE_STRIP,
			TRIANGLES,
			TRIANGLE_STRIP,
			TRIANGLE_FAN,
		};


	public:


		Mesh();
		Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> indices);
		Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uv, std::vector<unsigned int> indices);
		~Mesh();

		unsigned int VAO = 0;
		TOPOLOGY Topology = TRIANGLES;

		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> UV;
		std::vector<glm::vec3> Tangents;
		std::vector<glm::vec3> Bitangents;

		std::vector<unsigned int> Indices;

		//Create VBO/Vertex attributes...
		void finalize(bool interleaved = true);

	protected:
		unsigned int VBO;
		unsigned int EBO;


	};
}
