#pragma once

inline ftxui::Component valueComponent(const webcface::Value &value) {
    return ftxui::Renderer([value] {
        return ftxui::hbox({
                   ftxui::text(value.member().name()),
                   ftxui::text(":"),
                   ftxui::text(value.name()),
                   ftxui::text(" = "),
                   ftxui::text(std::to_string(value.get())) | ftxui::xflex,
               }) |
               ftxui::xflex;
    });
}
inline void addValueComponent(ftxui::ScreenInteractive &screen,
                              ftxui::Component &container,
                              const webcface::Value &value) {
    if (value.tryGet()) {
        container->Add(valueComponent(value));
    } else {
        auto handle = std::make_shared<webcface::Value::EventHandle>();
        *handle = value.prependListener([&screen, &container, value, handle] {
            screen.Post(
                [&container, value] { container->Add(valueComponent(value)); });
            value.removeListener(*handle);
        });
    }
}

inline ftxui::Component textComponent(const webcface::Text &text) {
    return ftxui::Renderer([text] {
        return ftxui::hbox({
                   ftxui::text(text.member().name()),
                   ftxui::text(":"),
                   ftxui::text(text.name()),
                   ftxui::text(" = "),
                   ftxui::text(text.get()) | ftxui::xflex,
               }) |
               ftxui::xflex;
    });
}
inline void addTextComponent(ftxui::ScreenInteractive &screen,
                             ftxui::Component &container,
                             const webcface::Text &text) {
    if (text.tryGet()) {
        container->Add(textComponent(text));
    } else {
        auto handle = std::make_shared<webcface::Text::EventHandle>();
        *handle = text.prependListener([&screen, &container, text, handle] {
            screen.Post(
                [&container, text] { container->Add(textComponent(text)); });
            text.removeListener(*handle);
        });
    }
}

class ViewUIContainer : public ftxui::ComponentBase {
    webcface::View view;
    std::unordered_map<webcface::ViewComponent::Id, ftxui::Component>
        ui_components;

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
                if (root->ChildAt(row)->ChildCount() > 0) {
                    row++;
                }
                if (root->ChildCount() <= row) {
                    root->Add(ftxui::Container::Horizontal({}));
                }
                break;
            case webcface::ViewComponentType::button:
                if (!this->ui_components.count(
                        cp.id()) /* && cp has changed */) {
                    this->ui_components[cp.id()] = ftxui::Button({
                        .text = cp.text(),
                        // todo: color
                    });
                }
                ftxui::Component ui_cp = this->ui_components[cp.id()];
                root->ChildAt(row)->Add(ui_cp);
            }
        }
    }

  public:
    explicit ViewUIContainer(const webcface::View &view)
        : ftxui::ComponentBase(), view(view), row_selector(0), col_selectors() {
        this->Add(ftxui::Container::Vertical({}));
        view.prependListener([this] { this->updateLayout(); });
    }
    ~ViewUIContainer() override {}
    ViewUIContainer(const ViewUIContainer &) = delete;
    ViewUIContainer(ViewUIContainer &&) = delete;
    ViewUIContainer &operator=(const ViewUIContainer &) = delete;
    ViewUIContainer &operator=(ViewUIContainer &&) = delete;

    // renderする (childrenのレイアウトは無視)
    ftxui::Element Render() override {
        std::vector<ftxui::Element> elements, elements_col;
        for (const auto &cp : this->view.get()) {
            switch (cp.type()) {
            case webcface::ViewComponentType::text:
                elements_col.push_back(ftxui::text(cp.text()));
                break;
            case webcface::ViewComponentType::new_line:
                elements.push_back(ftxui::hbox(elements_col));
                elements_col.clear();
                break;
            case webcface::ViewComponentType::button:
                ftxui::Component ui_cp = this->ui_components[cp.id()];
                if (ui_cp) {
                    elements.push_back(ui_cp->Render());
                }
                break;
            }
        }
        return ftxui::vbox(elements);
    }
};
inline ftxui::Component viewComponent(const webcface::View &view) {
    return std::make_shared<ViewUIContainer>(view);
}
inline void addViewComponent(ftxui::ScreenInteractive &screen,
                             ftxui::Component &container,
                             const webcface::View &view) {
    if (view.tryGet()) {
        container->Add(viewComponent(view));
    } else {
        auto handle = std::make_shared<webcface::View::EventHandle>();
        *handle = view.prependListener([&screen, &container, view, handle] {
            screen.Post(
                [&container, view] { container->Add(viewComponent(view)); });
            view.removeListener(*handle);
        });
    }
}
