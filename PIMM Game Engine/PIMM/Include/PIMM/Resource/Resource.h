#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Base.h>
#include <string>

namespace pimm
{
	class Resource : public Base
	{
		public:
			//CONSTRUCTOR
			explicit Resource(const ResourceDescriptor& descriptor);
		
		protected:
			std::wstring m_path{};
			ResourceManager& m_manager;
	};
}