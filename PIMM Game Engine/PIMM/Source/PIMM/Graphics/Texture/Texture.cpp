#include "Texture.h"

pimm::Texture::Texture(const TextureDescriptor& textureDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor) :
	GraphicsResource(graphicsResourceDescriptor)
{
	if (!textureDescriptor.size.width) PIMMLogThrowInvalidArgument("Width must be non-zero.");
	if (!textureDescriptor.size.height) PIMMLogThrowInvalidArgument("Height must be non-zero.");
	if (!textureDescriptor.pixels) PIMMLogThrowInvalidArgument("Pixels must be a valid array.");

	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = textureDescriptor.size.width;
	texDesc.Height = textureDescriptor.size.height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = textureDescriptor.pixels;
	initData.SysMemPitch = textureDescriptor.size.width * 4;

	PIMMGraphicsLogThrowOnFail(m_d3dDevice.CreateTexture2D(&texDesc, &initData, &m_texture),
		"CreateTexture2D failed.");

	D3D11_SHADER_RESOURCE_VIEW_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resDesc.Texture2D.MipLevels = 1;
	resDesc.Texture2D.MostDetailedMip = 0;

	PIMMGraphicsLogThrowOnFail(m_d3dDevice.CreateShaderResourceView(m_texture.Get(), &resDesc,
		&m_shaderResourceView), "CreateShaderResourceView failed.");
}
