#pragma once
#include "color.h"

inline ftxui::Component valueComponent(const webcface::Value &value) {
    return ftxui::Renderer([value](bool focused) {
        return ftxui::hbox({
                   ftxui::text(value.member().name()),
                   ftxui::text(":"),
                   ftxui::text(value.name()),
                   ftxui::text(" = "),
                   ftxui::text(std::to_string(value.get())) | ftxui::xflex,
               }) |
               ftxui::xflex | (focused ? ftxui::focus : ftxui::nothing);
    });
}
inline void addValueComponent(ftxui::ScreenInteractive &screen,
                              ftxui::Component &container,
                              const webcface::Value &value) {
    auto handle = std::make_shared<webcface::Value::EventHandle>();
    *handle = value.prependListener([&screen, &container, value, handle] {
        screen.Post(
            [&container, value] { container->Add(valueComponent(value)); });
        value.removeListener(*handle);
    });
}

inline ftxui::Component textComponent(const webcface::Text &text) {
    return ftxui::Renderer([text](bool focused) {
        return ftxui::hbox({
                   ftxui::text(text.member().name()),
                   ftxui::text(":"),
                   ftxui::text(text.name()),
                   ftxui::text(" = "),
                   ftxui::text(text.get()) | ftxui::xflex,
               }) |
               ftxui::xflex | (focused ? ftxui::focus : ftxui::nothing);
    });
}
inline void addTextComponent(ftxui::ScreenInteractive &screen,
                             ftxui::Component &container,
                             const webcface::Text &text) {
    auto handle = std::make_shared<webcface::Text::EventHandle>();
    *handle = text.prependListener([&screen, &container, text, handle] {
        screen.Post(
            [&container, text] { container->Add(textComponent(text)); });
        text.removeListener(*handle);
    });
}

class ViewUIContainer : public ftxui::ComponentBase {
    webcface::View view;
    std::unordered_map<std::string, ftxui::Component> ui_components;
    int focused_row;


    // childrenにレイアウトを反映 (renderはしない)
    // childrenは2次元vectorのような感じになる
    // 要素が足りなければAddし、多ければDetachする
    void updateLayout() {
        auto &root = this->ChildAt(0);
        // childrenを空にする (行は削除しない)
        for (std::size_t row = 0; row < root->ChildCount(); row++) {
            root->ChildAt(row)->DetachAllChildren();
        }
        if (root->ChildCount() == 0) {
            root->Add(ftxui::Container::Horizontal({}));
        }
        std::size_t row = 0;
        for (const auto &cp : this->view.get()) {
            switch (cp.type()) {
            case webcface::ViewComponentType::text:
                break;
            case webcface::ViewComponentType::new_line:
                // 空の行もfocusを取れるようにする(仮)
                if (root->ChildAt(row)->ChildCount() == 0) {
                    root->ChildAt(row)->Add(
                        ftxui::Renderer([](bool /*focused*/) {
                            return ftxui::emptyElement();
                        }));
                }
                if (root->ChildAt(row)->ChildCount() > 0) {
                    row++;
                }
                if (root->ChildCount() <= row) {
                    root->Add(ftxui::Container::Horizontal({}));
                }
                break;
            case webcface::ViewComponentType::button: {
                ftxui::Component ui_cp = this->ui_components[cp.id()];
                if (!ui_cp /* && cp has changed */) {
                    auto option = ftxui::ButtonOption::Animated(
                        convertColor(cp.bgColor(), webcface::ViewColor::green),
                        convertColor(cp.textColor(),
                                     webcface::ViewColor::black),
                        convertColor(cp.bgColor(), webcface::ViewColor::green,
                                     true),
                        convertColor(cp.textColor(),
                                     webcface::ViewColor::black));
                    option.transform = [](const ftxui::EntryState &s) {
                        if (s.focused) {
                            return ftxui::text("[" + s.label + "]") |
                                   ftxui::bold;
                        } else {
                            return ftxui::text("(" + s.label + ")");
                        }
                    };
                    this->ui_components[cp.id()] = ui_cp = ftxui::Button(
                        cp.text(),
                        [func = cp.onClick()] {
                            if (func) {
                                func->runAsync();
                            }
                        },
                        option);
                }
                root->ChildAt(row)->Add(ui_cp);
                break;
            }
            }
        }
    }

  public:
    explicit ViewUIContainer(const webcface::View &view)
        : ftxui::ComponentBase(), view(view), focused_row(0) {
        this->Add(ftxui::Container::Vertical({}, &focused_row));
        view.prependListener([this] { this->updateLayout(); });
        this->updateLayout();
    }
    ~ViewUIContainer() override {}
    ViewUIContainer(const ViewUIContainer &) = delete;
    ViewUIContainer(ViewUIContainer &&) = delete;
    ViewUIContainer &operator=(const ViewUIContainer &) = delete;
    ViewUIContainer &operator=(ViewUIContainer &&) = delete;

    ftxui::Element RenderCol(const std::vector<ftxui::Element> &elements_col,
                             std::size_t current_row) const {
        return ftxui::flexbox(elements_col) |
               (this->Focused() && focused_row == current_row
                    ? ftxui::bgcolor(ftxui::Color::GrayDark) | ftxui::focus
                    : ftxui::nothing);
    }
    // renderする (childrenのレイアウトは無視)
    ftxui::Element Render() override {
        std::vector<ftxui::Element> elements, elements_col;
        for (const auto &cp : this->view.get()) {
            switch (cp.type()) {
            case webcface::ViewComponentType::text:
                elements_col.push_back(ftxui::text(cp.text()));
                break;
            case webcface::ViewComponentType::new_line:
                elements.push_back(RenderCol(elements_col, elements.size()));
                elements_col.clear();
                break;

            case webcface::ViewComponentType::button:
                ftxui::Component ui_cp = this->ui_components[cp.id()];
                if (ui_cp) {
                    elements_col.push_back(ui_cp->Render());
                }
                break;
            }
        }
        elements.push_back(RenderCol(elements_col, elements.size()));
        return ftxui::window(ftxui::hbox({
                                 ftxui::text(view.member().name()),
                                 ftxui::text(":"),
                                 ftxui::text(view.name()),
                             }),
                             ftxui::vbox(elements));
    }
};
inline ftxui::Component viewComponent(const webcface::View &view) {
    return std::make_shared<ViewUIContainer>(view);
}
inline void addViewComponent(ftxui::ScreenInteractive &screen,
                             ftxui::Component &container,
                             const webcface::View &view) {
    auto handle = std::make_shared<webcface::View::EventHandle>();
    *handle = view.prependListener([&screen, &container, view, handle] {
        screen.Post(
            [&container, view] { container->Add(viewComponent(view)); });
        view.removeListener(*handle);
    });
}
