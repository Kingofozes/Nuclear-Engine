#pragma once
#include <NE_Common.h>
#include <API\Texture.h>
#include <API\Color.h>
#include <array>
namespace NuclearEngine {
	class NEAPI ResourceManager {
	public:
		static Texture_Data LoadTextureFromFile(const char* filename, const Texture_Desc& Desc);

		/*
		Order:
		 +X (right)
		 -X (left)
		 +Y (top)
		 -Y (bottom)
		 +Z (front)
		 -Z (back)
		*/
		static  std::array<NuclearEngine::Texture_Data, 6> LoadTextureCubeFromFile(const std::array<std::string, 6 >& filenames, const Texture_Desc& Desc);

		static Texture_Data FillWithColor(API::Color color, int width,int height, const Texture_Desc& Desc);

	};
}