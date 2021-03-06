#include <API\DepthStencilState.h>
#ifdef NE_USE_RUNTIME_RENDERER
#include <Core\Context.h>

namespace NuclearEngine {
	namespace API {
		DepthStencilState::DepthStencilState()
		{
		}

		DepthStencilState::~DepthStencilState()
		{
		
		}
		void DepthStencilState::Create(DepthStencilState* result,DepthStencilStateDesc* type)
		{
			if (Core::Context::GetRenderAPI() == Core::RenderAPI::OpenGL3)
			{
				OpenGL::GLDepthStencilState::Create(&result->GLObject, type);
			}
			else if (Core::Context::GetRenderAPI() == Core::RenderAPI::DirectX11)
			{
				DirectX::DX11DepthStencilState::Create(&result->DXObject, type);
			}
		}
		void DepthStencilState::Bind()
		{
			if (Core::Context::GetRenderAPI() == Core::RenderAPI::OpenGL3)
			{
				GLObject.Bind();
			}
			else if (Core::Context::GetRenderAPI() == Core::RenderAPI::DirectX11)
			{
				DXObject.Bind();
			}
		}
		void DepthStencilState::Unbind()
		{
			if (Core::Context::GetRenderAPI() == Core::RenderAPI::OpenGL3)
			{
				GLObject.Unbind();
			}
			else if (Core::Context::GetRenderAPI() == Core::RenderAPI::DirectX11)
			{
				DXObject.Unbind();
			}
		}
	}
}

#endif