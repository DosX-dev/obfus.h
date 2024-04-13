
RUNTIME_FUNCTION struc ; (sizeof=0xC, mappedto_1)
                        ; XREF: .pdata:ExceptionDir/r
                        ; .pdata:000000000040300C/r ...
FunctionStart dd ?      ; offset rva
FunctionEnd dd ?        ; offset rva pastend
UnwindInfo dd ?         ; offset rva
RUNTIME_FUNCTION ends


UNWIND_INFO_HDR struc ; (sizeof=0x4, mappedto_2)
                        ; XREF: .text:stru_4010E0/r
                        ; .text:stru_4011B0/r
Ver3_Flags db ?         ; base 16
PrologSize db ?         ; base 16
CntUnwindCodes db ?     ; base 16
FrReg_FrRegOff db ?     ; base 16
UNWIND_INFO_HDR ends


UNWIND_CODE struc ; (sizeof=0x2, mappedto_3)
                        ; XREF: .text:00000000004010E4/r
                        ; .text:00000000004010E6/r ...
PrologOff db ?          ; base 16
OpCode_OpInfo db ?      ; base 16
UNWIND_CODE ends

.686p
.mmx
.model flat


; Segment type: Pure code
; Segment permissions: Read/Execute
_text segment para public 'CODE' use64
assume cs:_text
;org 401000h
assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing


; Attributes: bp-based frame

sub_401000 proc near

Destination= qword ptr -10h
var_4= dword ptr -4

push    rbp
mov     rbp, rsp
sub     rsp, 30h

loc_40100B:
mov     eax, 0
mov     [rbp+var_4], eax
mov     eax, 0
mov     [rbp+var_4], eax

loc_40101B:
mov     eax, [rbp+var_4]
cmp     eax, 5
jge     loc_4010D7
jmp     loc_40103A

loc_40102C:
mov     eax, [rbp+var_4]
mov     rcx, rax
add     eax, 1
mov     [rbp+var_4], eax
jmp     short loc_40101B

loc_40103A:
mov     eax, 100h
mov     r10, rax
mov     rcx, r10        ; Size
call    malloc
movsxd  rax, eax
mov     [rbp+Destination], rax
lea     rax, Source     ; "%d) Hello, world!\n"
mov     r11, rax
mov     rax, [rbp+Destination]
mov     r10, rax
mov     rcx, r10        ; Destination
mov     rdx, r11        ; Source
call    strcpy
mov     eax, [rbp+var_4]
add     eax, 1
mov     r8, rax
mov     rax, [rbp+Destination]
mov     r11, rax
mov     rax, [rbp+Destination]
mov     r10, rax
mov     rcx, r10        ; Buffer
mov     rdx, r11        ; Format
call    sprintf
mov     rax, [rbp+Destination]
cmp     rax, 0
jz      loc_4010B1
mov     rax, [rbp+Destination]
mov     r10, rax
mov     rcx, r10        ; Format
call    printf
jmp     loc_4010C3

loc_4010B1:
lea     rax, Format     ; "Error!"
mov     r10, rax
mov     rcx, r10        ; Format
call    printf

loc_4010C3:
mov     rax, [rbp+Destination]
mov     r10, rax
mov     rcx, r10        ; Block
call    free
jmp     loc_40102C

loc_4010D7:
call    _getch
leave
retn
sub_401000 endp

algn_4010DE:
align 20h
stru_4010E0 UNWIND_INFO_HDR <1, 4, 2, 5>
UNWIND_CODE <4, 3>      ; UWOP_SET_FPREG
UNWIND_CODE <1, 50h>    ; UWOP_PUSH_NONVOL


; Attributes: noreturn bp-based frame

public start
start proc near

var_30= qword ptr -30h
var_18= qword ptr -18h
var_10= qword ptr -10h
var_4= dword ptr -4

push    rbp
mov     rbp, rsp
sub     rsp, 50h

