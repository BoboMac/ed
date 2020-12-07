#pragma once
#include "core.h"

const UCHAR TAB_SIZE = 4;
const UCHAR SCROLL_LINE_NUM = 1;
const UCHAR SCROLL_COL_NUM = 1;

class Buffer {
public:
    std::vector<std::string> text;
    std::string filename;
    std::vector<std::string> on_screen_text;

    // Horizontal and vertical scroll
    UINT y_offset = 0;
    UINT x_offset = 0;

    // Cursor position
    UINT cursor = 0;

    // Cursor pane coords
    UINT cursor_x = 0;
    UINT cursor_y = 0;

    void UpdateOnScrText(UINT width, UINT height) {
        std::string s;
        on_screen_text.clear();
        if(y_offset + height < text.size()) {
            for(int i = 0; i < height; i++) {
                if(x_offset + width < text[i + y_offset].size()) {
                    for(int j = 0; j < width; j++) {
                        s += text[i + y_offset][j + x_offset];
                    }
                }
                else if(x_offset < text[i + y_offset].size()) {
                    for(int j = x_offset; j < text[i + y_offset].size(); j++) {
                        s += text[i + y_offset][j];
                    }
                }
                on_screen_text.push_back(s);
                s.clear();
            }
        }
        else if(y_offset < text.size()) {
            for(int i = y_offset; i < text.size(); i++) {
                if(x_offset + width < text[i].size()) {
                    for(int j = 0; j < width; j++) {
                        s += text[i][j + x_offset];
                    }
                }
                else if(x_offset < text[i].size()) {
                    for(int j = x_offset; j < text[i].size(); j++) {
                        s += text[i][j];
                    }
                }
                on_screen_text.push_back(s);
                s.clear();
            }
        }
    }
    void MoveCursorLeft(UINT char_num = 1) {
        if(cursor >= 0) {
            cursor -= char_num;
        }
        else {
            cursor = 0;
        }
    }
    void MoveCursorRight(UINT char_num = 1) {
        if(cursor < text.size()) {
            cursor += char_num;
        }
        else {
            cursor = text.size() - 1;
        }
    }
};

struct PaneInfoStruct {
    UINT x = 0, y = 0;
    UINT width = 0, height = 0;
};
UCHAR active_pane = 0;
class Pane {
private:
    UCHAR FindBufferIndex(std::string filename) {
        for(int i = 0; i < buffers.size(); i++) {
            if(buffers[i].filename == filename) {
                return i;
            }
        }
    }
public:
    UINT x = 0, y = 0;
    UINT width = 0;
    UINT height = 0;
    std::vector<PaneInfoStruct> pos_history;

    std::vector<Buffer> buffers;
    UCHAR buf_index = 0;

    bool UndoPos() {
        if(pos_history.size() == 0) return 0;
        PaneInfoStruct info = pos_history[pos_history.size() - 1];
        pos_history.erase(pos_history.end() - 1, pos_history.end());
        x = info.x;
        y = info.y;
        width = info.width;
        height = info.height;
        return 1;
    }
    bool RemoveCurrentBuffer() {
        if(buffers.size() == 0) return 0;
        buffers.erase(buffers.begin()+buf_index, buffers.begin()+buf_index+1);
        buf_index = 0;
        return 1;
    }
    bool RemoveBuffer(std::string filename) {
        UCHAR index;
        if(index != -1) {
            buffers.erase(buffers.begin() + index, buffers.begin() + index+1);
            return 1;
        }
        return 0;
    }
    bool AddBuffer(std::string filename) {
        Buffer buf = {};
        buf.filename = filename;
        buf.text = ParseStrIntoVec(ReadFile(filename));
        buffers.push_back(buf);
        return 1;
    }
    bool AddExistingBufferFromPane(std::string filename, Pane &pane) {
        Buffer buf = {};
        char index = pane.FindBufferIndex(filename);
        if(index != -1) {
            buf = pane.buffers[index];
        } else { return 0; }
        pane.RemoveBuffer(filename);
        buffers.push_back(buf);
        return 1;
    }
    void ScrollUp(UINT line_num) {
        if(buffers[buf_index].y_offset >= line_num) {
            buffers[buf_index].y_offset -= line_num;
        }
        else {
            buffers[buf_index].y_offset = 0;
        }
    }
    void ScrollDown(UINT line_num) {
        buffers[buf_index].y_offset += line_num;
    }
    void ScrollLeft(UINT col_num) {
        if(buffers[buf_index].x_offset >= col_num) {
            buffers[buf_index].x_offset -= col_num;
        }
        else {
            buffers[buf_index].x_offset = 0;
        }
    }
    void ScrollRight(UINT col_num) {
        buffers[buf_index].x_offset += col_num;
    }
};

