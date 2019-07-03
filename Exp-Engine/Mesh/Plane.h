#pragma once
#include "Mesh.h"

namespace Exp
{
	class Plane :
		public Mesh
	{
	public:
		Plane(unsigned int xSegments, unsigned int ySegments);

		virtual ~Plane();
	};
}

