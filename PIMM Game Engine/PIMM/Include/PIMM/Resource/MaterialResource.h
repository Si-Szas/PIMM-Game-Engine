#pragma once
#include <PIMM/Resource/Resource.h>
#include <vector>
#include <span>

namespace pimm
{
	class MaterialResource final : public Resource
	{
		public:
			//CONSTRUCTOR
			explicit MaterialResource(const MaterialResourceDescriptor& descriptor);
			MaterialResource(const MaterialResource& material, const MaterialResourceDescriptor& descriptor);

			//FUNCTIONS
			const GraphicsPipelineState& GetGraphicsPipelineState() const noexcept;
			void SetData(const std::span<const std::byte>& data);
			const std::span<const std::byte> GetData() const noexcept;

			TextureResource* GetTexture(size_t index);
			size_t GetNumberOfTextures()  const noexcept;
			void SetTexture(size_t index, const pimm::RefPtr<TextureResource>& texture);

		public:
			static constexpr std::size_t MaxDataSize{ 256 };
		private:
			GraphicsDevice& m_graphicsDevice;

			RefPtr<GraphicsPipelineLayout> m_layout{};
			RefPtr<GraphicsPipelineState> m_pipeline{};

			std::byte m_data[MaxDataSize]{};
			size_t m_dataSize{};

			std::vector<RefPtr<TextureResource>> m_textures{};
	};
}
