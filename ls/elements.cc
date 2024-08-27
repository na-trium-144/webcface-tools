#include "./main.h"

ftxui::Element elemMember(const webcface::Member &m) {
    return ftxui::text(m.name()) |
           (m.pingStatus() ? ftxui::bold : ftxui::nothing);
}

ftxui::Element elemSpace1(){
    return ftxui::emptyElement() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1);
}
ftxui::Element elemColon(){
    return ftxui::text(":");
}

