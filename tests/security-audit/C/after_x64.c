
#include <windows.h>
#include <defs.h>

#include <stdarg.h>


//-------------------------------------------------------------------------
// Function declarations

void sub_401000();
__int64 __fastcall sub_401028(int a1);
double __fastcall sub_401085(double a1);
_BOOL8 __fastcall sub_4010D2();
__int64 __fastcall sub_40112B(int a1, int a2);
void *__fastcall sub_40130B(int a1);
BOOL __fastcall sub_4015B4(void *a1, const void *a2, DWORD a3, DWORD *a4, LPVOID lpReserved);
HANDLE __fastcall sub_401604(int a1);
FARPROC __fastcall sub_401632(HMODULE a1, const CHAR *a2);
HMODULE __fastcall sub_401664(const CHAR *a1);
__int64 __fastcall sub_401688(_BYTE *a1, _BYTE *a2);
__int64 __fastcall sub_401F47(char *a1);
__int64 __fastcall sub_402050(__int64 a1);
__int64 __fastcall sub_402460(__int64 a1);
__int64 __fastcall sub_402480(__int64 a1);
__int64 __fastcall sub_4024A0(__int64 a1);
__int64 __fastcall sub_4024C0(__int64 a1);
__int64 __fastcall sub_4024E0(__int64 a1);
__int64 __fastcall sub_402500(__int64 a1);
BOOL sub_402520();
void sub_40274C();
_BYTE *sub_40275A();
_BYTE *sub_403268();
_BYTE *sub_40327A();
_BYTE *sub_40328C();
_BYTE *sub_40329E();
_BYTE *sub_4032B0();
_BYTE *sub_4032C2();
_BYTE *sub_4032D4();
_BYTE *sub_4032E6();
_BYTE *sub_4032F8();
_BYTE *sub_40330A();
_BYTE *sub_40331C();
_BYTE *sub_40332E();
_BYTE *sub_403340();
_BYTE *sub_403352();
_BYTE *sub_403364();
_BYTE *sub_403376();
_BYTE *sub_403388();
BOOL sub_40339A(__int64 a1, const char *a2, ...);
char *__fastcall sub_40362C();
char *__fastcall sub_403644();
char *__fastcall sub_40365C();
char *__fastcall sub_403674();
char *__fastcall sub_40368C();
char *__fastcall sub_4036A4();
char *__fastcall sub_4036BC();
char *__fastcall sub_4036D4();
char *__fastcall sub_4036EC();
void *__fastcall sub_403704(void *a1, int a2, size_t a3);
char *__fastcall sub_403746();
char *__fastcall sub_40375E();
char *__fastcall sub_403776();
char *__fastcall sub_40378E();
char *__fastcall sub_4037A6();
void *__fastcall sub_4037BE(size_t a1, size_t a2);
__int64 __fastcall sub_4037EC(__int64 a1, __int64 a2);
__int64 __fastcall sub_40384E(__int64 a1);
__int64 sub_40386E(char *a1, size_t a2, const char *a3, ...);
int __fastcall sub_4038C0(char *a1, const char *a2, va_list a3);
int __fastcall sub_4038F9(char *a1, size_t a2, const char *a3, va_list a4);
char *__fastcall sub_40393D(const char *a1);
int __fastcall sub_40395D(const char *a1);
void __noreturn sub_40397D();
int __fastcall sub_40398F(void (__cdecl *a1)());
__int64 __fastcall sub_4039AF(char *a1, int a2);
int __fastcall sub_4039E0(int a1);
int __fastcall sub_4039FF(int a1);
__int64 sub_403A1E();
int __fastcall sub_404303(char *a1, size_t a2, const char *a3, va_list a4);
void __noreturn start();
__int64 __fastcall sub_404423(__int64 a1, __int64 a2);
// BOOL __stdcall WriteConsoleA(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved);
// HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
// FARPROC __stdcall GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
// HMODULE __stdcall GetModuleHandleA(LPCSTR lpModuleName);
// char *__cdecl strcat(char *Destination, const char *Source);
// int sprintf(char *const Buffer, const char *const Format, ...);
// BOOL __stdcall IsDebuggerPresent();
// void *__cdecl malloc(size_t Size);
// void *__cdecl memset(void *, int Val, size_t Size);
// void *__cdecl calloc(size_t Count, size_t Size);
// __int64 __fastcall gets(_QWORD); weak
// int __cdecl vsprintf(char *const Buffer, const char *const Format, va_list ArgList);
// char *__cdecl getenv(const char *VarName);
// int __cdecl system(const char *Command);
// void __cdecl __noreturn abort();
// int __cdecl atexit(void (__cdecl *)());
// char *__cdecl getcwd(char *DstBuf, int SizeInBytes);
// int __cdecl tolower(int C);
// int __cdecl toupper(int C);
// void __cdecl free(void *Block);
// int __cdecl getch();
// int __cdecl vsnprintf(char *const Buffer, const size_t BufferCount, const char *const Format, va_list ArgList);
// void __cdecl _set_app_type(_crt_app_type Type);
// unsigned int __cdecl controlfp(unsigned int NewValue, unsigned int Mask);
// __int64 __fastcall _getmainargs(_QWORD, _QWORD, _QWORD, _QWORD, _QWORD); weak
// void __cdecl __noreturn exit(int Code);

//-------------------------------------------------------------------------
// Data declarations

