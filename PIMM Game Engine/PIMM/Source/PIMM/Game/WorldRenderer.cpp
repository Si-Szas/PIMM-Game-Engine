#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Graphics/DeviceContext/DeviceContext.h>
#include <PIMM/Graphics/SwapChain/SwapChain.h>
#include <PIMM/Time/EngineTime.h>
#include <PIMM/Graphics/VertexBuffer/VertexBuffer.h>
#include <PIMM/Graphics/ConstantBuffer/ConstantBuffer.h>
#include <PIMM/Graphics/IndexBuffer/IndexBuffer.h>
#include <PIMM/Graphics/FrameBuffer/FrameBuffer.h>
#include <PIMM/UIManager/UIManager.h>
//GAME AND WORLD HEADER//
#include <PIMM/Game/World.h>
//GAME OBJECTS//
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AGameObject/Quad.h>
#include <PIMM/AGameObject/Cube.h>
#include <PIMM/AGameObject/Sphere.h>
#include <PIMM/AGameObject/Cylinder.h>
#include <PIMM/AGameObject/Capsule.h>
//COMPONENTS//
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/CubeComponent.h>
#include <PIMM/AComponent/SphereComponent.h>
#include <PIMM/AComponent/CameraComponent.h>
//MATERIALS//
#include <PIMM/Resource/MaterialResource.h>
//TEXTURES//
#include <PIMM/Resource/TextureResource.h>

#include <PIMM/Math/Vec2.h>
#include <PIMM/Math/Vec3.h>
#include <fstream>
#include <ranges>

pimm::WorldRenderer::WorldRenderer(const WorldRendererDescriptor& descriptor) :
	Base(descriptor.base),
	m_graphicsDevice(descriptor.graphicsEngine),
	m_uiManager(descriptor.uiManager)
{
	//Creates the deferred device context
	auto& device = m_graphicsDevice;
	m_deviceContext = device.CreateDeviceContext();

	//For textures
	m_textures.reserve(32);

	m_frameBuffer = device.CreateFrameBuffer({
			m_graphicsDevice,
			m_swapChainSize,
			1
	});

	//Create constant buffer
	m_objectConstantBuffer = device.CreateConstantBuffer
	({
		{},
		sizeof(ObjectData)
	});

	m_cameraConstantBuffer = device.CreateConstantBuffer
	({
		{},
		sizeof(CameraData)
	});

	m_materialConstantBuffer = device.CreateConstantBuffer
	({
		{},
		pimm::MaterialResource::MaxDataSize
	});

	m_sampler = device.CreateSampler({});
}

