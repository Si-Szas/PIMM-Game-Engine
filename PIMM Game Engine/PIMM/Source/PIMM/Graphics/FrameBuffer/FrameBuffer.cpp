#include <PIMM/Graphics/FrameBuffer/FrameBuffer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::FrameBuffer::FrameBuffer(const FrameBufferDescriptor& frameBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor) :
	GraphicsResource(graphicsResourceDescriptor)
{
    //Create the frame buffer on instantiation
    Create(frameBufferDescriptor.graphicsDevice, frameBufferDescriptor);
}

void pimm::FrameBuffer::Create(GraphicsDevice& graphicsDevice, const FrameBufferDescriptor& frameBufferDescriptor)
{
    m_sampleCount = frameBufferDescriptor.sampleCount > 0 ? frameBufferDescriptor.sampleCount : 1;

    Resize(graphicsDevice, frameBufferDescriptor.size);
}
    //Resize to make sure it resizes whenever panel is updated
void pimm::FrameBuffer::Resize(GraphicsDevice& graphicsDevice, Rect newSize)
{
    if (m_colorTexture && m_size.width == newSize.width && m_size.height == newSize.height) return;

    if (newSize.width == 0 || newSize.height == 0)
    {
        PIMMLogWarning("Attempted to resize FrameBuffer to 0 dimensions. Skipping allocation.");
        return;
    }

    // Get the Graphics Device
    auto& device = graphicsDevice.GetD3DDevice();
    ID3D11Device* d3dDevice = const_cast<ID3D11Device*>(device.Get());
    //Make sure the device exists
    if (!d3dDevice) PIMMLogThrowError("ID3D11Device is null");
   
    m_size.width = newSize.width;
    m_size.height =newSize.height;

    // move textures
    m_previousColorTexture = std::move(m_colorTexture);
    m_previousRtv = std::move(m_rtv);
    m_previousSrv = std::move(m_srv);
    m_dsv.Reset();

    // Render Target Texture
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = m_size.width;
    texDesc.Height = m_size.height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = m_sampleCount;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    PIMMGraphicsLogThrowOnFail(d3dDevice->CreateTexture2D(
        &texDesc, nullptr, &m_colorTexture), 
        "CreateTexture2D() failed.");

    PIMMGraphicsLogThrowOnFail(d3dDevice->CreateRenderTargetView(
        m_colorTexture.Get(), nullptr, &m_rtv), 
        "CreateRenderTargetView() failed.");

    PIMMGraphicsLogThrowOnFail(d3dDevice->CreateShaderResourceView(
        m_colorTexture.Get(), nullptr, &m_srv), 
        "CreateShaderResourceView() failed.");

    D3D11_TEXTURE2D_DESC depthDesc{};
    depthDesc.Width = m_size.width;
    depthDesc.Height = m_size.height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = m_sampleCount;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
    PIMMGraphicsLogThrowOnFail(d3dDevice->CreateTexture2D(
        &depthDesc, nullptr, &depthBuffer), 
        "CreateTexture2D() failed.");
    PIMMGraphicsLogThrowOnFail(d3dDevice->CreateDepthStencilView(
        depthBuffer.Get(), nullptr, &m_dsv), 
        "CreateDepthStencilView() failed.");
}

ID3D11ShaderResourceView* pimm::FrameBuffer::GetSRV() const noexcept
{
    return m_srv.Get();
}

ID3D11RenderTargetView* pimm::FrameBuffer::GetRTV() const noexcept
{
    return m_rtv.Get();
}

ID3D11DepthStencilView* pimm::FrameBuffer::GetDSV() const noexcept
{
    return m_dsv.Get();
}

pimm::ui32 pimm::FrameBuffer::GetFrameBufferSizeWidth() const noexcept
{
    return m_size.width;
}

pimm::ui32 pimm::FrameBuffer::GetFrameBufferSizeHeight() const noexcept
{
    return m_size.height;
}