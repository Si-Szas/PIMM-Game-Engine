#include <PIMM/Resource/MaterialResource.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Resource/ResourceManager.h>
#include <PIMM/Graphics/GraphicsPipelineLayout/GraphicsPipelineLayout.h>

#include <fstream>
#include <filesystem>

pimm::MaterialResource::MaterialResource(const MaterialResourceDescriptor& descriptor) : 
	Resource(descriptor.base), 
	m_graphicsDevice(descriptor.graphicsDevice)
{
	std::filesystem::path shaderFile = descriptor.base.path;

	//Read the shader file
	auto shaderFileStr = shaderFile.string();
	std::ifstream shaderStream(shaderFile);

	if (!shaderStream) PIMMLogThrowError("Failed to open shader file {}", shaderFileStr.c_str());
	std::string shaderCode{
		std::istreambuf_iterator<char>(shaderStream),
		std::istreambuf_iterator<char>()
	};

	//Create and compile the Vertex Shader
	auto vsBinary = m_graphicsDevice.CompileShader({ 
		shaderFileStr.c_str(), 
		shaderCode.c_str(),
		shaderCode.size(), 
		"VS_Main", 
		ShaderType::VertexShader 
	});

	//Create and compile the Hull Shader
	auto hsBinary = m_graphicsDevice.CompileShader({
		shaderFileStr.c_str(),
		shaderCode.c_str(),
		shaderCode.size(),
		"HS_Main",
		ShaderType::HullShader
	});

	//Create and compile the Domain Shader
	auto dsBinary = m_graphicsDevice.CompileShader({
		shaderFileStr.c_str(),
		shaderCode.c_str(),
		shaderCode.size(),
		"DS_Main",
		ShaderType::DomainShader
	});

	//Create and compile the Pixel Shader
	auto psBinary = m_graphicsDevice.CompileShader({
		shaderFileStr.c_str(), 
		shaderCode.c_str(),
		shaderCode.size(), "PS_Main", 
		ShaderType::PixelShader 
	});

	m_layout = m_graphicsDevice.CreateGraphicsPipelineLayout({ vsBinary, psBinary, hsBinary, dsBinary });
	m_pipeline = m_graphicsDevice.CreateGraphicsPipelineState({ *m_layout });
	m_textures.resize(m_layout->GetMaxTextureSlots());
}

pimm::MaterialResource::MaterialResource(const MaterialResource& material, const MaterialResourceDescriptor& descriptor) : 
	Resource(descriptor.base), 
	m_graphicsDevice(descriptor.graphicsDevice)
{
	m_layout = material.m_layout;
	m_pipeline = material.m_pipeline;
	m_textures.resize(m_layout->GetMaxTextureSlots());
}

const pimm::GraphicsPipelineState& pimm::MaterialResource::GetGraphicsPipelineState() const noexcept
{
	return *m_pipeline;
}

void pimm::MaterialResource::SetData(const std::span<const std::byte>& data)
{
	if (!data.size())
	{
		PIMMLogError("No material data provided.")
			return;
	}
	if (data.size() > MaxDataSize)
	{
		PIMMLogWarning("Material data size ({} bytes) exceeds the maximum allowed size of {} bytes. Data will be truncated.", data.size(), MaxDataSize)
	}

	auto size = std::min(data.size(), MaxDataSize);
	memcpy(m_data, data.data(), size);
	m_dataSize = size;
}

const std::span<const std::byte> pimm::MaterialResource::GetData() const noexcept
{
	return m_data;
}


pimm::TextureResource* pimm::MaterialResource::GetTexture(size_t index)
{
	if (index >= m_textures.size())
	{
		PIMMLogError("Index {} is out of bounds for list of size {}", index, m_textures.size());
		return {};
	}

	return m_textures[index].get();
}

size_t pimm::MaterialResource::GetNumberOfTextures() const noexcept
{
	return m_textures.size();
}

void pimm::MaterialResource::SetTexture(size_t index, const pimm::RefPtr<pimm::TextureResource>& texture)
{
	if (index >= m_textures.size())
	{
		PIMMLogError("Index {} is out of bounds for list of size {}", index, m_textures.size());
		return;
	}

	m_textures[index] = texture;
}
