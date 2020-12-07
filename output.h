#pragma once
#include "backend.h"
#include <ncurses.h>

void ncInit(UINT *width, UINT *height) {
    // Set all the flags
    initscr();
    raw();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    start_color();
    getmaxyx(stdscr, *height, *width);
}
void ncTerminate() {
    // Set all the flags back to default
    endwin();
}
void CreateColor(UCHAR index, UINT r, UINT g, UINT b) {
    init_color(index, r, g, b);
}
void CreateColorPair(UCHAR index, UCHAR fg_color, UCHAR bg_color) {
    init_pair((int)index, fg_color, bg_color);
}
void BindColorPair(UCHAR color_pair) {
    attron(COLOR_PAIR(color_pair));
}
void UnbindColorPair(UCHAR color_pair) {
    attroff(COLOR_PAIR(color_pair));
}
void DrawChar(char ch, UINT x, UINT y) {
    mvaddch(y, x, ch);
}
void DrawPane(UCHAR pane_id, Pane pane, UCHAR active_buf_info_color_pair, UCHAR buf_info_color_pair, UCHAR text_color_pair, UCHAR keyword_color_pair, UCHAR number_color_pair) {
    // Clear screen with for loop
    BindColorPair(text_color_pair);
    for(int i = pane.y; i < pane.y + pane.height; i++) {
        for(int j = pane.x; j < pane.x + pane.width; j++) {
            DrawChar(' ', j, i);
        }
    }

    // Output the text
    #define buf pane.buffers[pane.buf_index]
    if(pane.buffers.size() != 0) {
        buf.UpdateOnScrText(pane.width, pane.height);
        for(int i = 0; i < buf.on_screen_text.size(); i++) {
            mvprintw(pane.y + i, pane.x, buf.on_screen_text[i].c_str());
        }
    }
    // Move the cursor

    // Draw the buffer name and left border side
    std::string buf_info;
    buf_info.append("   ");
    if(pane.buffers.size() != 0) {
        for(int i = 0; i < buf.filename.size(); i++) {
            buf_info.push_back(buf.filename[i]);
        }
        if(pane_id == active_pane) {
            buf_info.append(" - ACTIVE");
        }
    }
    else {
        if(pane_id == active_pane) {
            buf_info.append("ACTIVE - NO BUFFER OPENED");
        }
    }
    while(buf_info.size() < pane.width) {
        buf_info += ' ';
    }
    UnbindColorPair(text_color_pair);
    if(pane_id == active_pane) BindColorPair(active_buf_info_color_pair);
    else BindColorPair(buf_info_color_pair);
    if(pane_id == active_pane) UnbindColorPair(active_buf_info_color_pair);
    mvprintw(pane.y + pane.height - 1, pane.x, buf_info.c_str());
    for(int i = pane.y; i < pane.y + pane.height - 1; i++) {
        DrawChar(' ', pane.x + pane.width - 1, i);
    }
    if(pane_id == active_pane) UnbindColorPair(active_buf_info_color_pair);
    else UnbindColorPair(buf_info_color_pair);
    #undef buf
}

void Present() {
    refresh();
    //clear();
}
