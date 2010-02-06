/* Windows defines and includes */
//#define WIN32_LEAN_AND_MEAN
//#define _WIN32_WINNT		0x0500
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <shellapi.h> 	/* For CommandLineToArgvW */

/* Shell Hook stuff */
typedef BOOL (*RegisterShellHookWindowProc) (HWND);
RegisterShellHookWindowProc hookProc;

UINT shellhookid;	/* Window Message id */

/* List Methods */
node* FindNode(HWND hwnd)
{
    node *temp = nodes;
    for (temp = (node*)nodes; temp; temp = (node*)temp->next) {
        if (temp->hwnd == hwnd) {
            return temp;
        }
    }
    return NULL;
}

void SwapWindowWithNode(node *window)
{
    if (current && window) {
        AddTransparency(window->hwnd);
        HWND temp = window->hwnd;
        window->hwnd = current->hwnd;
        current->hwnd = temp;
        current = window;
    }
}

void FocusCurrent()
{
    if (current) {
        SetForegroundWindow(current->hwnd);
        if (lockMouse) {
            RECT window;
            GetWindowRect(current->hwnd, &window);
            ClipCursor(&window);
            SetCursorPos(window.left + (window.right - window.left) / 2
                        , window.top + (window.bottom - window.top) / 2);
        }
        RemoveTransparency(current->hwnd);
    }
}

node* GetFirstNode()
{
    node *temp, *first = nodes;
    for (temp = (node*)nodes; temp; temp = (node*)temp->next)
    {
        if (temp->tags == current_tags) {
            return temp;
        }
        if (temp == first) return current;
    }
    return 0;
}

/*
* Returns the previous Node with the same tag as current
*/
node* GetPreviousNode()
{
    node *temp, *first = current;
    for (temp = (node*)current->prev; temp; temp = (node*)temp->prev) {
        if (temp->tags == current_tags) {
            return temp;
        }
        if (temp == first) return temp;
    }
    return 0;
}

/*
* Returns the next Node with the same tag as current
*/
node* GetNextNode()
{
    node *temp, *first = current;
    for (temp = current; temp;) {
        if (temp->next) { temp = (node*)temp->next; }
        else
        {
            temp = (node*)nodes;
        }

        if (temp->tags == current_tags) {
            return temp;
        }
        if (temp == first) return temp;
    }
    return 0;
}

/*
* Returns the number of Nodes with the same tag as current
*/
int CountNodes()
{
    node *temp = nodes;
    int i = 0;
    for (temp = (node*)nodes; temp; temp = (node*)temp->next) {
        if (temp->tags == current_tags)
            i++;
    }
    return i - 1;
}

void UpdateMousePos(HWND hwnd)
{
    POINT cursor;
    GetCursorPos(&cursor);
    if ((cursor.x < screen_x) || (cursor.x > screen_x + screen_width) || (cursor.y < screen_y) || (cursor.y > screen_y + screen_height)) {
        if (mouse_pos_out == 0) {
            mouse_pos_out = 1;
            UnregisterHotkeys(hwnd);
        }
    } else {
        if (mouse_pos_out == 1) {
            mouse_pos_out = 0;
            Sleep(500);
            RegisterHotkeys(hwnd);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        SetTimer(hwnd, 1, MemoryTrimmDelay, NULL);				/* Trim memory once a minute */
        if (experimental_mouse) {
            SetTimer(hwnd, 2, MousePollDelay, NULL); 			/* Poll for mouse position */
        }
        SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);	/* Trim memory now */
        break;
    case WM_CLOSE:
        {
            ClipCursor(0); 	/* Release Cursor Lock */
            DeregisterShellHookWindow(hwnd);
            UnregisterHotkeys(hwnd);
            KillTimer(hwnd, 1); 	/* Memory Trim Timer */
            if (experimental_mouse) {
                KillTimer(hwnd, 2); 	/* Mouse Poll Timer */
            }
            for (current = nodes; current;) { 	/* Add window borders, reset opacity and remove */
                node *temp = NULL;
                ShowWindow(current->hwnd, SW_RESTORE);
                AddBorder(current->hwnd);
                RemoveTransparency(current->hwnd);
                temp = (node*)current->next;
                free(current);
                current = temp;
            }
            DestroyWindow(hwnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;

    case WM_HOTKEY:
        // TODO : link to ViWm
        break;

    case WM_TIMER: 		/* Free Memory */
        switch (wParam)
        {
        case 1:
            SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
            break;
        case 2:
            UpdateMousePos(hwnd);
            break;
        }
        break;
    default:
        if (msg == shellhookid)
        {	/* Handle the Shell Hook message */
            switch (wParam)
            {
            case HSHELL_WINDOWCREATED:
                if (IsGoodWindow((HWND)lParam))
                {
                    AddNode((HWND)lParam);
                    ArrangeWindows();
                    FocusCurrent();
                }
                break;

            case HSHELL_WINDOWDESTROYED:
                RemoveNode((HWND)lParam);
                ArrangeWindows();
                FocusCurrent();
                break;

            case HSHELL_WINDOWACTIVATED:
                {
                    node *found = FindNode((HWND)lParam);
                    if (found) {
                        if (current) {
                            AddTransparency(current->hwnd);
                        }
                        current = found;
                        FocusCurrent();
                    }
                }
                break;
            }
        } else {
            /* We aren't handling this message so return DefWindowProc */
            return DefWindowProc(hwnd, msg, wParam, lParam); 
        }
    }
    return 0;
}

/*
*	BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
*
*	Add desktop windows to the grid to be tiled
*/
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    if (IsGoodWindow(hwnd)) { AddNode(hwnd); }
    return TRUE;
}

BOOL CALLBACK EnumWindowsRestore(HWND hwnd, LPARAM lParam)
{
    if (IsGoodWindow(hwnd)) { ShowWindow(hwnd, SW_RESTORE); }
    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASSEX winClass;
    HWND hwnd;
    MSG msg;

    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = 0;
    winClass.lpfnWndProc = WndProc;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = hInstance;
    winClass.hIcon = NULL;
    winClass.hIconSm = NULL;
    winClass.hCursor = NULL;
    winClass.hbrBackground = NULL;
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = NAME;

    if (!RegisterClassEx(&winClass)) {
        MessageBox(NULL, "Error Registering Window Class", "Error", MB_OK | MB_ICONERROR);
        return 0; /* Bail */
    }

    hwnd = CreateWindowEx(0, NAME, NAME, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, "Error Creating Window", "Error", MB_OK | MB_ICONERROR);
        return 0; /* Bail */
    }

    RegisterHotkeys(hwnd);
    UpdateMousePos(hwnd);

    EnumWindows(EnumWindowsRestore, 0); /* Restore windows on startup so they get tiled */
    EnumWindows(EnumWindowsProc, 0);

    ArrangeWindows();

    /* Get function pointer for RegisterShellHookWindow */
    hookProc(hwnd);

     /* Grab a dynamic id for the SHELLHOOK message to be used later */
    shellhookid = RegisterWindowMessage("ViWmSHELLHOOK"); 

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
