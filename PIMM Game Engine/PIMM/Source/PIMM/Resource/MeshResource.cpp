#include <PIMM/Resource/MeshResource.h>
#include <PIMM/Math/Vec2.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Resource/ResourceManager.h>

#include <filesystem>
#include <ranges>
#include <map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

pimm::MeshResource::MeshResource(const MeshResourceDescriptor& desc) : Resource(desc.base)
{
	auto inputfile{ std::filesystem::path(desc.base.path).string() };
	tinyobj::ObjReader objectReader{};

	if (!objectReader.ParseFromFile(inputfile)) PIMMLogThrowError("ParseFromFile(), mesh failed to load.");
	if (!objectReader.Error().empty()) PIMMLogThrowError("Failed to load mesh {}. Details: {}", inputfile, objectReader.Error());
	if (!objectReader.Warning().empty()) PIMMLogWarning("Mesh {} loaded with warnings: {}", inputfile, objectReader.Warning());

	const auto& attributes = objectReader.GetAttrib();
	const auto& shapes = objectReader.GetShapes();

	std::vector<MeshVertex> listVertices{};
	std::vector<pimm::ui32> listIndices{};

	std::map<pimm::i32, std::vector<pimm::ui32>> indicesPerMaterial{};

	for (const auto& shape : shapes)
	{
		auto indexOffset{ 0u };
		for (auto f : std::views::iota(0u, shape.mesh.num_face_vertices.size()))
		{
			auto numFaceVerts = shape.mesh.num_face_vertices[f];

			int material = shape.mesh.material_ids[f];
			auto& materialIndices = indicesPerMaterial[material];
			auto startIndexMaterial{ materialIndices.size() };

			for (auto v : std::views::iota(0u, numFaceVerts))
			{
				auto& index{ shape.mesh.indices[indexOffset + v] };

				MeshVertex vertex{};

				vertex.position = Vec3{
					attributes.vertices[index.vertex_index * 3 + 0],
					attributes.vertices[index.vertex_index * 3 + 1],
				   -attributes.vertices[index.vertex_index * 3 + 2]
				};

				if (index.texcoord_index >= 0)
				{
					vertex.texcoord = Vec2{
						attributes.texcoords[index.texcoord_index * 2 + 0],
						1.0f - attributes.texcoords[index.texcoord_index * 2 + 1]
					};
				}

				materialIndices.push_back(static_cast<pimm::ui32>(listVertices.size()));
				listVertices.push_back(vertex);
			}
			std::swap(materialIndices[startIndexMaterial + 1], materialIndices[startIndexMaterial + 2]);

			indexOffset += numFaceVerts;
		}
	}

	for (const auto& [materialIndex, materialIndices] : indicesPerMaterial)
	{
		MaterialSlot slot{
			static_cast<pimm::ui32>(listIndices.size()),
			static_cast<pimm::ui32>(materialIndices.size()),
			materialIndex
		};

		listIndices.insert(
			listIndices.end(),
			materialIndices.begin(),
			materialIndices.end());

		m_materialSlots.push_back(slot);
	}

	m_vertexBuffer = desc.graphicsDevice.CreateVertexBuffer(
		{ listVertices.data() ,static_cast<pimm::ui32>(listVertices.size()), sizeof(MeshVertex) });
	m_indexBuffer = desc.graphicsDevice.CreateIndexBuffer(
		{ listIndices.data(), static_cast<pimm::ui32>(listIndices.size()) });
}

const pimm::MaterialSlot* pimm::MeshResource::GetMaterialSlots(ui32& numSlots) const noexcept
{
	numSlots = static_cast<pimm::ui32>(m_materialSlots.size());
	return m_materialSlots.data();
}


pimm::ui32 pimm::MeshResource::GetNumberOfMaterialSlots() const noexcept
{
	return static_cast<pimm::ui32>(m_materialSlots.size());
}

const pimm::VertexBuffer& pimm::MeshResource::GetVertexBuffer() const noexcept
{
	return *m_vertexBuffer;
}

const pimm::IndexBuffer& pimm::MeshResource::GetIndexBuffer() const noexcept
{
	return *m_indexBuffer;
}