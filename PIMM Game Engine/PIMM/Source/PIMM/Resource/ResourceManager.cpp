#include <PIMM/Resource/ResourceManager.h>
#include <PIMM/Resource/MaterialResource.h>
#include <PIMM/Resource/TextureResource.h>
#include <PIMM/Resource/MeshResource.h>

#include <filesystem>

pimm::ResourceManager::ResourceManager(const ResourceManagerDescriptor& descriptor) :
	Base(descriptor.base), m_systemContext(descriptor.systemContext)
{
}

pimm::RefPtr<pimm::Resource> pimm::ResourceManager::CreateResourceFromFileConcrete(const wchar_t* file_path)
{
	//Get the file path
	std::filesystem::path resourcePath{ file_path };

	PIMMLogInformation("Creating resource from file {}", resourcePath.string().c_str());

	auto extension = resourcePath.extension();

	auto it = m_resources.find(file_path);
	if (it != m_resources.end())
	{
		auto material = std::dynamic_pointer_cast<MaterialResource>(it->second);
		
		if (material) return std::make_shared<MaterialResource>(*material, MaterialResourceDescriptor{ GetResourceDescriptor(file_path), m_systemContext.graphicsDevice });
		
		return it->second;
	}

	if (!std::filesystem::exists(resourcePath))
	{
		PIMMLogError("File {} doesn't exist.", resourcePath.string().c_str());
		return nullptr;
	}

	RefPtr<Resource> resourcePtr{};
	try
	{
		//For Materials
		if (!extension.compare(L".hlsl") || !extension.compare(L".fx"))
			resourcePtr = std::make_shared<MaterialResource>(MaterialResourceDescriptor{ GetResourceDescriptor(file_path), m_systemContext.graphicsDevice });
		//For Textures
		if (!extension.compare(L".jpg") || !extension.compare(L".png"))
			resourcePtr = std::make_shared<TextureResource>(TextureResourceDescriptor{ GetResourceDescriptor(file_path), m_systemContext.graphicsDevice });
		//For Meshes
		if (!extension.compare(L".obj"))
			resourcePtr = std::make_shared<MeshResource>(MeshResourceDescriptor{ GetResourceDescriptor(file_path), m_systemContext.graphicsDevice });
	}
	catch (...)
	{
		PIMMLogError("Failed to load resource {}", resourcePath.string().c_str());
	}

	if (resourcePtr)
	{
		m_resources.emplace(file_path, resourcePtr);

		PIMMLogInformation("Successfully created resource from file {}", resourcePath.string().c_str());

		return resourcePtr;
	}

	return nullptr;
}

pimm::ResourceDescriptor pimm::ResourceManager::GetResourceDescriptor(const wchar_t* file_path)
{
	return ResourceDescriptor{ { m_logger }, file_path, *this };
}
