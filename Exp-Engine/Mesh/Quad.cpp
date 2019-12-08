#include "Quad.h"

namespace Exp
{
	Quad::Quad(float width, float height)
	{
		positions =
		{
			{ -width,  height, 0.0f, },
			{ -width, -height, 0.0f, },
			{  width,  height, 0.0f, },
			{  width, -height, 0.0f, },
		};
		uv = {
			{ 0.0f, 1.0f, },
			{ 0.0f, 0.0f, },
			{ 1.0f, 1.0f, },
			{ 1.0f, 0.0f, },
		};
		topology = TRIANGLE_STRIP;

		Finalize();
	}

	Quad::~Quad()
	{
	}
}

