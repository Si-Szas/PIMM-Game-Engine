#include <PIMM/Resource/MeshResource.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

#include <filesystem>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// Helper structure to handle duplicate vertices efficiently via hashing
struct VertexHasher {
    size_t operator()(const pimm::MeshVertex& v) const {
        size_t h1 = std::hash<float>()(v.position[0]) ^ std::hash<float>()(v.position[1]) ^ std::hash<float>()(v.position[2]);
        size_t h2 = std::hash<float>()(v.uv[0]) ^ std::hash<float>()(v.uv[1]);
        return h1 ^ (h2 << 1);
    }
};

struct VertexEqual {
    bool operator()(const pimm::MeshVertex& lhs, const pimm::MeshVertex& rhs) const {
        return lhs.position[0] == rhs.position[0] && lhs.position[1] == rhs.position[1] && lhs.position[2] == rhs.position[2] &&
            lhs.normal[0] == rhs.normal[0] && lhs.normal[1] == rhs.normal[1] && lhs.normal[2] == rhs.normal[2] &&
            lhs.uv[0] == rhs.uv[0] && lhs.uv[1] == rhs.uv[1];
    }
};

pimm::MeshResource::MeshResource(const MeshResourceDescriptor& descriptor)
    : Resource(descriptor.base)
{
    LoadMeshFromFile(descriptor.base.path);

    auto& device = descriptor.graphicsDevice;

    m_vertexOffset = static_cast<ui32>(worldRenderer.GetVertexBuffer().size());
    m_indexLocation = static_cast<ui32>(worldRenderer.GetIndexBuffer().size());

    worldRenderer.GetVertexBuffer().push_back(device.CreateVertexBuffer
    ({
        m_vertices,
        std::size(m_vertices),
        sizeof(MeshVertex)
        }));

    worldRenderer.GetIndexBuffer().push_back(device.CreateIndexBuffer
    ({
        m_indices,//Index List
        std::size(m_indices)//Index List Size
        }));
}

void pimm::MeshResource::LoadMeshFromFile(const wchar_t* filePath)
{
    std::filesystem::path resourcePath(filePath);
    std::string stringPath = resourcePath.string();
    std::string baseDir = resourcePath.parent_path().string() + "/";

    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.mtl_search_path = baseDir; 

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(stringPath, readerConfig)) {
        if (!reader.Error().empty()) {
            // PIMMLogError("TinyObjLoader Error: {}", reader.Error().c_str());
        }
        return;
    }

    if (!reader.Warning().empty()) {
        // PIMMLogWarning("TinyObjLoader Warning: {}", reader.Warning().c_str());
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    std::unordered_map<MeshVertex, unsigned int, VertexHasher, VertexEqual> uniqueVertices{};

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                MeshVertex vertex{};

                // Positions
                vertex.position[0] = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                vertex.position[1] = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                vertex.position[2] = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                // Normals
                if (idx.normal_index >= 0) {
                    vertex.normal[0] = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    vertex.normal[1] = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    vertex.normal[2] = attrib.normals[3 * size_t(idx.normal_index) + 2];
                }

                // Texture Coordinates
                if (idx.texcoord_index >= 0) {
                    vertex.uv[0] = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vertex.uv[1] = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]; // Flip Y if your API requires it (like DirectX)
                }

                // Deduplicate vertices to optimize memory and index cache usage
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<unsigned int>(m_vertices.size());
                    m_vertices.push_back(vertex);
                }

                m_indices.push_back(uniqueVertices[vertex]);
            }
            index_offset += fv;
        }
    }
}