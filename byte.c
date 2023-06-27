/*** Includes ***/
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

/*** Defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // k & 00011111. mimics CTRL + k in the terminal


/*** Data ***/
struct termios orig_termios; // Global variable to store the original terminal attributes


/*** Terminal ***/
// --- Print error & exit ---//
void die (const char *s) {

    // --- Clear screen & reposition cursor on exit ---//
    write(STDOUT_FILENO, "\x1b[2J", 4); 
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s); // perror() looks at the global errno variable and prints a descriptive error message for it
    exit(1);
}

//--- Disable raw mode ---//
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1); // Discards any unread input before applying the changes to the terminal
    {
        die("tcsetattr");
    }
}

//--- Turning OFF echo mode ---// 
void enableRawMode() {

    // --- Disable raw mode when exit() is called or returning from main() ---//
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr"); // Read current attributes into struct orig_termios
    atexit(disableRawMode); // atexit() is a function that registers disableRawMode() to be called automatically when the program exits

    struct termios raw = orig_termios; // Assign a copy of the original attributes to raw
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // Modify the local flags in the struct raw. ECHO causes each key typed to be printed to the terminal. ICANON disables canonical mode. ISIG disables Ctrl-C and Ctrl-Z signals. Ctrl-C sends a SIGINT signal to the program, which causes it to terminate, and Ctrl-Z sends a SIGTSTP signal to the program, which causes it to suspend execution. IEXTEN disables Ctrl-V and Ctrl-O signals. Ctrl-V causes the next key you press to be literally inserted into the program input. Ctrl-O restarts output that has been stopped using Ctrl-S
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP); // IXON disables Ctrl-S and Ctrl-Q signals. Ctrl-S stops data from being transmitted to the terminal until Ctrl-Q is pressed. ICRNL disables Ctrl-M, which stands for carriage return, or enter key. When disabled, pressing enter will not translate to a 13 (carriage return) followed by a 10 (line feed). BRKINT disables Ctrl-C and Ctrl-Z signals. INPCK enables parity checking. ISTRIP sets the 8th bit of each input byte to 0.
    raw.c_oflag &= ~(OPOST); // OPOST disables all output processing features. This includes translating '\n' to '\r\n'
    raw.c_cflag |= (CS8); // CS8 sets the character size (CS) to 8 bits per byte
    raw.c_cc[VMIN] = 0; // VMIN sets the minimum number of bytes of input needed before read() can return. Set to 0 so that read() returns as soon as there is any input to be read
    raw.c_cc[VTIME] = 1; // VTIME sets the maximum amount of time to wait before read() returns. Set to 1/10 of a second (100 milliseconds) so that read() returns as soon as there is any input to be read

    // --- Disabling echo mode ---//
    tcgetattr(STDIN_FILENO, &raw); // Read current attributes into struct raw
    raw.c_lflag &= ~(ECHO); // Modify the local flags in the struct raw. ECHO causes each key typed to be printed to the terminal
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");  // Pass the modified attributes to the terminal to 'tcsetter' to enable raw mode 
}

//--- Read keypress & wait ---//
char editorKeyRead() {
    int nread;
    char c;
    while (nread = read(STDIN_FILENO, &c, 1) != 1) // read() reads 1 byte from the standard input into the variable c
    {
        if (nread == -1 && errno != EAGAIN) die("read"); // read() returns the number of bytes read and -1 if there is an error. EAGAIN means there is no data to read at the moment.
    }
    return c;
}


/*** Output ***/

// --- Draw tildes at the beginning of the line ---//
void editorDrawRows() {
    int y;
    for (int y = 0; y < 24; y++) // Since size of the terminal is not known yet, therefore hardcoded 24 rows
    {
        write(STDOUT_FILENO, "~\r\n", 3); // \r is carriage return. \n is line feed. \r\n is a Windows line ending
    }
    
}


// --- Refresh the screen ---//
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4); // \x1b is the escape character. [2J is the escape sequence to clear the entire screen & it takes 4 bytes to write
    write(STDOUT_FILENO, "\x1b[H", 3); // [H is the escape sequence to position the cursor at the top-left corner of the screen & it takes 3 bytes to write

    editorDrawRows(); // Draw tildes at the beginning of the line
    write(STDOUT_FILENO, "\x1b[H", 3); // [H is the escape sequence to position the cursor at the top-left corner of the screen & it takes 3 bytes to write
}


/*** Input ***/

//--- Waits for keypress & then handles it---//
void editorProcessKeypress() {
    char c = editorKeyRead();

    switch (c)
    {
    case CTRL_KEY('q'):
        // --- Clear screen & reposition cursor on exit ---//
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);

        exit(0);
        break;
    
    default:
        break;
    }
}

/*** Init ***/
int main() {

    enableRawMode(); // Disabling echo mode

    while (1)
    {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    // while (1)
    // {
    //     char c = '\0';

    //     read(STDIN_FILENO, &c, 1); // read() reads 1 byte from the standard input into the variable c
    //     if (read(STDIN_FILENO, &c, 1) == -1 & errno != EAGAIN) die("read"); // read() returns the number of bytes read and -1 if there is an error. EAGAIN means there is no data to read at the moment.
    //     if (iscntrl(c)) // iscntrl() tests whether a character is a control character
    //     {
    //         printf("%d\r\n", c);
    //     } else {
    //         printf("%d ('%c')\r\n", c, c);
    //     }
    //     if (c == CTRL_KEY('q')) break;  // When Ctrl-Q is pressed, break out of the while loop.
    // }
    
    
    //--- Reading keypress from user ---//
    // while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') // Read 1 byte from stadard input into c & keep doing that until there are no more bytes to read
    // {
        
    // }
    return 0;
}