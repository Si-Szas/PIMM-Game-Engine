#pragma once
#include <PIMM/Core/Logger.h>
#include <PIMM/Core/Base.h>
#include <d3d11.h>

namespace pimm 
{
	namespace GraphicsLogUtility
	{
		inline void CheckShaderCompile(Logger& logger, HRESULT hr, ID3DBlob* errorBlob) 
		{
			//If function failed, either we have error messages or we dont
				//If error blob is not null, we can retrieve the actual error message via GetBufferPointer
				//Method returns void pointer, which we can cast it to a const char pointer
				//If error blob unavailable, assign nullptr
			auto errorMsg = errorBlob ? static_cast<const char*>(errorBlob->GetBufferPointer()) : nullptr;

			if (FAILED(hr))
				PIMMLogThrow(logger, std::runtime_error, Logger::LogLevel::Error, "Shader compilation failed.\nDetails:\n{}",
					errorMsg ? errorMsg : "");
			if (errorMsg)
				PIMMLog(logger, Logger::LogLevel::Warning, "Shader compiled with warnings.\nDetails:\n{}", errorMsg);

		}
	}
}

#define PIMMGraphicsLogThrowOnFail(hr,message,...)\
	{\
		auto res = (hr);\
		if (FAILED(res))\
			PIMMLogThrowError(message, __VA_ARGS__);\
	}


#define PIMMGraphicsCheckShaderCompile(hr, errorBlob)\
{\
	auto res = (hr);\
	pimm::GraphicsLogUtility::CheckShaderCompile(GetLogger(), res, errorBlob);\
}