void pimm::WorldRenderer::Render(const World& world, SwapChain& swapChain, f32 deltaTime)
{
	////////// CAMERA SET-UP //////////
	m_swapChainSize = swapChain.GetSize();;
	Rect frameBufferSize = (m_sceneViewSize.width > 0 && m_sceneViewSize.height > 0) ? m_sceneViewSize : m_swapChainSize;
	////////// DEVICE CONTEXT //////////
	// - Update the constant buffer before everything
	// - context.UpdateConstantBuffer(vsConstantBuffer, &data);
	// - We want to first clear the buffer, then after rendering on a back buffer, we want to move that back to the front buffer
	// - Record render command that clears content of back buffer and binds it so we can render elements onto it
	// - UsPipeline
	//	- Bind all objects inside graphics pipeline state (shaders) to actual GPU pipeline
	auto& context = *m_deviceContext;
	
	//context.ClearAndSetBackBuffer(swapChain, { 0.251f, 0.141f, 0.31f, 1.0f });
	context.SetViewportSize(frameBufferSize);

	////////// TEXUTRES //////////
	Sampler* samplers[] = { m_sampler.get() };
	context.SetSamplers(std::span<Sampler*>{samplers});

	////////// ACOMPONENTS //////////
	auto numberOfComponents = 0u;

	pimm::FrameBufferDescriptor frameBufferDescriptor{
		.graphicsDevice = m_graphicsDevice,
		.size = frameBufferSize,
		.sampleCount = 1
	};
	
	m_frameBuffer->Create(m_graphicsDevice, frameBufferDescriptor);
	context.ClearAndSetFrameBuffer(*m_frameBuffer, { 0.251f, 0.141f, 0.31f, 1.0f });
	

	////////// CONSTANT BUFFER DATA //////////
	auto& cameraCB = *m_cameraConstantBuffer;
	auto& objectCB = *m_objectConstantBuffer;
	auto& materialCB = *m_materialConstantBuffer;

	{
		////////// CONSTANT BUFFER DATA //////////
		CameraData cameraData{};
		{
			auto cameraComponents = world.GetAComponent<CameraComponent>(numberOfComponents);

			for (auto i : std::views::iota(0u, numberOfComponents))
			{
				auto camComponent = cameraComponents[i];
				cameraData.view = camComponent->GetViewMatrix();
				camComponent->SetViewportSize(frameBufferSize);
				cameraData.projection = camComponent->GetProjectionMatrix();
				context.UpdateConstantBuffer(cameraCB, std::as_bytes(std::span{ &cameraData, 1 }));
				break;
			}
		}
		{
			ObjectData objectData{};
			auto gameObjects = world.GetAllGameObjects();
			ui32 totalGameObjects = static_cast<ui32>(gameObjects.size());

			//std::cout << "[LOG] Current Game Objects No.: " << totalGameObjects << std::endl;

			for (auto i : std::views::iota(0u, totalGameObjects))
			{
				auto object = gameObjects[i];
				if (!object) continue;
				auto& transform = object->GetTransform();
				size_t objectType = object->GetTypeID();
				auto material = object->GetMaterialComponent().GetMaterial();

				if (material)
				{
					objectData.world = transform.GetAffineWorldMatrix();

					context.SetGraphicsPipelineState(material->GetGraphicsPipelineState());
					context.UpdateConstantBuffer(objectCB, std::as_bytes(std::span{ &objectData, 1 }));
					context.UpdateConstantBuffer(materialCB, material->GetData());
					ConstantBuffer* cbs[] = { &objectCB, &cameraCB, &materialCB };
					context.SetConstantBuffers(std::span<ConstantBuffer*>{cbs});

					m_textures.clear();
					m_textures.resize(material->GetNumberOfTextures());
					for (auto t : std::views::iota(0u, m_textures.size()))
					{
						auto tex = material->GetTexture(t);
						if (tex) m_textures[t] = &tex->GetTexture();
					}
					context.SetTextures(std::span<Texture*>{m_textures});

					if (objectType == pimm::Quad::getTypeId()) context.SetVertexBuffer(object->GetComponent<QuadComponent>()->GetVertexBuffer());
					if (objectType == pimm::Cube::getTypeId()) context.SetVertexBuffer(object->GetComponent<CubeComponent>()->GetVertexBuffer());
					if (objectType == pimm::Sphere::getTypeId()) context.SetVertexBuffer(object->GetComponent<SphereComponent>()->GetVertexBuffer());
					if (objectType == pimm::Cylinder::getTypeId()) context.SetVertexBuffer(object->GetComponent<CylinderComponent>()->GetVertexBuffer());
					if (objectType == pimm::Capsule::getTypeId()) context.SetVertexBuffer(object->GetComponent<CapsuleComponent>()->GetVertexBuffer());
					
					if (objectType == pimm::Quad::getTypeId()) context.Draw4PatchIndexedTriangleList(object->GetComponent<QuadComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					if (objectType == pimm::Cube::getTypeId()) context.Draw4PatchIndexedTriangleList(object->GetComponent<CubeComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					if (objectType == pimm::Sphere::getTypeId()) context.Draw4PatchIndexedTriangleList(object->GetComponent<SphereComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					if (objectType == pimm::Cylinder::getTypeId()) context.Draw4PatchIndexedTriangleList(object->GetComponent<CylinderComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					if (objectType == pimm::Capsule::getTypeId()) context.Draw4PatchIndexedTriangleList(object->GetComponent<CapsuleComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
				}
			}
		}

		//Pass device context where we will extract the commands from
		m_graphicsDevice.ExecuteCommandList(context);

		auto immediateContext = m_graphicsDevice.GetD3DDeviceContext();
		m_deviceContext->ExecuteCommandList(immediateContext);
		auto rtv = swapChain.GetRenderTargetView();
		auto dsv = swapChain.GetDepthStencilView();
		immediateContext->OMSetRenderTargets(1, &rtv, dsv);
		float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

		// clear backbuffer

		immediateContext->ClearRenderTargetView(rtv, clearColor);
		immediateContext->ClearDepthStencilView(
			dsv,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0
		);

		m_uiManager.Render();

		//Present our back buffer with its rendered content on the window
		swapChain.Present();
	}
}

pimm::GraphicsDevice& pimm::WorldRenderer::GetGraphicsDevice() const noexcept
{
	return m_graphicsDevice;
}

pimm::Rect pimm::WorldRenderer::GetSwapChainSize() const noexcept
{
	return m_swapChainSize;
}

pimm::FrameBuffer* pimm::WorldRenderer::GetFrameBuffer() const noexcept
{
	return m_frameBuffer.get();
}

void pimm::WorldRenderer::SetSceneViewSize(pimm::Rect size) noexcept
{ 
	m_sceneViewSize = size;
}

pimm::WorldRenderer::~WorldRenderer()
{
}