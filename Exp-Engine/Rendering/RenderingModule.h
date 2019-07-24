#pragma once

#include "../Module/ModuleInterface.h"
#include "Buffers.h"
#include "../CommandBuffer/cmds/GLCommands.h"

namespace Exp
{
	class RenderingModule :
		public IModuleInterface
	{
	public:
		RenderingModule();
		virtual ~RenderingModule();

		virtual void StartUp();
		virtual void Shutdown();

	public:
		void Render();

	public:
		GeometryCommandBuffer m_geometryCommands;
	};
}