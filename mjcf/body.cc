#include "main.h"
#include <cmath>
#include <spdlog/spdlog.h>

/*
    mujocoでは、
        body座標系 = parent座標系 + body.pos + body.quat
        joint座標系 = parent座標系 + body.pos + body.quat + joint.pos (+
   joint.axis) geom座標系 = parent座標系 + body.pos + body.quat + geom.pos +
   geom.quat

    webcfaceでは、
        joint座標系 = parent座標系 + joint.origin
        geom座標系 = parent座標系 + joint.origin + geom.origin
    よって、
        joint.origin = body.pos + body.quat + joint.pos + joint.axis
        geom.origin = - joint.axis - joint.pos + geom.pos + geom.quat
*/

void parseBody(const pugi::xml_node &body, bool is_world,
               std::string_view parent_body_name) {
    // attribute
    // ignore: gravcomp, user
    // todo: childclass, mocap
    auto name = is_world ? "world" : body.attribute("name").as_string();
    webcface::Transform body_tf = parseTransform(body);
    std::vector<webcface::RobotLink> w_links = {
        webcface::RobotLink(
            name, webcface::fixedJoint(parent_body_name, body_tf), {}),
    };
    webcface::Transform joint_current_rot;
    webcface::Transform joint_tf_all;

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
        webcface::Transform j_rot_tf = webcface::rotEuler(
            std::atan2(j_axis.at(1), j_axis.at(0)),
            std::atan2(j_axis.at(2), std::hypot(j_axis.at(0), j_axis.at(1))), 0,
            webcface::AxisSequence::ZYX);
        webcface::Transform j_tf = joint_current_rot.inversed() *
                                   webcface::translate(j_pos) * j_rot_tf;
        joint_current_rot = j_rot_tf;

        pugi::xml_attribute type = joint.attribute("type");
        if (type.as_string() == "free"sv) {
            // todo
        } else if (type.as_string() == "slide"sv) {
            auto w_joint =
                webcface::prismaticJoint(j_name, w_links.back().name(), j_tf);
            w_links.push_back(webcface::RobotLink(j_name, w_joint, {}));
            joint_tf_all *= j_tf;
        } else if (type.as_string() == "hinge"sv) {
            auto w_joint =
                webcface::rotationalJoint(j_name, w_links.back().name(), j_tf);
            w_links.push_back(webcface::RobotLink(name, w_joint, {}));
            joint_tf_all *= j_tf;
        } else if (type.as_string() == "ball"sv) {
            spdlog::error("Ball joint is not implemented");
        }
    }
    for (pugi::xml_node geom = body.child("geom"); geom;
         geom = geom.next_sibling("geom")) {
        parseGeom(geom);
    }
}
