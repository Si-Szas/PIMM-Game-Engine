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
#include <PIMM/AGameObject/MeshObject.h>
#include <PIMM/AGameObject/CameraObject.h>
#include <PIMM/AGameObject/AGameObject.h>
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
//MESHES//
#include <PIMM/Resource/MeshResource.h>
//GIZMOS//
#include <PIMM/Game/GizmoRenderer.h>
#include <PIMM/AGameObject/CameraObject.h>
#include <PIMM/AComponent/CameraComponent.h>

#include <PIMM/Math/Vec2.h>
#include <PIMM/Math/Vec3.h>
#include <d3dcompiler.h>
#include <iostream>
#include <fstream>
#include <ranges>

#pragma comment(lib, "d3dcompiler")

namespace
{
	constexpr const char* kUnlitShaderSource = R"(
cbuffer MaterialData : register(b2)
{
	float3 materialColor;
}

float4 PS_Main(float4 position : SV_Position) : SV_Target
{
	return float4(materialColor, 1.0f);
}
)";
}

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

	//Create the gizmo renderer for editor visuals (e.g. camera frustums)
	m_gizmoRenderer = std::make_unique<GizmoRenderer>(*m_graphicsDevice.GetD3DDevice().Get(), m_logger);

	D3D11_RASTERIZER_DESC rsDesc{};
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	m_graphicsDevice.GetD3DDevice()->CreateRasterizerState(&rsDesc, &m_wireframeRasterizer);

	{
		size_t srcLen = std::char_traits<char>::length(kUnlitShaderSource);
		Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errBlob;
		HRESULT hr = D3DCompile(kUnlitShaderSource, srcLen, "UnlitPS", nullptr, nullptr, "PS_Main", "ps_4_0", 0, 0, &psBlob, &errBlob);
		if (SUCCEEDED(hr) && psBlob)
			m_graphicsDevice.GetD3DDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_unlitPixelShader);
	}
}

