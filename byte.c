#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>



struct termios orig_termios; // Global variable to store the original terminal attributes

//--- Disable raw mode ---//
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // Discards any unread input before applying the changes to the terminal
}

//--- Turning OFF echo mode ---// 
void enableRawMode() {

    // --- Disable raw mode when exit() is called or returning from main() ---//
    tcgetattr(STDIN_FILENO, &orig_termios); // Read current attributes into struct orig_termios
    atexit(disableRawMode); // atexit() is a function that registers disableRawMode() to be called automatically when the program exits

    struct termios raw = orig_termios; // Assign a copy of the original attributes to raw
    raw.c_lflag &= ~(ECHO | ICANON); // Modify the local flags in the struct raw. ECHO causes each key typed to be printed to the terminal. ICANON disables canonical mode


    // --- Disabling echo mode ---//
    tcgetattr(STDIN_FILENO, &raw); // Read current attributes into struct raw
    raw.c_lflag &= ~(ECHO); // Modify the local flags in the struct raw. ECHO causes each key typed to be printed to the terminal
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);  // Pass the modified attributes to the terminal to 'tcsetter' to enable raw mode 
}

int main() {

    enableRawMode(); // Disabling echo mode
    
    //--- Reading keypress from user ---//
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') // Read 1 byte from stadard input into c & keep doing that until there are no more bytes to read
    return 0;
}