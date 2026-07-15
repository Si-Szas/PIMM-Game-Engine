#pragma once
#include <unordered_map>
#include <string>
#include <PIMM/Core/Common.h>
#include <PIMM/Resource/Resource.h>

namespace pimm
{
	//Manager of our resources
	class ResourceManager final : public Base
	{
		public:
			//CONSTRUCTOR
			explicit ResourceManager(const ResourceManagerDescriptor& descriptor);

			template<typename Type>
			RefPtr<Type> CreateResourceFromFile(const wchar_t* file_path)
			{
				return std::dynamic_pointer_cast<Type>(CreateResourceFromFileConcrete(file_path));
			}

		private:
			RefPtr<Resource> CreateResourceFromFileConcrete(const wchar_t* file_path);
			ResourceDescriptor GetResourceDescriptor(const wchar_t* file_path);
		
		private:
			std::unordered_map<std::wstring, RefPtr<Resource>> m_resources{};
			SystemContext m_systemContext;
	};
}
