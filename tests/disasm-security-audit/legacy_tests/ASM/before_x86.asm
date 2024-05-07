_crt_unknown_app  = 0
_crt_console_app  = 1
_crt_gui_app     = 2

                .686p
                .mmx
                .model flat

_text           segment para public 'CODE' use32
                assume cs:_text

                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

sub_401000      proc near               

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

loc_40101A:                             
                mov     eax, [ebp+var_4]
                cmp     eax, 5
                jge     loc_4010A7
                jmp     loc_401036

loc_40102B:                             
                mov     eax, [ebp+var_4]
                mov     ecx, eax
                inc     eax
                mov     [ebp+var_4], eax
                jmp     short loc_40101A

loc_401036:                             
                mov     eax, 100h
                push    eax             
                call    malloc
                add     esp, 4
                mov     [ebp+Destination], eax
                mov     eax, offset Source 
                push    eax             
                mov     eax, [ebp+Destination]
                push    eax             
                call    strcpy
                add     esp, 8
                mov     eax, [ebp+var_4]
                inc     eax
                push    eax
                mov     eax, [ebp+Destination]
                push    eax             
                mov     eax, [ebp+Destination]
                push    eax             
                call    sprintf
                add     esp, 0Ch
                mov     eax, [ebp+Destination]
                cmp     eax, 0
                jz      loc_40108B
                mov     eax, [ebp+Destination]
                push    eax             
                call    printf
                add     esp, 4
                jmp     loc_401099

loc_40108B:                             
                mov     eax, offset Format 
                push    eax             
                call    printf
                add     esp, 4

loc_401099:                             
                mov     eax, [ebp+Destination]
                push    eax             
                call    free
                add     esp, 4
                jmp     short loc_40102B

loc_4010A7:                             
                call    _getch
                leave
                retn
sub_401000      endp

                align 10h

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
                push    eax             
                call    __set_app_type
                add     esp, 4
                mov     eax, 30000h
                push    eax             
                mov     eax, 10000h
                push    eax             
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
                push    eax             
                call    exit
start           endp

                add     esp, 4
                leave
                retn

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

                align 4
                xchg    ebp, [esp]
                push    ebp
                lea     ebp, [esp+4]
                push    ecx
                mov     ecx, ebp

loc_4011BF:                             
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

sub_4011E0      proc near               

                mov     eax, [ebp-14h]
                retn
sub_4011E0      endp

sub_4011E4      proc near               
                call    sub_4011E0
                mov     eax, [eax]
                mov     eax, [eax]
                retn
sub_4011E4      endp

                call    sub_4011E0
                push    eax
                call    sub_4011E4
                push    eax
                call    _XcptFilter
                add     esp, 8
                retn

                db 8Bh
                dd 0D9E8E865h, 50FFFFFFh, 0A7E8h, 0
dword_401214    dd 0FFFFFFFFh, 4011EEh, 401203h

loc_401220:                             
                jmp     loc_4012C0

sub_401225      proc near               

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

                align 10h

                align 4

                align 10h

                align 4

                align 10h

                align 4

                align 10h

                align 4

                align 10h

                align 4

                align 10h

                align 4

                align 10h

loc_4012C0:                             
                jmp     ds:_except_handler3

                align 200h
                dd 300h dup(?)
_text           ends

_data           segment para public 'DATA' use32
                assume cs:_data

Source          db '%d) Hello, world!',0Ah,0

Format          db 'Error!',0           
                align 10h
__IMPORT_DESCRIPTOR_msvcrt dd rva off_40208C 
                dd 0                    
                dd 0                    
                dd rva aMsvcrtDll       
                dd rva __imp_malloc     
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

                extrn __imp_malloc:dword 

                extrn __imp_strcpy:dword 

                extrn __imp_sprintf:dword 

                extrn __imp_printf:dword 

                extrn __imp_free:dword  

                extrn __imp__getch:dword 

                extrn __imp___set_app_type:dword

                extrn __imp__controlfp:dword 

                extrn __argc:dword      

                extrn __argv:dword      

                extrn _environ:dword    

                extrn __imp___getmainargs:dword

                extrn __imp_exit:dword  
                extrn __imp__XcptFilter:dword 

                extrn __imp__exit:dword 
                extrn _except_handler3:dword

_data           segment para public 'DATA' use32
                assume cs:_data

off_40208C      dd rva word_4020DB      
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
aMsvcrtDll      db 'msvcrt.dll',0       
word_4020DB     dw 0                    
                db 'malloc',0
word_4020E4     dw 0                    
                db 'strcpy',0
word_4020ED     dw 0                    
                db 'sprintf',0
word_4020F7     dw 0                    
                db 'printf',0
word_402100     dw 0                    
                db 'free',0
word_402107     dw 0                    
                db '_getch',0
word_402110     dw 0                    
                db '__set_app_type',0
word_402121     dw 0                    
                db '_controlfp',0
word_40212E     dw 0                    
                db '__argc',0
word_402137     dw 0                    
                db '__argv',0
word_402140     dw 0                    
                db '_environ',0
word_40214B     dw 0                    
                db '__getmainargs',0
word_40215B     dw 0                    
                db 'exit',0
word_402162     dw 0                    
                db '_XcptFilter',0
word_402170     dw 0                    
                db '_exit',0
word_402178     dw 0                    
                db '_except_handler3',0
                align 4
dword_40218C    dd 0                    
                align 1000h
_data           ends

                end start