char byte_405034 = 'a'; // weak
char byte_405036 = 'c'; // weak
char byte_405037 = 'd'; // weak
char byte_405038 = 'e'; // weak
char byte_40503E = 'k'; // weak
char byte_40503F = 'l'; // weak
char byte_405040 = 'm'; // weak
char byte_405041 = 'n'; // weak
char byte_405042 = 'o'; // weak
char byte_405045 = 'r'; // weak
char byte_405046 = 's'; // weak
char byte_405047 = 't'; // weak
char byte_405049 = 'v'; // weak
char byte_40504F = 'L'; // weak
char byte_405050 = 'A'; // weak
char byte_405054 = '\0'; // weak
char byte_405055 = '\x01'; // weak
char byte_405056 = '\x02'; // weak
char byte_405057 = '\x03'; // weak
char byte_405058 = '\x04'; // weak
char byte_405059 = '\x05'; // weak
char byte_40505A = '\x06'; // weak
char byte_40505B = '\a'; // weak
char byte_40505C = '\b'; // weak
char byte_40505D = '\t'; // weak
__int64 (__fastcall *qword_405060)(_QWORD) = NULL; // weak
char Source[] = "%d%d"; // idb
char Destination[] = "Library"; // idb
char aC[] = "%c"; // idb
char aScanf[6] = "scanf"; // weak
char aSprintf[8] = "sprintf"; // weak
char aFclose[7] = "fclose"; // weak
char aFopen[6] = "fopen"; // weak
char aFread[6] = "fread"; // weak
char aFwrite[7] = "fwrite"; // weak
char aExit[5] = "exit"; // weak
char aStrcpy[7] = "strcpy"; // weak
char aStrtok[7] = "strtok"; // weak
char aMemcpy[7] = "memcpy"; // weak
char aStrchr[7] = "strchr"; // weak
char aStrrchr[8] = "strrchr"; // weak
char aRand[5] = "rand"; // weak
char aRealloc[8] = "realloc"; // weak
char aDHelloWorld[19] = "%d) Hello, world!\n"; // weak
char aError[7] = "Error!"; // weak
// extern int _argc;
// extern char **_argv;
// extern char **environ;
_UNKNOWN unk_405490; // weak
char byte_4054A0[8] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' }; // idb
int dword_4054A8 = 0; // weak


//----- (0000000000401000) ----------------------------------------------------
void sub_401000()
{
  ;
}

//----- (0000000000401028) ----------------------------------------------------
__int64 __fastcall sub_401028(int a1)
{
  sub_401000();
  __asm { cpuid }
  return (unsigned int)(byte_405056 * byte_405058 + byte_405055 * a1 - byte_40505C);
}
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405058: using guessed type char byte_405058;
// 40505C: using guessed type char byte_40505C;

//----- (0000000000401085) ----------------------------------------------------
double __fastcall sub_401085(double a1)
{
  sub_401000();
  __asm { cpuid }
  return a1 * (double)byte_405055;
}
// 405055: using guessed type char byte_405055;

