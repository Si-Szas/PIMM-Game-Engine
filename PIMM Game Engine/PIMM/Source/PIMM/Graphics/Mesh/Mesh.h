#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>

namespace pimm
{
	class Mesh final : public GraphicsResource
	{
		public:
			Mesh(const MeshDescriptor& meshDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);

		friend class DeviceContext;
	};
}
