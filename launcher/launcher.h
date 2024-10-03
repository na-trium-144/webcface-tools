#pragma once
#include <webcface/client.h>
#include "command.h"
#include <toml++/toml.hpp>
#include <memory>
#include <vector>

std::shared_ptr<Process> parseTomlProcess(toml::node &config_node,
                                          const std::string &default_name);
std::vector<std::shared_ptr<Command>> parseToml(webcface::Client &wcli,
                                                toml::parse_result &config);
void launcherLoop(webcface::Client &wcli,
                  const std::vector<std::shared_ptr<Command>> &commands);
std::string tomlSourceInfo(const toml::source_region &src);
