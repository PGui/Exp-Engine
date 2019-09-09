#pragma once

#include "Mesh.h"

namespace Exp
{
    /* 
      3D unit sphere charactized by its number of horizontal (xSegments) and vertical (ySegments)
      rings.
    */
    class Sphere : 
		public Mesh
    {
    public:
        Sphere(unsigned int xSegments, unsigned int ySegments);
    };
}