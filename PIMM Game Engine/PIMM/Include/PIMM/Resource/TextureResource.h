#pragma once
#include <PIMM/Resource/Resource.h>

namespace pimm
{
	class TextureResource : public Resource
	{
		public:
			//CONSTRUCTOR
			explicit TextureResource(const TextureResourceDescriptor& descriptor);

			//GETTER
			pimm::Texture& GetTexture();

		private:
			RefPtr<Texture> m_texture{};
	};
}
