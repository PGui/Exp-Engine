#include "Quad.h"

namespace Exp
{
	Quad::Quad(float width, float height)
	{
		Positions =
		{
			{ -width,  height, 0.0f, },
			{ -width, -height, 0.0f, },
			{  width,  height, 0.0f, },
			{  width, -height, 0.0f, },
		};
		UV = {
			{ 0.0f, 1.0f, },
			{ 0.0f, 0.0f, },
			{ 1.0f, 1.0f, },
			{ 1.0f, 0.0f, },
		};
		Topology = TRIANGLE_STRIP;

		finalize();
	}

	Quad::~Quad()
	{
	}
}

