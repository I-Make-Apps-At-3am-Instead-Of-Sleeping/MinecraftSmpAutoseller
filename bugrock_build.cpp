#include <windows.h>
#include <stdio.h>

bool paused = false;
bool rshiftWasDown = false;

void pressKey(WORD vk, DWORD flags) {
    INPUT in = {0};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.dwFlags = flags;
    SendInput(1, &in, sizeof(INPUT));
}

void tap(WORD vk) {
    pressKey(vk, 0);
    Sleep(30);
    pressKey(vk, KEYEVENTF_KEYUP);
    Sleep(30);
}

int main() {

    int start = MessageBoxA(
        NULL,
        "press yes to start the fucking paste macro\npress no if you clicked this stupid thing by accident\n(escape kills it instantly, right shift pauses)",
        "autoseller-pasteonly",
        MB_YESNO | MB_ICONQUESTION
    );

    if (start != IDYES) return 0;

    printf("Paste-only macro started.\nESC = stop.\nRight Shift = pause.\n");

    while (true) {

        // ESC kills everything
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("Stopped.\n");
            break;
        }

        // Pause logic — one toggle per key press
        bool rshiftDown = (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
        if (rshiftDown && !rshiftWasDown) {
            paused = !paused;
            printf(paused ? "Paused.\n" : "Unpaused.\n");
            Sleep(120);
        }
        rshiftWasDown = rshiftDown;

        if (paused) {
            Sleep(50);
            continue;
        }

        // === MACRO ACTION: Ctrl+V, Enter ===
        pressKey(VK_CONTROL, 0);
        tap('V');
        pressKey(VK_CONTROL, KEYEVENTF_KEYUP);
        Sleep(100);

        tap(VK_RETURN);

        // Delay 1.5s with live ESC + pause
        for (int i = 0; i < 30; i++) {  
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return 0;

            bool down = (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
            if (down && !rshiftWasDown) {
                paused = !paused;
                printf(paused ? "Paused.\n" : "Unpaused.\n");
                Sleep(120);
                break;
            }
            rshiftWasDown = down;

            Sleep(50);
        }
    }

    return 0;
}