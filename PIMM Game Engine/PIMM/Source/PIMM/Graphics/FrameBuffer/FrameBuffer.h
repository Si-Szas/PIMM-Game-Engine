#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Common.h>
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>

namespace pimm
{
	class FrameBuffer final : public GraphicsResource
	{
		public:
			FrameBuffer(const FrameBufferDescriptor& frameBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
	};
}