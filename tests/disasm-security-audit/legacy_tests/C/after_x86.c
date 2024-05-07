
#include <windows.h>
#include <defs.h>


//-------------------------------------------------------------------------
// Function declarations

void sub_401000();
int __cdecl sub_40100D(int a1);
BOOL sub_401097();
int __cdecl sub_4010EF(int a1, int a2);
void *__cdecl sub_4012BF(signed int a1);
BOOL __cdecl sub_401511(HANDLE hConsoleOutput, void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved);
HANDLE __cdecl sub_40153A(int a1);
FARPROC __stdcall sub_40155C(HMODULE hModule, LPCSTR lpProcName);
HMODULE __cdecl sub_40157B(LPCSTR lpModuleName);
int __cdecl sub_401DB6(char *a1);
int __cdecl sub_401E94(int a1);
int __cdecl sub_4021E5(int a1);
int __cdecl sub_4021FD(int a1);
int __cdecl sub_402215(int a1);
int __cdecl sub_40222D(int a1);
int __cdecl sub_402245(int a1);
int __cdecl sub_40225D(int a1);
BOOL sub_402275();
void sub_40247C();
_BYTE *sub_402489();
_BYTE *sub_402EE4();
_BYTE *sub_402EF5();
_BYTE *sub_402F06();
_BYTE *sub_402F17();
_BYTE *sub_402F28();
_BYTE *sub_402F39();
_BYTE *sub_402F4A();
_BYTE *sub_402F5B();
_BYTE *sub_402F6C();
_BYTE *sub_402F7D();
_BYTE *sub_402F8E();
_BYTE *sub_402F9F();
_BYTE *sub_402FB0();
_BYTE *sub_402FC1();
_BYTE *sub_402FD2();
_BYTE *sub_402FE3();
_BYTE *sub_402FF4();
BOOL __cdecl sub_403005(int a1, char *Format);
char *sub_403299();
char *sub_403317();
char *sub_4033BF();
int sub_4035CE();
int __cdecl sub_403DEF(char *Buffer, size_t BufferCount, char *Format, va_list ArgList); // idb
void __noreturn start(); // weak
// int __usercall sub_403F44@<eax>(int a1@<ebp>);
// int __usercall sub_403F48@<eax>(int a1@<ebp>);
int __cdecl sub_403F89(_DWORD *a1, char a2);
// BOOL __stdcall WriteConsoleA(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved);
// HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
// FARPROC __stdcall GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
// HMODULE __stdcall GetModuleHandleA(LPCSTR lpModuleName);
// char *__cdecl strcat(char *Destination, const char *Source);
// int sprintf(char *const Buffer, const char *const Format, ...);
// BOOL __stdcall IsDebuggerPresent();
// void *__cdecl malloc(size_t Size);
// void __cdecl free(void *Block);
// int __cdecl getch();
// int __cdecl vsnprintf(char *const Buffer, const size_t BufferCount, const char *const Format, va_list ArgList);
// void __cdecl _set_app_type(_crt_app_type Type);
// unsigned int __cdecl controlfp(unsigned int NewValue, unsigned int Mask);
// int __cdecl _getmainargs(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD); weak
// void __cdecl __noreturn exit(int Code);

//-------------------------------------------------------------------------
// Data declarations

int dword_403F78[3] = { -1, 4210514, 4210535 }; // weak
_UNKNOWN j__except_handler3; // weak
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
int (__stdcall *dword_405060)(_DWORD) = NULL; // weak
char Source[] = "%d%d"; // idb
char Destination[] = "Library"; // idb
char aC[] = "%c"; // idb
char aSprintf[8] = "sprintf"; // weak
char aStrcpy[7] = "strcpy"; // weak
char aRealloc[8] = "realloc"; // weak
char aDHelloWorld[19] = "%d) Hello, world!\n"; // weak
char Format[] = "Error!"; // idb
// extern int _argc;
// extern char **_argv;
// extern char **environ;
_UNKNOWN unk_4053D4; // weak
char byte_4053E4[8] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' }; // idb
int dword_4053EC = 0; // weak


