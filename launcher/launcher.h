#pragma once
#include <webcface/webcface.h>
#include <toml++/toml.hpp>

void launcher(WebCFace::Client &wcli, toml::parse_result &config);
