#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Logger.h>
#include <PIMM/Math/Matrix4x4.h>

#include <d3d11.h>
#include <wrl.h>

#include <vector>

namespace pimm
{
	class World;

	class GizmoRenderer final
	{
	public:
		GizmoRenderer(ID3D11Device& device, Logger& logger);
		~GizmoRenderer();

		GizmoRenderer(const GizmoRenderer&) = delete;
		GizmoRenderer& operator=(const GizmoRenderer&) = delete;

		void RenderGizmos(ID3D11DeviceContext& context, const World& world,
			const Matrix4x4& view, const Matrix4x4& projection,
			const class CameraObject* activeCamera);

	private:
		void InitShaders(ID3D11Device& device);
		void InitBuffers(ID3D11Device& device);
		void InitRasterizer(ID3D11Device& device);

		struct LineVertex
		{
			float position[3];
			float color[4];
		};

		struct alignas(16) GizmoCameraData
		{
			Matrix4x4 view;
			Matrix4x4 projection;
		};

		Logger& m_logger;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>       m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>       m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

		std::vector<LineVertex> m_lineVertices;

		static constexpr size_t MaxLineVertices{ 4096 };
	};
}