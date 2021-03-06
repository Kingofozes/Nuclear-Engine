#include <API\DirectX\DX11Shader.h>

#ifdef NE_COMPILE_DIRECTX11
#include <API\DirectX\DX11Context.h>
#include <API\DirectX\DX11ConstantBuffer.h>
#include <d3dcompiler.h>
#include <API\API_Types.h>

namespace NuclearEngine
{
	namespace API
	{
		namespace DirectX
		{
			DX11Shader::DX11Shader()
			{
				m_vertexShader = nullptr;
				m_pixelShader = nullptr;
				m_geometryShader = nullptr;

				m_VSBL = nullptr;
				m_PSBL = nullptr;
				m_GSBL = nullptr;
			}

			DX11Shader::~DX11Shader()
			{
				if (m_vertexShader != nullptr)
				{
					m_vertexShader->Release();
				}
				if (m_geometryShader != nullptr)
				{
					m_geometryShader->Release();
				}
				if (m_pixelShader != nullptr)
				{
					m_pixelShader->Release();
				}
				m_vertexShader = nullptr;
				m_pixelShader = nullptr;
				m_geometryShader = nullptr;

				m_VSBL = nullptr;
				m_PSBL = nullptr;
				m_GSBL = nullptr;
			}

			void DX11Shader::Create(DX11Shader* result, BinaryShaderBlob* VertexShaderCode, BinaryShaderBlob* PixelShaderCode, BinaryShaderBlob* GeometryShaderCode)
			{	
				if (VertexShaderCode != nullptr)
				{
					if (VertexShaderCode->Language != ShaderLanguage::DXBC)
					{
						Log->Error("[DX11Shader] DirectX 11 Renderer Backend expects all -Vertex- shaders in DirectX Bytecode \"DXBC\" language!\n");

						return;
					}
				}
				if (PixelShaderCode != nullptr)
				{
					if (PixelShaderCode->Language != ShaderLanguage::DXBC)
					{
						Log->Error("[DX11Shader] DirectX 11 Renderer Backend expects all -Pixel- shaders in DirectX Bytecode \"DXBC\" language!\n");

						return;
					}
				}
				if (GeometryShaderCode != nullptr)
				{
					if (GeometryShaderCode->Language != ShaderLanguage::DXBC)
					{
						Log->Error("[DX11Shader] DirectX 11 Renderer Backend expects all -Geometry- shaders in DirectX Bytecode \"DXBC\" language!\n");

						return;
					}
				}

				result->m_VSBL = VertexShaderCode;
				result->m_PSBL = PixelShaderCode;
				result->m_GSBL = GeometryShaderCode;
								
				if (result->m_VSBL != nullptr)
				{	// encapsulate both shaders into shader Components
					if (FAILED(DX11Context::GetDevice()->CreateVertexShader(result->m_VSBL->GetBufferPointer(),
						result->m_VSBL->GetBufferSize(), 0, &result->m_vertexShader)))
					{
						Log->Info("[DX11Shader] Vertex Shader Creation Failed!\n");
						return;
					}

				}
				if (result->m_PSBL != nullptr)
				{
					if (FAILED(DX11Context::GetDevice()->CreatePixelShader(result->m_PSBL->GetBufferPointer(),
						result->m_PSBL->GetBufferSize(), 0, &result->m_pixelShader)))
					{
						Log->Info("[DX11Shader] Pixel Shader Creation Failed!\n");
						return;
					}
				}
				if (result->m_GSBL != nullptr)
				{
					if (FAILED(DX11Context::GetDevice()->CreateGeometryShader(result->m_GSBL->GetBufferPointer(),
						result->m_GSBL->GetBufferSize(), 0, &result->m_geometryShader)))
					{
						Log->Info("[DX11Shader] Geometry Shader Creation Failed!\n");
						return;
					}
				}
				return;
			}
			unsigned int DX11Shader::GetConstantBufferSlot(DX11ConstantBuffer * ubuffer, NuclearEngine::ShaderType type)
			{
				ID3D11ShaderReflection* pReflector = NULL;
				D3D11_SHADER_INPUT_BIND_DESC Desc;

				if (type == ShaderType::Vertex)
				{
					if (m_VSBL != nullptr)
					{
						D3DReflect(m_VSBL->GetBufferPointer(),
							m_VSBL->GetBufferSize(),
							IID_ID3D11ShaderReflection, (void**)&pReflector);

						if (SUCCEEDED(pReflector->GetResourceBindingDescByName(ubuffer->name, &Desc)))
						{
							pReflector->Release();
							return Desc.BindPoint;
						}
					}
				}
				else if (type == ShaderType::Pixel)
				{
					if (m_PSBL != nullptr)
					{
						D3DReflect(m_PSBL->GetBufferPointer(),
							m_PSBL->GetBufferSize(),
							IID_ID3D11ShaderReflection, (void**)&pReflector);

						if (SUCCEEDED(pReflector->GetResourceBindingDescByName(ubuffer->name, &Desc)))
						{
							return Desc.BindPoint;
						}
					}
				}
				else if (type == ShaderType::Geometry)
				{
					if (m_PSBL != nullptr)
					{
						D3DReflect(m_GSBL->GetBufferPointer(),
							m_GSBL->GetBufferSize(),
							IID_ID3D11ShaderReflection, (void**)&pReflector);

						if (SUCCEEDED(pReflector->GetResourceBindingDescByName(ubuffer->name, &Desc)))
						{
							return Desc.BindPoint;
						}
					}
				}

				Log->Warning(std::string("[DirectX] GetConstantBufferSlot for: \"" + std::string(ubuffer->name) + "\" failed, Returning 0 as default.\n"));
				return 0;
			}

			void DX11Shader::SetConstantBuffer(DX11ConstantBuffer* ubuffer, NuclearEngine::ShaderType type)
			{
				if (type == ShaderType::Vertex)
				{
					DX11Context::GetContext()->VSSetShader(m_vertexShader, 0, 0);
					DX11Context::GetContext()->VSSetConstantBuffers(this->GetConstantBufferSlot(ubuffer,type), 1, &ubuffer->buffer);
				}
				else if (type == ShaderType::Pixel)
				{
					DX11Context::GetContext()->PSSetShader(m_pixelShader, 0, 0);
					DX11Context::GetContext()->PSSetConstantBuffers(this->GetConstantBufferSlot(ubuffer, type), 1, &ubuffer->buffer);
				}
				else if (type == ShaderType::Geometry)
				{
					DX11Context::GetContext()->GSSetShader(m_geometryShader, 0, 0);
					DX11Context::GetContext()->GSSetConstantBuffers(this->GetConstantBufferSlot(ubuffer, type), 1, &ubuffer->buffer);
				}

				return;
			}
			
			void DX11Shader::Bind()
			{
				// set the shader Components
				if (m_vertexShader != nullptr)
				{
					DX11Context::GetContext()->VSSetShader(m_vertexShader, 0, 0);
				}
				if (m_pixelShader != nullptr)
				{
					DX11Context::GetContext()->PSSetShader(m_pixelShader, 0, 0);
				}
				if (m_geometryShader != nullptr)
				{
					DX11Context::GetContext()->GSSetShader(m_geometryShader, 0, 0);
				}
			}

			void DX11Shader::CheckShaderErrors(ID3D10Blob* Shader)
			{
				char* compileErrors;

				// Get a pointer to the error message text Buffer.
				compileErrors = (char*)(Shader->GetBufferPointer());

				Log->Info(compileErrors);

				// Release the error message.
				Shader->Release();
				Shader = 0;

				return;
			}
		}
	}
}

#endif