//----- (00000000004010D2) ----------------------------------------------------
_BOOL8 __fastcall sub_4010D2()
{
  __asm { cpuid }
  return byte_405055 && (byte_40505B + byte_40505D) / byte_40505C != byte_405055;
}
// 405055: using guessed type char byte_405055;
// 40505B: using guessed type char byte_40505B;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (000000000040112B) ----------------------------------------------------
__int64 __fastcall sub_40112B(int a1, int a2)
{
  int v8; // [rsp+20h] [rbp-10h]
  int v9; // [rsp+2Ch] [rbp-4h]

  v9 = sub_401028(byte_405055 * a2);
  if ( v9 == a1 * (3 * byte_405054 + byte_40505A + byte_405057 - byte_40505D) )
    return byte_40505C != byte_405056 * byte_405058 && !sub_4010D2();
  _RAX = (unsigned int)((4 * byte_405054 + byte_40505B + byte_40505D) / byte_40505C - byte_405055);
  if ( v9 == (_DWORD)_RAX )
    return (sub_4010D2()
         || 9 * byte_405054 + byte_40505A + byte_405057 != byte_40505D
         || (8 * byte_405054 + byte_40505B + byte_40505D) / byte_40505C != byte_405055)
        && (v8 = 15 * byte_405054 + byte_40505A + byte_405057 - byte_40505D, (unsigned int)sub_401028(byte_405055) + v8);
  __asm { cpuid }
  return sub_401028(a2);
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405058: using guessed type char byte_405058;
// 40505A: using guessed type char byte_40505A;
// 40505B: using guessed type char byte_40505B;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (000000000040130B) ----------------------------------------------------
void *__fastcall sub_40130B(int a1)
{
  int v1; // eax
  _BYTE *v3; // rcx
  _BYTE *v9; // [rsp+28h] [rbp-18h]
  int i; // [rsp+34h] [rbp-Ch]

  if ( byte_405054 < 14 && byte_405055 && byte_405056 > (int)sub_4010D2() )
  {
    v1 = a1 <= byte_405054
      || a1 >= (84 * byte_405054
              + byte_405058 * 16i64 * byte_405056 / (unsigned __int64)byte_40505C
              + byte_405058 * byte_405056 * 16i64 * byte_405055 / (unsigned __int64)byte_40505C) >> 1;
    if ( (unsigned int)sub_40112B(8, v1) && (sub_4010D2() || byte_405054) )
      return 0i64;
  }
  v9 = &unk_405490;
  for ( i = byte_405054; i < a1; ++i )
  {
    *v9 = 37;
    v3 = v9 + 1;
    v9 += 2;
    *v3 = byte_405036;
    LODWORD(v3) = i;
    if ( (_DWORD)v3 )
      sub_401028(21 * ((18 * byte_405054 + byte_40505B + byte_40505D) / byte_40505C - byte_405055));
  }
  _RAX = v9;
  *v9 = 0;
  __asm { cpuid }
  return &unk_405490;
}
// 405036: using guessed type char byte_405036;
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405058: using guessed type char byte_405058;
// 405059: using guessed type char byte_405059;
// 40505A: using guessed type char byte_40505A;
// 40505B: using guessed type char byte_40505B;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (00000000004015B4) ----------------------------------------------------
BOOL __fastcall sub_4015B4(void *a1, const void *a2, DWORD a3, DWORD *a4, LPVOID lpReserved)
{
  __asm { cpuid }
  return WriteConsoleA(a1, a2, a3, a4, lpReserved);
}

//----- (0000000000401604) ----------------------------------------------------
HANDLE __fastcall sub_401604(int a1)
{
  DWORD v6; // eax

  __asm { cpuid }
  v6 = sub_401028(a1);
  return GetStdHandle(v6);
}

//----- (0000000000401632) ----------------------------------------------------
FARPROC __fastcall sub_401632(HMODULE a1, const CHAR *a2)
{
  __asm { cpuid }
  return GetProcAddress(a1, a2);
}

//----- (0000000000401664) ----------------------------------------------------
HMODULE __fastcall sub_401664(const CHAR *a1)
{
  __asm { cpuid }
  return GetModuleHandleA(a1);
}

//----- (0000000000401688) ----------------------------------------------------
__int64 __fastcall sub_401688(_BYTE *a1, _BYTE *a2)
{
  int v3; // eax
  int v4; // eax
  int v6; // eax
  int v7; // eax
  int v12; // [rsp+28h] [rbp-18h]
  int v13; // [rsp+30h] [rbp-10h]
  int v14; // [rsp+38h] [rbp-8h]

  while ( 1 )
  {
    _RAX = 40i64;
    if ( byte_405054 >= 40 )
      break;
    _RAX = (unsigned int)byte_40505C;
    if ( (int)_RAX <= byte_405057 )
      break;
    _RAX = sub_4010D2();
    if ( !(_DWORD)_RAX )
      break;
    v3 = *a1 || *a2;
    _RAX = sub_40112B(13, v3);
    if ( !(_DWORD)_RAX )
      break;
    _RAX = (unsigned int)byte_405059;
    if ( !byte_405059 )
      break;
    sub_401028(15);
    if ( byte_405054 < 32
      && byte_405055
      && byte_405056 > (int)sub_4010D2()
      && (unsigned int)sub_40112B(17, (int)sub_401000) )
    {
      sub_4010D2();
    }
    while ( byte_405054 < 46
         && byte_40505C > byte_405057
         && sub_4010D2()
         && (unsigned int)sub_40112B(19, byte_405054)
         && byte_405059 )
      ;
    if ( byte_405054 < 40 && byte_405055 && byte_405056 > (int)sub_4010D2() )
    {
      v14 = sub_401028((char)*a1);
      v4 = v14 < (int)sub_401028((char)*a2) && (unsigned int)sub_401028(byte_405055);
      if ( (unsigned int)sub_40112B(21, v4) && (sub_4010D2() || byte_405054) )
        return (unsigned int)-((int)sub_401028(byte_405056) / byte_405056);
    }
    if ( byte_405054 < 46
      && byte_405055
      && byte_405056 > (int)sub_4010D2()
      && (unsigned int)sub_40112B(24, byte_405054 > 22)
      && (sub_4010D2() || byte_405054) )
    {
      sub_401000();
    }
    else if ( byte_405054 < 56
           && byte_405055
           && byte_405056 > (int)sub_4010D2()
           && (unsigned int)sub_40112B(29, 0)
           && (sub_4010D2() || byte_405054) )
    {
      sub_401028(byte_405057 - 30);
    }
    else if ( byte_405054 < 64
           && byte_405055
           && byte_405056 > (int)sub_4010D2()
           && (unsigned int)sub_40112B(33, 93 * byte_405054 + byte_40505A + byte_405057 - byte_40505D)
           && (sub_4010D2() || byte_405054) )
    {
      sub_401028(35);
      if ( byte_405054 < 72
        && byte_405055
        && byte_405056 > (int)sub_4010D2()
        && (unsigned int)sub_40112B(37, (int)sub_401000) )
      {
        sub_4010D2();
      }
      while ( byte_405054 < 66
           && byte_40505C > byte_405057
           && sub_4010D2()
           && (unsigned int)sub_40112B(39, byte_405054)
           && byte_405059 )
        ;
    }
    else if ( byte_405054 < 82
           && byte_405055
           && byte_405056 > (int)sub_4010D2()
           && ((v13 = 120 * byte_405054 + byte_40505A + byte_405057 - byte_40505D, (unsigned int)sub_401028(byte_405057))
             ? (v6 = byte_405056)
             : (v6 = byte_405054),
               (unsigned int)sub_40112B(42, v6 * v13) && (sub_4010D2() || byte_405054)) )
    {
      sub_401028(44);
      if ( byte_405054 < 90
        && byte_405055
        && byte_405056 > (int)sub_4010D2()
        && (unsigned int)sub_40112B(46, (int)sub_401000) )
      {
        sub_4010D2();
      }
      while ( byte_405054 < 75
           && byte_40505C > byte_405057
           && sub_4010D2()
           && (unsigned int)sub_40112B(48, byte_405054)
           && byte_405059 )
        ;
    }
    else if ( byte_405054 < 98 )
    {
      if ( byte_405055 )
      {
        if ( byte_405056 > (int)sub_4010D2() )
        {
          v12 = sub_401028((char)*a1);
          v7 = sub_401028((char)*a2);
          if ( (unsigned int)sub_40112B(50, v12 > v7) )
          {
            if ( sub_4010D2() || byte_405054 )
              return sub_401028(byte_405055 + byte_405054);
          }
        }
      }
    }
    a1 += (int)sub_401028(byte_405055);
    a2 += (int)sub_401028(byte_405056 - byte_405055);
  }
  __asm { cpuid }
  return (unsigned int)byte_405054;
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405059: using guessed type char byte_405059;
// 40505A: using guessed type char byte_40505A;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (0000000000401F47) ----------------------------------------------------
__int64 __fastcall sub_401F47(char *a1)
{
  __int64 v7; // [rsp+28h] [rbp-8h]

  v7 = byte_405054;
  while ( 1 )
  {
    _RAX = 79i64;
    if ( byte_405054 >= 79 )
      break;
    _RAX = (unsigned int)byte_40505C;
    if ( (int)_RAX <= byte_405057 )
      break;
    _RAX = sub_4010D2();
    if ( !(_DWORD)_RAX )
      break;
    _RAX = sub_40112B(52, *a1 != 0);
    if ( !(_DWORD)_RAX )
      break;
    _RAX = (unsigned int)byte_405059;
    if ( !byte_405059 )
      break;
    v7 += (int)sub_401028(byte_405055);
    a1 += (int)sub_401028(byte_405056 - byte_405055);
  }
  __asm { cpuid }
  return (int)sub_401028(byte_405054 + (int)v7);
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405059: using guessed type char byte_405059;
// 40505C: using guessed type char byte_40505C;

//----- (0000000000402050) ----------------------------------------------------
__int64 __fastcall sub_402050(__int64 a1)
{
  char *v1; // rax
  char *v2; // rax
  char *v13; // rax
  char *v14; // rax
  char Source[32]; // [rsp+58h] [rbp-78h] BYREF
  char *v17; // [rsp+78h] [rbp-58h]
  char *v18; // [rsp+80h] [rbp-50h]
  char *v19; // [rsp+88h] [rbp-48h]
  char *v20; // [rsp+90h] [rbp-40h]
  char *v21; // [rsp+98h] [rbp-38h]
  char v22; // [rsp+A7h] [rbp-29h]
  HMODULE v23; // [rsp+A8h] [rbp-28h]
  char Buffer[32]; // [rsp+B0h] [rbp-20h] BYREF

  if ( byte_405054 < 106
    && byte_405055
    && byte_405056 > (int)sub_4010D2()
    && (unsigned int)sub_40112B(54, qword_405060 == 0i64)
    && (sub_4010D2() || byte_405054) )
  {
    v1 = (char *)sub_40130B(byte_40505A);
    v2 = strcat(v1, ::Source);
    sprintf(
      Buffer,
      v2,
      (unsigned int)byte_40503E,
      (unsigned int)byte_405038,
      (unsigned int)byte_405045,
      (unsigned int)byte_405041,
      (unsigned int)byte_405038,
      (unsigned int)byte_40503F,
      (unsigned int)byte_405057,
      (unsigned int)byte_405056);
    v23 = sub_401664(Buffer);
    if ( byte_405054 < 110 )
    {
      if ( byte_405055 )
      {
        if ( byte_405056 > (int)sub_4010D2() )
        {
          if ( (unsigned int)sub_40112B(56, v23 != 0i64) )
          {
            _RAX = sub_4010D2();
            if ( (_DWORD)_RAX || (_RAX = (unsigned int)byte_405054, byte_405054) )
            {
              __asm { cpuid }
              v22 = byte_40504F;
              sub_401000();
              v21 = &byte_4054A0[byte_405058];
              *v21 = sub_401028(byte_405054);
              v20 = &byte_4054A0[byte_405057];
              *v20 = sub_401028(byte_405037);
              v19 = &byte_4054A0[byte_405056];
              *v19 = sub_401028(byte_405034);
              v18 = &byte_4054A0[byte_405055];
              *v18 = sub_401028(byte_405042);
              v17 = &byte_4054A0[byte_405054];
              _RAX = sub_401028(byte_40504F);
              *v17 = _RAX;
              __asm { cpuid }
              v13 = strcat(Destination, aC);
              sprintf(Source, v13, (unsigned int)byte_405050);
              v14 = strcat(byte_4054A0, Source);
              qword_405060 = (__int64 (__fastcall *)(_QWORD))sub_401632(v23, v14);
            }
          }
        }
      }
    }
  }
  if ( byte_405054 < 114
    && byte_405055
    && byte_405056 > (int)sub_4010D2()
    && (unsigned int)sub_40112B(58, qword_405060 != 0i64)
    && (sub_4010D2() || byte_405054) )
  {
    return qword_405060(a1);
  }
  else
  {
    return 0i64;
  }
}
// 405034: using guessed type char byte_405034;
// 405037: using guessed type char byte_405037;
// 405038: using guessed type char byte_405038;
// 40503E: using guessed type char byte_40503E;
// 40503F: using guessed type char byte_40503F;
// 405041: using guessed type char byte_405041;
// 405042: using guessed type char byte_405042;
// 405045: using guessed type char byte_405045;
// 40504F: using guessed type char byte_40504F;
// 405050: using guessed type char byte_405050;
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405058: using guessed type char byte_405058;
// 40505A: using guessed type char byte_40505A;
// 405060: using guessed type __int64 (__fastcall *qword_405060)(_QWORD);

//----- (0000000000402460) ----------------------------------------------------
__int64 __fastcall sub_402460(__int64 a1)
{
  return sub_402050(a1);
}

//----- (0000000000402480) ----------------------------------------------------
__int64 __fastcall sub_402480(__int64 a1)
{
  return sub_402460(a1);
}

//----- (00000000004024A0) ----------------------------------------------------
__int64 __fastcall sub_4024A0(__int64 a1)
{
  return sub_402480(a1);
}

//----- (00000000004024C0) ----------------------------------------------------
__int64 __fastcall sub_4024C0(__int64 a1)
{
  return sub_4024A0(a1);
}

//----- (00000000004024E0) ----------------------------------------------------
__int64 __fastcall sub_4024E0(__int64 a1)
{
  return sub_4024C0(a1);
}

//----- (0000000000402500) ----------------------------------------------------
__int64 __fastcall sub_402500(__int64 a1)
{
  return sub_4024E0(a1);
}

//----- (0000000000402520) ----------------------------------------------------
BOOL sub_402520()
{
  sub_401028(60);
  if ( byte_405054 < 122
    && byte_405055
    && byte_405056 > (int)sub_4010D2()
    && (unsigned int)sub_40112B(62, (int)sub_401000) )
  {
    sub_4010D2();
  }
  while ( byte_405054 < 91
       && byte_40505C > byte_405057
       && sub_4010D2()
       && (unsigned int)sub_40112B(64, byte_405054)
       && byte_405059 )
    ;
  sub_401028(66);
  if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(68, (int)sub_401000) )
    sub_4010D2();
  while ( byte_405054 < 97
       && byte_40505C > byte_405057
       && sub_4010D2()
       && (unsigned int)sub_40112B(70, byte_405054)
       && byte_405059 )
    ;
  return IsDebuggerPresent();
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405059: using guessed type char byte_405059;
// 40505C: using guessed type char byte_40505C;

//----- (000000000040274C) ----------------------------------------------------
void sub_40274C()
{
  __debugbreak();
}

//----- (000000000040275A) ----------------------------------------------------
_BYTE *sub_40275A()
{
  __int64 v0; // kr00_8
  int v1; // eax
  int v2; // eax
  _BYTE *v4; // [rsp+20h] [rbp-20h]
  _BYTE *v5; // [rsp+28h] [rbp-18h]
  int v6; // [rsp+30h] [rbp-10h]

  sub_401028(72);
  if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(74, (int)sub_401000) )
    sub_4010D2();
  while ( byte_405054 < 103
       && byte_40505C > byte_405057
       && sub_4010D2()
       && (unsigned int)sub_40112B(76, byte_405054)
       && byte_405059 )
    ;
  sub_401000();
  sub_401000();
  if ( byte_405055 && byte_405056 > (int)sub_4010D2() )
  {
    v1 = 1;
    if ( !IsDebuggerPresent() )
    {
      v0 = (byte_405055 + byte_405054 + byte_405055) * (byte_405054 / !sub_402520());
      if ( !(unsigned int)sub_401028(v0 / byte_405056) )
        v1 = 0;
    }
    if ( (unsigned int)sub_40112B(78, v1) && (sub_4010D2() || byte_405054) )
    {
      sub_40274C();
      __debugbreak();
    }
  }
  if ( byte_405055
    && byte_405056 > (int)sub_4010D2()
    && (unsigned int)sub_40112B(81, byte_405054 > 79)
    && (sub_4010D2() || byte_405054) )
  {
    sub_401000();
  }
  else if ( byte_405055
         && byte_405056 > (int)sub_4010D2()
         && (unsigned int)sub_40112B(86, 0)
         && (sub_4010D2() || byte_405054) )
  {
    sub_401028(byte_405057 - 87);
  }
  else if ( byte_405055
         && byte_405056 > (int)sub_4010D2()
         && (unsigned int)sub_40112B(90, 264 * byte_405054 + byte_40505A + byte_405057 - byte_40505D)
         && (sub_4010D2() || byte_405054) )
  {
    sub_401028(92);
    if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(94, (int)sub_401000) )
      sub_4010D2();
    while ( byte_405054 < 123
         && byte_40505C > byte_405057
         && sub_4010D2()
         && (unsigned int)sub_40112B(96, byte_405054)
         && byte_405059 )
      ;
  }
  else if ( byte_405055
         && byte_405056 > (int)sub_4010D2()
         && ((v6 = 291 * byte_405054 + byte_40505A + byte_405057 - byte_40505D, (unsigned int)sub_401028(byte_405057))
           ? (v2 = byte_405056)
           : (v2 = byte_405054),
             (unsigned int)sub_40112B(99, v2 * v6) && (sub_4010D2() || byte_405054)) )
  {
    sub_401028(101);
    if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(103, (int)sub_401000) )
      sub_4010D2();
    while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(105, byte_405054) && byte_405059 )
      ;
  }
  else
  {
    IsDebuggerPresent();
  }
  v5 = malloc(byte_40505B);
  v5[byte_405055 + byte_405056 + byte_405057] = 0;
  v4 = &v5[byte_405056 + byte_405056 + byte_405055];
  *v4 = sub_401028(byte_405047);
  v5[byte_405056 + byte_405055 * byte_405056] = byte_405045;
  sub_401028(107);
  if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(109, (int)sub_401000) )
    sub_4010D2();
  while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(111, byte_405054) && byte_405059 )
    ;
  v5[byte_405056 * byte_405058 - byte_405059] = byte_405036;
  v5[byte_405056 * byte_405055] = byte_405049;
  v5[byte_405055 + byte_405054] = byte_405046;
  v5[byte_40505C - byte_405057 - byte_405059] = byte_405040;
  sub_401028(113);
  if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(115, (int)sub_401000) )
    sub_4010D2();
  while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(117, byte_405054) && byte_405059 )
    ;
  return v5;
}
// 402EE9: ignored the value written to the shadow area of the succeeding call
// 405036: using guessed type char byte_405036;
// 405040: using guessed type char byte_405040;
// 405045: using guessed type char byte_405045;
// 405046: using guessed type char byte_405046;
// 405047: using guessed type char byte_405047;
// 405049: using guessed type char byte_405049;
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405058: using guessed type char byte_405058;
// 405059: using guessed type char byte_405059;
// 40505A: using guessed type char byte_40505A;
// 40505B: using guessed type char byte_40505B;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (0000000000403268) ----------------------------------------------------
_BYTE *sub_403268()
{
  return sub_40275A();
}

