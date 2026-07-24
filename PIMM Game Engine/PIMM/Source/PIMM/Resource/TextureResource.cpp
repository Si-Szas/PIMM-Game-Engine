#include <PIMM/ResourceManager/TextureResource.h>
#include <PIMM/ResourceManager/ResourceManager.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

#include <fstream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb-image/stb_image.h>

pimm::TextureResource::TextureResource(const TextureResourceDescriptor& descriptor) : Resource(descriptor.base)
{
	std::filesystem::path textureFile = descriptor.base.path;
	auto textureFileStr = textureFile.string();

	auto width{ 0 }, height{ 0 }, channels{ 0 };
	auto pixels = stbi_load(
		textureFileStr.c_str(),
		&width,
		&height,
		&channels,
		STBI_rgb_alpha // Force RGBA
	);

	if (!pixels) PIMMLogThrowError("Failed to load texture file {}", textureFileStr.c_str());
	m_texture = descriptor.graphicsDevice.CreateTexture({ {width,height}, pixels });
}

pimm::Texture& pimm::TextureResource::GetTexture()
{
	return *m_texture;
}