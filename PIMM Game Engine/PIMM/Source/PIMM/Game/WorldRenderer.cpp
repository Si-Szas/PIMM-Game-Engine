#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Graphics/DeviceContext/DeviceContext.h>
#include <PIMM/Graphics/SwapChain/SwapChain.h>
#include <PIMM/Time/EngineTime.h>
#include <PIMM/Graphics/VertexBuffer/VertexBuffer.h>
#include <PIMM/Graphics/ConstantBuffer/ConstantBuffer.h>
#include <PIMM/Graphics/IndexBuffer/IndexBuffer.h>
//GAME AND WORLD HEADER//
#include <PIMM/Game/World.h>
//GAME OBJECTS//
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AGameObject/Cube.h>
#include <PIMM/AGameObject/Sphere.h>
//COMPONENTS//
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/CubeComponent.h>
#include <PIMM/AComponent/SphereComponent.h>
#include <PIMM/AComponent/CameraComponent.h>
//IMGUI//
#include <PIMM/Graphics/ImGui/imgui_impl_dx11.h>
#include <PIMM/Graphics/ImGui/imgui_impl_win32.h>

#include <PIMM/Math/Vec2.h>
#include <PIMM/Math/Vec3.h>
#include <fstream>
#include <ranges>

pimm::WorldRenderer::WorldRenderer(const WorldRendererDescriptor& descriptor) :
	Base(descriptor.base),
	m_graphicsDevice(descriptor.graphicsEngine)
{
	//Creates the deferred device context
	auto& device = m_graphicsDevice;
	m_deviceContext = device.CreateDeviceContext();

	//Define the Shader File Path
		// Relative paths are relative to the root of project folder (DirectX Game Folder)
	constexpr char vertexShaderFilePath[] = "PIMM/Assets/Shaders/VertexShader.hlsl";
	//Read the contents of the shader file
	std::ifstream vertexShaderStream(vertexShaderFilePath);
	if (!vertexShaderStream) PIMMLogThrowError("Failed to open VertexShader.hlsl file.");
	//Retrieve file data. So calling Range would call the entire shader into a string
	std::string vertexShaderFileData{
		std::istreambuf_iterator<char>(vertexShaderStream),	//Beginning of the file
		std::istreambuf_iterator<char>()				//Beginning of the end
	};

	//Create a ShaderSourceCode using constexpr (evaluate value at compile time)
	auto vertexShaderSourceCode = vertexShaderFileData.c_str();
	auto vertexShaderSourceCodeSize = vertexShaderFileData.length();

	//Call our compile shader method, pass the shader we created
		//VERTEX SHADER
	auto vs = device.CompileShader({
		vertexShaderFilePath,
		vertexShaderSourceCode,
		vertexShaderSourceCodeSize,
		"VS_Main",
		ShaderType::VertexShader
		});

	//Define Shader File Path for Hull Shader
	constexpr char hullShaderFilePath[] = "PIMM/Assets/Shaders/HullShader.hlsl";
	std::ifstream hullShaderStream(hullShaderFilePath);
	if (!hullShaderStream) PIMMLogThrowError("Failed to open HullShader.hlsl file.");

	std::string hullShaderFileData{
		std::istreambuf_iterator<char>(hullShaderStream),	//Beginning of the file
		std::istreambuf_iterator<char>()				//Beginning of the end
	};

	auto hullShaderSourceCode = hullShaderFileData.c_str();
	auto hullShaderSourceCodeSize = hullShaderFileData.length();

	//Compile Hull Shader
	auto hs = device.CompileShader({
		hullShaderFilePath,
		hullShaderSourceCode,
		hullShaderSourceCodeSize,
		"HS_Main",
		ShaderType::HullShader
		});

	//Define Shader File Path for Domain Shader
	constexpr char domainShaderFilePath[] = "PIMM/Assets/Shaders/DomainShader.hlsl";
	std::ifstream domainShaderStream(domainShaderFilePath);
	if (!domainShaderStream) PIMMLogThrowError("Failed to open DomainShader.hlsl file.");

	std::string domainShaderFileData{
		std::istreambuf_iterator<char>(domainShaderStream),	//Beginning of the file
		std::istreambuf_iterator<char>()				//Beginning of the end
	};

	auto domainShaderSourceCode = domainShaderFileData.c_str();
	auto domainShaderSourceCodeSize = domainShaderFileData.length();

	//Compile Domain Shader
	auto ds = device.CompileShader({
		domainShaderFilePath,
		domainShaderSourceCode,
		domainShaderSourceCodeSize,
		"DS_Main",
		ShaderType::DomainShader
		});

	constexpr char pixelShaderFilePath[] = "PIMM/Assets/Shaders/PixelShader.hlsl";
	//Read the contents of the shader file
	std::ifstream pixelShaderStream(pixelShaderFilePath);
	if (!pixelShaderStream) PIMMLogThrowError("Failed to open PixelShader.hlsl file.");
	//Retrieve file data. So calling Range would call the entire shader into a string
	std::string pixelShaderFileData{
		std::istreambuf_iterator<char>(pixelShaderStream),	//Beginning of the file
		std::istreambuf_iterator<char>()				//Beginning of the end
	};

	//Create a ShaderSourceCode using constexpr (evaluate value at compile time)
	auto pixelShaderSourceCode = pixelShaderFileData.c_str();
	auto pixelShaderSourceCodeSize = pixelShaderFileData.length();

	//PIXEL SHADER
	auto ps = device.CompileShader({
		pixelShaderFilePath,
		pixelShaderSourceCode,
		pixelShaderSourceCodeSize,
		"PS_Main",
		ShaderType::PixelShader
		});

	auto vertexShaderSignature = device.CreateVertexShaderSignature({ vs });

	//Create Graphics Pipeline State
	m_pipeline = device.CreateGraphicsPipelineState({ *vertexShaderSignature, *ps, *hs, *ds });

	//Create constant buffer
	m_dsConstantBuffer = device.CreateConstantBuffer
	({
		&m_dsConstantBuffer,
		sizeof(ConstantData)
	});

	//We don't have any constant data to pass to the vertex, hull, or pixel shader
	m_vsConstantBuffer = nullptr;
	m_hsConstantBuffer = nullptr;
	m_psConstantBuffer = nullptr;

}

