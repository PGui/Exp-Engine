#include "Mesh.h"
#include <glad/glad.h>

#include <assert.h>

namespace Exp
{

	Mesh::Mesh()
	{
	}

	Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> indices) :
		positions(positions),
		normals(normals),
		indices(indices)
	{
	}

	Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uv, std::vector<unsigned int> indices) :
		positions(positions),
		normals(normals),
		uv(uv),
		indices(indices)
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Finalize(bool interleaved)
	{
		assert(positions.size() > 0);

		//Generatebuffers
		if (VAO == 0)
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);
		}

		//Prepare data
		std::vector<float> data;
		if (interleaved)
		{
			for (int i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);
				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
				if (tangents.size() > 0)
				{
					data.push_back(tangents[i].x);
					data.push_back(tangents[i].y);
					data.push_back(tangents[i].z);
				}
				if (bitangents.size() > 0)
				{
					data.push_back(bitangents[i].x);
					data.push_back(bitangents[i].y);
					data.push_back(bitangents[i].z);
				}
			}
		}
		//else
		//{
		//	// if any of the float arrays are empty, data won't be filled by them.
		//	for (int i = 0; i < positions.size(); ++i)
		//	{
		//		data.push_back(positions[i].x);
		//		data.push_back(positions[i].y);
		//		data.push_back(positions[i].z);
		//	}
		//	for (int i = 0; i < normals.size(); ++i)
		//	{
		//		data.push_back(normals[i].x);
		//		data.push_back(normals[i].y);
		//		data.push_back(normals[i].z);
		//	}
		//	for (int i = 0; i < uv.size(); ++i)
		//	{
		//		data.push_back(uv[i].x);
		//		data.push_back(uv[i].y);
		//	}
		//	/*
		//	for (int i = 0; i < tangents.size(); ++i)
		//	{
		//		data.push_back(tangents[i].x);
		//		data.push_back(tangents[i].y);
		//		data.push_back(tangents[i].z);
		//	}
		//	for (int i = 0; i < bitangents.size(); ++i)
		//	{
		//		data.push_back(bitangents[i].x);
		//		data.push_back(bitangents[i].y);
		//		data.push_back(bitangents[i].z);
		//	}*/
		//}

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		if (indices.size() > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		if (interleaved)
		{
			GLint offset = 0;
			GLsizei stride = 3 * sizeof(float);
			if (normals.size() > 0)		stride += 3 * sizeof(float);
			if (uv.size() > 0)			stride += 2 * sizeof(float);
			if (tangents.size() > 0)	stride += 3 * sizeof(float);
			if (bitangents.size() > 0)	stride += 3 * sizeof(float);

			//positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);

			if (normals.size() > 0)
			{
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
				offset += 3 * sizeof(float);

			}
			if (uv.size() > 0)
			{
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
				offset += 2 * sizeof(float);
			}
			if (tangents.size() > 0)
			{
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
				offset += 3 * sizeof(float);
			}
			if (bitangents.size() > 0)
			{
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
				offset += 3 * sizeof(float);
			}
		}
		//else
		//{
		//	/*size_t offset = 0;
		//	glEnableVertexAttribArray(0);
		//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
		//	offset += positions.size() * sizeof(float);
		//	
		//	if (normals.size() > 0)
		//	{
		//		glEnableVertexAttribArray(1);
		//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
		//		offset += normals.size() * sizeof(float);
		//	}
		//	if (uv.size() > 0)
		//	{
		//		glEnableVertexAttribArray(2);
		//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
		//		offset += uv.size() * sizeof(float);
		//	}*/
		//}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}