//----- (000000000040327A) ----------------------------------------------------
_BYTE *sub_40327A()
{
  return sub_403268();
}

//----- (000000000040328C) ----------------------------------------------------
_BYTE *sub_40328C()
{
  return sub_40327A();
}

//----- (000000000040329E) ----------------------------------------------------
_BYTE *sub_40329E()
{
  return sub_40328C();
}

//----- (00000000004032B0) ----------------------------------------------------
_BYTE *sub_4032B0()
{
  return sub_40329E();
}

//----- (00000000004032C2) ----------------------------------------------------
_BYTE *sub_4032C2()
{
  return sub_4032B0();
}

//----- (00000000004032D4) ----------------------------------------------------
_BYTE *sub_4032D4()
{
  return sub_4032C2();
}

//----- (00000000004032E6) ----------------------------------------------------
_BYTE *sub_4032E6()
{
  return sub_4032D4();
}

//----- (00000000004032F8) ----------------------------------------------------
_BYTE *sub_4032F8()
{
  return sub_4032E6();
}

//----- (000000000040330A) ----------------------------------------------------
_BYTE *sub_40330A()
{
  return sub_4032F8();
}

//----- (000000000040331C) ----------------------------------------------------
_BYTE *sub_40331C()
{
  return sub_40330A();
}

