#include "main.h"
#include <cmath>
#include <spdlog/spdlog.h>

// clang-format off
/*
    mujocoでは、
        body座標系 = body.pos + body.quat
        joint座標系 = body.pos + body.quat + joint.pos (+joint.axis)
        geom座標系 = body.pos + body.quat + geom.pos + geom.quat
        child body座標系 = body.pos + body.quat + (child.body...)

    webcfaceでは、
        link(joint)座標系 = joint.origin
        geom座標系 = joint.origin + geom.origin
        child link(joint)座標系 = joint.origin + (child.joint...)

    よって、
    - body_link: body.pos + body.quat
        - joint_link: joint.pos + joint.axis
        - joint_link2: - joint.axis - joint.pos + joint2.pos + joint2.axis // あってる?
        - ...
            - geom_link: - joint.axis - joint.pos + geom.pos + geom.quat
            - child_body_link: - joint.axis - joint.pos + body.pos + body.quat
                - ...
*/
// clang-format on

void parseBody(std::vector<webcface::RobotLink> &w_links,
               const pugi::xml_node &body, bool is_world,
               std::string_view parent_joint_link_name,
               const webcface::Transform &parent_joint_tf) {
    // attribute
    // ignore: gravcomp, user
    // todo: childclass, mocap
    auto body_name = is_world ? "world" : body.attribute("name").as_string();
    webcface::Transform body_tf = parseTransform(body);
    if (is_world) {
        w_links.push_back(webcface::RobotLink(
            body_name,
            webcface::fixedJoint(parent_joint_link_name,
                                 parent_joint_tf.inversed() * body_tf),
            {}));
    } else {
        w_links.push_back(webcface::RobotLink(
            body_name, webcface::fixedAbsolute(body_tf), {}));
    }
    webcface::Transform last_joint_tf;

    // children
    // ignore: inertial, camera, light,
    // todo: freejoint, attach, site, plugin, composite, flexcomp

    for (pugi::xml_node joint = body.child("joint"); joint;
         joint = joint.next_sibling("joint")) {
        // ignore: springdamper, sol*, stiffness, range, limited, actuator*,
        // margin,
        //  springref, armature, damping, frictionloss, user
        // todo: class, group, ref
        std::string j_name = joint.attribute("name").as_string();
        auto j_pos = parseReal(joint.attribute("pos").as_string("0 0 0"));
        auto j_axis = parseReal(joint.attribute("axis").as_string("0 0 1"));
        // zがj_axisの方向を向くように回転
        webcface::Transform current_j_tf =
            webcface::translation(j_pos) *
            webcface::rotEuler(
                std::atan2(j_axis.at(1), j_axis.at(0)),
                std::atan2(j_axis.at(2),
                           std::hypot(j_axis.at(0), j_axis.at(1))),
                0, webcface::AxisSequence::ZYX);
        webcface::Transform j_tf = last_joint_tf.inversed() * current_j_tf;
        last_joint_tf = current_j_tf;

        pugi::xml_attribute type = joint.attribute("type");
        if (type.as_string() == "free"sv) {
            // todo
        } else if (type.as_string() == "slide"sv) {
            w_links.push_back(webcface::RobotLink(
                j_name,
                webcface::prismaticJoint(j_name, w_links.back().name(), j_tf),
                {}));
        } else if (type.as_string() == "hinge"sv) {
            w_links.push_back(webcface::RobotLink(
                j_name,
                webcface::rotationalJoint(j_name, w_links.back().name(), j_tf),
                {}));
        } else if (type.as_string() == "ball"sv) {
            spdlog::error("Ball joint is not implemented");
        }
    }
    std::string_view last_joint_name = w_links.back().name();
    for (pugi::xml_node geom = body.child("geom"); geom;
         geom = geom.next_sibling("geom")) {
        auto geom_link = parseGeom(geom, last_joint_name, last_joint_tf);
        if (geom_link) {
            w_links.push_back(std::move(*geom_link));
        }
    }
    for (pugi::xml_node child_body = body.child("body"); child_body;
         child_body = child_body.next_sibling("body")) {
        parseBody(w_links, child_body, false, last_joint_name, last_joint_tf);
    }
}