//----- (00401000) --------------------------------------------------------
void sub_401000()
{
  ;
}

//----- (0040100D) --------------------------------------------------------
int __cdecl sub_40100D(int a1)
{
  sub_401000();
  __asm { cpuid }
  return byte_405056 * byte_405058 + byte_405055 * a1 - byte_40505C;
}
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405058: using guessed type char byte_405058;
// 40505C: using guessed type char byte_40505C;

//----- (00401097) --------------------------------------------------------
BOOL sub_401097()
{
  __asm { cpuid }
  return byte_405055 && (byte_40505B + byte_40505D) / byte_40505C != byte_405055;
}
// 405055: using guessed type char byte_405055;
// 40505B: using guessed type char byte_40505B;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (004010EF) --------------------------------------------------------
int __cdecl sub_4010EF(int a1, int a2)
{
  int v8; // [esp+0h] [ebp-8h]
  int v9; // [esp+4h] [ebp-4h]

  v9 = sub_40100D(byte_405055 * a2);
  if ( v9 == a1 * (3 * byte_405054 + byte_40505A + byte_405057 - byte_40505D) )
    return byte_40505C != byte_405056 * byte_405058 && !sub_401097();
  _EAX = (4 * byte_405054 + byte_40505B + byte_40505D) / byte_40505C - byte_405055;
  if ( v9 == _EAX )
    return (sub_401097()
         || 9 * byte_405054 + byte_40505A + byte_405057 != byte_40505D
         || (8 * byte_405054 + byte_40505B + byte_40505D) / byte_40505C != byte_405055)
        && (v8 = 15 * byte_405054 + byte_40505A + byte_405057 - byte_40505D, sub_40100D(byte_405055) + v8);
  __asm { cpuid }
  return sub_40100D(a2);
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

//----- (004012BF) --------------------------------------------------------
void *__cdecl sub_4012BF(signed int a1)
{
  int v1; // eax
  _BYTE *v4; // ecx
  _BYTE *v10; // [esp+4h] [ebp-Ch]
  signed int i; // [esp+8h] [ebp-8h]

  if ( byte_405054 < 14 && byte_405055 && byte_405056 > sub_401097() )
  {
    v1 = a1 <= byte_405054
      || a1 >= (84 * byte_405054
              + byte_405058 * 16 * byte_405056 / (unsigned int)byte_40505C
              + byte_405058 * byte_405056 * 16 * byte_405055 / (unsigned int)byte_40505C) >> 1;
    if ( sub_4010EF(8, v1) && (sub_401097() || byte_405054) )
      return 0;
  }
  v10 = &unk_4053D4;
  for ( i = byte_405054; i < a1; ++i )
  {
    *v10 = 37;
    v4 = v10 + 1;
    v10 += 2;
    *v4 = byte_405036;
    if ( i )
      sub_40100D(21 * ((18 * byte_405054 + byte_40505B + byte_40505D) / byte_40505C - byte_405055));
  }
  _EAX = v10;
  *v10 = 0;
  __asm { cpuid }
  return &unk_4053D4;
}
// 405036: using guessed type char byte_405036;
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405058: using guessed type char byte_405058;
// 40505A: using guessed type char byte_40505A;
// 40505B: using guessed type char byte_40505B;
// 40505C: using guessed type char byte_40505C;
// 40505D: using guessed type char byte_40505D;

//----- (00401511) --------------------------------------------------------
BOOL __cdecl sub_401511(
        HANDLE hConsoleOutput,
        void *lpBuffer,
        DWORD nNumberOfCharsToWrite,
        LPDWORD lpNumberOfCharsWritten,
        LPVOID lpReserved)
{
  __asm { cpuid }
  return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}

//----- (0040153A) --------------------------------------------------------
HANDLE __cdecl sub_40153A(int a1)
{
  int v6; // eax

  __asm { cpuid }
  v6 = sub_40100D(a1);
  return GetStdHandle(v6);
}

//----- (0040155C) --------------------------------------------------------
FARPROC __stdcall sub_40155C(HMODULE hModule, LPCSTR lpProcName)
{
  __asm { cpuid }
  return GetProcAddress(hModule, lpProcName);
}

//----- (0040157B) --------------------------------------------------------
HMODULE __cdecl sub_40157B(LPCSTR lpModuleName)
{
  __asm { cpuid }
  return GetModuleHandleA(lpModuleName);
}

//----- (00401DB6) --------------------------------------------------------
int __cdecl sub_401DB6(char *a1)
{
  int v7; // [esp+0h] [ebp-4h]

  v7 = byte_405054;
  while ( 1 )
  {
    _EAX = 79;
    if ( byte_405054 >= 79 )
      break;
    _EAX = byte_40505C;
    if ( byte_40505C <= byte_405057 )
      break;
    _EAX = sub_401097();
    if ( !_EAX )
      break;
    _EAX = sub_4010EF(52, *a1 != 0);
    if ( !_EAX )
      break;
    _EAX = byte_405059;
    if ( !byte_405059 )
      break;
    v7 += sub_40100D(byte_405055);
    a1 += sub_40100D(byte_405056 - byte_405055);
  }
  __asm { cpuid }
  return sub_40100D(byte_405054 + v7);
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405059: using guessed type char byte_405059;
// 40505C: using guessed type char byte_40505C;

//----- (00401E94) --------------------------------------------------------
int __cdecl sub_401E94(int a1)
{
  char *v1; // eax
  char *v2; // eax
  char *v13; // eax
  char *v14; // eax
  char Source[32]; // [esp+0h] [ebp-5Ch] BYREF
  char *v17; // [esp+20h] [ebp-3Ch]
  char *v18; // [esp+24h] [ebp-38h]
  char *v19; // [esp+28h] [ebp-34h]
  char *v20; // [esp+2Ch] [ebp-30h]
  char *v21; // [esp+30h] [ebp-2Ch]
  char v22; // [esp+37h] [ebp-25h]
  HMODULE hModule; // [esp+38h] [ebp-24h]
  char Buffer[32]; // [esp+3Ch] [ebp-20h] BYREF

  if ( byte_405054 < 106
    && byte_405055
    && byte_405056 > sub_401097()
    && sub_4010EF(54, dword_405060 == 0)
    && (sub_401097() || byte_405054) )
  {
    v1 = (char *)sub_4012BF(byte_40505A);
    v2 = strcat(v1, ::Source);
    sprintf(
      Buffer,
      v2,
      byte_40503E,
      byte_405038,
      byte_405045,
      byte_405041,
      byte_405038,
      byte_40503F,
      byte_405057,
      byte_405056);
    hModule = sub_40157B(Buffer);
    if ( byte_405054 < 110 )
    {
      if ( byte_405055 )
      {
        if ( byte_405056 > sub_401097() )
        {
          if ( sub_4010EF(56, hModule != 0) )
          {
            _EAX = sub_401097();
            if ( _EAX || (_EAX = byte_405054) != 0 )
            {
              __asm { cpuid }
              v22 = byte_40504F;
              sub_401000();
              v21 = &byte_4053E4[byte_405058];
              *v21 = sub_40100D(byte_405054);
              v20 = &byte_4053E4[byte_405057];
              *v20 = sub_40100D(byte_405037);
              v19 = &byte_4053E4[byte_405056];
              *v19 = sub_40100D(byte_405034);
              v18 = &byte_4053E4[byte_405055];
              *v18 = sub_40100D(byte_405042);
              v17 = &byte_4053E4[byte_405054];
              _EAX = sub_40100D(byte_40504F);
              *v17 = _EAX;
              __asm { cpuid }
              v13 = strcat(Destination, aC);
              sprintf(Source, v13, byte_405050);
              v14 = strcat(byte_4053E4, Source);
              dword_405060 = (int (__stdcall *)(_DWORD))sub_40155C(hModule, v14);
            }
          }
        }
      }
    }
  }
  if ( byte_405054 < 114
    && byte_405055
    && byte_405056 > sub_401097()
    && sub_4010EF(58, dword_405060 != 0)
    && (sub_401097() || byte_405054) )
  {
    return dword_405060(a1);
  }
  else
  {
    return 0;
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
// 405060: using guessed type int (__stdcall *dword_405060)(_DWORD);

//----- (004021E5) --------------------------------------------------------
int __cdecl sub_4021E5(int a1)
{
  return sub_401E94(a1);
}

//----- (004021FD) --------------------------------------------------------
int __cdecl sub_4021FD(int a1)
{
  return sub_4021E5(a1);
}

//----- (00402215) --------------------------------------------------------
int __cdecl sub_402215(int a1)
{
  return sub_4021FD(a1);
}

//----- (0040222D) --------------------------------------------------------
int __cdecl sub_40222D(int a1)
{
  return sub_402215(a1);
}

//----- (00402245) --------------------------------------------------------
int __cdecl sub_402245(int a1)
{
  return sub_40222D(a1);
}

//----- (0040225D) --------------------------------------------------------
int __cdecl sub_40225D(int a1)
{
  return sub_402245(a1);
}

//----- (00402275) --------------------------------------------------------
BOOL sub_402275()
{
  sub_40100D(60);
  if ( byte_405054 < 122 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(62, (int)sub_401000) )
    sub_401097();
  while ( byte_405054 < 91 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(64, byte_405054) && byte_405059 )
    ;
  sub_40100D(66);
  if ( byte_405054 != 134 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(68, (int)sub_401000) )
    sub_401097();
  while ( byte_405054 < 97 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(70, byte_405054) && byte_405059 )
    ;
  return IsDebuggerPresent();
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405059: using guessed type char byte_405059;
// 40505C: using guessed type char byte_40505C;

//----- (0040247C) --------------------------------------------------------
void sub_40247C()
{
  __debugbreak();
}

//----- (00402489) --------------------------------------------------------
_BYTE *sub_402489()
{
  int v0; // eax
  int v1; // eax
  _BYTE *v3; // [esp+4h] [ebp-18h]
  _BYTE *v4; // [esp+8h] [ebp-14h]
  int v5; // [esp+14h] [ebp-8h]
  BOOL v6; // [esp+18h] [ebp-4h]

  sub_40100D(72);
  if ( byte_405054 != 146 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(74, (int)sub_401000) )
    sub_401097();
  while ( byte_405054 < 103 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(76, byte_405054) && byte_405059 )
    ;
  sub_401000();
  sub_401000();
  if ( byte_405054 != 154 && byte_405055 && byte_405056 > sub_401097() )
  {
    v0 = 1;
    if ( !IsDebuggerPresent() )
    {
      v6 = !sub_402275();
      if ( !sub_40100D((byte_405055 + byte_405054 + byte_405055) * (byte_405054 / v6) / byte_405056) )
        v0 = 0;
    }
    if ( sub_4010EF(78, v0) && (sub_401097() || byte_405054) )
    {
      sub_40247C();
      __debugbreak();
    }
  }
  if ( byte_405054 != 160
    && byte_405055
    && byte_405056 > sub_401097()
    && sub_4010EF(81, byte_405054 > 79)
    && (sub_401097() || byte_405054) )
  {
    sub_401000();
  }
  else if ( byte_405054 != 170
         && byte_405055
         && byte_405056 > sub_401097()
         && sub_4010EF(86, 0)
         && (sub_401097() || byte_405054) )
  {
    sub_40100D(byte_405057 - 87);
  }
  else if ( byte_405054 != 178
         && byte_405055
         && byte_405056 > sub_401097()
         && sub_4010EF(90, 264 * byte_405054 + byte_40505A + byte_405057 - byte_40505D)
         && (sub_401097() || byte_405054) )
  {
    sub_40100D(92);
    if ( byte_405054 != 186 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(94, (int)sub_401000) )
      sub_401097();
    while ( byte_405054 < 123 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(96, byte_405054) && byte_405059 )
      ;
  }
  else if ( byte_405054 != 196
         && byte_405055
         && byte_405056 > sub_401097()
         && ((v5 = 291 * byte_405054 + byte_40505A + byte_405057 - byte_40505D, sub_40100D(byte_405057))
           ? (v1 = byte_405056)
           : (v1 = byte_405054),
             sub_4010EF(99, v1 * v5) && (sub_401097() || byte_405054)) )
  {
    sub_40100D(101);
    if ( byte_405054 != 204 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(103, (int)sub_401000) )
      sub_401097();
    while ( byte_405054 != 132
         && byte_40505C > byte_405057
         && sub_401097()
         && sub_4010EF(105, byte_405054)
         && byte_405059 )
      ;
  }
  else
  {
    IsDebuggerPresent();
  }
  v4 = malloc(byte_40505B);
  v4[byte_405055 + byte_405056 + byte_405057] = 0;
  v3 = &v4[byte_405056 + byte_405056 + byte_405055];
  *v3 = sub_40100D(byte_405047);
  v4[byte_405056 + byte_405055 * byte_405056] = byte_405045;
  sub_40100D(107);
  if ( byte_405054 != 216 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(109, (int)sub_401000) )
    sub_401097();
  while ( byte_405054 != 138 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(111, byte_405054) && byte_405059 )
    ;
  v4[byte_405056 * byte_405058 - byte_405059] = byte_405036;
  v4[byte_405056 * byte_405055] = byte_405049;
  v4[byte_405055 + byte_405054] = byte_405046;
  v4[byte_40505C - byte_405057 - byte_405059] = byte_405040;
  sub_40100D(113);
  if ( byte_405054 != 228 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(115, (int)sub_401000) )
    sub_401097();
  while ( byte_405054 != 144 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(117, byte_405054) && byte_405059 )
    ;
  return v4;
}
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

//----- (00402EE4) --------------------------------------------------------
_BYTE *sub_402EE4()
{
  return sub_402489();
}

//----- (00402EF5) --------------------------------------------------------
_BYTE *sub_402EF5()
{
  return sub_402EE4();
}

//----- (00402F06) --------------------------------------------------------
_BYTE *sub_402F06()
{
  return sub_402EF5();
}

//----- (00402F17) --------------------------------------------------------
_BYTE *sub_402F17()
{
  return sub_402F06();
}

//----- (00402F28) --------------------------------------------------------
_BYTE *sub_402F28()
{
  return sub_402F17();
}

//----- (00402F39) --------------------------------------------------------
_BYTE *sub_402F39()
{
  return sub_402F28();
}

//----- (00402F4A) --------------------------------------------------------
_BYTE *sub_402F4A()
{
  return sub_402F39();
}

//----- (00402F5B) --------------------------------------------------------
_BYTE *sub_402F5B()
{
  return sub_402F4A();
}

//----- (00402F6C) --------------------------------------------------------
_BYTE *sub_402F6C()
{
  return sub_402F5B();
}

//----- (00402F7D) --------------------------------------------------------
_BYTE *sub_402F7D()
{
  return sub_402F6C();
}

//----- (00402F8E) --------------------------------------------------------
_BYTE *sub_402F8E()
{
  return sub_402F7D();
}

//----- (00402F9F) --------------------------------------------------------
_BYTE *sub_402F9F()
{
  return sub_402F8E();
}

//----- (00402FB0) --------------------------------------------------------
_BYTE *sub_402FB0()
{
  return sub_402F9F();
}

//----- (00402FC1) --------------------------------------------------------
_BYTE *sub_402FC1()
{
  return sub_402FB0();
}

//----- (00402FD2) --------------------------------------------------------
_BYTE *sub_402FD2()
{
  return sub_402FC1();
}

//----- (00402FE3) --------------------------------------------------------
_BYTE *sub_402FE3()
{
  return sub_402FD2();
}

//----- (00402FF4) --------------------------------------------------------
_BYTE *sub_402FF4()
{
  return sub_402FE3();
}

//----- (00403005) --------------------------------------------------------
BOOL __cdecl sub_403005(int a1, char *Format)
{
  int v2; // eax
  HANDLE v3; // eax
  int v4; // eax
  HANDLE hConsoleOutput; // [esp+0h] [ebp-410h]
  char Buffer[1024]; // [esp+10h] [ebp-400h] BYREF

  sub_40100D(119);
  if ( byte_405054 != 240 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(121, (int)sub_401000) )
    sub_401097();
  while ( byte_405054 != 150 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(123, byte_405054) && byte_405059 )
    ;
  sub_403DEF(
    Buffer,
    (203 * byte_405054
   + byte_405058 * (byte_405056 << 10) / (unsigned int)byte_40505C
   + byte_405058 * byte_405056 * (byte_405055 << 10) / (unsigned int)byte_40505C) >> 1,
    Format,
    (va_list)&Format
  + ((((202 * byte_405054
      + byte_405058 * 4 * byte_405056 / (unsigned int)byte_40505C
      + byte_405058 * byte_405056 * 4 * byte_405055 / (unsigned int)byte_40505C) >> 1)
    + 3) & 0xFFFFFFFC));
  v2 = sub_40100D(-11);
  v3 = sub_40153A(v2);
  hConsoleOutput = (HANDLE)sub_40100D((int)v3);
  sub_401000();
  v4 = sub_401DB6(Buffer);
  return sub_401511(hConsoleOutput, Buffer, v4, 0, 0);
}
// 405054: using guessed type char byte_405054;
// 405055: using guessed type char byte_405055;
// 405056: using guessed type char byte_405056;
// 405057: using guessed type char byte_405057;
// 405058: using guessed type char byte_405058;
// 405059: using guessed type char byte_405059;
// 40505C: using guessed type char byte_40505C;

//----- (00403299) --------------------------------------------------------
char *sub_403299()
{
  __asm { cpuid }
  return aSprintf;
}

//----- (00403317) --------------------------------------------------------
char *sub_403317()
{
  __asm { cpuid }
  return aStrcpy;
}

//----- (004033BF) --------------------------------------------------------
char *sub_4033BF()
{
  __asm { cpuid }
  return aRealloc;
}

//----- (004035CE) --------------------------------------------------------
int sub_4035CE()
{
  int v1; // eax
  _BYTE *v2; // eax
  char *v3; // eax
  _BYTE *v4; // eax
  char *v5; // eax
  int v6; // eax
  int v8; // [esp+0h] [ebp-24h]
  int v9; // [esp+4h] [ebp-20h]
  FARPROC v10; // [esp+8h] [ebp-1Ch]
  HMODULE v11; // [esp+Ch] [ebp-18h]
  FARPROC v12; // [esp+10h] [ebp-14h]
  HMODULE hModule; // [esp+14h] [ebp-10h]
  char *Block; // [esp+18h] [ebp-Ch]
  int i; // [esp+20h] [ebp-4h]

  for ( i = 0; i < 5; ++i )
  {
    v1 = sub_40100D(256);
    Block = (char *)malloc(v1);
    v2 = sub_402FF4();
    hModule = (HMODULE)sub_40225D((int)v2);
    v3 = sub_403317();
    v12 = sub_40155C(hModule, v3);
    ((void (*)(char *, const char *, ...))v12)(Block, aDHelloWorld);
    v4 = sub_402FF4();
    v11 = (HMODULE)sub_40225D((int)v4);
    v5 = sub_403299();
    v10 = sub_40155C(v11, v5);
    ((void (__cdecl *)(char *, char *, int))v10)(Block, Block, i + 1);
    if ( byte_405054 != 262
      && byte_405055
      && byte_405056 > sub_401097()
      && sub_4010EF(132, (int)Block)
      && (sub_401097() || byte_405054) )
    {
      do
      {
        sub_401000();
        sub_403005(134, Block);
      }
      while ( byte_405054 != 164 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(137, 0) && byte_405059 );
    }
    else if ( byte_405054 != 278
           && byte_405055
           && byte_405056 > sub_401097()
           && sub_4010EF(140, 0)
           && (sub_401097() || byte_405054) )
    {
      sub_401000();
    }
    else if ( byte_405054 != 288
           && byte_405055
           && byte_405056 > sub_401097()
           && sub_4010EF(145, 0)
           && (sub_401097() || byte_405054) )
    {
      sub_40100D(byte_405057 - 146);
    }
    else if ( byte_405054 != 296
           && byte_405055
           && byte_405056 > sub_401097()
           && sub_4010EF(149, 441 * byte_405054 + byte_40505A + byte_405057 - byte_40505D)
           && (sub_401097() || byte_405054) )
    {
      sub_40100D(151);
      if ( byte_405054 != 304 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(153, (int)sub_401000) )
        sub_401097();
      while ( byte_405054 != 182
           && byte_40505C > byte_405057
           && sub_401097()
           && sub_4010EF(155, byte_405054)
           && byte_405059 )
        ;
    }
    else if ( byte_405054 != 314
           && byte_405055
           && byte_405056 > sub_401097()
           && ((v9 = 468 * byte_405054 + byte_40505A + byte_405057 - byte_40505D, sub_40100D(byte_405057))
             ? (v6 = byte_405056)
             : (v6 = byte_405054),
               sub_4010EF(158, v6 * v9) && (sub_401097() || byte_405054)) )
    {
      sub_40100D(160);
      if ( byte_405054 != 322 && byte_405055 && byte_405056 > sub_401097() && sub_4010EF(162, (int)sub_401000) )
        sub_401097();
      while ( byte_405054 != 191
           && byte_40505C > byte_405057
           && sub_401097()
           && sub_4010EF(164, byte_405054)
           && byte_405059 )
        ;
    }
    else
    {
      do
      {
        sub_401000();
        sub_403005(166, Format);
      }
      while ( byte_405054 != 196 && byte_40505C > byte_405057 && sub_401097() && sub_4010EF(169, 0) && byte_405059 );
    }
    free(Block);
    if ( i )
      sub_40100D(140 * (((byte_405054 << 8) + byte_40505B + byte_40505D) / byte_40505C - byte_405055));
  }
  v8 = getch();
  return sub_40100D(510 * byte_405054 + byte_40505A + byte_405057 - byte_40505D + v8);
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

//----- (00403DEF) --------------------------------------------------------
int __cdecl sub_403DEF(char *Buffer, size_t BufferCount, char *Format, va_list ArgList)
{
  return vsnprintf(Buffer, BufferCount, Format, ArgList);
}

//----- (00403E14) --------------------------------------------------------
void __noreturn start()
{
  int v0; // eax
  char v1; // [esp+0h] [ebp-24h]
  int v2; // [esp+8h] [ebp-1Ch] BYREF
  int v3[6]; // [esp+Ch] [ebp-18h] BYREF

  sub_403F89(v3, v1);
  v2 = 0;
  _set_app_type(_crt_console_app);
  controlfp(0x10000u, 0x30000u);
  _getmainargs(_argc, _argv, environ, dword_4053EC, &v2);
  v0 = sub_4035CE();
  exit(v0);
}
// 403E22: variable 'v1' is possibly undefined
// 403E14: using guessed type void __noreturn start();
// 404088: using guessed type int __cdecl _getmainargs(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 4053EC: using guessed type int dword_4053EC;
// 403E14: using guessed type int var_18[6];

//----- (00403F44) --------------------------------------------------------
int __usercall sub_403F44@<eax>(int a1@<ebp>)
{
  return *(_DWORD *)(a1 - 20);
}

//----- (00403F48) --------------------------------------------------------
int __usercall sub_403F48@<eax>(int a1@<ebp>)
{
  return **(_DWORD **)sub_403F44(a1);
}

//----- (00403F89) --------------------------------------------------------
int __cdecl sub_403F89(_DWORD *a1, char a2)
{
  int result; // eax

  *a1 = &a2;
  a1[1] = 0;
  a1[2] = NtCurrentTeb()->NtTib.ExceptionList;
  a1[3] = &sub_403F84;
  a1[4] = dword_403F78;
  result = 0;
  a1[5] = 0;
  return result;
}
// 403F78: using guessed type int dword_403F78[3];

// nfuncs=77 queued=47 decompiled=47 lumina nreq=0 worse=0 better=0
// ALL OK, 47 function(s) have been successfully decompiled