//----- (000000000040332E) ----------------------------------------------------
_BYTE *sub_40332E()
{
  return sub_40331C();
}

//----- (0000000000403340) ----------------------------------------------------
_BYTE *sub_403340()
{
  return sub_40332E();
}

//----- (0000000000403352) ----------------------------------------------------
_BYTE *sub_403352()
{
  return sub_403340();
}

//----- (0000000000403364) ----------------------------------------------------
_BYTE *sub_403364()
{
  return sub_403352();
}

//----- (0000000000403376) ----------------------------------------------------
_BYTE *sub_403376()
{
  return sub_403364();
}

//----- (0000000000403388) ----------------------------------------------------
_BYTE *sub_403388()
{
  return sub_403376();
}

//----- (000000000040339A) ----------------------------------------------------
BOOL sub_40339A(__int64 a1, const char *a2, ...)
{
  int v2; // eax
  int v3; // eax
  DWORD v4; // eax
  void *v6; // [rsp+38h] [rbp-418h]
  char v7[1024]; // [rsp+50h] [rbp-400h] BYREF
  va_list va; // [rsp+470h] [rbp+20h] BYREF

  va_start(va, a2);
  sub_401028(119);
  if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(121, (int)sub_401000) )
    sub_4010D2();
  while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(123, byte_405054) && byte_405059 )
    ;
  sub_404303(
    v7,
    (202 * byte_405054
   + byte_405058 * ((__int64)byte_405056 << 10) / (unsigned __int64)byte_40505C
   + byte_405058 * byte_405056 * ((__int64)byte_405055 << 10) / (unsigned __int64)byte_40505C) >> 1,
    a2,
    va);
  v2 = sub_401028(-11);
  v3 = (unsigned int)sub_401604(v2);
  v6 = (void *)(int)sub_401028(v3);
  sub_401000();
  v4 = sub_401F47(v7);
  return sub_4015B4(v6, v7, v4, 0i64, 0i64);
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405058: using guessed type char byte_405058;
// 405059: using guessed type char byte_405059;
// 40505C: using guessed type char byte_40505C;
// 40339A: using guessed type char var_400[1024];

