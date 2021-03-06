#include "Components\Mesh.h"
#include "Core\Context.h"
#include <API\InputLayout.h>

namespace NuclearEngine {

	namespace Components {
		/*  Functions  */

		unsigned int GetVerticesSize(InputSignatures Signature)
		{
			switch (Signature)
			{
			case InputSignatures::Position: return 12;
			case InputSignatures::Position_Texcoord: return 20;
			case InputSignatures::Position_Normal_Texcoord: return 32;
			default: return 32;
			}
		}

		// constructor
		Mesh::Mesh(std::vector<float> _vertices, std::vector<unsigned int> _indices, std::vector<MaterialTexture> _textures, InputSignatures Signature)
		{
			Mesh_NoIndices::Mesh_NoIndices(_vertices, _textures, Signature);

			this->indices = _indices;
			API::IndexBuffer::Create(&IBO, indices.data(), indices.size() * 4);
		}
		void Mesh::Draw(API::Shader* _shader)
		{
			if (this->RenderInit == false)
			{
				API::InputLayout layout;
				layout.Push("POSITION", 0, DataType::Float3);

				if (signature == InputSignatures::Position_Texcoord)
				{
					layout.Push("TEXCOORD", 0, DataType::Float2);
				}
				else if (signature == InputSignatures::Position_Normal_Texcoord)
				{
					layout.Push("NORMAL", 0, DataType::Float3);
					layout.Push("TEXCOORD", 0, DataType::Float2);
				}
				VBO.SetInputLayout(&layout, _shader);

				RenderInit = true;
			}

			for (unsigned int i = 0; i < textures.size(); i++)
			{
				textures[i].tex->PSBind(TextureBindings[i], _shader, i);
			}


			// draw mesh
			_shader->Bind();
			VBO.Bind();
			IBO.Bind();
			Core::Context::DrawIndexed(indices.size());
		}

		// render the mesh
		void Mesh_NoIndices::PreCalculate_TextureBindings()
		{	
			for (unsigned int i = 0; i < textures.size(); i++)
			{

				std::string number;
				std::string name;

				if (textures[i].type == MaterialTextureType::Diffuse)
				{
					name = "NE_Tex_Diffuse";
					number = std::to_string(diffuseNr++);
				}
				else if (textures[i].type == MaterialTextureType::Specular)
				{
					name = "NE_Tex_Specular";
					number = std::to_string(specularNr++); // transfer unsigned int to stream
				}
				
				TextureBindings.push_back((name + number).c_str());
			}
		}


		Mesh_NoIndices::Mesh_NoIndices()
		{
		}

		Mesh_NoIndices::Mesh_NoIndices(std::vector<float> _vertices, std::vector<MaterialTexture> _textures, InputSignatures Signature)
		{
			this->vertices = _vertices;
			this->textures = _textures;

			this->signature = Signature;

			VertexBufferDesc desc;
			desc.data = vertices.data();
			desc.size = (unsigned int)vertices.size() * sizeof(float);
			desc.usage = BufferGPUUsage::Dynamic;
			desc.access = BufferCPUAccess::Default;
			
			API::VertexBuffer::Create(&VBO,&desc);

			PreCalculate_TextureBindings();

			RenderInit = false;
		}

		void Mesh_NoIndices::Draw(API::Shader * shader)
		{

			if (this->RenderInit == false)
			{
				API::InputLayout layout;
				layout.Push("POSITION", 0, DataType::Float3);

				if (signature == InputSignatures::Position_Texcoord)
				{
					layout.Push("TEXCOORD", 0, DataType::Float2);
				}
				else if (signature == InputSignatures::Position_Normal_Texcoord)
				{
					layout.Push("NORMAL", 0, DataType::Float3);
					layout.Push("TEXCOORD", 0, DataType::Float2);
				}
				VBO.SetInputLayout(&layout, shader);

				RenderInit = true;
			}

			for (unsigned int i = 0; i < textures.size(); i++)
			{
				textures[i].tex->PSBind(TextureBindings[i], shader, i);
			}

			// draw mesh
			shader->Bind();
			VBO.Bind();
			Core::Context::Draw(vertices.size());
		}
		
	}
}