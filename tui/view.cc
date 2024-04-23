#include "view.h"
#include "color.h"
#include "func.h"
#include "status.h"
#include <cctype>
#include <webcface/member.h>
#include <sstream>

ViewUIContainer::ViewUIContainer(const webcface::View &view,
                                 std::shared_ptr<std::string> help,
                                 std::shared_ptr<ftxui::Element> result,
                                 bool light)
    : ftxui::ComponentBase(), view(view), ui_components(), prev_components(),
      focused_row(0), has_no_component(), help(help), result(result),
      light(light) {
    this->Add(ftxui::Container::Vertical({}, &focused_row));
    view.prependListener([this] { this->updateLayout(); });
    this->updateLayout();
}
ftxui::Component viewComponent(const webcface::View &view,
                               std::shared_ptr<std::string> help,
                               std::shared_ptr<ftxui::Element> result,
                               bool light) {
    return std::make_shared<ViewUIContainer>(view, help, result, light);
}
void addViewComponent(ftxui::ScreenInteractive &screen,
                      ftxui::Component &container, const webcface::View &view,
                      std::shared_ptr<std::string> help,
                      std::shared_ptr<ftxui::Element> result, bool light) {
    auto handle = std::make_shared<webcface::View::EventHandle>();
    *handle = view.prependListener([=, &screen, &container] {
        screen.Post([=, &container] {
            container->Add(viewComponent(view, help, result, light));
        });
        view.removeListener(*handle);
    });
}

void ViewUIContainer::updateLayout() {
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
                root->ChildAt(row)->Add(ftxui::Renderer(
                    [](bool /*focused*/) { return ftxui::emptyElement(); }));
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
                this->ui_components[cp.id()] = ui_cp = buttonComponent(cp);
            }
            root->ChildAt(row)->Add(ui_cp);
            break;
        }
        case webcface::ViewComponentType::text_input:
        case webcface::ViewComponentType::decimal_input:
        case webcface::ViewComponentType::number_input: {
            ftxui::Component ui_cp = this->ui_components[cp.id()];
            if (!ui_cp && cp != this->prev_components[cp.id()]) {
                this->prev_components[cp.id()] = cp;
                this->ui_components[cp.id()] = ui_cp = inputComponent(cp);
            }
            root->ChildAt(row)->Add(ui_cp);
            break;
        }
        case webcface::ViewComponentType::select_input: {
            ftxui::Component ui_cp = this->ui_components[cp.id()];
            if (!ui_cp && cp != this->prev_components[cp.id()]) {
                this->prev_components[cp.id()] = cp;
                this->ui_components[cp.id()] = ui_cp = dropdownComponent(cp);
            }
            root->ChildAt(row)->Add(ui_cp);
            break;
        }
        case webcface::ViewComponentType::toggle_input: {
            ftxui::Component ui_cp = this->ui_components[cp.id()];
            if (!ui_cp && cp != this->prev_components[cp.id()]) {
                this->prev_components[cp.id()] = cp;
                this->ui_components[cp.id()] = ui_cp = toggleComponent(cp);
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


ftxui::Component
ViewUIContainer::buttonComponent(const webcface::ViewComponent &cp) const {
    auto option = ftxui::ButtonOption::Animated(
        convertColor(cp.bgColor(), webcface::ViewColor::green, light),
        convertColor(cp.textColor(), webcface::ViewColor::black, light),
        convertColor(cp.bgColor(), webcface::ViewColor::green, light, true),
        convertColor(cp.textColor(), webcface::ViewColor::black, light));
    option.transform = [](const ftxui::EntryState &s) {
        if (s.focused) {
            return ftxui::text("(" + s.label + ")") | ftxui::bold;
        } else {
            return ftxui::text("(" + s.label + ")");
        }
    };
    return ftxui::Button(
        cp.text(),
        [func = cp.onClick(), result = this->result] {
            if (func) {
                runAsync(*func, result);
            }
        },
        option);
}

ftxui::Component
ViewUIContainer::inputComponent(const webcface::ViewComponent &cp) const {
    auto bind_ref = std::make_shared<std::string>();
    if (cp.bind()) {
        *bind_ref = cp.bind()->get();
        cp.bind()->appendListener([bind_ref](const auto &b) { *bind_ref = b; });
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
                if (!std::isdigit(c) && c != '.') {
                    return;
                }
            }
            try {
                val = std::stod(*content_ref);
            } catch (const std::exception &e) {
                return;
            }
            if ((cp.min() && val < cp.min()) || (cp.max() && val > cp.max())) {
                return;
            }
            break;
        }
        case webcface::ViewComponentType::number_input: {
            int val = 0;
            for (char c : *content_ref) {
                if (!std::isdigit(c)) {
                    return;
                }
            }
            try {
                val = std::stoi(*content_ref);
            } catch (const std::exception &e) {
                return;
            }
            if ((cp.min() && val < cp.min()) || (cp.max() && val > cp.max())) {
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
        option.on_enter = [content_ref, func = *cp.onChange(), is_error,
                           result = this->result] {
            if (!*is_error) {
                runAsync(func, result, *content_ref);
            }
        };
    }
    option.transform = [content_ref, bind_ref, is_error,
                        light = this->light](ftxui::InputState state) {
        state.element = ftxui::hbox({
            ftxui::text("["),
            state.element | (state.focused ? ftxui::bold : ftxui::nothing) |
                ftxui::xflex,
            ftxui::text("]"),
        });
        state.element |= ftxui::color(
            *is_error ? convertColor(webcface::ViewColor::red, light)
                      : convertColor(webcface::ViewColor::black, light));
        if (state.focused || state.hovered) {
            state.element |= ftxui::bgcolor(
                convertColor(webcface::ViewColor::white, light, state.hovered));
        }
        if (!state.focused) {
            *content_ref = *bind_ref;
        }
        return state.element;
    };
    auto ui_input = ftxui::Input(option);
    return ftxui::Renderer(ui_input, [ui_input] {
        return ui_input->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10);
    });
}

ftxui::Component
ViewUIContainer::dropdownComponent(const webcface::ViewComponent &cp) const {
    auto bind_ref = std::make_shared<std::string>();
    if (cp.bind()) {
        *bind_ref = cp.bind()->get();
        cp.bind()->appendListener([bind_ref](const auto &b) { *bind_ref = b; });
    }
    ftxui::DropdownOption option{};
    auto options = std::make_shared<std::vector<std::string>>();
    for (const auto &o : cp.option()) {
        options->push_back(o.asStringRef());
    }
    option.radiobox.entries = options.get();
    auto selected = std::make_shared<int>();
    option.radiobox.selected = selected.get();
    if (cp.onChange()) {
        option.radiobox.on_change = [cp, func = *cp.onChange(),
                                     options_v = cp.option(), bind_ref,
                                     selected, result = this->result] {
            auto selected_v = options_v.at(*selected);
            if (*bind_ref != selected_v) {
                runAsync(func, result, selected_v);
            }
        };
    }
    option.checkbox.transform = [bind_ref, options_v = cp.option(), options,
                                 selected](const ftxui::EntryState &state) {
        auto t = ftxui::text(state.label);
        if (!state.focused) {
            for (std::size_t i = 0; i < options_v.size(); i++) {
                if (options_v[i] == *bind_ref) {
                    *selected = i;
                }
            }
        }
        if (state.focused) {
            t |= ftxui::bold;
        }
        return ftxui::hbox(
            {ftxui::text(state.state ? "[↓ " : "[→ "), t, ftxui::text("]")});
    };
    option.radiobox.transform = [](const ftxui::EntryState &s) {
        auto prefix = ftxui::text(s.state ? "* " : "  ");
        auto t = ftxui::text(s.label);
        if (s.focused) {
            t |= ftxui::bold;
        }
        return ftxui::hbox({prefix, t});
    };
    option.transform = [options, light = this->light](bool open,
                                                      ftxui::Element checkbox,
                                                      ftxui::Element radiobox) {
        if (open) {
            return ftxui::vbox({
                checkbox,
                radiobox | ftxui::vscroll_indicator | ftxui::yframe |
                    ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 5) |
                    ftxui::bgcolor(
                        convertColor(webcface::ViewColor::white, light, true)),
                ftxui::filler(),
            });
        }
        return ftxui::vbox({checkbox, ftxui::filler()});
    };
    return ftxui::Dropdown(option);
}

