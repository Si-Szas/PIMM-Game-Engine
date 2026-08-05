#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/AComponent/AComponent.h>
#include <vector>

namespace pimm{
	class MeshComponent final : public AComponent
	{
		pimm_typeid(MeshComponent)
		
		public:
			//CONSTRUCTOR
			explicit MeshComponent(const AComponentDescriptor& data);

			//GETTER
			MeshResource* GetMesh() const noexcept;
			MaterialResource* GetMaterial(ui32 index) const noexcept;

			//SETTER
			void SetMesh(const RefPtr<MeshResource>& newMesh);
			void SetMaterial(ui32 index, const RefPtr<MaterialResource>& newMaterial);

		private:
			RefPtr<MeshResource> m_mesh{};
			std::vector<RefPtr<MaterialResource>> m_materials{};
	};
}

