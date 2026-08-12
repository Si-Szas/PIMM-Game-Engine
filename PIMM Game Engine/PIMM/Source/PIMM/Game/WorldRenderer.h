#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Base.h>
#include <PIMM/Math/Rect.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/Math/Vec4.h>
#include <PIMM/Math/Matrix4x4.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <memory>

namespace pimm
{
	class GizmoRenderer;

	enum class RenderMode
	{
		Lit = 0,
		Unlit,
		Wireframe,
		LitWireframe,
		UnlitWireframe
	};

	enum class ViewportLayout
	{
		Single = 0,
		Quad
	};

	class WorldRenderer final : public Base
	{
	public:
		//CONSTRUCTOR
		explicit WorldRenderer(const WorldRendererDescriptor& descriptor);

		//RENDER
		void Render(const World& world, SwapChain& swapChain, f32 deltaTime); //Important for rendering objects to the screen (app window)

		//GETTERS
		GraphicsDevice& GetGraphicsDevice() const noexcept;
		Rect GetSwapChainSize() const noexcept;
		std::vector<RefPtr<VertexBuffer>>& GetVertexBuffer() noexcept;
		std::vector<RefPtr<IndexBuffer>>& GetIndexBuffer() noexcept;
		FrameBuffer* GetFrameBuffer() const noexcept;
		void SetSceneViewSize(Rect size) noexcept;
		void SetSceneCameraMode(bool enabled) noexcept;
		bool IsSceneCameraMode() const noexcept;

		void SetRenderMode(RenderMode mode) noexcept { m_renderMode = mode; }
		RenderMode GetRenderMode() const noexcept { return m_renderMode; }

		void SetViewportLayout(ViewportLayout layout) noexcept;
		ViewportLayout GetViewportLayout() const noexcept { return m_viewportLayout; }
		ui32 GetViewportCount() const noexcept;
		FrameBuffer* GetViewportFrameBuffer(ui32 index) const;

		virtual ~WorldRenderer() override;

		struct alignas(16) ObjectData
		{
			Matrix4x4 world{};
		};

		struct alignas(16) CameraData
		{
			Matrix4x4 view{};
			Matrix4x4 projection{};
		};

	private:
		Matrix4x4 BuildOrthoViewMatrix(ui32 viewIndex, const Vec3& focusPoint) const;

		Rect m_swapChainSize{};
		Rect m_sceneViewSize{};
		UIManager& m_uiManager;
		GraphicsDevice& m_graphicsDevice;
		RefPtr<DeviceContext> m_deviceContext{};
		RefPtr<GraphicsPipelineState> m_pipeline{};

		std::vector<RefPtr<VertexBuffer>> m_vertexBuffer{};
		std::vector<RefPtr<IndexBuffer>> m_indexBuffer{};

		RefPtr<ConstantBuffer> m_objectConstantBuffer{};
		RefPtr<ConstantBuffer> m_cameraConstantBuffer{};
		RefPtr<ConstantBuffer> m_materialConstantBuffer{};

		UniquePtr<FrameBuffer> m_frameBuffer{};
		UniquePtr<FrameBuffer> m_viewportFrameBuffers[4]{};

		RefPtr<Sampler> m_sampler{};
		std::vector<Texture*> m_textures{};
		bool m_sceneCameraMode = false;
		RenderMode m_renderMode = RenderMode::Lit;
		ViewportLayout m_viewportLayout = ViewportLayout::Single;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizer{};
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_unlitPixelShader{};

		std::unique_ptr<GizmoRenderer> m_gizmoRenderer{};
	};
}