ftxui::Component
ViewUIContainer::toggleComponent(const webcface::ViewComponent &cp) const {
    auto option = ftxui::ButtonOption::Animated(
        convertColor(webcface::ViewColor::white, light),
        convertColor(webcface::ViewColor::black, light),
        convertColor(webcface::ViewColor::white, light, true),
        convertColor(webcface::ViewColor::black, light));
    auto bind_ref = std::make_shared<std::string>();
    if (cp.bind()) {
        *bind_ref = cp.bind()->get();
        cp.bind()->appendListener([bind_ref](const auto &b) { *bind_ref = b; });
    }
    option.transform = [bind_ref](const ftxui::EntryState &s) {
        if (s.focused) {
            return ftxui::text("[" + *bind_ref + "]") | ftxui::bold;
        } else {
            return ftxui::text("[" + *bind_ref + "]");
        }
    };
    return ftxui::Button(
        cp.text(),
        [func = cp.onChange(), option = cp.option(), bind_ref,
         result = this->result] {
            int current = -1;
            for (std::size_t i = 0; i < option.size(); i++) {
                if (*bind_ref == option[i]) {
                    current = i;
                    break;
                }
            }
            if (func) {
                runAsync(*func, result, option[(current + 1) % option.size()]);
            }
        },
        option);
}

ftxui::Element
ViewUIContainer::RenderCol(const std::vector<ftxui::Element> &elements_col,
                           std::size_t current_row) const {
    if (this->Focused() && focused_row == current_row &&
        has_no_component[current_row]) {
        return ftxui::flexbox(elements_col) | ftxui::bold | ftxui::focus;
        // ftxui::bgcolor(ftxui::Color::GrayDark)
    } else {
        return ftxui::flexbox(elements_col);
    }
}

ftxui::Element ViewUIContainer::Render() {
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
        case webcface::ViewComponentType::decimal_input:
        case webcface::ViewComponentType::number_input:
        case webcface::ViewComponentType::select_input:
        case webcface::ViewComponentType::toggle_input: {
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
                    case webcface::ViewComponentType::number_input:
                        if (cp.min()) {
                            ss << "min. " << *cp.min() << ", ";
                        }
                        if (cp.max()) {
                            ss << "max. " << *cp.max() << ", ";
                        }
                        ss << "Press Enter to send";
                        break;
                    case webcface::ViewComponentType::select_input:
                        ss << "Press Enter or Space to select";
                        break;
                    case webcface::ViewComponentType::toggle_input:
                        ss << "Press Enter to toggle value";
                        break;
                    default:
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
