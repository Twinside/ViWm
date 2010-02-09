#include "GridLayout.h"

namespace Layout
{
#if 0
void griLayout()
{
    int ah, aw, rows, cols;
    for (cols = 0; cols <= (nodeCount + 1)/2; cols++) {
        if (cols * cols >= (nodeCount + 1)) {
            break;
        }
    }
    rows = (cols && (cols - 1) * cols >= (nodeCount + 1)) ? cols - 1 : cols;
    height = screen_height / (rows ? rows : 1);
    width = screen_width / (cols ? cols : 1);
    if (rows > 1 && i == (rows * cols) - cols && ((nodeCount + 1) - i) <= ((nodeCount + 1) % cols)) {
        width = screen_width / ((nodeCount + 1) - i);
    }
    x = (i % cols) * width;
    y = (i / cols) * height;
    ah = (i >= cols * (rows - 1)) ? screen_height - height * rows: 0;
    if (rows > 1 && i == (nodeCount + 1) - 1 && ((nodeCount + 1) - i) < ((nodeCount + 1) % cols)) {
        aw = screen_width - width * ((nodeCount + 1) % cols);
    } else {
        aw = ((i + 1) % cols == 0) ? screen_width - width * cols : 0;
    }
    width += aw;
    height += ah;

            SetWindowPos(temp->hwnd, HWND_TOP, x + screen_x, y + screen_y, width, height, SWP_SHOWWINDOW);
}
#endif
}