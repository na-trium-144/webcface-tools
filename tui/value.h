#pragma once
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <webcface/value.h>
#include <string>
#include <memory>

ftxui::Component valueComponent(const webcface::Value &value,
                                std::shared_ptr<std::string> help,
                                std::shared_ptr<ftxui::Element> result,
                                bool light);
void addValueComponent(ftxui::ScreenInteractive &screen,
                       ftxui::Component &container,
                       const webcface::Value &value,
                       std::shared_ptr<std::string> help,
                       std::shared_ptr<ftxui::Element> result, bool light);
