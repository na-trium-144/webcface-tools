#pragma once
#include "color.h"
#include "func.h"

class ViewUIContainer : public ftxui::ComponentBase {
    webcface::View view;
    std::unordered_map<std::string, ftxui::Component> ui_components;
    std::unordered_map<std::string, webcface::ViewComponent> prev_components;
    int focused_row;
    std::vector<bool> has_no_component;
    std::shared_ptr<std::string> help;
    std::shared_ptr<ftxui::Element> result;

    // childrenにレイアウトを反映 (renderはしない)
    // childrenは2次元vectorのような感じになる
    // 要素が足りなければAddし、多ければDetachする
    void updateLayout() {
        auto &root = this->ChildAt(0);
        // childrenを空にする (行は削除しない)
        for (std::size_t row = 0; row < root->ChildCount(); row++) {
            root->ChildAt(row)->DetachAllChildren();
            has_no_component[row] = false;
        }
        if (root->ChildCount() == 0) {
            root->Add(ftxui::Container::Horizontal({}));
            has_no_component.push_back(false);
        }
        std::size_t row = 0;
        for (const auto &cp : this->view.get()) {
            switch (cp.type()) {
            case webcface::ViewComponentType::text:
                break;
            case webcface::ViewComponentType::new_line:
                // 空の行もfocusを取れるようにする(仮)
                if (root->ChildAt(row)->ChildCount() == 0) {
                    has_no_component[row] = true;
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
                    has_no_component.push_back(false);
                }
                break;
            case webcface::ViewComponentType::button: {
                ftxui::Component ui_cp = this->ui_components[cp.id()];
                if (!ui_cp && cp != this->prev_components[cp.id()]) {
                    this->prev_components[cp.id()] = cp;
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
                            return ftxui::text("(" + s.label + ")") |
                                   ftxui::bold;
                        } else {
                            return ftxui::text("(" + s.label + ")");
                        }
                    };
                    this->ui_components[cp.id()] = ui_cp = ftxui::Button(
                        cp.text(),
                        [func = cp.onClick(), result = this->result] {
                            if (func) {
                                runAsync(*func, result);
                            }
                        },
                        option);
                }
                root->ChildAt(row)->Add(ui_cp);
                break;
            }
            case webcface::ViewComponentType::text_input:
            case webcface::ViewComponentType::decimal_input: {
                ftxui::Component ui_cp = this->ui_components[cp.id()];
                if (!ui_cp && cp != this->prev_components[cp.id()]) {
                    this->prev_components[cp.id()] = cp;
                    auto bind_ref = std::make_shared<std::string>();
                    if (cp.bind()) {
                        *bind_ref = cp.bind()->get();
                        cp.bind()->appendListener(
                            [bind_ref](const auto &b) { *bind_ref = b; });
                    }
                    auto content_ref = std::make_shared<std::string>();
                    auto is_error = std::make_shared<bool>(false);
                    ftxui::InputOption option{};
                    option.content = ftxui::StringRef(content_ref.get());
                    option.multiline = false;
                    option.on_change = [cp, is_error, content_ref] {
                        *is_error = true;
                        switch (cp.type()) {
                        case webcface::ViewComponentType::text_input:
                            if ((cp.min() && content_ref->size() < cp.min()) ||
                                (cp.max() && content_ref->size() > cp.max())) {
                                return;
                            }
                            break;
                        case webcface::ViewComponentType::decimal_input: {
                            double val = 0;
                            for (char c : *content_ref) {
                                if ((c < '0' || c > '9') && c != '.') {
                                    return;
                                }
                            }
                            try {
                                val = std::stod(*content_ref);
                            } catch (const std::exception &e) {
                                return;
                            }
                            if ((cp.min() && val < cp.min()) ||
                                (cp.max() && val > cp.max())) {
                                return;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        *is_error = false;
                    };
                    if (cp.onChange()) {
                        option.on_enter = [content_ref, func = *cp.onChange(),
                                           is_error, result = this->result] {
                            if (!*is_error) {
                                runAsync(func, result, *content_ref);
                            }
                        };
                    }
                    option.transform = [content_ref, bind_ref,
                                        is_error](ftxui::InputState state) {
                        state.element = ftxui::hbox({
                            ftxui::text("["),
                            state.element |
                                (state.focused ? ftxui::bold : ftxui::nothing) |
                                ftxui::xflex,
                            ftxui::text("]"),
                        });
                        state.element |=
                            ftxui::color(*is_error ? ftxui::Color::Red
                                                   : ftxui::Color::White);
                        if (state.focused) {
                            state.element |=
                                ftxui::bgcolor(ftxui::Color::Black);
                        } else {
                            *content_ref = *bind_ref;
                        }
                        if (state.hovered) {
                            state.element |=
                                ftxui::bgcolor(ftxui::Color::GrayDark);
                        }
                        return state.element;
                    };
                    auto ui_input = ftxui::Input(option);
                    this->ui_components[cp.id()] = ui_cp =
                        ftxui::Renderer(ui_input, [ui_input] {
                            return ui_input->Render() |
                                   ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10);
                        });
                }
                root->ChildAt(row)->Add(ui_cp);
                break;
            }
            }
        }
        if (root->ChildAt(row)->ChildCount() == 0) {
            has_no_component[row] = true;
            root->ChildAt(row)->Add(ftxui::Renderer(
                [](bool /*focused*/) { return ftxui::emptyElement(); }));
        }
    }

