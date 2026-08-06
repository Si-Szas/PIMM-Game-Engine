#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/MeshComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>

namespace pimm
{
	class MeshObject final : public AGameObject
	{
		pimm_typeid(MeshObject)
	public:
		//CONSTRUCTOR
		explicit MeshObject(const AGameObjectDescriptor& descriptor);

		void OnCreate();
		void OnUpdate(f32 deltaTime);

		//DESTRUCTOR
		~MeshObject();

	private:
		MeshComponent* m_meshComponent{};
		//MaterialComponent* m_materialComponent{};
	};
}