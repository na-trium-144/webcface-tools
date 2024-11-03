#pragma once
#include "webcface/transform.h"
#include "webcface/robot_link.h"
#include <pugixml.hpp>
#include <vector>
#include <optional>
#include <string_view>

void parseBody(std::vector<webcface::RobotLink> &w_links,
               const pugi::xml_node &body, bool is_world,
               std::string_view parent_joint_link_name,
               const webcface::Transform &parent_joint_tf);
std::optional<webcface::RobotLink>
parseGeom(const pugi::xml_node &geom, std::string_view parent_joint_link_name,
          const webcface::Transform &parent_joint_tf);

std::vector<double> parseReal(std::string_view str);
webcface::Transform parseTransform(const pugi::xml_node &node);

using namespace std::string_view_literals;
using namespace std::string_literals;