  public:
    explicit ViewUIContainer(const webcface::View &view,
                             std::shared_ptr<std::string> help,
                             std::shared_ptr<ftxui::Element> result)
        : ftxui::ComponentBase(), view(view), ui_components(),
          prev_components(), focused_row(0), has_no_component(), help(help),
          result(result) {
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
        if (this->Focused() && focused_row == current_row &&
            has_no_component[current_row]) {
            return ftxui::flexbox(elements_col) | ftxui::bold | ftxui::focus;
            // ftxui::bgcolor(ftxui::Color::GrayDark)
        } else {
            return ftxui::flexbox(elements_col);
        }
    }
    // renderする (childrenのレイアウトは無視)
    ftxui::Element Render() override {
        if (this->Focused()) {
            *help = defaultStatus();
        }
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
            case webcface::ViewComponentType::text_input:
            case webcface::ViewComponentType::decimal_input: {
                ftxui::Component ui_cp = this->ui_components[cp.id()];
                if (ui_cp) {
                    elements_col.push_back(ui_cp->Render());
                    if (ui_cp->Focused()) {
                        std::stringstream ss;
                        switch (cp.type()) {
                        case webcface::ViewComponentType::button:
                            ss << "Enter = click button";
                            break;
                        case webcface::ViewComponentType::text_input:
                            if (cp.min()) {
                                ss << "min. " << *cp.min() << " chars, ";
                            }
                            if (cp.max()) {
                                ss << "max. " << *cp.max() << " chars, ";
                            }
                            ss << "Press Enter to send";
                            break;
                        case webcface::ViewComponentType::decimal_input:
                            if (cp.min()) {
                                ss << "min. " << *cp.min() << ", ";
                            }
                            if (cp.max()) {
                                ss << "max. " << *cp.max() << ", ";
                            }
                            ss << "Press Enter to send";
                            break;
                        }
                        *help = ss.str();
                    }
                }
                break;
            }
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
inline ftxui::Component viewComponent(const webcface::View &view,
                                      std::shared_ptr<std::string> help,
                                      std::shared_ptr<ftxui::Element> result) {
    return std::make_shared<ViewUIContainer>(view, help, result);
}
inline void addViewComponent(ftxui::ScreenInteractive &screen,
                             ftxui::Component &container,
                             const webcface::View &view,
                             std::shared_ptr<std::string> help,
                             std::shared_ptr<ftxui::Element> result) {
    auto handle = std::make_shared<webcface::View::EventHandle>();
    *handle =
        view.prependListener([&screen, &container, view, handle, help, result] {
            screen.Post([&container, view, help, result] {
                container->Add(viewComponent(view, help, result));
            });
            view.removeListener(*handle);
        });
}
