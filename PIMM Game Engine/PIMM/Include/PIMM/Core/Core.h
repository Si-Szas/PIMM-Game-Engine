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
		//Shaders
	class ShaderBinary;
	class VertexShaderSignature;
		//Buffers
	class VertexBuffer;
	class ConstantBuffer;
	class IndexBuffer;
	
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

	// ui //

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

	///////////// MATERIALS /////////////
	class ResourceManager;
	class Resource;
	class MaterialResource;

	//Type Aliases
	using i32 = int;
	using ui32 = unsigned int;
	using f32 = float;
	using d64 = double;
	
	template <typename Type> using RefPtr = std::shared_ptr<Type>;
	template <typename Type> using UniquePtr = std::unique_ptr<Type>;
}