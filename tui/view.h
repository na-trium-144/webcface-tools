#pragma once
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <webcface/view.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

class ViewUIContainer : public ftxui::ComponentBase {
    webcface::View view;
    std::unordered_map<std::string, ftxui::Component> ui_components;
    std::unordered_map<std::string, webcface::ViewComponent> prev_components;
    int focused_row;
    std::vector<bool> has_no_component;
    std::shared_ptr<std::string> help;
    std::shared_ptr<ftxui::Element> result;
    bool light;

    // childrenにレイアウトを反映 (renderはしない)
    // childrenは2次元vectorのような感じになる
    // 要素が足りなければAddし、多ければDetachする
    void updateLayout();

    ftxui::Component buttonComponent(const webcface::ViewComponent &cp) const;
    ftxui::Component inputComponent(const webcface::ViewComponent &cp) const;
    ftxui::Component dropdownComponent(const webcface::ViewComponent &cp) const;
    ftxui::Component toggleComponent(const webcface::ViewComponent &cp) const;
    ftxui::Component sliderComponent(const webcface::ViewComponent &cp) const;
    ftxui::Component checkComponent(const webcface::ViewComponent &cp) const;

  public:
    explicit ViewUIContainer(const webcface::View &view,
                             std::shared_ptr<std::string> help,
                             std::shared_ptr<ftxui::Element> result,
                             bool light);

    ~ViewUIContainer() override {}
    ViewUIContainer(const ViewUIContainer &) = delete;
    ViewUIContainer(ViewUIContainer &&) = delete;
    ViewUIContainer &operator=(const ViewUIContainer &) = delete;
    ViewUIContainer &operator=(ViewUIContainer &&) = delete;

    ftxui::Element RenderCol(const std::vector<ftxui::Element> &elements_col,
                             std::size_t current_row) const;
    // renderする (childrenのレイアウトは無視)
    ftxui::Element OnRender() override;
};

ftxui::Component viewComponent(const webcface::View &view,
                               std::shared_ptr<std::string> help,
                               std::shared_ptr<ftxui::Element> result,
                               bool light);
void addViewComponent(ftxui::ScreenInteractive &screen,
                      ftxui::Component &container, const webcface::View &view,
                      std::shared_ptr<std::string> help,
                      std::shared_ptr<ftxui::Element> result, bool light);
