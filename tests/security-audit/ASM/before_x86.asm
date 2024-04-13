
; ---------------------------------------------------------------------------

; enum _crt_app_type, copyof_1
_crt_unknown_app  = 0
_crt_console_app  = 1
_crt_gui_app     = 2

                .686p
                .mmx
                .model flat

; ===========================================================================

; Segment type: Pure code
; Segment permissions: Read/Execute
_text           segment para public 'CODE' use32
                assume cs:_text
                ;org 401000h
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

; int __cdecl sub_401000(_DWORD, _DWORD, _DWORD)
sub_401000      proc near               ; CODE XREF: start+8F↓p
                                        ; .text:004011A7↓p

Destination     = dword ptr -8
var_4           = dword ptr -4

                push    ebp
                mov     ebp, esp
                sub     esp, 8
                nop
                mov     eax, 0
                mov     [ebp+var_4], eax
                mov     eax, 0
                mov     [ebp+var_4], eax

loc_40101A:                             ; CODE XREF: sub_401000+34↓j
                mov     eax, [ebp+var_4]
                cmp     eax, 5
                jge     loc_4010A7
                jmp     loc_401036
; ---------------------------------------------------------------------------

loc_40102B:                             ; CODE XREF: sub_401000+A5↓j
                mov     eax, [ebp+var_4]
                mov     ecx, eax
                inc     eax
                mov     [ebp+var_4], eax
                jmp     short loc_40101A
; ---------------------------------------------------------------------------

loc_401036:                             ; CODE XREF: sub_401000+26↑j
                mov     eax, 100h
                push    eax             ; Size
                call    malloc
                add     esp, 4
                mov     [ebp+Destination], eax
                mov     eax, offset Source ; "%d) Hello, world!\n"
                push    eax             ; Source
                mov     eax, [ebp+Destination]
                push    eax             ; Destination
                call    strcpy
                add     esp, 8
                mov     eax, [ebp+var_4]
                inc     eax
                push    eax
                mov     eax, [ebp+Destination]
                push    eax             ; Format
                mov     eax, [ebp+Destination]
                push    eax             ; Buffer
                call    sprintf
                add     esp, 0Ch
                mov     eax, [ebp+Destination]
                cmp     eax, 0
                jz      loc_40108B
                mov     eax, [ebp+Destination]
                push    eax             ; Format
                call    printf
                add     esp, 4
                jmp     loc_401099
; ---------------------------------------------------------------------------

loc_40108B:                             ; CODE XREF: sub_401000+74↑j
                mov     eax, offset Format ; "Error!"
                push    eax             ; Format
                call    printf
                add     esp, 4

loc_401099:                             ; CODE XREF: sub_401000+86↑j
                mov     eax, [ebp+Destination]
                push    eax             ; Block
                call    free
                add     esp, 4
                jmp     short loc_40102B
; ---------------------------------------------------------------------------

loc_4010A7:                             ; CODE XREF: sub_401000+20↑j
                call    _getch
                leave
                retn
sub_401000      endp

; ---------------------------------------------------------------------------
                align 10h

; =============== S U B R O U T I N E =======================================

; Attributes: noreturn bp-based frame

                public start
start           proc near

var_24          = dword ptr -24h
var_20          = dword ptr -20h
var_1C          = dword ptr -1Ch
var_18          = byte ptr -18h

                push    ebp
                mov     ebp, esp
                sub     esp, 24h
                nop
                lea     eax, [ebp+var_18]
                push    eax
                call    sub_401225
                add     esp, 4
                mov     eax, 0
                mov     [ebp+var_1C], eax
                mov     eax, 1
                push    eax             ; Type
                call    __set_app_type
                add     esp, 4
                mov     eax, 30000h
                push    eax             ; Mask
                mov     eax, 10000h
                push    eax             ; NewValue
                call    _controlfp
                add     esp, 8
                mov     eax, ds:__argc
                mov     ecx, ds:__argv
                mov     edx, ds:_environ
                mov     [ebp+var_20], eax
                lea     eax, [ebp+var_1C]
                push    eax
                mov     eax, dword_40218C
                push    eax
                push    edx
                push    ecx
                mov     eax, [ebp+var_20]
                push    eax
                call    __getmainargs
                add     esp, 14h
                mov     eax, ds:__argc
                mov     ecx, ds:__argv
                mov     edx, ds:_environ
                mov     [ebp+var_24], eax
                mov     eax, [edx]
                push    eax
                mov     eax, [ecx]
                push    eax
                mov     eax, [ebp+var_24]
                mov     eax, [eax]
                push    eax
                call    sub_401000
                add     esp, 0Ch
                push    eax             ; Code
                call    exit
start           endp

; ---------------------------------------------------------------------------
                add     esp, 4
                leave
                retn
