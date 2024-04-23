#pragma once
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <webcface/text.h>
#include <string>
#include <memory>

ftxui::Component textComponent(const webcface::Text &text,
                               std::shared_ptr<std::string> help,
                               std::shared_ptr<ftxui::Element> result,
                               bool light);
void addTextComponent(ftxui::ScreenInteractive &screen,
                      ftxui::Component &container, const webcface::Text &text,
                      std::shared_ptr<std::string> help,
                      std::shared_ptr<ftxui::Element> result, bool light);