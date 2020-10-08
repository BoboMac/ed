#pragma onces
#include "core.h"

// Each buffer will contain a file that will be edited
class Buffer {
public:
    std::string text;
    std::string filename;
    std::fstream file;
    UINT y_offset;
    UINT x_offset;
    UINT cursor_x;
    UINT cursor_y;

    void MoveCursorLeft(UINT char_num = 1) {
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
            buffers.erase(index, index+1);
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
        Buffer buf;
        buf.file.open(filename);
        buf.filename = filename;
        {
            char c;
            while(buf.file >> std::noskipws >> c) {
                buf.text += c;
            }
        }
        buffers.push_back(buf);
    }
    bool AddExistingBufferFromPane(std::string filename, Pane *pane) {
        char index = pane->FindBufferIndex(filename);
        if(index != -1) {
            Buffer buf = pane->buffers[index];
        } else { return 0; }
        pane->RemoveBuffer(filename);
        buffers.push_back(buf);
        return 1;
    }
};

// All panes will be empty at creation
// and user will add the buffers he needs
void CreateFirstPane(std::vector<Pane> *panes, UINT width, UINT height) {
    Pane pane;
    pane.x = 0;
    pane.y = 0;
    pane.width = width;
    pane.height = height;
    panes->push_back(pane);
}
void SplitHorizPane(std::vector<Pane> *panes, char index) {
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
    panes->erase(index, index + 1);
    panes->push_back(pane1);
    panes->push_back(pane2);
}
void SplitVertPane(std::vector<Pane> *panes, char index) {
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
    panes->erase(index, index + 1);
    panes->push_back(pane1);
    panes->push_back(pane2);
}
