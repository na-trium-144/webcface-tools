#include <webcface/client.h>
#include <ftxui/dom/elements.hpp>

ftxui::Element elemMember(const webcface::Member &m);
ftxui::Element elemSpace1();
ftxui::Element elemColon();

ftxui::Element listMember(webcface::Client &wcli, bool recursive);
ftxui::Element lsFlex(std::vector<ftxui::Element>&& elements);
ftxui::Element listMemberShort(webcface::Client &wcli);
ftxui::Element listMemberShortRecurse(webcface::Client &wcli);

