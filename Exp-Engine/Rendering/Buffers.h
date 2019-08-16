#pragma once


#include "../CommandBuffer/CommandBuffer.h"
#include "../CommandBuffer/CommandKeys.h"

#include "Shader.h"
#include "Material.h"
#include "GLCache.h"

#include "../Module/ModuleInterface.h"
#include "../Module/ModuleManager.h"

#include "../Core/EngineModule.h"

namespace Exp
{
	class EngineModule;
	// Example of a material binder with state that has redundancy checks.
	struct MaterialBinder
	{
		EngineModule* module;
		GLCache m_GlCache;

		struct Material
		{
			std::string name;
			GLuint location = 0;
			GLuint ubo = 0;
			Shader* shader = nullptr;
		};

		///@note Returns true if there are more passes to bind.
		bool operator()(cb::MaterialId material)
		{
			m_GlCache.Reset();

			if (material.id == 0) // first material is a dummy, nothing to bind
				return false;

			module = Exp::ModuleManager::Get().GetModule<Exp::EngineModule>("Engine");

			Exp::Material* mat = materials[material.id];
			assert(mat != nullptr && mat->GetShader() != nullptr);
			if (mat->GetShader() != activeShader)
			{
				//bind only if different shaders
				mat->GetShader()->use();
				activeShader = mat->GetShader();

				//glm::mat4 proj = module->RenderCamera->m_projection;
				activeShader->setMat4("projection", module->m_Camera.m_projection);
				activeShader->setMat4("view", module->m_Camera.m_view);
				activeShader->setMat4("model", glm::mat4(1.0f));

				unsigned int projIndex = glGetUniformBlockIndex(activeShader->ID, "Global");
				glUniformBlockBinding(activeShader->ID, projIndex, 0);
			}

			if (true)
			{
				m_GlCache.SetBlend(mat->Blend);
				if (mat->Blend)
				{
					m_GlCache.SetBlendFunc(mat->BlendSrc, mat->BlendDst);
				}
				m_GlCache.SetDepthFunc(mat->DepthCompare);
				m_GlCache.SetDepthTest(mat->DepthTest);
				m_GlCache.SetCull(mat->Cull);
				m_GlCache.SetCullFace(mat->CullFace);
			}

			auto samplers = mat->GetSamplerUniforms();
			for (auto it = samplers.begin(); it != samplers.end(); ++it)
			{
				it->second.Texture->Bind(it->second.Unit);
			}

			auto uniforms = mat->GetUniforms();
			for (auto it = uniforms.begin(); it != uniforms.end(); ++it)
			{
				switch (it->second.Type)
				{
				case SHADER_TYPE_INT:
					mat->GetShader()->setInt(it->first, it->second.Int);
					break;
				case SHADER_TYPE_FLOAT:
					mat->GetShader()->setFloat(it->first, it->second.Float);
					break;
				case SHADER_TYPE_VEC3:
					mat->GetShader()->setVec3(it->first, it->second.Vec3);
					break;
				default:
					break;
				}
			}

			return false;
		}

		void debugMsg(cb::MaterialId material)
		{
			if (material.id != 0) // first material is a dummy
				std::cout << "Binding \'%s\' material " << materials[material.id]->GetShader()->Name << std::endl;
		}

		void reset()
		{
			activeMaterial = -1;
			activeShader = nullptr;
		}

		void useMaterial(Exp::Material* Material)
		{
			if (materials.find(Material->MaterialId) == materials.end())
			{
				materials[Material->MaterialId] = Material;
			}
		}

		std::map<int, Exp::Material*> materials;
		mutable	int activeMaterial = -1;
		mutable Shader* activeShader = nullptr;
	};

	typedef cb::CommandBuffer<cb::DrawKey, cb::DefaultKeyDecoder, MaterialBinder> GeometryCommandBuffer;
	typedef cb::CommandBuffer<uint32_t, cb::DummyKeyDecoder<uint32_t>> DeferredCommandBuffer;
	typedef cb::CommandBuffer<uint16_t, cb::DummyKeyDecoder<uint16_t>> PostProcessCommandBuffer;
}