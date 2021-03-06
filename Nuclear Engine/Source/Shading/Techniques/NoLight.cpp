#include <Shading\Techniques\NoLight.h>
#include <Core\FileSystem.h>

namespace NuclearEngine {
	namespace Shading {
		namespace Techniques {
			NoLight::NoLight()
			{
				this->m_type = Technique_Type::LightShading;

				this->m_shaderpath = "Assets/NuclearEngine/Shaders/Techniques/NoLight.hlsl";
			}
		}
	}
}