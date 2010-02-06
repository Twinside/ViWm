#include "ViWm.h"

ViWm::ViWm(void)
    : experimental_mouse( 0 )
    , mouse_pos_out( 0 )
    , margin( 120 )
    , tilingMode( VerticalTilling )
    , alpha( 255 )
    , borders( 1 )
    , ignoreCount( 0 )
    , ignoreCountBorders( 0 )
    , masterarea_count( 1 )
    , modkeys( DEFAULT_MODKEY )
{
    EnumDisplayMonitors( NULL, NULL, &ViWm::monitorEnumerator, &currentLayout );
}

ViWm::~ViWm(void)
{
}

void ViWm::UnregisterHotkeys(HWND hwnd)
{
    int i;
    for (i = 1; i <= 27; i++) UnregisterHotKey(hwnd, i);
}

void ViWm::RegisterHotkeys(HWND hwnd)
{
    RegisterHotKey(hwnd, 1, modkeys, 'K'); 		/* Select Up */
    RegisterHotKey(hwnd, 2, modkeys, 'J'); 		/* Select Down */
    RegisterHotKey(hwnd, 3, modkeys, VK_RETURN); 	/* Move Window in to Main Area */
    RegisterHotKey(hwnd, 4, modkeys, VK_ESCAPE); 	/* Exit */
    RegisterHotKey(hwnd, 5, modkeys, 'H'); 		/* Margin Left */
    RegisterHotKey(hwnd, 6, modkeys, 'L'); 		/* Margin Right */
    RegisterHotKey(hwnd, 7, modkeys, 'I'); 		/* Ignore Mode */
    RegisterHotKey(hwnd, 8, modkeys, 'U'); 		/* Mouse Lock Mode */
    RegisterHotKey(hwnd, 9, modkeys, VK_SPACE); 	/* Switch Tiling Mode */
    RegisterHotKey(hwnd, 10, modkeys | MOD_SHIFT, 'K'); 	/* Move Window Up */
    RegisterHotKey(hwnd, 11, modkeys | MOD_SHIFT, 'J'); 	/* Move Window Down */
    RegisterHotKey(hwnd, 12, modkeys, 'Y'); 	/* Display Window Class */
    RegisterHotKey(hwnd, 13, modkeys, 'O'); 	/* Tile Window */
    RegisterHotKey(hwnd, 14, modkeys, 'P'); 	/* Untile Window */
    RegisterHotKey(hwnd, 15, modkeys, 'Z'); 	/* Increase Master Area */
    RegisterHotKey(hwnd, 16, modkeys, 'X'); 	/* Decrease Master Area */
    RegisterHotKey(hwnd, 17, modkeys, 'C'); 	/* Close Foreground Window */

    // Tags
    RegisterHotKey(hwnd, 18, modkeys, '1'); 	/* Switch to tag 1 */
    RegisterHotKey(hwnd, 19, modkeys, '2'); 	/* Switch to tag 2 */
    RegisterHotKey(hwnd, 20, modkeys, '3'); 	/* Switch to tag 3 */
    RegisterHotKey(hwnd, 21, modkeys, '4'); 	/* Switch to tag 4 */
    RegisterHotKey(hwnd, 22, modkeys, '5'); 	/* Switch to tag 5 */

    RegisterHotKey(hwnd, 23, modkeys | MOD_SHIFT, '1'); 	/* Move to tag 1 */
    RegisterHotKey(hwnd, 24, modkeys | MOD_SHIFT, '2'); 	/* Move to tag 2 */
    RegisterHotKey(hwnd, 25, modkeys | MOD_SHIFT, '3'); 	/* Move to tag 3 */
    RegisterHotKey(hwnd, 26, modkeys | MOD_SHIFT, '4'); 	/* Move to tag 4 */
    RegisterHotKey(hwnd, 27, modkeys | MOD_SHIFT, '5'); 	/* Move to tag 5 */
}

void ViWm::ArrangeWindows()
{
    int nodeCount, i, x, y, width, height;

    nodeCount = CountNodes();
    if (nodeCount == -1) return;
    i = 0;

    node *temp = nodes;
    //	for (temp = nodes; i <= a; temp = temp->next, i++) {
    for (temp = (node*)nodes; temp; temp = (node*)temp->next) {
        if (temp->tags == current_tags) {
            ShowWindow(temp->hwnd, SW_RESTORE);
            if (nodeCount == 0) { 	/* I think this is universal to all tiling modes */
                x = 0;
                y = 0;
                width = screen_width;
                height = screen_height;
            } else {
                switch (tilingMode)
                {
                default:
                case VerticalTilling:
                    {
                        if (i < masterarea_count) {
                            x = 0;
                            y = (screen_height / masterarea_count) * i;
                            width = (screen_width / 2) + margin;
                            height = (screen_height / masterarea_count);
                        } else {
                            x = (screen_width / 2) + margin;
                            y = (screen_height / ((nodeCount + 1) - masterarea_count)) * (nodeCount - i);
                            width = (screen_width / 2) - margin;
                            height = (screen_height / ((nodeCount + 1) - masterarea_count));
                        }
                    }
                    break;
                case HorizontalTilling:
                    {
                        if (i < masterarea_count) {
                            /* Main window */
                            x = (screen_width / masterarea_count) * i;
                            y = 0;
                            width = (screen_width / masterarea_count);
                            height = (screen_height / 2) + margin;
                        } else {
                            /* Normal windows to be tiled */
                            x = (screen_width / ((nodeCount + 1) - masterarea_count)) * (nodeCount - i);
                            y = (screen_height / 2) + margin;
                            width = (screen_width / ((nodeCount + 1) - masterarea_count));
                            height = (screen_height / 2) - margin;
                        }
                    }
                    break;
                case GridTilling:
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
                    }
                    break;

                case FullScreenTilling:
                    x = 0;
                    y = 0;
                    width = screen_width;
                    height = screen_height;
                    break;
                }
            }
            SetWindowPos(temp->hwnd, HWND_TOP, x + screen_x, y + screen_y, width, height, SWP_SHOWWINDOW);
            i++;
        } else {
            ShowWindow(temp->hwnd, SW_MINIMIZE);
        }
    }
    FocusCurrent();
}


BOOL ViWm::monitorEnumerator( HMONITOR hMonitor
                            , HDC hdcMonitor
                            , LPRECT intersectionRect
                            , LPARAM userData
                            )
{
    DesktopLayout   &myLayout = *reinterpret_cast<DesktopLayout*>( userData );
    MONITORINFOEX   minfo;

    minfo.cbSize = sizeof( MONITORINFOEX );
    GetMonitorInfo( hMonitor, &minfo );

    // as MSDN state that some value may be negative for non-primary
    // displays, we abs the width & height to get correct value to
    // work on.
    Screen          newScreen( minfo.rcWork.left
                             , minfo.rcWork.top
                             , abs(minfo.rcWork.right - minfo.rcWork.left)
                             , abs(minfor.rcWork.bottom - minfo.rcWork.top)
                             );

    myLayout.displays.push_back( newScreen );

    return TRUE;
}