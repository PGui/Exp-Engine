#pragma once


#include "../CommandBuffer/CommandBuffer.h"
#include "../CommandBuffer/CommandKeys.h"

#include "Shader.h"
#include "Material.h"

namespace Exp
{
	// Example of a material binder with state that has redundancy checks.
	struct MaterialBinder
	{
		struct Material
		{
			std::string name;
			GLuint location = 0;
			GLuint ubo = 0;
			Shader* shader = nullptr;
		};

		///@note Returns true if there are more passes to bind.
		CB_FORCE_INLINE bool operator()(cb::MaterialId material) const
		{
			//if (material.id == 0) // first material is a dummy, nothing to bind
			//	return false;

			//const Material&  mat = materials[material.id];
			//assert(mat.shader);
			//if (mat.shader != activeShader)
			//{
			//	//bind only if different shaders
			//	mat.shader->use();
			//	activeShader = mat.shader;
			//}
			//if (material.id != activeMaterial)
			//{
			//	// bind material ubo
			//	glBindBufferBase(GL_UNIFORM_BUFFER, mat.location, mat.ubo);
			//	activeMaterial = material.id;
			//}

			//return false;
			return false;
		}

		void debugMsg(cb::MaterialId material)
		{
			if (material.id != 0) // first material is a dummy
				std::cout << "Binding \'%s\' material " << materials[material.id].name.c_str() << std::endl;
		}

		void reset()
		{
			activeMaterial = -1;
			activeShader = nullptr;
		}

		std::vector<Material> materials;
		mutable	int activeMaterial = -1;
		mutable Shader* activeShader = nullptr;
	};

	typedef cb::CommandBuffer<cb::DrawKey, cb::DefaultKeyDecoder, MaterialBinder> GeometryCommandBuffer;
	typedef cb::CommandBuffer<uint32_t, cb::DummyKeyDecoder<uint32_t>> DeferredCommandBuffer;
	typedef cb::CommandBuffer<uint16_t, cb::DummyKeyDecoder<uint16_t>> PostProcessCommandBuffer;
}