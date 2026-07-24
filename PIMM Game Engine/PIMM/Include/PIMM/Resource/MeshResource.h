#pragma once

#include <PIMM/Core/Common.h>
#include <PIMM/Resource/Resource.h>
#include <vector>

namespace pimm {

    struct MeshVertex {
        float position[3];
        float normal[3];
        float uv[2];
    };

    class MeshResource : public Resource {
    public:
        MeshResource(const MeshResourceDescriptor& descriptor);
        //virtual ~MeshResource() override = default;

        const std::vector<MeshVertex>& GetVertices() const { return m_vertices; }
        const std::vector<unsigned int>& GetIndices() const { return m_indices; }

    private:
        void LoadMeshFromFile(const wchar_t* filePath);

        std::vector<MeshVertex> m_vertices;
        std::vector<unsigned int> m_indices;
    };
}