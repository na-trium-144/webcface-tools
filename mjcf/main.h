#pragma once
#include "webcface/transform.h"
#include "webcface/robot_link.h"
#include <pugixml.hpp>
#include <vector>
#include <optional>
#include <string_view>

void parseBody(const pugi::xml_node &body);
std::optional<webcface::RobotLink> parseGeom(std::string_view body_name,
                                             const pugi::xml_node &geom);
std::vector<double> parseReal(std::string_view str);
webcface::Transform parseTransform(const pugi::xml_node &node);

using namespace std::string_view_literals;
