#pragma once
#include <stdexcept>
#include <memory>

#define pimm_disable_copy_and_move(Class)\
protected:\
	Class(const Class&) = delete;\
	Class& operator=(const Class&) = delete;\
	Class(Class&&) = delete;\
	Class& operator=(Class&&) = delete;

namespace pimm {
	///////////// WINDOW /////////////
	class Base;
	class Window;
	class Display;
	class Game;
	//Logger
	class Logger;

	///////////// RENDERER /////////////
	//Graphics Engine
	class GraphicsEngine;
	class GraphicsDevice;
	class SwapChain;
	class DeviceContext;
	class GraphicsPipelineState;
	class GraphicsPipelineLayout;
		//Shaders
	class ShaderBinary;
		//Buffers
	class VertexBuffer;
	class ConstantBuffer;
	class IndexBuffer;
	class FrameBuffer;
	
	///////////// WORLD /////////////
	// Identifier
	class Identifier;
	//World
	class World;
	class WorldRenderer;
	//Game Object
	class AGameObject;
		class Quad;
		class Cube;
		class Sphere;
		class Cylinder;
		class Capsule;
	//Components
	class AComponent;
		class TransformComponent;
		class CameraComponent;
		class QuadComponent;
		class CubeComponent;
		class SphereComponent;
		class CylinderComponent;
		class CapsuleComponent;
		class MaterialComponent;

	///////////// UI /////////////
	class UIManager;
	
	///////////// INPUT SYSTEM /////////////
	class InputSystem;
	class InputCommand;
		class MoveForwardCommand;
		class MoveBackwardCommand;
		class MoveRightCommand;
		class MoveLeftCommand;
		class CreateAGameObjectCommand;

	/////////// GAME LOOP - ENGINE TIME ///////////
	class EngineTime;

	///////////// RESOURCES /////////////
	class ResourceManager;
		class Resource;
		class MaterialResource;
	///////////// TEXTURES /////////////
	class TextureResource;
		class Texture;
		class Sampler;
	///////////// MESH /////////////
	class MeshManager;
		class MeshResource;

	//Type Aliases
	using i32 = int;
	using ui32 = unsigned int;
	using f32 = float;
	using d64 = double;
	
	template <typename Type> using RefPtr = std::shared_ptr<Type>;
	template <typename Type> using UniquePtr = std::unique_ptr<Type>;
}