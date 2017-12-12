#pragma once
#include "Common.h"

class Demo4 : public Core::Game
{
protected:
	API::Shader *CubeShader;
	API::Texture *WoodenBoxTex;
	Components::FlyCamera *Camera;

	Components::Cube *Cube;

	float lastX = _Width_ / 2.0f;
	float lastY = _Height_ / 2.0f;
	bool firstMouse = true;

public:
	Demo4()
	{
	}
	void Load()
	{
		CubeShader = new API::Shader("CubeShader",
			&API::CompileShader(Core::FileSystem::LoadFileToString("Assets/Demo4/Shaders/CubeShader.vs").c_str(), ShaderType::Vertex, ShaderLanguage::HLSL),
			&API::CompileShader(Core::FileSystem::LoadFileToString("Assets/Demo4/Shaders/CubeShader.ps").c_str(), ShaderType::Pixel, ShaderLanguage::HLSL));
		
		Camera = new Components::FlyCamera();
		Camera->Initialize(Math::Perspective(Math::ToRadians(45.0f), (float)800 / (float)600, 0.1f, 100.0f));
		CubeShader->SetUniformBuffer(Camera->GetCBuffer() ,0, ShaderType::Vertex);

		Texture_Desc Desc;
		Desc.Filter = TextureFilter::Trilinear;
		Desc.Wrap = TextureWrap::Repeat;
		Desc.Format = TextureFormat::R8G8B8A8;
		Desc.Type = TextureType::Texture2D;

		WoodenBoxTex = new API::Texture(ResourceManager::LoadTextureFromFile("Assets/Common/Textures/woodenbox.jpg", Desc), Desc);
		
		Shading::Material CubeMat;
		CubeMat.Diffuse = WoodenBoxTex;
		Cube = new Components::Cube(Components::InputSignatures::Position_Texcoord, &CubeMat);

		Core::Context::EnableDepthBuffer(true);

		Core::Context::SetPrimitiveType(PrimitiveType::TriangleList);
	}

	void PreRender(float deltatime) override
	{
		if (Input::Keyboard::IsKeyPressed(Input::Keyboard::Key::W))
			Camera->ProcessMovement(Components::Camera_Movement::FORWARD, deltatime);
		if (Input::Keyboard::IsKeyPressed(Input::Keyboard::Key::A))
			Camera->ProcessMovement(Components::Camera_Movement::LEFT, deltatime);
		if (Input::Keyboard::IsKeyPressed(Input::Keyboard::Key::S))
			Camera->ProcessMovement(Components::Camera_Movement::BACKWARD, deltatime);
		if (Input::Keyboard::IsKeyPressed(Input::Keyboard::Key::D))
			Camera->ProcessMovement(Components::Camera_Movement::RIGHT, deltatime);
		
		Camera->Update();		
	}

	void MouseMovementCallback(double xpos, double ypos) override
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		Camera->ProcessEye(xoffset, yoffset);
	}

	void Render()
	{
		Core::Context::Begin();

		//Change Background Color to Blue in RGBA format
		Core::Context::ClearColor(API::Color(0.2f, 0.3f, 0.3f, 1.0f));
		//Don't Forget to clear the depth buffer each frame
		Core::Context::ClearDepthBuffer();

	 	CubeShader->Bind();

		Cube->Draw(CubeShader);

		CubeShader->Unbind();

		Core::Context::End();
	}
	void ExitRendering()	// Exit Rendering
	{
		delete CubeShader;
		delete Cube;
	    delete Camera;
		delete WoodenBoxTex;
	}
};