void pimm::WorldRenderer::Render(const World& world, SwapChain& swapChain, f32 deltaTime)
{	
	////////// CAMERA SET-UP //////////
	m_swapChainSize = swapChain.GetSize();;
	
	////////// DEVICE CONTEXT //////////
	// - Update the constant buffer before everything
	// - context.UpdateConstantBuffer(vsConstantBuffer, &data);
	// - We want to first clear the buffer, then after rendering on a back buffer, we want to move that back to the front buffer
	// - Record render command that clears content of back buffer and binds it so we can render elements onto it
	// - Use Pipeline
	//	- Bind all objects inside graphics pipeline state (shaders) to actual GPU pipeline
	auto& context = *m_deviceContext;
	context.ClearAndSetBackBuffer(swapChain, { 0.251f, 0.141f, 0.31f, 1.0f });
	context.SetGraphicsPipelineState(*m_pipeline);
	context.SetViewportSize(m_swapChainSize);

	////////// ACOMPONENTS //////////
	auto numberOfComponents = 0u;
	
	////////// CONSTANT BUFFER DATA //////////
	ConstantData data{};
	{
		auto cameraComponents = world.GetAComponent<CameraComponent>(numberOfComponents);

		for (auto i : std::views::iota(0u, numberOfComponents))
		{
			auto camComponent = cameraComponents[i];
			data.view = camComponent->GetViewMatrix();
			camComponent->SetViewportSize(m_swapChainSize);
			data.projection = camComponent->GetProjectionMatrix();
			break;
		}
	}
	{
		auto gameObjects = world.GetAllGameObjects();
		ui32 totalGameObjects = static_cast<ui32>(gameObjects.size());

		//std::cout << "[LOG] Current Game Objects No.: " << totalGameObjects << std::endl;

		for (auto i : std::views::iota(0u, totalGameObjects))
		{
			auto object = gameObjects[i];
			if (!object) continue;
			auto& transform = object->GetTransform();
			size_t objectType = object->GetTypeID();

			data.world = transform.GetAffineWorldMatrix();

			////////// UPDATE EACH CONSTANT BUFFER PASSED TO THE SHADERS //////////
			auto& vsConstantBuffer = *m_vsConstantBuffer;
			auto& hsConstantBuffer = *m_hsConstantBuffer;
			auto& dsConstantBuffer = *m_dsConstantBuffer;
			auto& psConstantBuffer = *m_psConstantBuffer;
			context.UpdateConstantBuffer(vsConstantBuffer, &data);
			context.UpdateConstantBuffer(hsConstantBuffer, &data);
			context.UpdateConstantBuffer(dsConstantBuffer, &data);
			context.UpdateConstantBuffer(psConstantBuffer, &data);

			auto& vb = *m_vertexBuffer[object->GetVertexOffset()];
			auto& ib = *m_indexBuffer[object->GetIndexLocation()];

			context.SetVertexBuffer(vb);
			////////// SET EACH CONSTANT BUFFER PASSED TO THE SHADERS //////////
			context.SetVSConstantBuffer(0, 1, vsConstantBuffer);
			context.SetHSConstantBuffer(0, 1, hsConstantBuffer);
			context.SetDSConstantBuffer(0, 1, dsConstantBuffer);
			context.SetPSConstantBuffer(0, 1, psConstantBuffer);

			context.SetIndexBuffer(ib);
			context.Draw4PatchIndexedTriangleList(ib.GetIndexListSize(), 0u, 0u);
		}
	}

	//Pass device context where we will extract the commands from
	m_graphicsDevice.ExecuteCommandList(context);

	auto immediateContext = m_graphicsDevice.GetD3DDeviceContext();
	m_deviceContext->ExecuteCommandList(immediateContext);
	auto rtv = swapChain.GetRenderTargetView();
	auto dsv = swapChain.GetDepthStencilView();
	immediateContext->OMSetRenderTargets(1, &rtv, dsv);


	//IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	//Present our back buffer with its rendered content on the window
	swapChain.Present();
}

pimm::GraphicsDevice& pimm::WorldRenderer::GetGraphicsDevice() const noexcept
{
	return m_graphicsDevice;
}

pimm::Rect pimm::WorldRenderer::GetSwapChainSize() const noexcept
{
	return m_swapChainSize;
}

std::vector<pimm::RefPtr<pimm::VertexBuffer>>& pimm::WorldRenderer::GetVertexBuffer() const noexcept
{
	return const_cast<WorldRenderer*>(this)->m_vertexBuffer;
}

std::vector<pimm::RefPtr<pimm::IndexBuffer>>& pimm::WorldRenderer::GetIndexBuffer() const noexcept
{
	return const_cast<WorldRenderer*>(this)->m_indexBuffer;
}

pimm::WorldRenderer::~WorldRenderer()
{
}
