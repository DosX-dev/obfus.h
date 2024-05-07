
#include <defs.h>


//-------------------------------------------------------------------------
// Function declarations

int sub_401000();
void __noreturn start(); // weak
// int __usercall sub_4011E0@<eax>(int a1@<ebp>);
// int __usercall sub_4011E4@<eax>(int a1@<ebp>);
int __cdecl sub_401225(_DWORD *a1, char a2);
// void *__cdecl malloc(size_t Size);
// char *__cdecl strcpy(char *Destination, const char *Source);
// int sprintf(char *const Buffer, const char *const Format, ...);
// int printf(const char *const Format, ...);
// void __cdecl free(void *Block);
// int __cdecl getch();
// void __cdecl _set_app_type(_crt_app_type Type);
// unsigned int __cdecl controlfp(unsigned int NewValue, unsigned int Mask);
// int __cdecl _getmainargs(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD); weak
// void __cdecl __noreturn exit(int Code);

//-------------------------------------------------------------------------
// Data declarations

int dword_401214[3] = { -1, 4198894, 4198915 }; // weak
_UNKNOWN j__except_handler3; // weak
char Source[] = "%d) Hello, world!\n"; // idb
// extern int _argc;
// extern char **_argv;
// extern char **environ;
int dword_40218C = 0; // weak


//----- (00401000) --------------------------------------------------------
int sub_401000()
{
  char *Destination; // [esp+0h] [ebp-8h]
  int i; // [esp+4h] [ebp-4h]

  for ( i = 0; i < 5; ++i )
  {
    Destination = (char *)malloc(0x100u);
    strcpy(Destination, Source);
    sprintf(Destination, Destination, i + 1);
    if ( Destination )
      printf(Destination);
    else
      printf("Error!");
    free(Destination);
  }
  return getch();
}

//----- (004010B0) --------------------------------------------------------
void __noreturn start()
{
  int v0; // eax
  int v1; // [esp+8h] [ebp-1Ch] BYREF
  char v2[24]; // [esp+Ch] [ebp-18h] BYREF

  sub_401225(v2);
  v1 = 0;
  _set_app_type(_crt_console_app);
  controlfp(0x10000u, 0x30000u);
  _getmainargs(_argc, _argv, environ, dword_40218C, &v1);
  v0 = sub_401000(_argc, _argv, environ);
  exit(v0);
}
// 401000: using guessed type int __cdecl sub_401000(_DWORD, _DWORD, _DWORD);
// 4010B0: using guessed type void __noreturn start();
// 4012A0: using guessed type int __cdecl _getmainargs(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 40218C: using guessed type int dword_40218C;
// 4010B0: using guessed type char var_18[24];

//----- (004011E0) --------------------------------------------------------
int __usercall sub_4011E0@<eax>(int a1@<ebp>)
{
  return *(_DWORD *)(a1 - 20);
}

//----- (004011E4) --------------------------------------------------------
int __usercall sub_4011E4@<eax>(int a1@<ebp>)
{
  return **(_DWORD **)sub_4011E0(a1);
}

//----- (00401225) --------------------------------------------------------
int __cdecl sub_401225(_DWORD *a1, char a2)
{
  int result; // eax

  *a1 = &a2;
  a1[1] = 0;
  a1[2] = NtCurrentTeb()->NtTib.ExceptionList;
  a1[3] = &sub_401220;
  a1[4] = dword_401214;
  result = 0;
  a1[5] = 0;
  return result;
}
// 401214: using guessed type int dword_401214[3];

// nfuncs=19 queued=5 decompiled=5 lumina nreq=0 worse=0 better=0
// ALL OK, 5 function(s) have been successfully decompiled
