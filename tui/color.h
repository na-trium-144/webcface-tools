#pragma once

inline ftxui::Color rgbColor(std::uint32_t col) {
    return ftxui::Color(col >> 16 & 0xff, col >> 8 & 0xff, col & 0xff);
}
inline ftxui::Color convertColor(webcface::ViewColor color, bool light,
                                 bool active = false) {
    switch (color) {
    case webcface::ViewColor::black:
        if (light) {
            return active ? rgbColor(0x333333) : rgbColor(0x000000);
        } else {
            return active ? rgbColor(0xcccccc) : rgbColor(0xffffff);
        }
    case webcface::ViewColor::white:
        if (light) {
            return active ? rgbColor(0xcccccc) : rgbColor(0xffffff);
        } else {
            return active ? rgbColor(0x333333) : rgbColor(0x000000);
        }
    case webcface::ViewColor::gray:
        return active ? rgbColor(0x666666) : rgbColor(0x999999);
    case webcface::ViewColor::red:
        return active ? rgbColor(0xcc3300) : rgbColor(0xff0000);
    case webcface::ViewColor::orange:
        return active ? rgbColor(0xffcc33) : rgbColor(0xffa500);
    case webcface::ViewColor::yellow:
        return active ? rgbColor(0xffff66) : rgbColor(0xffff00);
    case webcface::ViewColor::green:
        return active ? rgbColor(0x33cc33) : rgbColor(0x008000);
    default:
        return ftxui::Color::Default;
    }
}
inline ftxui::Color convertColor(webcface::ViewColor color,
                                 webcface::ViewColor default_color, bool light,
                                 bool active = false) {
    return convertColor(color == webcface::ViewColor::inherit ? default_color
                                                              : color,
                        light, active);
}