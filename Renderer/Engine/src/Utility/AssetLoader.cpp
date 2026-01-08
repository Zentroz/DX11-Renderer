#include"Utility/AssetLoader.h"
#include<stb_image/stb_image.h>
// Shader
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool MeshLoader::Load(zRender::MeshCPU& mesh, const std::string& path) {
	return true;
}

void TextureLoader::FlipImage(bool flip) {
	stbi_set_flip_vertically_on_load(flip);
}
bool TextureLoader::Load(zRender::TextureCPU& texture, const std::string& path) {
	texture.pixels = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channels, 4);
	return true;
}

bool ShaderLoader::Load(zRender::ShaderCPU& shader, const std::string& path) {
	std::ifstream file(path);
	std::string file_contents;

	if (file.is_open()) {
		// Read the entire file buffer into an ostringstream
		std::ostringstream buffer;
		buffer << file.rdbuf();

		// Get the string from the stringstream
		file_contents = buffer.str();

		file.close();
	}
	else {
		printf("Unable to load shader.");
		return false;
	}

	shader.vertexShaderSrc = file_contents.c_str();
	shader.pixelShaderSrc = file_contents.c_str();

	return true;
}

using namespace zRender;

ModelAsset::~ModelAsset() {}

bool ModelLoader::Load(ModelAsset& model, const std::string& filepath) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipWindingOrder |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GlobalScale
	);

	if (scene == nullptr) return false;

	model.name = scene->mRootNode->mName.C_Str();

	aiMatrix4x4 localTransform = scene->mRootNode->mTransformation;

	aiVector3D scaling;
	aiQuaternion rotation;
	aiVector3D position;

	localTransform.Decompose(scaling, rotation, position);

	model.modelMatrix = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) *
		DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w)) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	uint64_t totalVertexCount = 0;
	uint64_t totalIndexCount = 0;

	MeshCPU* rawMesh = new MeshCPU();

	for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++) {
		// Node Processing
		aiNode* node = scene->mRootNode->mChildren[i];
		//std::string name = node->mName.C_Str();

		localTransform = node->mTransformation;
		localTransform.Decompose(scaling, rotation, position);

		// Mesh Processing
		for (unsigned int y = 0; y < node->mNumMeshes; y++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[y]];

			SubMesh subMesh{};

			subMesh.localModel = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) *
				DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w)) *
				DirectX::XMMatrixTranslation(position.x, position.y, position.z);

			subMesh.vertexOffset = totalVertexCount;
			subMesh.indexOffset = totalIndexCount;

			for (unsigned int z = 0; z < mesh->mNumVertices; z++)
			{
				Vertex vertex;

				aiVector3D position = mesh->mVertices[z];
				vertex.position = { position.x, position.y, position.z };
				// Store vertex.x, vertex.y, vertex.z

				// Normals
				if (mesh->HasNormals()) // Check if normals exist
				{
					aiVector3D normal = mesh->mNormals[z];
					vertex.normal = { normal.x, normal.y, normal.z };
					// Store normal.x, normal.y, normal.z
				}

				// UV Coordinates (Texture Coordinates) - Assimp supports multiple UV channels (up to 4)
				if (mesh->HasTextureCoords(0)) // Check for the first UV channel
				{
					aiVector3D uv = mesh->mTextureCoords[0][z];
					vertex.uv = { uv.x, uv.y };
					// Store uv.x, uv.y. The z component can be ignored for 2D textures.
				}

				if (mesh->HasTangentsAndBitangents()) {
					aiVector3D tangent = mesh->mTangents[z];
					vertex.tangent = { tangent.x, tangent.y, tangent.z };
				}

				rawMesh->vertices.push_back(vertex);

				totalVertexCount++;
			}

			for (unsigned int z = 0; z < mesh->mNumFaces; z++)
			{
				const aiFace& face = mesh->mFaces[z];

				for (unsigned int j = 0; j < face.mNumIndices; j++)
				{
					rawMesh->indices.push_back(face.mIndices[j]);
					totalIndexCount++;
				}
			}

			subMesh.vertexCount = totalVertexCount - subMesh.vertexOffset;
			subMesh.indexCount = totalIndexCount - subMesh.indexOffset;

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			bool createMaterial = true;

			for (auto& mat : model.materials) {
				if (mat.name == material->GetName().C_Str()) {
					createMaterial = false;
					mat.subMeshIndices.push_back(rawMesh->subMeshes.size());
					break;
				}
			}

			if (createMaterial) {
				aiString* texturePath = nullptr;
				aiTexture* diffuseTexture = nullptr;
				/*
				aiReturn ret = material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePath);

				if (texturePath->data[0] == '*') {
					// Embedded Texture
				}
				else {
					// External texture
				}
				*/

				aiTexture normalTexture;
				aiTexture metallicTexture;
				aiTexture rougnessTexture;
				aiColor3D baseColor(0.0f, 0.0f, 0.0f);
				float rougness = 0;
				float metallic = 0;
				material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
				material->Get(AI_MATKEY_ROUGHNESS_FACTOR, rougness);
				material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);

				ModelAsset::Material mat;
				mat.name = material->GetName().C_Str();
				mat.subMeshIndices.push_back(rawMesh->subMeshes.size());
				mat.baseColor = vec4(baseColor.r, baseColor.g, baseColor.b, 1);
				mat.roughness = rougness;
				mat.metallic = metallic;

				model.materials.push_back(mat);
			}

			subMesh.name = mesh->mName.C_Str();

			rawMesh->subMeshes.push_back(subMesh);
		}
	}

	model.mesh = rawMesh;

	return true;
}