//----- (000000000040362C) ----------------------------------------------------
char *__fastcall sub_40362C()
{
  __asm { cpuid }
  return aScanf;
}

//----- (0000000000403644) ----------------------------------------------------
char *__fastcall sub_403644()
{
  __asm { cpuid }
  return aSprintf;
}

//----- (000000000040365C) ----------------------------------------------------
char *__fastcall sub_40365C()
{
  __asm { cpuid }
  return aFclose;
}

//----- (0000000000403674) ----------------------------------------------------
char *__fastcall sub_403674()
{
  __asm { cpuid }
  return aFopen;
}

//----- (000000000040368C) ----------------------------------------------------
char *__fastcall sub_40368C()
{
  __asm { cpuid }
  return aFread;
}

//----- (00000000004036A4) ----------------------------------------------------
char *__fastcall sub_4036A4()
{
  __asm { cpuid }
  return aFwrite;
}

//----- (00000000004036BC) ----------------------------------------------------
char *__fastcall sub_4036BC()
{
  __asm { cpuid }
  return aExit;
}

//----- (00000000004036D4) ----------------------------------------------------
char *__fastcall sub_4036D4()
{
  __asm { cpuid }
  return aStrcpy;
}

//----- (00000000004036EC) ----------------------------------------------------
char *__fastcall sub_4036EC()
{
  __asm { cpuid }
  return aStrtok;
}

