#include "Mesh.h"
#include <glad/glad.h>

#include <assert.h>

namespace Exp
{

	Mesh::Mesh()
	{
	}

	Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> indices) :
		Positions(positions),
		Normals(normals),
		Indices(indices)
	{
	}

	Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uv, std::vector<unsigned int> indices) :
		Positions(positions),
		Normals(normals),
		UV(uv),
		Indices(indices)
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::finalize(bool interleaved)
	{
		assert(Positions.size() > 0);

		//Generatebuffers
		if (VAO == 0)
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
		}

		//Prepare data
		std::vector<float> data;
		if (interleaved)
		{
			for (int i = 0; i < Positions.size(); ++i)
			{
				data.push_back(Positions[i].x);
				data.push_back(Positions[i].y);
				data.push_back(Positions[i].z);
				if (Normals.size() > 0)
				{
					data.push_back(Normals[i].x);
					data.push_back(Normals[i].y);
					data.push_back(Normals[i].z);
				}
				if (UV.size() > 0)
				{
					data.push_back(UV[i].x);
					data.push_back(UV[i].y);
				}
				if (Tangents.size() > 0)
				{
					data.push_back(Tangents[i].x);
					data.push_back(Tangents[i].y);
					data.push_back(Tangents[i].z);
				}
				if (Bitangents.size() > 0)
				{
					data.push_back(Bitangents[i].x);
					data.push_back(Bitangents[i].y);
					data.push_back(Bitangents[i].z);
				}
			}
		}
		//else
		//{
		//	// if any of the float arrays are empty, data won't be filled by them.
		//	for (int i = 0; i < Positions.size(); ++i)
		//	{
		//		data.push_back(Positions[i].x);
		//		data.push_back(Positions[i].y);
		//		data.push_back(Positions[i].z);
		//	}
		//	for (int i = 0; i < Normals.size(); ++i)
		//	{
		//		data.push_back(Normals[i].x);
		//		data.push_back(Normals[i].y);
		//		data.push_back(Normals[i].z);
		//	}
		//	for (int i = 0; i < UV.size(); ++i)
		//	{
		//		data.push_back(UV[i].x);
		//		data.push_back(UV[i].y);
		//	}
		//	/*
		//	for (int i = 0; i < Tangents.size(); ++i)
		//	{
		//		data.push_back(Tangents[i].x);
		//		data.push_back(Tangents[i].y);
		//		data.push_back(Tangents[i].z);
		//	}
		//	for (int i = 0; i < Bitangents.size(); ++i)
		//	{
		//		data.push_back(Bitangents[i].x);
		//		data.push_back(Bitangents[i].y);
		//		data.push_back(Bitangents[i].z);
		//	}*/
		//}

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		if (Indices.size() > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);
		}

		if (interleaved)
		{
			GLint offset = 0;
			GLsizei stride = 3 * sizeof(float);
			if (Normals.size() > 0)		stride += 3 * sizeof(float);
			if (UV.size() > 0)			stride += 2 * sizeof(float);
			if (Tangents.size() > 0)	stride += 3 * sizeof(float);
			if (Bitangents.size() > 0)	stride += 3 * sizeof(float);

			//Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
			offset += 3 * sizeof(float);

			if (Normals.size() > 0)
			{
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
				offset += 3 * sizeof(float);

			}
			if (UV.size() > 0)
			{
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
				offset += 2 * sizeof(float);
			}
			if (Tangents.size() > 0)
			{
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
				offset += 3 * sizeof(float);
			}
			if (Bitangents.size() > 0)
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
		//	offset += Positions.size() * sizeof(float);
		//	
		//	if (Normals.size() > 0)
		//	{
		//		glEnableVertexAttribArray(1);
		//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
		//		offset += Normals.size() * sizeof(float);
		//	}
		//	if (UV.size() > 0)
		//	{
		//		glEnableVertexAttribArray(2);
		//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
		//		offset += UV.size() * sizeof(float);
		//	}*/
		//}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}