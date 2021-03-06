#include <API\DirectX\DX11Context.h>

#ifdef NE_COMPILE_DIRECTX11
#include <Core\Application.h>

#include <wrl.h>
template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxguid.lib")

namespace NuclearEngine
{
	namespace API
	{
		namespace DirectX
		{
			static ComPtr<ID3D11Device1> Device;
			static ComPtr<ID3D11DeviceContext1> Context;
			static ComPtr<IDXGISwapChain1> SwapChain;
			static D3D_PRIMITIVE_TOPOLOGY D3DPrimitiveType;
			static ID3D11RenderTargetView* RenderTarget;
			static ComPtr<ID3D11Device> _Device;
			static ComPtr<ID3D11DeviceContext> _Context;
			static ComPtr<IDXGISwapChain> _SwapChain;
			static ID3D11Texture2D *depthBuffer;
			static ID3D11DepthStencilState* m_depthStencilState;
			static ID3D11DepthStencilView* m_depthStencilView;
			static ID3D11RasterizerState* m_rasterState;
			static bool vsync;

			bool DX11Context::Initialize()
			{
				Log->Info("[Engine] Initializing DirectX 11 Renderer.\n");
				IDXGIFactory1* factory;
				IDXGIAdapter1* adapter;
				IDXGIOutput* adapterOutput;
				unsigned int numModes, i, numerator, denominator, stringLength;
				DXGI_MODE_DESC* displayModeList;
				DXGI_ADAPTER_DESC adapterDesc;
				char m_videoCardDescription[128];

				// Create a DirectX graphics interface factory.
				if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
				{
					return false;
				}

				// Use the factory to create an adapter for the primary graphics interface (video card).
				if (FAILED(factory->EnumAdapters1(0, &adapter)))
				{
					return false;
				}

				// Enumerate the primary adapter output (monitor).
				if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
				{
					return false;
				}

				// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
				if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
				{
					return false;
				}

				// Create a list to hold all the possible display modes for this monitor/video card combination.
				displayModeList = new DXGI_MODE_DESC[numModes];
				if (!displayModeList)
				{
					return false;
				}

				// Now fill the display mode list structures.
				if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
				{
					return false;
				}

				// Now go through all the display modes and find the one that matches the screen width and height.
				// When a match is found store the numerator and denominator of the refresh rate for that monitor.
				for (i = 0; i < numModes; i++)
				{
					if (displayModeList[i].Width == (unsigned int)Core::Application::GetWidth())
					{
						if (displayModeList[i].Height == (unsigned int)Core::Application::GetHeight())
						{
							numerator = displayModeList[i].RefreshRate.Numerator;
							denominator = displayModeList[i].RefreshRate.Denominator;
						}
					}
				}
				// Get the adapter (video card) description.
				if (FAILED(adapter->GetDesc(&adapterDesc)))
				{
					return false;
				}


				// Convert the name of the video card to a character array and store it
				if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
				{
					return false;
				}
				factory->Release();

				DXGI_SWAP_CHAIN_DESC SwapChainDesc;
				ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

				SwapChainDesc.BufferCount = 1;
				SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

				//set refresh rate
				SwapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
				SwapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;

				SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				SwapChainDesc.OutputWindow = Core::Application::GetInternalWindow()->GetHandle();
				SwapChainDesc.SampleDesc.Count = 1;                               // how many multisamples
				SwapChainDesc.SampleDesc.Quality = 0;                             // multisample quality level
				SwapChainDesc.Windowed = TRUE;
				// Set the scan line ordering and scaling to unspecified.
				SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

				// Discard the back buffer contents after presenting.
				SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

				// Don't set the advanced flags.
				SwapChainDesc.Flags = 0;

				HRESULT result;

				D3D_FEATURE_LEVEL FeatureLevelsRequested[]
				{
					D3D_FEATURE_LEVEL_10_1,
				};

				UINT creationFlags = 0;

#ifdef _DEBUG
				creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
				result = D3D11CreateDeviceAndSwapChain(
					adapter,
					D3D_DRIVER_TYPE_UNKNOWN,
					NULL,
					creationFlags, // | D3D11_CREATE_DEVICE_BGRA_SUPPORT,   If there is an error please uncomment this  URGENT
					FeatureLevelsRequested,
					1,
					D3D11_SDK_VERSION,
					&SwapChainDesc,
					&_SwapChain,
					&_Device,
					NULL,
					&_Context);

				if (SUCCEEDED(_Device.As(&Device)))
				{
					(void)_Context.As(&Context);
					(void)_SwapChain.As(&SwapChain);
				}

				if (FAILED(result))
				{

					Log->FatalError("[DirectX] Create Device And Swap Chain Failed!!\n");
					//Release the Adapter interface
					adapter->Release();
					return false;
				}


				//TODO Redo the dwrite backend for renderering fonts
				//GUI::Internals::InitializeDWrite(adapter);

				//Release the Adapter interface
				adapter->Release();


				ID3D11Texture2D *backBuffer;
				ZeroMemory(&backBuffer, sizeof(ID3D11Texture2D));

				SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

				if (FAILED(Device->CreateRenderTargetView(backBuffer, NULL, &RenderTarget)))
				{
					backBuffer->Release();
					Log->FatalError("[DirectX] Create BackBuffer Failed!!.\n");
					return false;

				}
				backBuffer->Release();

				D3D11_TEXTURE2D_DESC depthBufferDesc;
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
				D3D11_RASTERIZER_DESC rasterDesc;

				ZeroMemory(&depthBuffer, sizeof(depthBuffer));
				ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

				// Set up the description of the depth buffer.
				depthBufferDesc.Width = Core::Application::GetInternalWindow()->GetWidth();
				depthBufferDesc.Height = Core::Application::GetInternalWindow()->GetHeight();
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.ArraySize = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depthBufferDesc.CPUAccessFlags = 0;
				depthBufferDesc.MiscFlags = 0;

				// Create the texture for the depth buffer using the filled out description.
				result = Device->CreateTexture2D(&depthBufferDesc, NULL, &depthBuffer);
				if (FAILED(result))
				{
					Log->FatalError("[DirectX] Create Depth Buffer Failed!!\n");

					return false;
				}

				// Initialize the description of the stencil state.
				ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

				// Set up the description of the stencil state.
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

				depthStencilDesc.StencilEnable = true;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;

				// Stencil operations if pixel is front-facing.
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				// Stencil operations if pixel is back-facing.
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				// Create the depth stencil state.
				result = Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
				if (FAILED(result))
				{
					Log->FatalError("[DirectX] Create Depth Stencil State Failed!!.\n");

					return false;
				}

				// Set the depth stencil state.
				Context->OMSetDepthStencilState(m_depthStencilState, 1);

				// Initialize the depth stencil view.
				ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

				// Set up the depth stencil view description.
				depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				depthStencilViewDesc.Texture2D.MipSlice = 0;

				// Create the depth stencil view.
				result = Device->CreateDepthStencilView(depthBuffer, &depthStencilViewDesc, &m_depthStencilView);
				if (FAILED(result))
				{
					Log->FatalError("[DirectX] Create Depth Stencil View Failed!!.\n");

					return false;
				}

				// Bind the render target view and depth stencil buffer to the output render pipeline.
				Context->OMSetRenderTargets(1, &RenderTarget, m_depthStencilView);

				// Setup the raster description which will determine how and what polygons will be drawn.
				rasterDesc.AntialiasedLineEnable = false;
				rasterDesc.CullMode = D3D11_CULL_NONE;
				rasterDesc.DepthBias = 0;
				rasterDesc.DepthBiasClamp = 0.0f;
				rasterDesc.DepthClipEnable = true;
				rasterDesc.FillMode = D3D11_FILL_SOLID;
				rasterDesc.FrontCounterClockwise = true;
				rasterDesc.MultisampleEnable = false;
				rasterDesc.ScissorEnable = false;
				rasterDesc.SlopeScaledDepthBias = 0.0f;
				// Create the rasterizer state from the description we just filled out.
				result = Device->CreateRasterizerState(&rasterDesc, &m_rasterState);
				if (FAILED(result))
				{
					Log->FatalError("[DirectX] Create Rasterizer State Failed!!.\n");

					return false;
				}

				// Now set the rasterizer state.
				Context->RSSetState(m_rasterState);

				D3D11_VIEWPORT viewPort;
				ZeroMemory(&viewPort, sizeof(viewPort));

				viewPort.TopLeftX = 0;
				viewPort.TopLeftY = 0;
				viewPort.Width = Core::Application::GetInternalWindow()->GetWidth();
				viewPort.Height = Core::Application::GetInternalWindow()->GetHeight();
				viewPort.MinDepth = 0.0f;
				viewPort.MaxDepth = 1.0f;
				Context->RSSetViewports(1, &viewPort);

				//TODO Redo the dwrite backend for renderering fonts
				//GUI::Internals::InitializeD2DResources();

				D3D11_FEATURE_DATA_D3D11_OPTIONS fl;
				Device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &fl, sizeof(fl));

