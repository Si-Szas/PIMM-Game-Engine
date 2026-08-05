#pragma once
#include <PIMM/Resource/Resource.h>
#include <vector>

namespace pimm {
	class MeshResource final : public Resource
	{
		public:
			//CONSTRUCTOR
			MeshResource(const MeshResourceDescriptor& descriptor);

			//GETTERS
			const MaterialSlot* GetMaterialSlots(ui32& numberOfSlots) const noexcept;
			ui32 GetNumberOfMaterialSlots() const noexcept;
			const VertexBuffer& GetVertexBuffer() const noexcept;
			const IndexBuffer& GetIndexBuffer() const noexcept;

		private:
			RefPtr<VertexBuffer> m_vertexBuffer{};
			RefPtr<IndexBuffer> m_indexBuffer{};
			std::vector<MaterialSlot> m_materialSlots{};
	};
}