#pragma once
#include <PIMM/Core/Common.h>
#include <d3d11.h>
#include <bit>

namespace pimm
{
	//Creating a namespace for purely helper functions
	//Good to avoid stateless utility classes and use namespaces instead
	namespace GraphicsUtility 
	{
		//Gets shader model target, returns string
		//Having inline allows it to be included in multiple translation units without causing linker errors
		//Inline ensures safe linkage and reuse
		inline const char* GetShaderModelTarget(ShaderType type)
		{
			switch (type)
			{
				//Vertex/Pixel Shader, 5.0 is shader model version
				case pimm::ShaderType::VertexShader: return "vs_5_0"; break;
				case pimm::ShaderType::PixelShader:	return "ps_5_0";  break;
				case pimm::ShaderType::HullShader: return "hs_5_0"; break;
				case pimm::ShaderType::DomainShader: return "ds_5_0"; break;
				default: return "";
			}
		}

		//Get the DXGI Format
		inline DXGI_FORMAT GetDXGIFormatFromMask(D3D_REGISTER_COMPONENT_TYPE type, UINT mask)
		{
			//Bit mask is sequence of bits used to represent or manipulate specific data. If x is used, first bit is set to 1, else 0.
			auto componentCount = std::popcount(mask);
			if (componentCount < 1) return DXGI_FORMAT_UNKNOWN;

			constexpr DXGI_FORMAT formatTable[1][4] = 
			{
				{
					DXGI_FORMAT_R32_FLOAT,
					DXGI_FORMAT_R32G32_FLOAT,
					DXGI_FORMAT_R32G32B32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT
				}
			};

			auto typeIndex = 0u;
			switch (type)
			{
				case D3D_REGISTER_COMPONENT_FLOAT32: 
					typeIndex = 0u;
					break;
				
				default: return DXGI_FORMAT_UNKNOWN;
			};

			return formatTable[typeIndex][componentCount - 1];

		}
	}
}