
#include <defs.h>


//-------------------------------------------------------------------------
// Function declarations

int sub_401000();
void __noreturn start();
int __fastcall sub_4011C3(__int64 a1, __int64 a2);
// void *__cdecl malloc(size_t Size);
// char *__cdecl strcpy(char *Destination, const char *Source);
// int sprintf(char *const Buffer, const char *const Format, ...);
// int printf(const char *const Format, ...);
// void __cdecl free(void *Block);
// int __cdecl getch();
// void __cdecl _set_app_type(_crt_app_type Type);
// unsigned int __cdecl controlfp(unsigned int NewValue, unsigned int Mask);
// __int64 __fastcall _getmainargs(_QWORD, _QWORD, _QWORD, _QWORD, _QWORD); weak
// void __cdecl __noreturn exit(int Code);

//-------------------------------------------------------------------------
// Data declarations

char Source[] = "%d) Hello, world!\n"; // idb
// extern int _argc;
// extern char **_argv;
// extern char **environ;
int dword_4021C0 = 0; // weak


//----- (0000000000401000) ----------------------------------------------------
int sub_401000()
{
  char *Destination; // [rsp+20h] [rbp-10h]
  int i; // [rsp+2Ch] [rbp-4h]

  for ( i = 0; i < 5; ++i )
  {
    Destination = (char *)(int)malloc(0x100ui64);
    strcpy(Destination, Source);
    sprintf(Destination, Destination, (unsigned int)(i + 1));
    if ( Destination )
      printf(Destination);
    else
      printf("Error!");
    free(Destination);
  }
  return getch();
}

//----- (00000000004010E8) ----------------------------------------------------
void __noreturn start()
{
  int v0; // eax
  int v1; // [rsp+4Ch] [rbp-4h] BYREF

  v1 = 0;
  _set_app_type(_crt_console_app);
  controlfp(0x10000u, 0x30000u);
  _getmainargs(*(_QWORD *)&_argc, _argv, environ, (unsigned int)dword_4021C0, &v1);
  v0 = sub_401000();
  exit(v0);
}
// 401280: using guessed type __int64 __fastcall _getmainargs(_QWORD, _QWORD, _QWORD, _QWORD, _QWORD);
// 4021C0: using guessed type int dword_4021C0;

//----- (00000000004011C3) ----------------------------------------------------
// write access to const memory has been detected, the output may be wrong!
int __fastcall sub_4011C3(__int64 a1, __int64 a2)
{
  __int64 v2; // rbp

  *(_QWORD *)(v2 + 16) = a1;
  *(_QWORD *)(v2 + 24) = a2;
  _argc = *(_DWORD *)(v2 + 16);
  _argv = *(char ***)(v2 + 24);
  controlfp(0x10000u, 0x30000u);
  *(_QWORD *)(v2 - 8) = *(_QWORD *)&_argc;
  return sub_401000();
}
// 4011D5: write access to const memory at 402088 has been detected
// 4011E2: write access to const memory at 402090 has been detected
// 4011C3: variable 'v2' is possibly undefined

// nfuncs=13 queued=3 decompiled=3 lumina nreq=0 worse=0 better=0
// ALL OK, 3 function(s) have been successfully decompiled