loc_4010F3:
mov     eax, 0
mov     [rbp+var_4], eax
mov     eax, 1
mov     r10, rax
mov     rcx, r10        ; Type
call    __set_app_type
mov     eax, 30000h
mov     r11, rax
mov     eax, 10000h
mov     r10, rax
mov     rcx, r10        ; NewValue
mov     rdx, r11        ; Mask
call    _controlfp
mov     rax, cs:__argc
mov     rcx, cs:__argv
mov     rdx, cs:_environ
mov     [rbp+var_10], rax
lea     rax, [rbp+var_4]
mov     [rsp+50h+var_30], rax
mov     eax, cs:dword_4021C0
mov     r9, rax
mov     r8, rdx
mov     r11, rcx
mov     rax, [rbp+var_10]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    __getmainargs
mov     rax, cs:__argc
mov     rcx, cs:__argv
mov     rdx, cs:_environ
mov     [rbp+var_18], rax
mov     rax, [rdx]
mov     r8, rax
mov     rax, [rcx]
mov     r11, rax
mov     rax, [rbp+var_18]
mov     eax, [rax]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401000
mov     r10, rax
mov     rcx, r10        ; Code
call    exit
start endp

leave
retn
algn_4011AF:
align 10h
stru_4011B0 UNWIND_INFO_HDR <1, 4, 2, 5>
UNWIND_CODE <4, 3>      ; UWOP_SET_FPREG
UNWIND_CODE <1, 50h>    ; UWOP_PUSH_NONVOL
push    rbp
mov     rbp, rsp
sub     rsp, 30h



sub_4011C3 proc near
mov     [rbp+10h], rcx
mov     [rbp+18h], rdx
mov     rax, cs:__argc
mov     ecx, [rbp+10h]
mov     [rax], ecx
mov     rax, cs:__argv
mov     rcx, [rbp+18h]
mov     [rax], rcx
mov     eax, 30000h
mov     r11, rax
mov     eax, 10000h
mov     r10, rax
mov     rcx, r10        ; NewValue
mov     rdx, r11        ; Mask
call    _controlfp
mov     rax, cs:__argc
mov     rcx, cs:__argv
mov     rdx, cs:_environ
mov     [rbp-8], rax
mov     rax, [rdx]
mov     r8, rax
mov     rax, [rcx]
mov     r11, rax
mov     rax, [rbp-8]
mov     eax, [rax]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401000
leave
retn
sub_4011C3 endp

algn_40123B:
align 20h
; [00000006 BYTES: COLLAPSED FUNCTION malloc. PRESS CTRL-NUMPAD+ TO EXPAND]
align 8
; [00000006 BYTES: COLLAPSED FUNCTION strcpy. PRESS CTRL-NUMPAD+ TO EXPAND]
align 10h
; [00000006 BYTES: COLLAPSED FUNCTION sprintf. PRESS CTRL-NUMPAD+ TO EXPAND]
align 8
; [00000006 BYTES: COLLAPSED FUNCTION printf. PRESS CTRL-NUMPAD+ TO EXPAND]
align 20h
; [00000006 BYTES: COLLAPSED FUNCTION free. PRESS CTRL-NUMPAD+ TO EXPAND]
align 8
; [00000006 BYTES: COLLAPSED FUNCTION _getch. PRESS CTRL-NUMPAD+ TO EXPAND]
align 10h
; [00000006 BYTES: COLLAPSED FUNCTION __set_app_type. PRESS CTRL-NUMPAD+ TO EXPAND]
align 8
; [00000006 BYTES: COLLAPSED FUNCTION _controlfp. PRESS CTRL-NUMPAD+ TO EXPAND]
align 20h
; [00000006 BYTES: COLLAPSED FUNCTION __getmainargs. PRESS CTRL-NUMPAD+ TO EXPAND]
align 8
; [00000006 BYTES: COLLAPSED FUNCTION exit. PRESS CTRL-NUMPAD+ TO EXPAND]
align 200h
dq 180h dup(?)
_text ends

