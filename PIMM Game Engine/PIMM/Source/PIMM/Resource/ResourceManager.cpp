#include <PIMM/Resource/ResourceManager.h>
#include <PIMM/Resource/MaterialResource.h>

#include <filesystem>

pimm::ResourceManager::ResourceManager(const ResourceManagerDescriptor& descriptor) :
	Base(descriptor.base), m_systemContext(descriptor.systemContext)
{
}

pimm::RefPtr<pimm::Resource> pimm::ResourceManager::CreateResourceFromFileConcrete(const wchar_t* file_path)
{
	//Get the file path
	std::filesystem::path resourcePath{ file_path };

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
		if (!extension.compare(L".hlsl") || !extension.compare(L".fx"))
			resourcePtr = std::make_shared<MaterialResource>(MaterialResourceDescriptor{ GetResourceDescriptor(file_path), m_systemContext.graphicsDevice });
	}
	catch (...)
	{
		PIMMLogError("Failed to load resource {}", resourcePath.string().c_str());
	}

	if (resourcePtr)
	{
		m_resources.emplace(file_path, resourcePtr);
		return resourcePtr;
	}

	return nullptr;
}

pimm::ResourceDescriptor pimm::ResourceManager::GetResourceDescriptor(const wchar_t* file_path)
{
	return ResourceDescriptor{ { m_logger }, file_path, *this };
}
