#pragma once


#include<Renderer/Core/ResourcesCPU.h>

inline unsigned char ReadChannel(const unsigned char* data, int index, int channels, int channelIndex) {
	if (!data) {
		return 255;
	}

	if (channels == 1) {
		return data[index];
	}

	return data[index * channels + channelIndex];
}

zRender::TextureCPU* PackTextureRM(const zRender::TextureCPU* Roughness, const zRender::TextureCPU* Metallic) {
	int width = std::max(Roughness ? Roughness->width : 0, Metallic ? Metallic->width : 0);
	int height = std::max(Roughness ? Roughness->height : 0, Metallic ? Metallic->height : 0);

	if (width == 0 || height == 0) return nullptr;

	zRender::TextureCPU* rm = new zRender::TextureCPU();
	rm->width = width;
	rm->height = height;

	int pixelCount = height * width;
	rm->pixels = new unsigned char[pixelCount * 4];

	for (size_t i = 0; i < pixelCount; i++) {
		unsigned char metallic = 255;
		unsigned char roughness = 255;

		if (Metallic) metallic = ReadChannel(Metallic->pixels, i, Metallic->channels, 0);
		if (Roughness) roughness = ReadChannel(Roughness->pixels, i, Roughness->channels, 0);

		rm->pixels[i * 4 + 0] = roughness;
		rm->pixels[i * 4 + 1] = metallic;
		rm->pixels[i * 4 + 2] = 255;
		rm->pixels[i * 4 + 3] = 255;
	}

	rm->name = Roughness->name + Metallic->name;

	return rm;
}