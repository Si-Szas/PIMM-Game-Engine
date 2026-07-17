#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Base.h>
#include <PIMM/Math/Rect.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/Math/Vec4.h>
#include <PIMM/Math/Matrix4x4.h>
#include <vector>

namespace pimm
{
	//We don't want the graphics engine to be further dervied by other classes
	class WorldRenderer final: public Base
	{
		public:
			//CONSTRUCTOR
			explicit WorldRenderer(const WorldRendererDescriptor& descriptor);

			//RENDER
			void Render(const World& world, SwapChain& swapChain, f32 deltaTime); //Important for rendering objects to the screen (app window)

			//GETTERS
			GraphicsDevice& GetGraphicsDevice() const noexcept;
			Rect GetSwapChainSize() const noexcept;
			std::vector<RefPtr<VertexBuffer>>& GetVertexBuffer() const noexcept;
			std::vector<RefPtr<IndexBuffer>>& GetIndexBuffer() const noexcept;
			FrameBuffer* GetFrameBuffer() const noexcept { return m_frameBuffer.get(); }
			void SetSceneViewSize(Rect size) noexcept { m_sceneViewSize = size; }


			//DESTRUCTOR
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
			Rect m_swapChainSize{};
			Rect m_sceneViewSize{};
			UIManager& m_uiManager;
			//Define a smart pointer to a render system variable of class Render System
			GraphicsDevice& m_graphicsDevice;
			RefPtr<DeviceContext> m_deviceContext{};
			RefPtr<GraphicsPipelineState> m_pipeline{};

			std::vector<RefPtr<VertexBuffer>> m_vertexBuffer{};
			std::vector<RefPtr<IndexBuffer>> m_indexBuffer{};

			RefPtr<ConstantBuffer> m_objectConstantBuffer{};
			RefPtr<ConstantBuffer> m_cameraConstantBuffer{};
			RefPtr<ConstantBuffer> m_materialConstantBuffer{};

			UniquePtr<FrameBuffer> m_frameBuffer{};

			RefPtr<Sampler> m_sampler{};
			std::vector<Texture*> m_textures{};
	};
}

