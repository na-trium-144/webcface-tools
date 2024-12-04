#include "main.h"
#include "webcface/components.h"
#include "webcface/geometry.h"
#include <spdlog/spdlog.h>
#include <string>

std::optional<webcface::RobotLink>
parseGeom(const pugi::xml_node &geom, std::string_view parent_joint_link_name,
          const webcface::Transform &parent_joint_tf) {
    // ignore: con*, priority, friction, mass, density, shellinertia,
    // sol*,
    //  margin, gap, fluid*, user
    // todo: class, group, material, fromto, mesh, fitscale
    std::string name = geom.attribute("name").as_string();
    auto size = parseReal(geom.attribute("size").as_string("0 0 0"));
    auto rgba = parseReal(geom.attribute("rgba").as_string("0.5 0.5 0.5 1"));
    auto tf = parseTransform(geom);

    webcface::Geometry w_geom;
    auto type = geom.attribute("type").as_string("sphere");
    if (type == "plane"sv) {
        if (size.at(0) > 0 && size.at(1) > 0) {
            w_geom = webcface::plane(tf, size.at(0), size.at(1));
        } else {
            w_geom = webcface::plane(tf, 10, 10);
        }
    } else if (type == "sphere"sv) {
        w_geom = webcface::sphere(tf.pos(), size.at(0));
    } else if (type == "capsule"sv) {
        spdlog::warn(
            "Geom type {} is not implemented, cylinder is used instead.", type);
        // mujocoではz方向に押し出すが、webcfaceではx方向
        w_geom = webcface::cylinder(tf * webcface::rotY(-M_PI / 2), size.at(0),
                                    size.at(1) * 2);
    } else if (type == "cylinder"sv) {
        // mujocoではz方向に押し出すが、webcfaceではx方向
        w_geom = webcface::cylinder(tf * webcface::rotY(-M_PI / 2), size.at(0),
                                    size.at(1) * 2);
    } else if (type == "box"sv) {
        w_geom = webcface::box(webcface::Point(tf) - webcface::Point(size),
                               webcface::Point(tf) + webcface::Point(size));
    } else {
        // todo: hfield, ellipsoid, mesh, sdf
        spdlog::error("Geom type {} is not implemented", type);
        return std::nullopt;
    }
    return webcface::RobotLink(
        name, webcface::fixedJoint(parent_joint_link_name, parent_joint_tf),
        w_geom, webcface::colorFromRGB(rgba.at(0), rgba.at(1), rgba.at(2)));
}
