//-----------------------------------------------------------------------------
//  HELLO_DLL.C - Windows DLL example - Main Application Entry Point
//-----------------------------------------------------------------------------

#include <windows.h>
#include "../include/obfus.h"

// Function prototype for DLL function
void hello_func(void);

// Import external constant defined in the DLL
__declspec(dllimport) extern const char *hello_data;

// Entry point for the Windows application
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    // Set the shared DLL data to a greeting message
    hello_data = "Hello World!";

    // Call the DLL's function to display or process the message
    hello_func();

    // Exit the application
    return 0;
}
