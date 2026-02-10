#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

bool running = true;
bool paused = false;

Display* display;

void pressKey(KeySym keysym, bool press) {
    KeyCode keycode = XKeysymToKeycode(display, keysym);
    if (keycode == 0) return;

    XTestFakeKeyEvent(display, keycode, press, CurrentTime);
    XFlush(display);
}

void tap(KeySym keysym) {
    pressKey(keysym, true);
    usleep(30000);
    pressKey(keysym, false);
    usleep(30000);
}

bool keyDown(KeySym keysym) {
    char keys[32];
    XQueryKeymap(display, keys);

    KeyCode kc = XKeysymToKeycode(display, keysym);
    return keys[kc / 8] & (1 << (kc % 8));
}

int main() {
    display = XOpenDisplay(nullptr);
    if (!display) {
        printf("Failed to open X display. Are you even running X?\n");
        return 1;
    }

    printf(
        "press enter to start the fucking macro\n"
        "ESC = stop\n"
        "Right Shift = pause/unpause\n"
    );

    getchar();

    printf("Macro started.\n");

    while (running) {

        // STOP with ESC
        if (keyDown(XK_Escape)) {
            printf("Macro stopped.\n");
            break;
        }

        // TOGGLE pause with Right Shift
        if (keyDown(XK_Shift_R)) {
            paused = !paused;
            printf(paused ? "Paused.\n" : "Unpaused.\n");
            usleep(300000);
        }

        if (paused) {
            usleep(50000);
            continue;
        }

        // === MACRO ACTIONS ===
        tap(XK_T);
        usleep(150000);

        pressKey(XK_Control_L, true);
        tap(XK_V);
        pressKey(XK_Control_L, false);
        usleep(150000);

        tap(XK_Return);

        // WAIT ~1.5 seconds with checks
        for (int i = 0; i < 30; i++) {

            if (keyDown(XK_Escape)) {
                printf("Macro stopped.\n");
                XCloseDisplay(display);
                return 0;
            }

            if (keyDown(XK_Shift_R)) {
                paused = !paused;
                printf(paused ? "Paused.\n" : "Unpaused.\n");
                usleep(300000);
                break;
            }

            usleep(50000);
        }
    }

    XCloseDisplay(display);
    return 0;
}
