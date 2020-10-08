#pragma once

void GetConsoleSize(UINT &width, UINT &height) {
    getmaxyx(screen, height, width);
}
void DrawPanes(std::vector<Pane> panes) {
    for(int i = 0; i < panes.size(); i++) {
        
    }
}
