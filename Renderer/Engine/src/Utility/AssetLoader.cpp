#include"Utility/AssetLoader.h"
#include<stb_image/stb_image.h>
// Shader
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

bool MeshLoader::Load(MeshCPU& mesh, const std::string& path) {
	return true;
}

void TextureLoader::FlipImage(bool flip) {
	stbi_set_flip_vertically_on_load(flip);
}
bool TextureLoader::Load(TextureCPU& texture, const std::string& path) {
	texture.pixels = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channels, 4);
	return true;
}

bool ShaderLoader::Load(ShaderCPU& shader, const std::string& path) {
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