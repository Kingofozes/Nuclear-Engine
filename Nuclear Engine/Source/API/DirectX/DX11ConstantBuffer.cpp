#include <API\DirectX\DX11ConstantBuffer.h>

#ifdef NE_COMPILE_DIRECTX11
#include <API\DirectX\DX11Context.h>
#include <API\API_Types.h>

namespace NuclearEngine
{
	namespace API
	{
		namespace DirectX
		{
			DX11ConstantBuffer::DX11ConstantBuffer()
			{
				name = "";
				buffer = nullptr;
			}
			DX11ConstantBuffer::~DX11ConstantBuffer()
			{
				if (buffer != nullptr)
				{
					buffer->Release();
				}

				buffer = nullptr;
			}
			void DX11ConstantBuffer::Create(DX11ConstantBuffer* result, const char * Nameinshader, unsigned int size)
			{
				int remainder = size % 16;
				if (remainder != 0)
				{
					Log->Warning("[DX11ConstantBuffer] The size of buffer isn't a multiple of 16 which can cause many unexpected problems! \n");
				}

				result->name = Nameinshader;

				D3D11_BUFFER_DESC bufferDesc;
				ZeroMemory(&bufferDesc, sizeof(bufferDesc));

				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.ByteWidth = size;
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.CPUAccessFlags = 0;
				bufferDesc.MiscFlags = 0;

				DX11Context::GetDevice()->CreateBuffer(&bufferDesc, NULL, &result->buffer);
			}

			void DX11ConstantBuffer::Update(const void * data, unsigned int size)
			{
				DX11Context::GetContext()->UpdateSubresource(buffer, 0, NULL, data, 0, 0);
			}
			
		}
	}
}
#endif