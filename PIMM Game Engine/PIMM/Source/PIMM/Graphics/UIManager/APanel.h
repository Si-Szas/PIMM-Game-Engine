#pragma once

namespace pimm {
    class APanel {
    public:
        virtual ~APanel() = default;
        virtual void Render() = 0;
    };
}