; Section 2. (virtual address 00002000)
; Virtual size                  : 000001C4 (    452.)
; Section size in file          : 00000200 (    512.)
; Offset to raw data for section: 00000600
; Flags C0000040: Data Readable Writable
; Alignment     : default

; Segment type: Pure data
; Segment permissions: Read/Write
_data segment para public 'DATA' use64
assume cs:_data
;org 402000h
; char Source[]
Source db '%d) Hello, world!',0Ah,0
; char Format[]
Format db 'Error!',0
align 20h
__IMPORT_DESCRIPTOR_msvcrt dd rva off_4020B8 ; Import Name Table
dd 0                    ; Time stamp
dd 0                    ; Forwarder Chain
dd rva aMsvcrtDll       ; DLL Name
dd rva __imp_malloc     ; Import Address Table
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
db    0
_data ends

;
; Imports from msvcrt.dll
;

; Segment type: Externs
; _idata
; void *(__cdecl *malloc)(size_t Size)
extrn __imp_malloc:qword
; char *(__cdecl *strcpy)(char *Destination, const char *Source)
extrn __imp_strcpy:qword
; int (*sprintf)(char *const Buffer, const char *const Format, ...)
extrn __imp_sprintf:qword
; int (*printf)(const char *const Format, ...)
extrn __imp_printf:qword
; void (__cdecl *free)(void *Block)
extrn __imp_free:qword
; int (__cdecl *_getch)()
extrn __imp__getch:qword
; void (__cdecl *__set_app_type)(_crt_app_type Type)
extrn __imp___set_app_type:qword
; unsigned int (__cdecl *_controlfp)(unsigned int NewValue, unsigned int Mask)
extrn __imp__controlfp:qword
; int _argc
extrn __argc:qword
; char **_argv
extrn __argv:qword
; char **environ
extrn _environ:qword
extrn __imp___getmainargs:qword
; void (__cdecl __noreturn *exit)(int Code)
extrn __imp_exit:qword


;
; Import names for msvcrt.dll
;

; Segment type: Pure data
; Segment permissions: Read/Write
_data segment para public 'DATA' use64
assume cs:_data
;org 4020B8h
off_4020B8 dq rva word_402133
dq rva word_40213C
dq rva word_402145
dq rva word_40214F
dq rva word_402158
dq rva word_40215F
dq rva word_402168
dq rva word_402179
dq rva word_402186
dq rva word_40218F
dq rva word_402198
dq rva word_4021A3
dq rva word_4021B3
dq 0
aMsvcrtDll db 'msvcrt.dll',0
word_402133 dw 0
db 'malloc',0
word_40213C dw 0
db 'strcpy',0
word_402145 dw 0
db 'sprintf',0
word_40214F dw 0
db 'printf',0
word_402158 dw 0
db 'free',0
word_40215F dw 0
db '_getch',0
word_402168 dw 0
db '__set_app_type',0
word_402179 dw 0
db '_controlfp',0
word_402186 dw 0
db '__argc',0
word_40218F dw 0
db '__argv',0
word_402198 dw 0
db '_environ',0
word_4021A3 dw 0
db '__getmainargs',0
word_4021B3 dw 0
db 'exit',0
align 20h
dword_4021C0 dd 0
align 1000h
_data ends

; Section 3. (virtual address 00003000)
; Virtual size                  : 00000024 (     36.)
; Section size in file          : 00000200 (    512.)
; Offset to raw data for section: 00000800
; Flags 40000040: Data Readable
; Alignment     : default

; Segment type: Pure data
; Segment permissions: Read
_pdata segment para public 'DATA' use64
assume cs:_pdata
;org 403000h
ExceptionDir RUNTIME_FUNCTION <rva loc_40100B, \
                  rva algn_4010DE, \
                  rva stru_4010E0>
RUNTIME_FUNCTION <rva loc_4010F3, \
                  rva algn_4011AF, \
                  rva stru_4011B0>
RUNTIME_FUNCTION <rva sub_4011C3, \
                  rva algn_40123B, \
                  rva stru_4011B0>
align 1000h
_pdata ends


end start
