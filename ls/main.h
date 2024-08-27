#include <map>
#include <webcface/client.h>
#include <ftxui/dom/elements.hpp>

extern bool ok;

ftxui::Element elemMember(const webcface::Member &m);
ftxui::Element elemSpace1();
ftxui::Element elemColon();

// -l オプション時の表示
ftxui::Element listMemberAll(webcface::Client &wcli, bool recursive);
ftxui::Element listFields(webcface::Client &wcli,
                          const std::vector<std::string> &fields,
                          bool recursive);
// member1行の表示
ftxui::Element memberInfo(const webcface::Member &m);
std::multimap<std::string, ftxui::Element>
// field1行の表示
fieldInfo(const webcface::Member &m, const std::string &field_prefix,
          bool recursive, int tab);

ftxui::Element lsFlex(std::vector<ftxui::Element> &&elements);
ftxui::Element listMemberShort(webcface::Client &wcli);
ftxui::Element listMemberShortRecurse(webcface::Client &wcli);
