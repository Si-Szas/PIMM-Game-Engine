#include <PIMM/Game/GizmoRenderer.h>
#include <PIMM/Game/World.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AGameObject/CameraObject.h>
#include <PIMM/AComponent/CameraComponent.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/Math/MathUtility.h>

#include <d3dcompiler.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

#pragma comment(lib, "d3dcompiler")

namespace
{
	constexpr const char* kGizmoShaderSource = R"(
struct VS_INPUT
{
    float3 position : POSITION0;
    float4 color    : COLOR0;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

cbuffer GizmoCameraData : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 projection;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 viewPos = mul(float4(input.position, 1.0f), view);
    output.position = mul(viewPos, projection);
    output.color = input.color;
    return output;
}

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    return input.color;
}
)";

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShaderBlob(
		const char* source, size_t sourceSize, const char* entryPoint, const char* target,
		pimm::Logger& logger)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		HRESULT hr = D3DCompile(
			source, sourceSize, "GizmoShader", nullptr, nullptr,
			entryPoint, target, flags, 0, &blob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
				logger.Log(pimm::Logger::LogLevel::Error, "Gizmo shader compile failed: {}", static_cast<const char*>(errorBlob->GetBufferPointer()));
			else
				logger.Log(pimm::Logger::LogLevel::Error, "Gizmo shader compile failed (hr = 0x{:X}).", static_cast<unsigned long>(hr));
			return nullptr;
		}

		return blob;
	}

	// Transforms a point by a row-major Matrix4x4 (row-vector convention, matching the shaders).
	pimm::Vec3 TransformPoint(const pimm::Matrix4x4& m, const pimm::Vec3& p)
	{
		return
		{
			p.x * m.Row(0).x + p.y * m.Row(1).x + p.z * m.Row(2).x + 1.0f * m.Row(3).x,
			p.x * m.Row(0).y + p.y * m.Row(1).y + p.z * m.Row(2).y + 1.0f * m.Row(3).y,
			p.x * m.Row(0).z + p.y * m.Row(1).z + p.z * m.Row(2).z + 1.0f * m.Row(3).z,
		};
	}
}

pimm::GizmoRenderer::GizmoRenderer(ID3D11Device& device, Logger& logger) : m_logger(logger)
{
	InitShaders(device);
	InitBuffers(device);
	InitRasterizer(device);
}

pimm::GizmoRenderer::~GizmoRenderer() = default;

void pimm::GizmoRenderer::InitShaders(ID3D11Device& device)
{
	size_t sourceSize = std::char_traits<char>::length(kGizmoShaderSource);

	auto vsBlob = CompileShaderBlob(kGizmoShaderSource, sourceSize, "VS_Main", "vs_4_0", m_logger);
	auto psBlob = CompileShaderBlob(kGizmoShaderSource, sourceSize, "PS_Main", "ps_4_0", m_logger);
	if (!vsBlob || !psBlob)
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: failed to compile gizmo shaders; gizmos will be disabled.");
		return;
	}

	D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(device.CreateInputLayout(inputElements, 2,
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout)))
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: CreateInputLayout failed.");
		return;
	}

	if (FAILED(device.CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader)))
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: CreateVertexShader failed.");
		return;
	}

	if (FAILED(device.CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader)))
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: CreatePixelShader failed.");
		return;
	}
}

void pimm::GizmoRenderer::InitBuffers(ID3D11Device& device)
{
	D3D11_BUFFER_DESC vbDesc{};
	vbDesc.ByteWidth = static_cast<UINT>(MaxLineVertices * sizeof(LineVertex));
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(device.CreateBuffer(&vbDesc, nullptr, &m_vertexBuffer)))
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: failed to create gizmo vertex buffer.");
		return;
	}

	D3D11_BUFFER_DESC cbDesc{};
	cbDesc.ByteWidth = sizeof(GizmoCameraData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(device.CreateBuffer(&cbDesc, nullptr, &m_constantBuffer)))
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: failed to create gizmo constant buffer.");
		return;
	}
}