void AddInfoPosToAllPanes(std::vector<Pane> &panes) {
    for(int i = 0; i < panes.size(); i++) {
        panes[i].pos_history.push_back({panes[i].x, panes[i].y, panes[i].width, panes[i].height});
    }
}
void CreateFirstPane(std::vector<Pane> &panes, UINT width, UINT height) {
    Pane pane;
    pane.x = 0;
    pane.y = 0;
    pane.width = width;
    pane.height = height;
    active_pane = 0;
    pane.pos_history.push_back({0, 0, width, height});
    panes.push_back(pane);
}
void RemoveActivePane(std::vector<Pane> &panes, UCHAR active_pane) {
    panes.erase(panes.begin() + active_pane, panes.begin() + active_pane + 1);
    active_pane = 0;
    for(int i = 0; i < panes.size(); i++) {
        panes[i].UndoPos();
    }
}
void SplitHorizPane(std::vector<Pane> &panes, char index) {
    Pane pane = panes[index];
    UCHAR odd = 1;
    if(pane.height % 2 == 0) odd = 0;
    Pane pane1;
    Pane pane2;
    pane1.x = pane.x;
    pane1.y = pane.y;
    pane1.width = pane.width;
    pane1.height = pane.height / 2;
    pane2.x = pane.x;
    pane2.y = pane.y + pane.height / 2;
    pane2.width = pane.width;
    pane2.height = pane.height / 2 + odd;
    panes.erase(panes.begin() + index, panes.begin() + index + 1);
    pane1.pos_history.push_back({pane.x, pane.y, pane.width, pane.height});
    pane2.pos_history.push_back({pane.x, pane.y, pane.width, pane.height});
    AddInfoPosToAllPanes(panes);
    panes.push_back(pane1);
    panes.push_back(pane2);
}
void SplitVertPane(std::vector<Pane> &panes, char index) {
    Pane pane = panes[index];
    UCHAR odd = 1;
    if(pane.width % 2 == 0) odd = 0;
    Pane pane1;
    Pane pane2;
    pane1.x = pane.x;
    pane1.y = pane.y;
    pane1.width = pane.width / 2;
    pane1.height = pane.height;
    pane2.x = pane.x + pane.width / 2;
    pane2.y = pane.y;
    pane2.width = pane.width / 2 + odd;
    pane2.height = pane.height;
    panes.erase(panes.begin() + index, panes.begin() + index + 1);
    pane1.pos_history.push_back({pane.x, pane.y, pane.width, pane.height});
    pane2.pos_history.push_back({pane.x, pane.y, pane.width, pane.height});
    AddInfoPosToAllPanes(panes);
    panes.push_back(pane1);
    panes.push_back(pane2);
}

#ifndef CTRL
    #define CTRL(c) ((c) & 037)
#endif
#define KEY_ESC 27
// TODO: Add error checking
std::map<std::string, UINT> keymap;
std::vector<std::string> keymap_commands = {
    "exit",
    "switch_pane",
    "switch_buffer",
    "kill_buffer",
    "split_vert",
    "split_horiz",
    "save_file",
    "open_file"
};
void GetKeyMapping(std::string filename) {
    for(int i = 0; i < keymap_commands.size(); i++) {
        keymap.emplace(keymap_commands[i], 0);
    }
    std::ifstream file;
    file.open(filename);
    std::string token;
    while(file >> token) {
        std::string name = token;
        if(std::find(keymap_commands.begin(), keymap_commands.end(), name) != keymap_commands.end()) {
            file >> token;
            if(token == "ctrl") {
                file >> token;
                if(token == "ESC") {
                    keymap[name] = KEY_ESC;
                }
                else {
                    keymap[name] = CTRL(token[0]);
                }
            }
            else if (token == "null") {
                file >> token;
                if(token == "ESC") {
                    keymap[name] = KEY_ESC;
                }
                else {
                    keymap[name] = token[0];
                }
            }
        }
    }
    //std::cout << "haya" << std::endl;

}