//----- (0000000000403704) ----------------------------------------------------
void *__fastcall sub_403704(void *a1, int a2, size_t a3)
{
  return memset(a1, byte_405055 * a2, a3);
}
// 405055: using guessed type char byte_405055;

//----- (0000000000403746) ----------------------------------------------------
char *__fastcall sub_403746()
{
  __asm { cpuid }
  return aMemcpy;
}

//----- (000000000040375E) ----------------------------------------------------
char *__fastcall sub_40375E()
{
  __asm { cpuid }
  return aStrchr;
}

//----- (0000000000403776) ----------------------------------------------------
char *__fastcall sub_403776()
{
  __asm { cpuid }
  return aStrrchr;
}

//----- (000000000040378E) ----------------------------------------------------
char *__fastcall sub_40378E()
{
  __asm { cpuid }
  return aRand;
}

//----- (00000000004037A6) ----------------------------------------------------
char *__fastcall sub_4037A6()
{
  __asm { cpuid }
  return aRealloc;
}

//----- (00000000004037BE) ----------------------------------------------------
void *__fastcall sub_4037BE(size_t a1, size_t a2)
{
  return calloc(a1, a2);
}

//----- (00000000004037EC) ----------------------------------------------------
__int64 __fastcall sub_4037EC(__int64 a1, __int64 a2)
{
  _BYTE *v2; // rax
  char *v3; // rax
  FARPROC v5; // [rsp+20h] [rbp-10h]
  HMODULE v6; // [rsp+28h] [rbp-8h]

  v2 = sub_403388();
  v6 = (HMODULE)sub_402500((__int64)v2);
  v3 = sub_4037A6();
  v5 = sub_401632(v6, v3);
  return ((__int64 (__fastcall *)(__int64, __int64))v5)(a1, a2);
}

//----- (000000000040384E) ----------------------------------------------------
__int64 __fastcall sub_40384E(__int64 a1)
{
  return gets(a1);
}
// 4044F0: using guessed type __int64 __fastcall gets(_QWORD);

//----- (000000000040386E) ----------------------------------------------------
__int64 sub_40386E(char *a1, size_t a2, const char *a3, ...)
{
  va_list va; // [rsp+58h] [rbp+28h] BYREF

  va_start(va, a3);
  return (unsigned int)sub_404303(a1, a2, a3, va);
}

//----- (00000000004038C0) ----------------------------------------------------
int __fastcall sub_4038C0(char *a1, const char *a2, va_list a3)
{
  return vsprintf(a1, a2, a3);
}

//----- (00000000004038F9) ----------------------------------------------------
int __fastcall sub_4038F9(char *a1, size_t a2, const char *a3, va_list a4)
{
  return sub_404303(a1, a2, a3, a4);
}

//----- (000000000040393D) ----------------------------------------------------
char *__fastcall sub_40393D(const char *a1)
{
  return getenv(a1);
}

//----- (000000000040395D) ----------------------------------------------------
int __fastcall sub_40395D(const char *a1)
{
  return system(a1);
}

//----- (000000000040397D) ----------------------------------------------------
void __noreturn sub_40397D()
{
  abort();
}

//----- (000000000040398F) ----------------------------------------------------
int __fastcall sub_40398F(void (__cdecl *a1)())
{
  return atexit(a1);
}

//----- (00000000004039AF) ----------------------------------------------------
__int64 __fastcall sub_4039AF(char *a1, int a2)
{
  return (int)getcwd(a1, a2);
}

//----- (00000000004039E0) ----------------------------------------------------
int __fastcall sub_4039E0(int a1)
{
  return tolower(a1);
}

//----- (00000000004039FF) ----------------------------------------------------
int __fastcall sub_4039FF(int a1)
{
  return toupper(a1);
}

