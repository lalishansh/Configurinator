import Configurinator.Model;

import <ftxui/component/component.hpp>;
import <memory>;

#include <include.h>

export module Configurinator.Frontend:CustomComponents;

BEGIN_NAMESPACE

struct ResizeableStackedContainerOption {
    struct ComponentWithTitle {
        std::string_view Title;
        ftxui::Component Component;
    };
    std::vector<ComponentWithTitle> ComponentsWithTitles;
    b8 Horizontal = false;
    c8 SeperatorChar = '-';
};

export ftxui::Component ResizeableStackedContainer(ResizeableStackedContainerOption options);

namespace ui = ftxui;
class ResizeableStackedContainerBase : public ftxui::ComponentBase {
public:
    explicit ResizeableStackedContainerBase(ResizeableStackedContainerOption options)
        : m_Options(std::move(options))
    {
        for(auto& [title, component] : m_Options.ComponentsWithTitles) {
            Add(component);
        }
    }

    virtual ui::Element Render() final;
    virtual bool OnEvent(ui::Event event) final;
    virtual bool Event(ui::Event event) final;
private:
    ui::Ref<ResizeableStackedContainerOption> m_Options;
};

ftxui::Component ResizeableStackedContainer(ResizeableStackedContainerOption options) {
    return ftxui::Make<ResizeableStackedContainerBase>(std::move(options));
}

END_NAMESPACE
