#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Logger.h>
#include <PIMM/Math/Rect.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/Math/Vec4.h>

namespace pimm 
{
	//Groups all dependencies used by Base Class
	struct BaseDescriptor
	{
		Logger& logger;
	};

	struct WindowDescriptor
	{
		BaseDescriptor base;
		Rect size{};
	};

	struct DisplayDescriptor
	{
		WindowDescriptor window;
		GraphicsDevice& graphicsDevice;
	};

	struct GameContext
	{
		InputSystem& inputSystem;
	};

	struct UIManagerDescriptor
	{
		BaseDescriptor base;
		GraphicsDevice& graphicsDevice;
		void* windowHandle;
	};

	struct WorldRendererDescriptor 
	{
		BaseDescriptor base;
		GraphicsDevice& graphicsEngine;
		UIManager& uiManager;
	};

	struct GraphicsDeviceDescriptor
	{
		BaseDescriptor base;
	};

	struct SwapChainDescriptor
	{
		void* windowHandle{};
		Rect windowSize{};
	};

	enum class ShaderType 
	{
		VertexShader = 0,
		PixelShader,
		HullShader,
		DomainShader
	};

	struct ShaderCompileDescriptor
	{
		//Instead of using a string, we use const char to make it ABI safe
		//ABI is application binary interface, a structure is ABI safe if it has same memory layout and size across all major compilers
		const char* shaderSourceName{};	//Source name of the shader
		const void* shaderSourceCode{}; //Point to actual source code of shader
		size_t shaderSourceCodeSize{};  //Size of the source code in bytes (use size_t) since it can be large
		const char* shaderEntryPoint{};	//Entry point of the shader, which is a string
		ShaderType shaderType{};		//Type of shader we are compiling
	};

	struct VertexShaderSignatureDescriptor
	{
		const RefPtr<ShaderBinary>& vertexShaderBinary;
	};

	struct GraphicsPipelineStateDescriptor
	{
		const VertexShaderSignature& vertexShader;
		const ShaderBinary& pixelShader;
		const ShaderBinary& hullShader;
		const ShaderBinary& domainShader;
	};

	struct BinaryData
	{
		const void* data{};
		size_t dataSize{};
	};

	struct VertexBufferDescriptor
	{
		const void* vertexList{};	//Vertex list
		ui32 vertexListSize{};		//Size in bytes of the vertex list
		ui32 vertexSize;			//Holds size in bytes of a single vertex
	};

	struct ConstantBufferDescriptor
	{
		const void* buffer{};		//Since we have yet to define what constants we want to pass, make a const void*
		ui32 bufferSize{};			//Size of the buffer
	};

	struct IndexBufferDescriptor
	{
		const ui32* indexList{};	//Since the index buffer just contains the indices of objects, it is not a void*
		ui32 indexListSize{};		//Size of the buffer
	};

	//POINTS
	struct Vertex
	{
		Vec3 position;
		Vec4 color;
	};

	struct AGameObjectDescriptor
	{
		BaseDescriptor base;
		GameContext gameContext;
		World& world;
		WorldRenderer& worldRenderer;
	};

	struct AComponentDescriptor
	{
		BaseDescriptor base;
		AGameObject& object;
		World& world;
	};

	//INPUT SYSTEM
	struct InputSystemDescriptor
	{
		BaseDescriptor base;
		World* world;
		UIManager* uiManager;
	};

	enum class KeyCode
	{
		Unknown = 0,

		A, B, C, D, E, F, 
		G, H, I, J, K, L, 
		M, N, O, P, Q, R, 
		S, T, U, V, W, X, 
		Y, Z,

		Num0, Num1, Num2,
		Num3, Num4, Num5, 
		Num6, Num7, Num8, 
		Num9,

		Escape, Shift, Space, 
		Enter, Backspace, Delete,
		LeftControl, RightControl,

		Up, Down, Left, Right,

		Count
	};

	enum class MouseInput
	{
		MouseLeft = 0,
		MouseRight,
		MouseMiddle,

		Count
	};

	struct WorldDescriptor {
		BaseDescriptor base;
		GameContext gameContext;
		WorldRenderer& worldRenderer;
	};

	struct GameDescriptor
	{
		Rect windowSize{ 1280, 720 };
		Logger::LogLevel logLevel = Logger::LogLevel::Error;
	};

}