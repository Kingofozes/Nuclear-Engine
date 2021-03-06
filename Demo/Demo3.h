#pragma once
#include "Common.h"

struct Shader_Uniforms_t {
	Math::Matrix4 Model;
	Math::Matrix4 View;
	Math::Matrix4 Projection;
}Shader_Uniforms;

class Demo3 : public Core::Game
{
protected:
	API::Shader CubeShader;
	API::VertexBuffer CubeVB;
	API::ConstantBuffer CubeCB;
	API::Texture WoodenBoxTex;
public:
	Demo3()
	{
	}
	void Load()
	{

		API::Shader::Create(&CubeShader,
		&API::CompileShader(Core::FileSystem::LoadFileToString("Assets/Demo3/Shaders/CubeShader.vs").c_str(), ShaderType::Vertex, ShaderLanguage::HLSL),
		&API::CompileShader(Core::FileSystem::LoadFileToString("Assets/Demo3/Shaders/CubeShader.ps").c_str(), ShaderType::Pixel, ShaderLanguage::HLSL));

		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		VertexBufferDesc vDesc;
		vDesc.data = vertices;
		vDesc.size = sizeof(vertices);
		vDesc.usage = BufferGPUUsage::Dynamic;
		vDesc.access = BufferCPUAccess::Default;
		API::VertexBuffer::Create(&CubeVB, &vDesc);

		API::InputLayout CubeIL;
		CubeIL.Push("POSITION", 0, DataType::Float3);
		CubeIL.Push("TEXCOORD", 0, DataType::Float2);

		CubeVB.SetInputLayout(&CubeIL, &CubeShader);
		
		API::ConstantBuffer::Create(&CubeCB, "NE_Camera", sizeof(Shader_Uniforms));
		CubeShader.SetConstantBuffer(&CubeCB, ShaderType::Vertex);
	
		Texture_Desc Desc;
		Desc.Filter = TextureFilter::Trilinear;
		Desc.Wrap = TextureWrap::Repeat;
		Desc.Format = TextureFormat::R8G8B8A8;
		Desc.Type = TextureType::Texture2D;

		API::Texture::Create(&WoodenBoxTex,&ResourceManager::LoadTextureFromFile("Assets/Common/Textures/woodenbox.jpg", Desc), &Desc);

		Core::Context::EnableDepthBuffer(true);
		Core::Context::SetPrimitiveType(PrimitiveType::TriangleList);
	}

	void Render()
	{
		Core::Context::Begin();

		//Change Background Color to Blue in RGBA format
		Core::Context::Clear(API::Color(0.2f, 0.3f, 0.3f, 1.0f), ClearFlags::Depth);

		WoodenBoxTex.PSBind(0);
		CubeShader.Bind();
		CubeVB.Bind();

		Shader_Uniforms.Model = Math::Rotate(Math::Vector3(0.5f, 1.0f, 0.0f),5.0f);
		Shader_Uniforms.View = Math::Translate(Math::Vector3(0.0f, 0.0f, -3.0f));
		Shader_Uniforms.Projection = Math::Perspective(45.0f, (float)800 / (float)600, 0.1f, 100.0f);

		CubeCB.Update(&Shader_Uniforms, sizeof(Shader_Uniforms));

		Core::Context::Draw(36);

		Core::Context::End();
	}

};
