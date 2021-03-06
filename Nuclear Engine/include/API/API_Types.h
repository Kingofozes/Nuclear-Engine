#pragma once
#include <string>

namespace NuclearEngine {

	enum class DataType { Float, Float2, Float3, Float4 };

	enum class Comparison_Func {
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_EQUAL,
		ALWAYS
	};
	enum class Stencil_OP
	{
		KEEP,
		ZERO,
		REPLACE,
		INCREASE_SAT,
		DECREASE_SAT,
		INVERT,
		INCREASE,
		DECREASE
	};

	struct Stencil_Face_Desc {
		Stencil_OP StencilFailOp = Stencil_OP::KEEP;
		Stencil_OP StencilDepthFailOp = Stencil_OP::KEEP;
		Stencil_OP StencilPassOp = Stencil_OP::KEEP;
		Comparison_Func StencilFunc = Comparison_Func::NOT_EQUAL;
	};

	struct DepthStencilStateDesc {
		bool DepthStencilEnabled = false;
		bool DepthMaskEnabled = false;
		Comparison_Func DepthFunc = Comparison_Func::LESS;

		unsigned char StencilReadMask = 0xFF;
		unsigned char StencilWriteMask = 0xFF;

		Stencil_Face_Desc StencilFrontFace;
		Stencil_Face_Desc StencilBackFace;
	};


	enum class NEAPI ClearFlags
	{
		None,
		Depth,
		Stencil,
		Depth_Stencil
	};

	enum class ShaderLanguage {
		HLSL,
		DXBC,
		GLSL
	};

	enum class ShaderType {
		Vertex,
		Pixel,
		Geometry,

		Vertex_Pixel,
		Vertex_Geometry,
		Vertex_Pixel_Geometry
	};


	struct BinaryShaderBlob {
		void* GetBufferPointer()
		{
			return this->hlslsourcecode;
		}
		size_t GetBufferSize()
		{
			return this->hlslsize;
		}

		void* hlslsourcecode;
		size_t hlslsize;

		std::string glslsourcecode;
		ShaderLanguage Language;
	};

	enum class TextureFormat { R8 = 8, R8G8 = 16, R8G8B8 = 24, R8G8B8A8 = 32 };
	enum class TextureWrap { Repeat, MirroredReapeat, ClampToEdge, ClampToBorder };
	enum class TextureFilter { Point2D, Linear2D, Point, Bilinear, Trilinear };
	enum class TextureType { Texture1D, Texture2D, Texture3D, TextureCube };
	enum class AnisotropicFilter { None, AnisotropicX2, AnisotropicX4, AnisotropicX8, AnisotropicX16 };
	enum class RenderTargetPrecision { Float, Half_Float, Unsigned_Int };

	struct Texture_Desc
	{
		TextureType Type = TextureType::Texture2D;
		TextureFormat Format = TextureFormat::R8G8B8A8;
		TextureWrap Wrap = TextureWrap::ClampToEdge;
		TextureFilter Filter = TextureFilter::Point;
		AnisotropicFilter AnisoFilter = AnisotropicFilter::None;
	};

	struct RenderTarget_Attachment_Desc
	{
		TextureFormat Format = TextureFormat::R8G8B8A8;
		TextureFilter Filter = TextureFilter::Point2D;
		RenderTargetPrecision precision = RenderTargetPrecision::Float;
		bool Read;
		int width;
		int height;
	};



	struct Texture_Data {
		int width;
		int height;
		int depth;
		int no_of_components;
		unsigned char *databuf;
	};

	enum class BufferGPUUsage {
		Default,
		Static,
		Dynamic
	};

	enum class BufferCPUAccess {
		Default,
		ReadOnly,
		WriteOnly
	};
	struct VertexBufferDesc {
		const void* data;
		unsigned int size;
		BufferGPUUsage usage = BufferGPUUsage::Default;
		BufferCPUAccess access = BufferCPUAccess::Default;
	};
	enum class Format
	{
		R8,
		R8G8,
		R8G8B8,
		R8G8B8A8,

		R16,
		R16G16,
		R16G16B16,
		R16G16B16A16,

		R32,
		R32G32,
		R32G32B32,
		R32G32B32A32
	};
}