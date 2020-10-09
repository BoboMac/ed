#pragma onces
#include "core.h"

// Each buffer will contain a file that will be edited
class Buffer {
public:
    std::string text;
    std::string filename;
    std::string on_screen_text;

    // Horizontal and vertical scroll
    UINT y_offset;
    UINT x_offset;

    // Cursor position
    UINT cursor;

    // Cursor pane coords
    UINT cursor_x;
    UINT cursor_y;

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

// Panes will not use width and height
// since they will be somewhat proportional
class Pane {
private:
    char FindBufferIndex(std::string filename) {
        for(int i = 0; i < buffers.size(); i++) {
            if(buffers[i].filename == filename) {
                return i;
            }
        }
    }
public:
    UINT x, y;
    UINT width;
    UINT height;

    UCHAR position;
    std::vector<Buffer> buffers;

    bool RemoveBuffer(std::string filename) {
        char index;
        if(index != -1) {
            buffers.erase(buffers.begin() + index, buffers.begin() + index+1);
            return 1;
        }
        return 0;
    }
    bool AddBuffer(std::string filename) {
        /*
        - open file
        - read contents
        - store them in RAM
        - write to the file every time the user saves
        - close the file and erase the contents from RAM
        */
        Buffer buf = {};
        buf.filename = filename;
        buf.text = ReadFile(filename);
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
};

// All panes will be empty at creation
// and user will add the buffers he needs as he goes
void CreateFirstPane(std::vector<Pane> &panes, UINT width, UINT height) {
    Pane pane;
    pane.x = 0;
    pane.y = 0;
    pane.width = width;
    pane.height = height;
    panes.push_back(pane);
}
void SplitHorizPane(std::vector<Pane> &panes, char index) {
    Pane pane = panes[index];
    Pane pane1;
    Pane pane2;
    pane1.x = pane.x;
    pane1.y = pane.y;
    pane1.width = pane.width / 2;
    pane1.height = pane.height;
    pane2.x = pane.x + pane.height / 2;
    pane2.y = pane.y;
    pane2.width = pane.width / 2;
    pane2.height = pane.height;
    panes.erase(panes.begin() + index, panes.begin() + index + 1);
    panes.push_back(pane1);
    panes.push_back(pane2);
}
void SplitVertPane(std::vector<Pane> &panes, char index) {
    Pane pane = panes[index];
    Pane pane1;
    Pane pane2;
    pane1.x = pane.x;
    pane1.y = pane.y;
    pane1.width = pane.width / 2;
    pane1.height = pane.height;
    pane2.x = pane.x + pane.height / 2;
    pane2.y = pane.y;
    pane2.width = pane.width / 2;
    pane2.height = pane.height;
    panes.erase(panes.begin() + index, panes.begin() + index + 1);
    panes.push_back(pane1);
    panes.push_back(pane2);
}
