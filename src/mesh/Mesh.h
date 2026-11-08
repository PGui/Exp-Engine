#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

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
		virtual ~Mesh();
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		unsigned int vao = 0;
		TOPOLOGY topology = TRIANGLES;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		std::vector<unsigned int> indices;

		//Create vbo/Vertex attributes...
		void Finalize(bool interleaved = true);

	protected:
		unsigned int vbo = 0;
		unsigned int ebo = 0;


	};
}
