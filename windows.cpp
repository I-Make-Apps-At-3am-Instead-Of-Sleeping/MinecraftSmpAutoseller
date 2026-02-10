#include <windows.h>
#include <stdio.h>

bool running = true;
bool paused = false;
bool rshiftWasDown = false; // tracking key state for toggling

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
        "press yes to start the fucking macro\npress no if you accidentally clicked this\n(press escape to end the fucking process, idiot)",
        "autoseller",
        MB_YESNO | MB_ICONQUESTION
    );

    if (start != IDYES) {
        return 0;
    }

    printf("Macro started. Press ESC to stop. Press Right Shift to pause/unpause.\n");

    while (running) {

        // ========= STOP ON ESC =========
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("Macro stopped.\n");
            break;
        }

        // ========= PAUSE / UNPAUSE TOGGLE =========
        bool rshiftDown = (GetAsyncKeyState(VK_RSHIFT) & 0x8000);

        // Toggle ONLY once per key press
        if (rshiftDown && !rshiftWasDown) {
            paused = !paused;
            printf(paused ? "Paused.\n" : "Unpaused.\n");
            Sleep(120); // small debounce to avoid flickering
        }

        rshiftWasDown = rshiftDown;

        // If paused, skip all macro actions
        if (paused) {
            Sleep(50);
            continue;
        }

        // ========= MACRO ACTIONS =========
        tap('T');          // open chat
        Sleep(150);

        pressKey(VK_CONTROL, 0);
        tap('V');          // paste
        pressKey(VK_CONTROL, KEYEVENTF_KEYUP);
        Sleep(150);

        tap(VK_RETURN);    // enter
        

        // ========= DELAY: 1.5 seconds =========
        for (int i = 0; i < 30; i++) {   // 30 × 50ms = 1500ms

            // ESC kills instantly
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                printf("Macro stopped.\n");
                return 0;
            }

            // Pause toggle still works during delay
            bool rdown = (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
            if (rdown && !rshiftWasDown) {
                paused = !paused;
                printf(paused ? "Paused.\n" : "Unpaused.\n");
                Sleep(120);
                break; // break delay early so macro respects pause instantly
            }
            rshiftWasDown = rdown;

            Sleep(50);
        }
    }

    return 0;
}