				//if (fl.ConstantBufferPartialUpdate != TRUE || fl.ConstantBufferOffsetting != TRUE)
				//{
				//	Log->FatalError("[DirectX] Your GPU doesn't support constant buffer Partial update which is required to run the Nuclear Engine, Please Update your Drivers.\n");
				//	return false;
				//}

				Log->Info("[DirectX] Successfully Initialized\n");
				Log->Info("[DirectX] Version: 11.1\n");
				Log->Info("[DirectX] Vendor: ");
				Log->Info(m_videoCardDescription);
				Log->Info("\n");

				return true;
			}

			uint GetClearFlags(ClearFlags flags)
			{
				switch (flags)
				{
				case ClearFlags::Depth:
					return D3D11_CLEAR_DEPTH;
				case ClearFlags::Stencil:
					return D3D11_CLEAR_STENCIL;
				case ClearFlags::Depth_Stencil:
					return (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
				}

				return 0;
			}

			void DX11Context::Clear(API::Color color, ClearFlags flags, float depth, float stencil)
			{
				float Colors[4] = {
					color.r,color.g,color.b,color.a
				};

				Context->ClearRenderTargetView(RenderTarget, Colors);

				if (flags != ClearFlags::None)
				{
					Context->ClearDepthStencilView(m_depthStencilView, GetClearFlags(flags), depth, stencil);
				}
				return;
			}

			void DX11Context::EnableDepthBuffer(bool state)
			{
				if (state)
					Context->OMSetRenderTargets(1, &RenderTarget, m_depthStencilView);
				else
					Context->OMSetRenderTargets(1, &RenderTarget, 0);
			}

			void DX11Context::SwapBuffers()
			{
				if (vsync)
					SwapChain->Present(1, 0);
				else
					SwapChain->Present(0, 0);
			}

			void DX11Context::Shutdown()
			{
				//GUI::Internals::ShutdownDWrite();
				depthBuffer->Release();
				m_depthStencilState->Release();
				m_depthStencilView->Release();
				m_rasterState->Release();
				RenderTarget->Release();
				SwapChain.Reset();
				Context.Reset();
				Device.Reset();

			}

			void DX11Context::Draw(unsigned int count)
			{
				Context->Draw(count, 0);
			}

			void DX11Context::DrawIndexed(unsigned int vertexCount)
			{
				Context->DrawIndexed(vertexCount, 0, 0);
			}

			void DX11Context::SetPrimitiveType(int primitiveType)
			{
				switch (primitiveType)
				{
				case 0:
				{
					D3DPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
					break;
				}
				case 1:
				{
					D3DPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
					break;
				}
				case 2:
				{
					D3DPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
					break;
				}
				case 3:
				{
					D3DPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					break;
				}
				case 4:
				{
					D3DPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
					break;
				}
				default:
					break;
				}
				Context->IASetPrimitiveTopology(D3DPrimitiveType);
			}

			ID3D11Device1* DX11Context::GetDevice()
			{
				return Device.Get();
			}

			ID3D11DeviceContext1* DX11Context::GetContext()
			{
				return Context.Get();
			}

			void DX11Context::SetViewPort(int x, int y, int width, int height)
			{
				D3D11_VIEWPORT viewPort;
				ZeroMemory(&viewPort, sizeof(viewPort));

				viewPort.TopLeftX = x;
				viewPort.TopLeftY = y;
				viewPort.Width = width;
				viewPort.Height = height;
				viewPort.MinDepth = 0.0f;
				viewPort.MaxDepth = 1.0f;
				Context->RSSetViewports(1, &viewPort);
			}
		}
	}
}

#endif