; ---------------------------------------------------------------------------
                push    ebp
                mov     ebp, esp
                sub     esp, 4
                nop
                mov     eax, ds:__argc
                mov     ecx, [ebp+8]
                mov     [eax], ecx
                mov     eax, ds:__argv
                mov     ecx, [ebp+0Ch]
                mov     [eax], ecx
                mov     eax, 30000h
                push    eax
                mov     eax, 10000h
                push    eax
                call    _controlfp
                add     esp, 8
                mov     eax, ds:__argc
                mov     ecx, ds:__argv
                mov     edx, ds:_environ
                mov     [ebp-4], eax
                mov     eax, [edx]
                push    eax
                mov     eax, [ecx]
                push    eax
                mov     eax, [ebp-4]
                mov     eax, [eax]
                push    eax
                call    sub_401000
                add     esp, 0Ch
                leave
                retn
; ---------------------------------------------------------------------------
                align 4
                xchg    ebp, [esp]
                push    ebp
                lea     ebp, [esp+4]
                push    ecx
                mov     ecx, ebp

loc_4011BF:                             ; CODE XREF: .text:004011D1↓j
                sub     ecx, 1000h
                test    [ecx], eax
                sub     eax, 1000h
                cmp     eax, 1000h
                jge     short loc_4011BF
                sub     ecx, eax
                test    [ecx], eax
                mov     eax, esp
                mov     esp, ecx
                mov     ecx, [eax]
                jmp     dword ptr [eax+4]

; =============== S U B R O U T I N E =======================================


sub_4011E0      proc near               ; CODE XREF: sub_4011E4↓p
                                        ; .text:004011EE↓p
                mov     eax, [ebp-14h]
                retn
sub_4011E0      endp


; =============== S U B R O U T I N E =======================================


sub_4011E4      proc near               ; CODE XREF: .text:004011F4↓p
                call    sub_4011E0
                mov     eax, [eax]
                mov     eax, [eax]
                retn
sub_4011E4      endp

; ---------------------------------------------------------------------------
                call    sub_4011E0
                push    eax
                call    sub_4011E4
                push    eax
                call    _XcptFilter
                add     esp, 8
                retn
; ---------------------------------------------------------------------------
                db 8Bh
                dd 0D9E8E865h, 50FFFFFFh, 0A7E8h, 0
dword_401214    dd 0FFFFFFFFh, 4011EEh, 401203h
                                        ; DATA XREF: sub_401225+22↓o
; ---------------------------------------------------------------------------

loc_401220:                             ; DATA XREF: sub_401225+1A↓o
                jmp     loc_4012C0

; =============== S U B R O U T I N E =======================================


; _DWORD __cdecl sub_401225(_DWORD)
sub_401225      proc near               ; CODE XREF: start+E↑p

arg_0           = dword ptr  4
arg_4           = byte ptr  8

                push    ebp
                mov     ebp, [esp+4+arg_0]
                lea     eax, [esp+4+arg_4]
                mov     [ebp+0], eax
                xor     eax, eax
                mov     [ebp+4], eax
                mov     eax, large fs:0
                mov     [ebp+8], eax
                mov     eax, offset loc_401220
                mov     [ebp+0Ch], eax
                mov     eax, offset dword_401214
                mov     [ebp+10h], eax
                xor     eax, eax
                mov     [ebp+14h], eax
                lea     eax, [ebp+8]
                mov     large fs:0, eax
                pop     ebp
                retn
sub_401225      endp

; ---------------------------------------------------------------------------
                align 10h
