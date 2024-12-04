#include "main.h"
#include <spdlog/spdlog.h>

std::vector<double> parseReal(std::string_view str) {
    std::vector<double> ret;
    while (str.find(' ') != std::string::npos) {
        ret.push_back(std::stod(std::string(str.substr(0, str.find(' ')))));
        str = str.substr(str.find(' ') + 1);
    }
    ret.push_back(std::stod(std::string(str)));
    return ret;
}

webcface::Transform parseTransform(const pugi::xml_node &node) {
    auto pos = parseReal(node.attribute("pos").as_string("0 0 0"));
    if (node.attribute("quat")) {
        auto quat = parseReal(node.attribute("quat").as_string());
        return webcface::Transform(pos, webcface::rotQuat(quat));
    } else if (node.attribute("axisangle")) {
        // todo: compilerの設定によってradianかdegreeかが切り替わる
        spdlog::warn("Angle is always treated as radian.");
        auto axisangle = parseReal(node.attribute("axisangle").as_string());
        return webcface::Transform(
            pos, webcface::rotAxisAngle(
                     {axisangle.at(0), axisangle.at(1), axisangle.at(2)},
                     axisangle.at(3)));
    } else if (node.attribute("euler")) {
        // todo: compilerの設定によってaxisの順番が変わる
        spdlog::warn("Axis order of euler angle not implemented:"
                     "it will be always treated as zyx.");
        auto euler = parseReal(node.attribute("euler").as_string());
        return webcface::Transform(pos, webcface::rotEuler(euler));
    } else if (node.attribute("xyaxes")) {
        spdlog::error("Frame orientation by xyaxes is not implemented");
    } else if (node.attribute("zaxis")) {
        spdlog::error("Frame orientation by zaxis is not implemented");
    }
    return webcface::translation(pos);
}
