#pragma once

#include<Renderer/Core/Resources.h>

zRender::Texture* PackTextureRM(const zRender::Texture* Roughness, const zRender::Texture* Metallic) {
#undef max

	int width = std::max(Roughness ? Roughness->width : 0, Metallic ? Metallic->width : 0);
	int height = std::max(Roughness ? Roughness->height : 0, Metallic ? Metallic->height : 0);

	if (width == 0 || height == 0) return nullptr;

	zRender::Texture* rm = new zRender::Texture();
	rm->width = width;
	rm->height = height;

	int pixelCount = height * width;
	rm->pixels = new zRender::Pixel[pixelCount * 4];

	for (size_t i = 0; i < pixelCount; i++) {
		unsigned char metallic = 255;
		unsigned char roughness = 255;

		if (Metallic) metallic = Metallic->pixels[pixelCount].r;
		if (Roughness) roughness = Roughness->pixels[pixelCount].r;

		rm->pixels[i].r = roughness;
		rm->pixels[i].g = metallic;
		rm->pixels[i].b = 255;
		rm->pixels[i].a = 255;
	}

	rm->name = Roughness->name + Metallic->name;

	return rm;
}