; [00000006 BYTES: COLLAPSED FUNCTION malloc. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 4
; [00000006 BYTES: COLLAPSED FUNCTION strcpy. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 10h
; [00000006 BYTES: COLLAPSED FUNCTION sprintf. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 4
; [00000006 BYTES: COLLAPSED FUNCTION printf. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 10h
; [00000006 BYTES: COLLAPSED FUNCTION free. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 4
; [00000006 BYTES: COLLAPSED FUNCTION _getch. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 10h
; [00000006 BYTES: COLLAPSED FUNCTION __set_app_type. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 4
; [00000006 BYTES: COLLAPSED FUNCTION _controlfp. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 10h
; [00000006 BYTES: COLLAPSED FUNCTION __getmainargs. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 4
; [00000006 BYTES: COLLAPSED FUNCTION exit. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 10h
; [00000006 BYTES: COLLAPSED FUNCTION _XcptFilter. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 4
; [00000006 BYTES: COLLAPSED FUNCTION _exit. PRESS CTRL-NUMPAD+ TO EXPAND]
                align 10h

loc_4012C0:                             ; CODE XREF: .text:loc_401220↑j
                jmp     ds:_except_handler3
; ---------------------------------------------------------------------------
                align 200h
                dd 300h dup(?)
_text           ends

; Section 2. (virtual address 00002000)
; Virtual size                  : 00000190 (    400.)
; Section size in file          : 00000200 (    512.)
; Offset to raw data for section: 00000600
; Flags C0000040: Data Readable Writable
; Alignment     : default
; ===========================================================================

; Segment type: Pure data
; Segment permissions: Read/Write
_data           segment para public 'DATA' use32
                assume cs:_data
                ;org 402000h
; char Source[]
Source          db '%d) Hello, world!',0Ah,0
                                        ; DATA XREF: sub_401000+47↑o
; char Format[]
Format          db 'Error!',0           ; DATA XREF: sub_401000:loc_40108B↑o
                align 10h
__IMPORT_DESCRIPTOR_msvcrt dd rva off_40208C ; Import Name Table
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
_data           ends

;
; Imports from msvcrt.dll
;
; ===========================================================================

; Segment type: Externs
; _idata
; void *(__cdecl *malloc)(size_t Size)
                extrn __imp_malloc:dword ; DATA XREF: malloc↑r
                                        ; .data:00402030↑o
; char *(__cdecl *strcpy)(char *Destination, const char *Source)
                extrn __imp_strcpy:dword ; DATA XREF: strcpy↑r
; int (*sprintf)(char *const Buffer, const char *const Format, ...)
                extrn __imp_sprintf:dword ; DATA XREF: sprintf↑r
; int (*printf)(const char *const Format, ...)
                extrn __imp_printf:dword ; DATA XREF: printf↑r
; void (__cdecl *free)(void *Block)
                extrn __imp_free:dword  ; DATA XREF: free↑r
; int (__cdecl *_getch)()
                extrn __imp__getch:dword ; DATA XREF: _getch↑r
; void (__cdecl *__set_app_type)(_crt_app_type Type)
                extrn __imp___set_app_type:dword
                                        ; DATA XREF: __set_app_type↑r
; unsigned int (__cdecl *_controlfp)(unsigned int NewValue, unsigned int Mask)
                extrn __imp__controlfp:dword ; DATA XREF: _controlfp↑r
; int _argc
                extrn __argc:dword      ; DATA XREF: start+40↑r
                                        ; start+6E↑r ...
; char **_argv
                extrn __argv:dword      ; DATA XREF: start+46↑r
                                        ; start+74↑r ...
; char **environ
                extrn _environ:dword    ; DATA XREF: start+4C↑r
                                        ; start+7A↑r ...
                extrn __imp___getmainargs:dword
                                        ; DATA XREF: __getmainargs↑r
; void (__cdecl __noreturn *exit)(int Code)
                extrn __imp_exit:dword  ; DATA XREF: exit↑r
                extrn __imp__XcptFilter:dword ; DATA XREF: _XcptFilter↑r
; void (__cdecl __noreturn *_exit)(int Code)
                extrn __imp__exit:dword ; DATA XREF: _exit↑r
                extrn _except_handler3:dword
                                        ; DATA XREF: .text:loc_4012C0↑r


;
; Import names for msvcrt.dll
;
; ===========================================================================

; Segment type: Pure data
; Segment permissions: Read/Write
_data           segment para public 'DATA' use32
                assume cs:_data
                ;org 40208Ch
off_40208C      dd rva word_4020DB      ; DATA XREF: .data:__IMPORT_DESCRIPTOR_msvcrt↑o
                dd rva word_4020E4
                dd rva word_4020ED
                dd rva word_4020F7
                dd rva word_402100
                dd rva word_402107
                dd rva word_402110
                dd rva word_402121
                dd rva word_40212E
                dd rva word_402137
                dd rva word_402140
                dd rva word_40214B
                dd rva word_40215B
                dd rva word_402162
                dd rva word_402170
                dd rva word_402178
                dd 0
aMsvcrtDll      db 'msvcrt.dll',0       ; DATA XREF: .data:0040202C↑o
word_4020DB     dw 0                    ; DATA XREF: .data:off_40208C↑o
                db 'malloc',0
word_4020E4     dw 0                    ; DATA XREF: .data:00402090↑o
                db 'strcpy',0
word_4020ED     dw 0                    ; DATA XREF: .data:00402094↑o
                db 'sprintf',0
word_4020F7     dw 0                    ; DATA XREF: .data:00402098↑o
                db 'printf',0
word_402100     dw 0                    ; DATA XREF: .data:0040209C↑o
                db 'free',0
word_402107     dw 0                    ; DATA XREF: .data:004020A0↑o
                db '_getch',0
word_402110     dw 0                    ; DATA XREF: .data:004020A4↑o
                db '__set_app_type',0
word_402121     dw 0                    ; DATA XREF: .data:004020A8↑o
                db '_controlfp',0
word_40212E     dw 0                    ; DATA XREF: .data:004020AC↑o
                db '__argc',0
word_402137     dw 0                    ; DATA XREF: .data:004020B0↑o
                db '__argv',0
word_402140     dw 0                    ; DATA XREF: .data:004020B4↑o
                db '_environ',0
word_40214B     dw 0                    ; DATA XREF: .data:004020B8↑o
                db '__getmainargs',0
word_40215B     dw 0                    ; DATA XREF: .data:004020BC↑o
                db 'exit',0
word_402162     dw 0                    ; DATA XREF: .data:004020C0↑o
                db '_XcptFilter',0
word_402170     dw 0                    ; DATA XREF: .data:004020C4↑o
                db '_exit',0
word_402178     dw 0                    ; DATA XREF: .data:004020C8↑o
                db '_except_handler3',0
                align 4
dword_40218C    dd 0                    ; DATA XREF: start+59↑r
                align 1000h
_data           ends


                end start
