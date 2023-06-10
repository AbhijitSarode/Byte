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
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // Modify the local flags in the struct raw. ECHO causes each key typed to be printed to the terminal. ICANON disables canonical mode. ISIG disables Ctrl-C and Ctrl-Z signals. Ctrl-C sends a SIGINT signal to the program, which causes it to terminate, and Ctrl-Z sends a SIGTSTP signal to the program, which causes it to suspend execution. IEXTEN disables Ctrl-V and Ctrl-O signals. Ctrl-V causes the next key you press to be literally inserted into the program input. Ctrl-O restarts output that has been stopped using Ctrl-S
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP); // IXON disables Ctrl-S and Ctrl-Q signals. Ctrl-S stops data from being transmitted to the terminal until Ctrl-Q is pressed. ICRNL disables Ctrl-M, which stands for carriage return, or enter key. When disabled, pressing enter will not translate to a 13 (carriage return) followed by a 10 (line feed). BRKINT disables Ctrl-C and Ctrl-Z signals. INPCK enables parity checking. ISTRIP sets the 8th bit of each input byte to 0.
    raw.c_oflag &= ~(OPOST); // OPOST disables all output processing features. This includes translating '\n' to '\r\n'
    raw.c_cflag |= (CS8); // CS8 sets the character size (CS) to 8 bits per byte


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
    {
        if (iscntrl(c)) // iscntrl() tests whether a character is a control character
        {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        
    }
    return 0;
}