void pimm::GizmoRenderer::InitRasterizer(ID3D11Device& device)
{
	D3D11_RASTERIZER_DESC rsDesc{};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	if (FAILED(device.CreateRasterizerState(&rsDesc, &m_rasterizerState)))
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: failed to create rasterizer state.");
		return;
	}

	D3D11_DEPTH_STENCIL_DESC dsDesc{};
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.StencilEnable = FALSE;
	if (FAILED(device.CreateDepthStencilState(&dsDesc, &m_depthStencilState)))
	{
		m_logger.Log(pimm::Logger::LogLevel::Error, "GizmoRenderer: failed to create depth stencil state.");
		return;
	}
}

void pimm::GizmoRenderer::RenderGizmos(ID3D11DeviceContext& context, const World& world,
	const Matrix4x4& view, const Matrix4x4& projection, const CameraObject* activeCamera)
{
	if (!m_vertexShader || !m_pixelShader || !m_inputLayout || !m_vertexBuffer || !m_constantBuffer)
		return;

	m_lineVertices.clear();

	ui32 cameraCount = 0;
	ui32 drawnCount = 0;

	auto gameObjects = world.GetAllGameObjects();
	static size_t lastObjectCount = static_cast<size_t>(-1);
	static size_t lastCameraCount = static_cast<size_t>(-1);
	if (gameObjects.size() != lastObjectCount)
	{
		lastObjectCount = gameObjects.size();
		m_logger.Log(pimm::Logger::LogLevel::Information,
			"GizmoRenderer: GetAllGameObjects returned {} object(s). CameraObject typeId = {}",
			gameObjects.size(), CameraObject::getTypeId());
		for (auto* object : gameObjects)
		{
			if (!object) continue;
			m_logger.Log(pimm::Logger::LogLevel::Information,
				"GizmoRenderer: object '{}' typeId={} (match={})",
				object->GetObjectName(), object->GetTypeID(),
				(object->GetTypeID() == CameraObject::getTypeId()));
		}
	}
	for (auto* object : gameObjects)
	{
		if (!object) continue;
		if (object->GetTypeID() != CameraObject::getTypeId()) continue;
		++cameraCount;

		if (object == activeCamera)
		{
			m_logger.Log(pimm::Logger::LogLevel::Information, "GizmoRenderer: skipping active camera '{}' (we are looking through it).", object->GetObjectName());
			continue;
		}

		auto* camComp = object->GetComponent<CameraComponent>();
		if (!camComp) continue;

		const f32 fov = camComp->GetFieldOfView();
		const f32 nearPlane = camComp->GetNearPlane();
		const f32 farPlane = camComp->GetFarPlane();
		const Rect viewport = camComp->GetViewportSize();
		const f32 aspect = (viewport.height > 0) ? static_cast<f32>(viewport.width) / static_cast<f32>(viewport.height) : 1.0f;

		constexpr f32 kGizmoScale = 0.01f;

		const f32 halfH = std::tan(fov * 0.5f);
		const f32 halfW = halfH * aspect;

		const f32 gizmoNear = nearPlane * kGizmoScale;
		const f32 gizmoFar = farPlane * kGizmoScale;

		const Vec3 camSpace[8] =
		{
			{ -halfW * gizmoNear,  halfH * gizmoNear, gizmoNear },
			{  halfW * gizmoNear,  halfH * gizmoNear, gizmoNear },
			{  halfW * gizmoNear, -halfH * gizmoNear, gizmoNear },
			{ -halfW * gizmoNear, -halfH * gizmoNear, gizmoNear },
			{ -halfW * gizmoFar,   halfH * gizmoFar,  gizmoFar  },
			{  halfW * gizmoFar,   halfH * gizmoFar,  gizmoFar  },
			{  halfW * gizmoFar,  -halfH * gizmoFar,  gizmoFar  },
			{ -halfW * gizmoFar,  -halfH * gizmoFar,  gizmoFar  },
		};

		Vec3 worldCorners[8];
		const Matrix4x4 camWorld = object->GetTransform().GetRigidWorldMatrix();
		for (int i = 0; i < 8; ++i)
			worldCorners[i] = TransformPoint(camWorld, camSpace[i]);

		// Gizmo color: cyan for inactive cameras, yellow for the active one.
		const float color[4] = { 1.0f, 0.85f, 0.0f, 1.0f };

		auto AddLine = [&](const Vec3& a, const Vec3& b)
		{
			if (m_lineVertices.size() + 2 > MaxLineVertices) return;
			m_lineVertices.push_back({ { a.x, a.y, a.z }, { color[0], color[1], color[2], color[3] } });
			m_lineVertices.push_back({ { b.x, b.y, b.z }, { color[0], color[1], color[2], color[3] } });
		};

		// Near plane
		AddLine(worldCorners[0], worldCorners[1]);
		AddLine(worldCorners[1], worldCorners[2]);
		AddLine(worldCorners[2], worldCorners[3]);
		AddLine(worldCorners[3], worldCorners[0]);
		// Far plane
		AddLine(worldCorners[4], worldCorners[5]);
		AddLine(worldCorners[5], worldCorners[6]);
		AddLine(worldCorners[6], worldCorners[7]);
		AddLine(worldCorners[7], worldCorners[4]);
		// Connecting edges
		AddLine(worldCorners[0], worldCorners[4]);
		AddLine(worldCorners[1], worldCorners[5]);
		AddLine(worldCorners[2], worldCorners[6]);
		AddLine(worldCorners[3], worldCorners[7]);

		// A short "forward" indicator from near center to make orientation obvious.
		const Vec3 nearCenter = TransformPoint(camWorld, { 0.0f, 0.0f, gizmoNear });
		const Vec3 tip = TransformPoint(camWorld, { 0.0f, 0.0f, gizmoNear + std::min(gizmoFar - gizmoNear, 0.125f) });
		AddLine(nearCenter, tip);

		++drawnCount;
	}

	if (cameraCount != lastCameraCount)
	{
		lastCameraCount = cameraCount;
		m_logger.Log(pimm::Logger::LogLevel::Information,
			"GizmoRenderer: found {} camera object(s), drew {} frustum(s), {} line vertices.",
			cameraCount, drawnCount, m_lineVertices.size());
	}

	if (m_lineVertices.empty())
		return;

	// Update constant buffer with the rendering camera's view/projection.
	D3D11_MAPPED_SUBRESOURCE mapped{};
	if (SUCCEEDED(context.Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		GizmoCameraData data{ view, projection };
		std::memcpy(mapped.pData, &data, sizeof(data));
		context.Unmap(m_constantBuffer.Get(), 0);
	}

	// Update vertex buffer with the line vertices.
	if (SUCCEEDED(context.Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		std::memcpy(mapped.pData, m_lineVertices.data(), m_lineVertices.size() * sizeof(LineVertex));
		context.Unmap(m_vertexBuffer.Get(), 0);
	}

	// Bind gizmo pipeline and draw.
	context.IASetInputLayout(m_inputLayout.Get());
	context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	UINT stride = sizeof(LineVertex);
	UINT offset = 0;
	ID3D11Buffer* vbs[] = { m_vertexBuffer.Get() };
	context.IASetVertexBuffers(0, 1, vbs, &stride, &offset);

	context.VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context.HSSetShader(nullptr, nullptr, 0);
	context.DSSetShader(nullptr, nullptr, 0);
	context.GSSetShader(nullptr, nullptr, 0);
	context.PSSetShader(m_pixelShader.Get(), nullptr, 0);

	ID3D11Buffer* cbs[] = { m_constantBuffer.Get() };
	context.VSSetConstantBuffers(0, 1, cbs);
	context.PSSetConstantBuffers(0, 1, cbs);

	context.PSSetShaderResources(0, 0, nullptr);
	ID3D11SamplerState* samplers[] = { nullptr };
	context.PSSetSamplers(0, 0, nullptr);

	if (m_rasterizerState)
		context.RSSetState(m_rasterizerState.Get());

	if (m_depthStencilState)
		context.OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	context.Draw(static_cast<UINT>(m_lineVertices.size()), 0);
}