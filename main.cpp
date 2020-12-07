#include "output.h" // backend.h included
#include "input.h"

int main(int argc, char **argv) {

    GetKeyMapping("config.ed");
    UINT width = 0, height = 0;
    std::vector<Pane> panes;
    ncInit(&width, &height);
    CreateFirstPane(panes, width, height);

    // Using the CLI arguments to open the first file(s)
    bool loading_errors = 0;
    for(int i = 1; i < argc; i++) {
        // Check if file exists
        if(access(argv[i], F_OK) != -1) {
            // Read file and create buffer on the first pane
            panes[0].AddBuffer(argv[i]);
        }
        else {
            ncTerminate();
            std::cout << "Error: could not find file \"" << argv[i] << "\"\n";
            loading_errors = 1;
        }
    }
    if(loading_errors) exit(0);

    std::cout << "HJELLLL\n";
    // ncurses

    // ncurses colors (0 -> 1000)
    const UCHAR bkg_color      =  9;
    const UCHAR text_color     = 10;
    const UCHAR keyword_color  = 11;
    const UCHAR number_color   = 12;
    const UCHAR buf_info_bkg_color = 13;
    const UCHAR active_buf_info_color = 14;
    CreateColor(bkg_color, 50, 50, 50);
    CreateColor(text_color, 1000, 1000, 1000);
    CreateColor(keyword_color, 500, 500, 500);
    CreateColor(number_color, 500, 500, 500);
    CreateColor(buf_info_bkg_color, 1000, 1000, 1000);

    // ncurses color pairs
    const UCHAR text_color_pair     = 1;
    const UCHAR keyword_color_pair  = 2;
    const UCHAR number_color_pair   = 3;
    const UCHAR buf_info_color_pair = 4;
    const UCHAR active_buf_info_color_pair = 5;
    CreateColorPair(text_color_pair, text_color, bkg_color);
    CreateColorPair(keyword_color_pair, keyword_color, bkg_color);
    CreateColorPair(number_color_pair, number_color, bkg_color);
    CreateColorPair(buf_info_color_pair, bkg_color, buf_info_bkg_color);
    CreateColorPair(active_buf_info_color_pair, bkg_color, active_buf_info_color);

    bool running = 1;
    #define pane panes[active_pane]
    while(running) {
        for(int i = 0; i < panes.size(); i++) {
            DrawPane(
                i, panes[i],
                active_buf_info_color_pair,
                buf_info_color_pair,
                text_color_pair,
                keyword_color_pair,
                number_color_pair
            );
        }
        UINT character = getch();
        if(character == keymap["exit"]) {
                running = 0;
        }
        else if(character == KEY_UP) {
            panes[active_pane].ScrollUp(SCROLL_LINE_NUM);
        }
        else if(character == KEY_DOWN) {
            panes[active_pane].ScrollDown(SCROLL_LINE_NUM);
        }
        else if(character == KEY_LEFT) {
            panes[active_pane].ScrollLeft(SCROLL_COL_NUM);
        }
        else if(character == KEY_RIGHT) {
            panes[active_pane].ScrollRight(SCROLL_COL_NUM);
        }
        // Pane switching
        else if(character == keymap["switch_pane"]) {
            switch(getch()) {
                case KEY_LEFT:
                    if(active_pane > 0) active_pane--;
                    else active_pane = panes.size() - 1;
                    break;
                case KEY_RIGHT:
                    if(active_pane < panes.size() - 1) active_pane++;
                    else active_pane = 0;
                    break;
            }
        }
        // Buffer switching
        else if(character == keymap["switch_buffer"]) {
            switch(getch()) {
                case KEY_LEFT:
                    if(pane.buf_index > 0) pane.buf_index--;
                    else pane.buf_index = pane.buffers.size() - 1;
                    break;
                case KEY_RIGHT:
                    if(pane.buf_index < pane.buffers.size() - 1) pane.buf_index++;
                    else pane.buf_index = 0;
                    break;
            }
        }
        else if(character == keymap["kill_buffer"]) {
            pane.RemoveCurrentBuffer();
        }
        Present();
    }
    #undef pane
    // Terminate and return to terminal
    ncTerminate();
    return 0;
}
