#include "sphere.h" 

#include <math.h>
#include "../Glm/glm.hpp"
#include "../Glm/gtc/constants.hpp"

namespace Exp
{
    // TODO: check geodesic (and icosahedron tesselation); much better texture space mapping.
    // ------------------------------------------------------------------------
    // arametric equation for a sphere F(u,v, r) = [cos(u)*sin(v)*r, cos(v), sin(u)*sin(v)*r] where 
    // u is longitude [0, 2PI] and v is lattitude [0, PI] (note the difference in their range)
    Sphere::Sphere(unsigned int xSegments, unsigned int ySegments)
    {
        for (unsigned int y = 0; y <= ySegments; ++y)
        {
            for (unsigned int x = 0; x <= xSegments; ++x)
            {
                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;
                float xPos = std::cos(xSegment * (glm::pi<float>() * 2.0f)) * std::sin(ySegment * glm::pi<float>()); // TAU is 2PI
                float yPos = std::cos(ySegment * glm::pi<float>());
                float zPos = std::sin(xSegment * (glm::pi<float>() * 2.0f)) * std::sin(ySegment * glm::pi<float>());

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < ySegments; ++y)
        {
            for (unsigned int x = 0; x < xSegments; ++x)
            {
                indices.push_back((y + 1) * (xSegments + 1) + x);
                indices.push_back(y       * (xSegments + 1) + x);
                indices.push_back(y       * (xSegments + 1) + x + 1);

                indices.push_back((y + 1) * (xSegments + 1) + x);
                indices.push_back(y       * (xSegments + 1) + x + 1);
                indices.push_back((y + 1) * (xSegments + 1) + x + 1);
            }
        }

        topology = TRIANGLES;
        Finalize();
    }
}
