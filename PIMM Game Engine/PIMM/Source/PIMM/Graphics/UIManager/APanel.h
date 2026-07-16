#pragma once
#include <string>

namespace pimm {
    class APanel {
    public:
        virtual ~APanel() = default;
        virtual void Render() = 0;

        const std::string& GetName() const noexcept { return m_name; }

    protected:
        explicit APanel(std::string name) : m_name(std::move(name)) {}

    private:
        std::string m_name;
    };
}