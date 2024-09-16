#pragma once
#include <map>
#include <webcface/client.h>
#include <ftxui/dom/elements.hpp>

extern bool ok;

ftxui::Element elemMember(const webcface::Member &m);
ftxui::Element elemSpace1();
ftxui::Element elemColon();

// -l オプション時の表示 (中でmemberInfoとfieldInfoを呼ぶ)
ftxui::Element listMemberAll(webcface::Client &wcli, bool recursive);
ftxui::Element listFields(webcface::Client &wcli,
                          const std::vector<std::string> &fields,
                          bool recursive);
// member1行の表示
ftxui::Element memberInfo(const webcface::Member &m);
std::multimap<std::string, ftxui::Element>
// 指定した名前以下のfieldの列挙
fieldInfo(const webcface::Member &m, const std::string &field_prefix,
          bool recursive, int tab);

ftxui::Element lsFlex(std::vector<ftxui::Element> &&elements);

// -l オプションなしの場合の表示 (中でfieldInfoShortを呼ぶ)
ftxui::Element listMemberShort(webcface::Client &wcli);
ftxui::Element listMemberShortRecurse(webcface::Client &wcli);
ftxui::Element listFieldsShort(webcface::Client &wcli,
                               const std::vector<std::string> &fields,
                               bool recursive);

ftxui::Element fieldInfoShort(const webcface::Member &m,
                              const std::string &field_prefix, bool recursive,
                              bool with_header);
