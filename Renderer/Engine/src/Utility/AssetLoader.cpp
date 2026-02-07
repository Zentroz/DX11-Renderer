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
#include <assimp/GltfMaterial.h>

// Model
#include<filesystem>
#include<Utility/TexturePacker.h>

bool MeshLoader::Load(zRender::MeshCPU& mesh, const std::string& path) {
	return true;
}

void TextureLoader::FlipImage(bool flip) {
	stbi_set_flip_vertically_on_load(flip);
}
bool TextureLoader::Load(zRender::TextureCPU& texture, const std::string& path) {
	texture.pixels = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channels, 4);
	return texture.pixels != nullptr;
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
namespace fs = std::filesystem;

ModelAsset::~ModelAsset() {
	Dispose();
}

void ModelAsset::Dispose() {
	if (mesh) {
		delete mesh;
		mesh = nullptr;
	}

	for (auto& pair : textures) {
		delete pair.second;
	}

	textures.clear();
}

bool LoadTextureFromAiScene(TextureCPU** pTexture, aiTextureType type, uint32_t index, const aiMaterial* material, const aiScene* scene, fs::path meshPath) {
	uint32_t count = material->GetTextureCount(type);
	if (index < count) {
		aiString pathStr;
		aiReturn ret = material->GetTexture(type, index, &pathStr);

		const char* path = pathStr.C_Str();

		if (ret == aiReturn_SUCCESS) {
			if (path[0] == '*') {
				(*pTexture) = new TextureCPU();

				int index = atoi(path + 1);
				aiTexture* texture = scene->mTextures[index];

				if (texture->mHeight != 0) {
					// Uncompressed embedded texture
					(*pTexture)->width = texture->mWidth;
					(*pTexture)->height = texture->mHeight;
					(*pTexture)->channels = 4;

					int pixelCount = texture->mWidth * texture->mHeight;
					(*pTexture)->pixels = new unsigned char[pixelCount * 4];

					aiTexel* src = texture->pcData;
					(*pTexture)->channels = 4;

					for (size_t i = 0; i < pixelCount; i++) {
						(*pTexture)->pixels[i * 4 + 0] = src[i].r;
						(*pTexture)->pixels[i * 4 + 1] = src[i].g;
						(*pTexture)->pixels[i * 4 + 2] = src[i].b;
						(*pTexture)->pixels[i * 4 + 3] = src[i].a;
					}
				}
				else {
					printf("Embedded compressed textures not supported!");
					return false;
					/*
					// Compressed embedded texture
					int width, height, channels;

					unsigned char* decodedPixels = stbi_load_from_memory(
						reinterpret_cast<unsigned char*>(texture->pcData),
						texture->mWidth,
						&width,
						&height,
						&channels,
						4
					);

					if (!decodedPixels)
					{
						printf("Failed to decode embedded texture\n");
						return false;
					}

					TextureCPU* tex = new TextureCPU();
					tex->width = width;
					tex->height = height;
					tex->channels = channels;
					tex->pixels = decodedPixels;

					(*pTexture) = tex;
				*/
				}
			}
			else {
				// External Texture
				std::string texturePath = "";

				if (!fs::exists(fs::path(path)))
					texturePath = (meshPath.parent_path() / path).string();
				else texturePath = path;

				if (!fs::exists(texturePath)) {
					printf("Failed to load texture - path is wrong: %s \n", path);
					return false;
				}

				(*pTexture) = new TextureCPU();
				TextureLoader loader;
				if (!loader.Load(**pTexture, texturePath)) {
					delete (*pTexture);
					(*pTexture) = nullptr;
					return false;
				}
				(*pTexture)->name = fs::path(texturePath).stem().string();
			}
		}
	}
	else return false;

	return true;
}

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
			bool isMaterialLoaded = true;

			for (auto& mat : model.materials) {
				if (mat.name == material->GetName().C_Str()) {
					isMaterialLoaded = false;
					mat.subMeshIndices.push_back(rawMesh->subMeshes.size());
					break;
				}
			}

			if (isMaterialLoaded) {
				ModelAsset::Material mat;
				mat.name = material->GetName().C_Str();
				mat.subMeshIndices.push_back(rawMesh->subMeshes.size());
				

				aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
				aiString aplhaMode;
				float roughness = 0;
				float metallic = 0;
				float opacity = 1;
				float aplhaCutoff = 0.5f;

				material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
				material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
				material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
				material->Get(AI_MATKEY_OPACITY, opacity);
				material->Get(AI_MATKEY_GLTF_ALPHACUTOFF, aplhaCutoff);

				mat.renderMode = ModelAsset::Material::RenderMode::Opaque;

				if (material->Get(AI_MATKEY_GLTF_ALPHAMODE, aplhaMode) == aiReturn_SUCCESS) {
					if (aplhaMode == aiString("MASK")) {
						mat.renderMode = ModelAsset::Material::RenderMode::AplhaTest;
					}
					else if (aplhaMode == aiString("BLEND")) {
						mat.renderMode = ModelAsset::Material::RenderMode::AplhaTest;
					}
				}

				mat.baseColor = vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
				mat.aplhaCutoff = aplhaCutoff;

				auto containsTextureWithName = [&](std::string name) {
					if (model.textures.contains(name)) return true;
					return false;
				};
				auto addTextureIfDoesntExistOtherwiseDelete = [&](TextureCPU* texture) {
					std::string name = texture->name;
					if (!model.textures.contains(texture->name)) model.textures[texture->name] = texture;
					else {
						delete texture;
					}
					return name;
				};

				TextureCPU* albedo = nullptr;
				if (LoadTextureFromAiScene(&albedo, aiTextureType_DIFFUSE, 0, material, scene, filepath)) {
					mat.albedoTextureName = addTextureIfDoesntExistOtherwiseDelete(albedo);
					albedo->filterMode = TextureCPU::Linear;
				}
				TextureCPU* normal = nullptr;
				if (LoadTextureFromAiScene(&normal, aiTextureType_NORMALS, 0, material, scene, filepath)) {
					normal->filterMode = TextureCPU::FilterMode::Linear;
					mat.normalTextureName = addTextureIfDoesntExistOtherwiseDelete(normal);
				}

				TextureCPU* packedORM = nullptr;
				LoadTextureFromAiScene(&packedORM, aiTextureType_UNKNOWN, 0, material, scene, filepath);

				if (packedORM) {
					// // Combined Textures RM Format
					mat.roughnessFactor = 1;
					mat.metallicFactor = 1;

					mat.rmTextureName = addTextureIfDoesntExistOtherwiseDelete(packedORM);
				}
				else {
					TextureCPU* rough = nullptr;
					TextureCPU* metal = nullptr;

					LoadTextureFromAiScene(&metal, aiTextureType_METALNESS, 0, material, scene, filepath);
					LoadTextureFromAiScene(&rough, aiTextureType_DIFFUSE_ROUGHNESS, 0, material, scene, filepath);

					if (rough && metal) {
						mat.roughnessFactor = 1;
						mat.metallicFactor = 1;

						if (rough->name == metal->name) {
							mat.rmTextureName = addTextureIfDoesntExistOtherwiseDelete(rough);
						}
						else {
							mat.rmTextureName = addTextureIfDoesntExistOtherwiseDelete(PackTextureRM(rough, metal));
						}
					}
					else if (rough || metal) {
						TextureCPU* orm = rough ? rough : metal;
						mat.rmTextureName = addTextureIfDoesntExistOtherwiseDelete(packedORM);

						TextureCPU* textureToDelete = (!rough) ? rough : metal;
						delete textureToDelete;
					}
					else {
						mat.roughnessFactor = roughness;
						mat.metallicFactor = metallic;
					}
				}

				if (!mat.rmTextureName.empty()) {
					model.textures[mat.rmTextureName]->filterMode = TextureCPU::FilterMode::Point;
				}

				model.materials.push_back(mat);
			}

			subMesh.name = node->mName.C_Str();

			rawMesh->subMeshes.push_back(subMesh);
		}
	}

	model.mesh = rawMesh;

	return true;
}