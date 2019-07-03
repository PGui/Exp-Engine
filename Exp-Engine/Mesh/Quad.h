#pragma once
#include "Mesh.h"

namespace Exp
{
	class Quad :
		public Mesh
	{
	public:
		Quad(float width = 1.0f, float height = 1.0f);
		~Quad();
	};
}

