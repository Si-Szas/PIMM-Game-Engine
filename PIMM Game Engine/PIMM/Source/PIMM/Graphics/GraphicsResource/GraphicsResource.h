#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Base.h>
#include <PIMM/Graphics/GraphicsUtility/GraphicsLogUtility.h>

#include <d3d11.h>
#include <wrl.h>

namespace pimm
{
	//Dependency aggregation strategy
	struct GraphicsResourceDescriptor
	{
		BaseDescriptor base;
		std::shared_ptr<const GraphicsDevice> graphicsDevice;
		ID3D11Device& d3dDevice;
		IDXGIFactory& dxgiFactory;
	};

	class GraphicsResource : public Base
	{
		public:
			//CONSTRUCTOR
			explicit GraphicsResource(const GraphicsResourceDescriptor& descriptor):
				//Member initializer list
				Base(descriptor.base),
				m_graphicsDevice(descriptor.graphicsDevice),
				m_d3dDevice(descriptor.d3dDevice),
				m_dxgiFactory(descriptor.dxgiFactory)
			{
			}

		protected:
			std::shared_ptr<const GraphicsDevice> m_graphicsDevice;
			ID3D11Device& m_d3dDevice;
			IDXGIFactory& m_dxgiFactory;
	};
}