//----- (0000000000403A1E) ----------------------------------------------------
__int64 sub_403A1E()
{
  int v1; // eax
  _BYTE *v2; // rax
  char *v3; // rax
  _BYTE *v4; // rax
  char *v5; // rax
  int v6; // eax
  int v7; // eax
  int v9; // [rsp+30h] [rbp-40h]
  FARPROC v10; // [rsp+38h] [rbp-38h]
  HMODULE v11; // [rsp+40h] [rbp-30h]
  FARPROC v12; // [rsp+48h] [rbp-28h]
  HMODULE v13; // [rsp+50h] [rbp-20h]
  char *Block; // [rsp+58h] [rbp-18h]
  int i; // [rsp+6Ch] [rbp-4h]

  for ( i = 0; i < 5; ++i )
  {
    v1 = sub_401028(256);
    Block = (char *)malloc(v1);
    v2 = sub_403388();
    v13 = (HMODULE)sub_402500((__int64)v2);
    v3 = sub_4036D4();
    v12 = sub_401632(v13, v3);
    ((void (*)(char *, const char *, ...))v12)(Block, aDHelloWorld);
    v4 = sub_403388();
    v11 = (HMODULE)sub_402500((__int64)v4);
    v5 = sub_403644();
    v10 = sub_401632(v11, v5);
    ((void (__fastcall *)(char *, char *, _QWORD))v10)(Block, Block, (unsigned int)(i + 1));
    if ( byte_405055
      && byte_405056 > (int)sub_4010D2()
      && (unsigned int)sub_40112B(130, (int)Block)
      && (sub_4010D2() || byte_405054) )
    {
      do
      {
        sub_401000();
        sub_40339A(132i64, Block);
      }
      while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(135, 0) && byte_405059 );
    }
    else if ( byte_405055
           && byte_405056 > (int)sub_4010D2()
           && (unsigned int)sub_40112B(138, 0)
           && (sub_4010D2() || byte_405054) )
    {
      sub_401000();
    }
    else if ( byte_405055
           && byte_405056 > (int)sub_4010D2()
           && (unsigned int)sub_40112B(143, 0)
           && (sub_4010D2() || byte_405054) )
    {
      sub_401028(byte_405057 - 144);
    }
    else if ( byte_405055
           && byte_405056 > (int)sub_4010D2()
           && (unsigned int)sub_40112B(147, 435 * byte_405054 + byte_40505A + byte_405057 - byte_40505D)
           && (sub_4010D2() || byte_405054) )
    {
      sub_401028(149);
      if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(151, (int)sub_401000) )
        sub_4010D2();
      while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(153, byte_405054) && byte_405059 )
        ;
    }
    else if ( byte_405055
           && byte_405056 > (int)sub_4010D2()
           && ((v9 = 462 * byte_405054 + byte_40505A + byte_405057 - byte_40505D, (unsigned int)sub_401028(byte_405057))
             ? (v6 = byte_405056)
             : (v6 = byte_405054),
               (unsigned int)sub_40112B(156, v6 * v9) && (sub_4010D2() || byte_405054)) )
    {
      sub_401028(158);
      if ( byte_405055 && byte_405056 > (int)sub_4010D2() && (unsigned int)sub_40112B(160, (int)sub_401000) )
        sub_4010D2();
      while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(162, byte_405054) && byte_405059 )
        ;
    }
    else
    {
      do
      {
        sub_401000();
        sub_40339A(164i64, aError);
      }
      while ( byte_40505C > byte_405057 && sub_4010D2() && (unsigned int)sub_40112B(167, 0) && byte_405059 );
    }
    free(Block);
    if ( i )
      sub_401028(138 * ((252 * byte_405054 + byte_40505B + byte_40505D) / byte_40505C - byte_405055));
  }
  v7 = getch();
  return sub_401028(504 * byte_405054 + byte_40505A + byte_405057 - byte_40505D + v7);
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405059: using guessed type char byte_405059;
// 40505A: using guessed type char byte_40505A;
// 40505B: using guessed type char byte_40505B;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (0000000000404303) ----------------------------------------------------
int __fastcall sub_404303(char *a1, size_t a2, const char *a3, va_list a4)
{
  return vsnprintf(a1, a2, a3, a4);
}

//----- (0000000000404348) ----------------------------------------------------
void __noreturn start()
{
  int v0; // eax
  int v1; // [rsp+4Ch] [rbp-4h] BYREF

  v1 = 0;
  _set_app_type(_crt_console_app);
  controlfp(0x10000u, 0x30000u);
  _getmainargs(*(_QWORD *)&_argc, _argv, environ, (unsigned int)dword_4054A8, &v1);
  v0 = sub_403A1E();
  exit(v0);
}
// 404560: using guessed type __int64 __fastcall _getmainargs(_QWORD, _QWORD, _QWORD, _QWORD, _QWORD);
// 4054A8: using guessed type int dword_4054A8;

//----- (0000000000404423) ----------------------------------------------------
// write access to const memory has been detected, the output may be wrong!
__int64 __fastcall sub_404423(__int64 a1, __int64 a2)
{
  __int64 v2; // rbp

  *(_QWORD *)(v2 + 16) = a1;
  *(_QWORD *)(v2 + 24) = a2;
  _argc = *(_DWORD *)(v2 + 16);
  _argv = *(char ***)(v2 + 24);
  controlfp(0x10000u, 0x30000u);
  *(_QWORD *)(v2 - 8) = *(_QWORD *)&_argc;
  return sub_403A1E();
}
// 404435: write access to const memory at 405204 has been detected
// 404442: write access to const memory at 40520C has been detected
// 404423: variable 'v2' is possibly undefined

// nfuncs=98 queued=72 decompiled=72 lumina nreq=0 worse=0 better=0
// ALL OK, 72 function(s) have been successfully decompiled
