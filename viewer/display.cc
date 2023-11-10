#include <ncurses.h>
#include <sstream>
#include "main.h"

int y;
int width, height;
WINDOW *win;
void init_display() {
    win = initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}
void update_display() {
    std::lock_guard lock(mtx);
    clear();
    getmaxyx(win, height, width);
    for (int i = y; i < y + height - 1 && i < data.size(); i++) {
        std::stringstream ss_l, ss_r;
        if (std::holds_alternative<WebCFace::Value>(data[i])) {
            auto v = std::get<WebCFace::Value>(data[i]);
            ss_l << v.member().name() << "::" << v.name();
            ss_r << " = " << v.get();
        } else if (std::holds_alternative<WebCFace::Text>(data[i])) {
            auto v = std::get<WebCFace::Text>(data[i]);
            ss_l << v.member().name() << "::" << v.name();
            ss_r << " = " << v.get();
        }
        std::string s_l = ss_l.str(), s_r = ss_r.str();
        if (s_l.size() + s_r.size() > width) {
            s_l = s_l.substr(0, width - s_r.size() - 3) + "...";
        }
        mvprintw(i - y, 0, "%s", (s_l + s_r).c_str());
    }
    mvprintw(height - 1, 0, "[%d/%u] up / down to scroll, q to quit", y,
             data.size());
    refresh();
}
void loop_display() {
    while (true) {
        int ch = getch();
        switch (ch) {
        case 'q':
            return;
        case KEY_UP:
            if (y > 0) {
                y--;
                update_display();
            }
            break;
        case KEY_DOWN:
            if (y < static_cast<int>(data.size()) - (height - 1) - 1) {
                y++;
                update_display();
            }
            break;
        }
    }
}
void exit_display() { endwin(); }