void pimm::WorldRenderer::Render(const World& world, SwapChain& swapChain, f32 deltaTime)
{
	m_swapChainSize = swapChain.GetSize();
	Rect frameBufferSize = (m_sceneViewSize.width > 0 && m_sceneViewSize.height > 0) ? m_sceneViewSize : m_swapChainSize;

	auto& context = *m_deviceContext;

	context.SetViewportSize(frameBufferSize);

	Sampler* samplers[] = { m_sampler.get() };
	context.SetSamplers(std::span<Sampler*>{samplers});

	auto numberOfComponents = 0u;

	Rect viewportSize = frameBufferSize;
	if (m_viewportLayout == ViewportLayout::Quad)
	{
		viewportSize.width /= 2;
		viewportSize.height /= 2;
	}

	pimm::FrameBufferDescriptor viewportFbDesc{
		.graphicsDevice = m_graphicsDevice,
		.size = viewportSize,
		.sampleCount = 1
	};

	for (ui32 vp = 0; vp < GetViewportCount(); ++vp)
	{
		if (!m_viewportFrameBuffers[vp])
			m_viewportFrameBuffers[vp] = m_graphicsDevice.CreateFrameBuffer(viewportFbDesc);
		else
			m_viewportFrameBuffers[vp]->Create(m_graphicsDevice, viewportFbDesc);
	}

	pimm::FrameBufferDescriptor frameBufferDescriptor{
		.graphicsDevice = m_graphicsDevice,
		.size = frameBufferSize,
		.sampleCount = 1
	};

	m_frameBuffer->Create(m_graphicsDevice, frameBufferDescriptor);

	auto& cameraCB = *m_cameraConstantBuffer;
	auto& objectCB = *m_objectConstantBuffer;
	auto& materialCB = *m_materialConstantBuffer;

	Vec3 editorCamPos{};
	Vec3 editorCamForward{};

	for (ui32 vp = 0; vp < GetViewportCount(); ++vp)
	{
		CameraData cameraData{};

		if (m_viewportLayout == ViewportLayout::Quad && vp > 0)
		{
			Vec3 lookTarget = { editorCamPos.x + editorCamForward.x * 20.0f, editorCamPos.y + editorCamForward.y * 20.0f, editorCamPos.z + editorCamForward.z * 20.0f };
			cameraData.view = BuildOrthoViewMatrix(vp, lookTarget);
			f32 halfSize = 25.0f;
			cameraData.projection = Matrix4x4::OrthoLH(
				halfSize * 2.0f * (f32(viewportSize.width) / f32(viewportSize.height)),
				halfSize * 2.0f, 0.01f, 100.0f);
		}
		else
		{
			if (m_sceneCameraMode)
			{
				auto* cameraObject = world.GetActiveCameraObject();
				if (cameraObject && cameraObject->GetTypeID() == CameraObject::getTypeId())
				{
					auto* camComponent = cameraObject->GetComponent<CameraComponent>();
					if (camComponent)
					{
						cameraData.view = camComponent->GetViewMatrix();
						camComponent->SetViewportSize(frameBufferSize);
						cameraData.projection = camComponent->GetProjectionMatrix();
						editorCamPos = cameraObject->GetTransform().GetPosition();
						editorCamForward = cameraObject->GetTransform().Forward();
					}
				}
			}
			else
			{
				auto cameraComponents = world.GetAComponent<CameraComponent>(numberOfComponents);
				for (auto i : std::views::iota(0u, numberOfComponents))
				{
					auto camComponent = cameraComponents[i];
					if (camComponent->GetGameObject().GetTypeID() == CameraObject::getTypeId())
						continue;
					cameraData.view = camComponent->GetViewMatrix();
					camComponent->SetViewportSize(frameBufferSize);
					cameraData.projection = camComponent->GetProjectionMatrix();
					editorCamPos = camComponent->GetGameObject().GetTransform().GetPosition();
					editorCamForward = camComponent->GetGameObject().GetTransform().Forward();
					break;
				}
			}
		}

		context.UpdateConstantBuffer(cameraCB, std::as_bytes(std::span{ &cameraData, 1 }));

		FrameBuffer& targetFb = (m_viewportLayout == ViewportLayout::Quad)
			? *m_viewportFrameBuffers[vp] : *m_frameBuffer;
		context.ClearAndSetFrameBuffer(targetFb, { 0.251f, 0.141f, 0.31f, 1.0f });

		{
			ObjectData objectData{};
			auto gameObjects = world.GetAllGameObjects();
			ui32 totalGameObjects = static_cast<ui32>(gameObjects.size());
			const auto& searchFilter = world.GetSearchFilter();
			bool hasFilter = !searchFilter.empty();
			bool wireframePass = (m_renderMode == RenderMode::Wireframe || m_renderMode == RenderMode::LitWireframe || m_renderMode == RenderMode::UnlitWireframe);
			bool unlitPass = (m_renderMode == RenderMode::Unlit || m_renderMode == RenderMode::UnlitWireframe);

			for (auto i : std::views::iota(0u, totalGameObjects))
			{
				auto object = gameObjects[i];
				if (!object) continue;

				if (!object->IsEnabled()) continue;

				if (hasFilter)
				{
					std::string nameLower = object->GetObjectName();
					std::string filterLower = searchFilter;
					std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
					std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), ::tolower);
					if (nameLower.find(filterLower) == std::string::npos)
						continue;
				}

				auto& transform = object->GetTransform();
				size_t objectType = object->GetTypeID();

				pimm::MaterialResource* material = nullptr;
				if (objectType == pimm::MeshObject::getTypeId())
				{
					auto meshComp = object->GetComponent<pimm::MeshComponent>();
					if (meshComp) material = meshComp->GetMaterial(0);
				}

				if (!material) material = object->GetMaterialComponent().GetMaterial();

				if (material)
				{
					objectData.world = transform.GetAffineWorldMatrix();

					context.SetGraphicsPipelineState(material->GetGraphicsPipelineState());

					auto d3dContext = context.GetD3D11DeviceContext();

					if (unlitPass && m_unlitPixelShader)
						d3dContext->PSSetShader(m_unlitPixelShader.Get(), nullptr, 0);

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

					if (wireframePass && m_wireframeRasterizer)
						d3dContext->RSSetState(m_wireframeRasterizer.Get());

					if (objectType == pimm::Quad::getTypeId())
					{
						context.SetVertexBuffer(object->GetComponent<QuadComponent>()->GetVertexBuffer());
						context.SetIndexBuffer(object->GetComponent<QuadComponent>()->GetIndexBuffer());
						context.Draw4PatchIndexedTriangleList(object->GetComponent<QuadComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					}
					else if (objectType == pimm::Cube::getTypeId())
					{
						context.SetVertexBuffer(object->GetComponent<CubeComponent>()->GetVertexBuffer());
						context.SetIndexBuffer(object->GetComponent<CubeComponent>()->GetIndexBuffer());
						context.Draw4PatchIndexedTriangleList(object->GetComponent<CubeComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					}
					else if (objectType == pimm::Sphere::getTypeId())
					{
						context.SetVertexBuffer(object->GetComponent<SphereComponent>()->GetVertexBuffer());
						context.SetIndexBuffer(object->GetComponent<SphereComponent>()->GetIndexBuffer());
						context.Draw4PatchIndexedTriangleList(object->GetComponent<SphereComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					}
					else if (objectType == pimm::Cylinder::getTypeId()) {
						context.SetVertexBuffer(object->GetComponent<CylinderComponent>()->GetVertexBuffer());
						context.SetIndexBuffer(object->GetComponent<CylinderComponent>()->GetIndexBuffer());
						context.Draw4PatchIndexedTriangleList(object->GetComponent<CylinderComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					}
					else if (objectType == pimm::Capsule::getTypeId()) {
						context.SetVertexBuffer(object->GetComponent<CapsuleComponent>()->GetVertexBuffer());
						context.SetIndexBuffer(object->GetComponent<CapsuleComponent>()->GetIndexBuffer());
						context.Draw4PatchIndexedTriangleList(object->GetComponent<CapsuleComponent>()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					}
					else if (objectType == pimm::MeshObject::getTypeId()) {
						context.SetVertexBuffer(object->GetComponent<MeshComponent>()->GetMesh()->GetVertexBuffer());
						context.SetIndexBuffer(object->GetComponent<MeshComponent>()->GetMesh()->GetIndexBuffer());
						context.Draw3PatchIndexedTriangleList(object->GetComponent<MeshComponent>()->GetMesh()->GetIndexBuffer().GetIndexListSize(), 0u, 0u);
					}

					if (wireframePass && m_wireframeRasterizer)
						d3dContext->RSSetState(nullptr);

				}
			}
		}

		if (m_viewportLayout == ViewportLayout::Single && m_gizmoRenderer)
		{
			const CameraObject* skipCamera = m_sceneCameraMode ? world.GetActiveCameraObject() : nullptr;

			m_gizmoRenderer->RenderGizmos(
				*context.GetD3D11DeviceContext().Get(),
				world,
				cameraData.view,
				cameraData.projection,
				skipCamera);
		}
	}

	m_graphicsDevice.ExecuteCommandList(context);

	auto immediateContext = m_graphicsDevice.GetD3DDeviceContext();
	m_deviceContext->ExecuteCommandList(immediateContext);
	auto rtv = swapChain.GetRenderTargetView();
	auto dsv = swapChain.GetDepthStencilView();
	immediateContext->OMSetRenderTargets(1, &rtv, dsv);
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	immediateContext->ClearRenderTargetView(rtv, clearColor);
	immediateContext->ClearDepthStencilView(
		dsv,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	m_uiManager.Render();

	swapChain.Present();
}

void pimm::WorldRenderer::SetViewportLayout(ViewportLayout layout) noexcept
{
	m_viewportLayout = layout;
}

pimm::ui32 pimm::WorldRenderer::GetViewportCount() const noexcept
{
	return (m_viewportLayout == ViewportLayout::Quad) ? 4 : 1;
}

pimm::FrameBuffer* pimm::WorldRenderer::GetViewportFrameBuffer(ui32 index) const
{
	if (index >= 4)
		return nullptr;
	if (m_viewportLayout == ViewportLayout::Quad)
		return m_viewportFrameBuffers[index].get();
	return m_frameBuffer.get();
}

pimm::Matrix4x4 pimm::WorldRenderer::BuildOrthoViewMatrix(ui32 viewIndex, const Vec3& focusPoint) const
	{
		f32 dist = 50.0f;
		Vec3 eye = focusPoint;
		Matrix4x4 translate{};
		Matrix4x4 rotate{};

		switch (viewIndex)
		{
		case 1:
			eye.z -= dist;
			translate = Matrix4x4::Translate({ -eye.x, -eye.y, -eye.z });
			return translate;
		case 2:
			eye.x += dist;
			translate = Matrix4x4::Translate({ -eye.x, -eye.y, -eye.z });
			rotate = Matrix4x4::RotateAlongY(90.0f);
			return translate * rotate;
		case 3:
			eye.y += dist;
			translate = Matrix4x4::Translate({ -eye.x, -eye.y, -eye.z });
			rotate = Matrix4x4::RotateAlongX(-90.0f);
			return translate * rotate;
		}

		return Matrix4x4::Identity();
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

void pimm::WorldRenderer::SetSceneCameraMode(bool enabled) noexcept
{
	m_sceneCameraMode = enabled;
}

bool pimm::WorldRenderer::IsSceneCameraMode() const noexcept
{
	return m_sceneCameraMode;
}

pimm::WorldRenderer::~WorldRenderer()
{
}
std::vector<pimm::RefPtr<pimm::VertexBuffer>>& pimm::WorldRenderer::GetVertexBuffer() noexcept
{
	return m_vertexBuffer;
}

std::vector<pimm::RefPtr<pimm::IndexBuffer>>& pimm::WorldRenderer::GetIndexBuffer() noexcept
{
	return m_indexBuffer;
}
