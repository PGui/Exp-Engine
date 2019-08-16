#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "RenderingModule.h"
#include "Buffers.h"
#include "../CommandBuffer/CommandBuffer.h"
#include "GLCache.h"
namespace Exp
{
	struct BeginFrameCommand
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		MaterialBinder* materialBinder;
		GLCache* glCache;
		GLuint projUBOId;
		ProjectionUBO projUBOData;

		static void execute(const void* data, cb::RenderContext* rc)
		{
			auto& cmd = *reinterpret_cast<const BeginFrameCommand*>(data);
			cmd.materialBinder->reset();

			//cmd.glCache->Reset();

			glBindBuffer(GL_UNIFORM_BUFFER, cmd.projUBOId);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionUBO), &cmd.projUBOData, GL_STATIC_DRAW);

			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferRange(GL_UNIFORM_BUFFER, 0, cmd.projUBOId, 0, sizeof(ProjectionUBO));

		}
	};
}


