_crt_unknown_app  = 0
_crt_console_app  = 1
_crt_gui_app  = 2

.686p
.mmx
.model flat

_text segment para public 'CODE' use32
assume cs:_text

assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

sub_401000 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
leave
retn
sub_401000 endp

sub_40100D proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
movsx   eax, byte_405057
push    eax
movsx   eax, byte_405054
push    eax
call    sub_401000
add     esp, 8
nop
cpuid
nop
mov     eax, [ebp+arg_0]
movsx   ecx, byte_405055
imul    eax, ecx
movsx   ecx, byte_405058
movsx   edx, byte_405056
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505C
sub     eax, ecx
leave
retn
sub_40100D endp

push    ebp
mov     ebp, esp
sub     esp, 0
nop
movsx   eax, byte_40505A
push    eax
movsx   eax, byte_405056
push    eax
call    sub_401000
add     esp, 8
nop
cpuid
nop
movsx   eax, byte_405055
push    eax
fild    dword ptr [esp]
add     esp, 4
fmul    qword ptr [ebp+8]
leave
retn

sub_401097 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4010E8
movsx   eax, byte_40505D
movsx   ecx, byte_40505B
add     eax, ecx
movsx   ecx, byte_40505C
cdq
idiv    ecx
movsx   ecx, byte_405055
sub     eax, ecx
cmp     eax, 0
jz      loc_4010E8
mov     eax, 1
jmp     short locret_4010ED

loc_4010E8:
mov     eax, 0

locret_4010ED:
leave
retn
sub_401097 endp

sub_4010EF proc near

var_8= dword ptr -8
var_4= dword ptr -4
arg_0= dword ptr  8
arg_4= dword ptr  0Ch

push    ebp
mov     ebp, esp
sub     esp, 8
nop
mov     eax, [ebp+arg_4]
movsx   ecx, byte_405055
imul    eax, ecx
push    eax
call    sub_40100D
add     esp, 4
mov     [ebp+var_4], eax
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 3
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
mov     ecx, [ebp+arg_0]
imul    eax, ecx
mov     ecx, [ebp+var_4]
cmp     ecx, eax
jnz     loc_401199
movsx   eax, byte_405058
movsx   ecx, byte_405056
imul    eax, ecx
movsx   ecx, byte_40505C
sub     ecx, eax
cmp     ecx, 0
jz      loc_40118A
call    sub_401097
cmp     eax, 0
jz      loc_401183
jmp     loc_40118A

loc_401183:
mov     eax, 1
jmp     short loc_40118F

loc_40118A:
mov     eax, 0

loc_40118F:
jmp     locret_4012BD
jmp     loc_4012AD

loc_401199:
movsx   eax, byte_40505D
movsx   ecx, byte_40505B
add     eax, ecx
movsx   ecx, byte_405054
shl     ecx, 2
add     eax, ecx
movsx   ecx, byte_40505C
cdq
idiv    ecx
movsx   ecx, byte_405055
sub     eax, ecx
mov     ecx, [ebp+var_4]
cmp     ecx, eax
jnz     loc_4012AD
call    sub_401097
cmp     eax, 0
jnz     loc_401251
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 9
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
cmp     eax, 0
jnz     loc_401251
movsx   eax, byte_40505D
movsx   ecx, byte_40505B
add     eax, ecx
movsx   ecx, byte_405054
shl     ecx, 3
add     eax, ecx
movsx   ecx, byte_40505C
cdq
idiv    ecx
movsx   ecx, byte_405055
sub     eax, ecx
cmp     eax, 0
jnz     loc_401251
jmp     loc_4012A3

loc_401251:
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 0Fh
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
movsx   ecx, byte_405055
push    ecx
mov     [ebp+var_8], eax
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_8]
add     ecx, eax
cmp     ecx, 0
jz      loc_4012A3
mov     eax, 1
jmp     short loc_4012A8

loc_4012A3:
mov     eax, 0

loc_4012A8:
jmp     locret_4012BD

loc_4012AD:
nop
cpuid
nop
mov     eax, [ebp+arg_4]
push    eax
call    sub_40100D
add     esp, 4

locret_4012BD:
leave
retn
sub_4010EF endp

sub_4012BF proc near

var_10= dword ptr -10h
var_C= dword ptr -0Ch
var_8= dword ptr -8
var_4= dword ptr -4
arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 10h
nop
mov     eax, 0Eh
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4013D5
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4013D5
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_4013D5
mov     eax, [ebp+arg_0]
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40138B
movsx   eax, byte_405055
shl     eax, 4
movsx   ecx, byte_405056
imul    eax, ecx
movsx   ecx, byte_405058
imul    eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
movsx   ecx, byte_405056
shl     ecx, 4
movsx   edx, byte_405058
imul    ecx, edx
mov     [ebp+var_4], eax
mov     eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
mov     ecx, [ebp+var_4]
add     ecx, eax
movsx   eax, byte_405054
mov     edx, 54h 
imul    eax, edx
add     ecx, eax
shr     ecx, 1
mov     eax, [ebp+arg_0]
cmp     eax, ecx
jnb     loc_40138B
mov     eax, 0
jmp     short loc_401390

loc_40138B:
mov     eax, 1

loc_401390:
push    eax
mov     eax, 8
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4013D5
call    sub_401097
cmp     eax, 0
jnz     loc_4013CB
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4013CB
jmp     loc_4013D5

loc_4013CB:
mov     eax, 0
jmp     locret_40150F

loc_4013D5:
movsx   eax, byte_405055
movsx   ecx, byte_405059
imul    eax, ecx
movsx   ecx, byte_405058
sub     eax, ecx
movsx   ecx, byte_405055
add     eax, ecx
movsx   ecx, byte_405056
sub     eax, ecx
mov     [ebp+var_8], eax
mov     eax, offset unk_4053D4
mov     [ebp+var_C], eax
movsx   eax, byte_405054
mov     [ebp+var_8], eax

loc_401416:
mov     eax, [ebp+var_8]
mov     ecx, [ebp+arg_0]
cmp     eax, ecx
jge     loc_4014FC
jmp     loc_4014D5

loc_401429:
mov     eax, [ebp+var_8]
mov     ecx, eax
inc     eax
mov     [ebp+var_8], eax
cmp     ecx, 0
jz      loc_4014C0
movsx   eax, byte_40505D
movsx   ecx, byte_40505B
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 12h
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505C
cdq
idiv    ecx
movsx   ecx, byte_405055
sub     eax, ecx
mov     ecx, 15h
imul    eax, ecx
push    eax
call    sub_40100D
add     esp, 4
movsx   ecx, byte_405057
movsx   edx, byte_40505A
add     ecx, edx
movsx   edx, byte_405054
mov     [ebp+var_10], eax
mov     eax, 21h 
imul    edx, eax
add     ecx, edx
movsx   eax, byte_40505D
sub     ecx, eax
mov     eax, [ebp+var_10]
add     eax, ecx
cmp     eax, 0
jz      loc_4014C0
jmp     loc_4014D0

loc_4014C0:
movsx   eax, byte_405055
cmp     eax, 0
jnz     $+6

loc_4014D0:
jmp     loc_401416

loc_4014D5:
mov     eax, [ebp+var_C]
mov     ecx, eax
inc     eax
mov     [ebp+var_C], eax
mov     eax, 25h 
mov     [ecx], al
mov     eax, [ebp+var_C]
mov     ecx, eax
inc     eax
mov     [ebp+var_C], eax
movsx   eax, byte_405036
mov     [ecx], al
jmp     loc_401429

loc_4014FC:
mov     eax, [ebp+var_C]
mov     ecx, 0
mov     [eax], cl
nop
cpuid
nop
mov     eax, offset unk_4053D4

locret_40150F:
leave
retn
sub_4012BF endp

sub_401511 proc near

hConsoleOutput= dword ptr  8
lpBuffer= dword ptr  0Ch
nNumberOfCharsToWrite= dword ptr  10h
lpNumberOfCharsWritten= dword ptr  14h
lpReserved= dword ptr  18h

push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, [ebp+lpReserved]
push    eax             
mov     eax, [ebp+lpNumberOfCharsWritten]
push    eax             
mov     eax, [ebp+nNumberOfCharsToWrite]
push    eax             
mov     eax, [ebp+lpBuffer]
push    eax             
mov     eax, [ebp+hConsoleOutput]
push    eax             
call    WriteConsoleA
leave
retn
sub_401511 endp

sub_40153A proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, [ebp+arg_0]
push    eax
call    sub_40100D
add     esp, 4
push    eax             
call    GetStdHandle
leave
retn
sub_40153A endp

sub_40155C proc near

hModule= dword ptr  8
lpProcName= dword ptr  0Ch

push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, [ebp+lpProcName]
push    eax             
mov     eax, [ebp+hModule]
push    eax             
call    GetProcAddress
leave
retn    8
sub_40155C endp

sub_40157B proc near

lpModuleName= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, [ebp+lpModuleName]
push    eax             
call    GetModuleHandleA
leave
retn
sub_40157B endp

push    ebp
mov     ebp, esp
sub     esp, 0Ch
nop

loc_40159E:
mov     eax, 28h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401DA9
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_401DA9
call    sub_401097
cmp     eax, 0
jz      loc_401DA9
mov     eax, [ebp+8]
movsx   ecx, byte ptr [eax]
cmp     ecx, 0
jnz     loc_4015FB
mov     eax, [ebp+0Ch]
movsx   ecx, byte ptr [eax]
cmp     ecx, 0
jnz     loc_4015FB
mov     eax, 0
jmp     short loc_401600

loc_4015FB:
mov     eax, 1

loc_401600:
push    eax
mov     eax, 0Dh
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401DA9
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_401DA9
mov     eax, 0Fh
push    eax
call    sub_40100D
add     esp, 4
add     eax, 38h 
mov     eax, 20h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4016B2
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4016B2
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_4016B2
mov     eax, offset sub_401000
push    eax
mov     eax, 11h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4016B2
call    sub_401097
cmp     eax, 0
jnz     loc_4016B1
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4016B1
jmp     loc_4016B2

loc_4016B1:
nop

loc_4016B2:
nop
nop
mov     eax, 2Eh 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401720
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_401720
call    sub_401097
cmp     eax, 0
jz      loc_401720
movsx   eax, byte_405054
push    eax
mov     eax, 13h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401720
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_401720
jmp     loc_4016B2

loc_401720:
mov     eax, 28h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401810
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_401810
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_401810
mov     eax, [ebp+8]
movsx   ecx, byte ptr [eax]
push    ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+0Ch]
movsx   edx, byte ptr [ecx]
push    edx
mov     [ebp-4], eax
call    sub_40100D
add     esp, 4
mov     ecx, [ebp-4]
cmp     ecx, eax
jge     loc_4017A4
movsx   eax, byte_405055
push    eax
call    sub_40100D
add     esp, 4
cmp     eax, 0
jz      loc_4017A4
mov     eax, 1
jmp     short loc_4017A9

loc_4017A4:
mov     eax, 0

loc_4017A9:
push    eax
mov     eax, 15h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401810
call    sub_401097
cmp     eax, 0
jnz     loc_4017E4
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4017E4
jmp     loc_401810

loc_4017E4:
movsx   eax, byte_405056
push    eax
call    sub_40100D
add     esp, 4
movsx   ecx, byte_405056
cdq
idiv    ecx
mov     ecx, 0FFFFFFFFh
imul    eax, ecx
jmp     locret_401DB4
jmp     loc_401D6B

loc_401810:
mov     eax, 2Eh 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4018A8
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4018A8
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_4018A8
movsx   eax, byte_405054
cmp     eax, 16h
mov     eax, 0
setnle  al
push    eax
mov     eax, 18h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4018A8
call    sub_401097
cmp     eax, 0
jnz     loc_401895
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_401895
jmp     loc_4018A8

loc_401895:
mov     eax, 0Ch
push    eax
call    sub_401000
add     esp, 4
jmp     loc_401D6B

loc_4018A8:
mov     eax, 38h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401938
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_401938
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_401938
mov     eax, 0
push    eax
mov     eax, 1Dh
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401938
call    sub_401097
cmp     eax, 0
jnz     loc_401920
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_401920
jmp     loc_401938

loc_401920:
movsx   eax, byte_405057
sub     eax, 1Eh
push    eax
call    sub_40100D
add     esp, 4
jmp     loc_401D6B

loc_401938:
mov     eax, 40h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401AD5
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_401AD5
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_401AD5
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 5Dh 
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
push    eax
mov     eax, 21h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401AD5
call    sub_401097
cmp     eax, 0
jnz     loc_4019D5
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4019D5
jmp     loc_401AD5

loc_4019D5:
mov     eax, 23h 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 88h
mov     eax, 48h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401A62
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_401A62
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_401A62
mov     eax, offset sub_401000
push    eax
mov     eax, 25h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401A62
call    sub_401097
cmp     eax, 0
jnz     loc_401A61
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_401A61
jmp     loc_401A62

loc_401A61:
nop

loc_401A62:
nop
nop
mov     eax, 42h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401AD0
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_401AD0
call    sub_401097
cmp     eax, 0
jz      loc_401AD0
movsx   eax, byte_405054
push    eax
mov     eax, 27h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401AD0
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_401AD0
jmp     loc_401A62

loc_401AD0:
jmp     loc_401D6B

loc_401AD5:
mov     eax, 52h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401CAC
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_401CAC
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_401CAC
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 78h 
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
movsx   ecx, byte_405057
push    ecx
mov     [ebp-8], eax
call    sub_40100D
add     esp, 4
cmp     eax, 0
jz      loc_401B58
jmp     loc_401B64

loc_401B58:
movsx   eax, byte_405054
jmp     loc_401B6B

loc_401B64:
movsx   eax, byte_405056

loc_401B6B:
mov     ecx, [ebp-8]
imul    ecx, eax
push    ecx
mov     eax, 2Ah 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401CAC
call    sub_401097
cmp     eax, 0
jnz     loc_401BAC
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_401BAC
jmp     loc_401CAC

loc_401BAC:
mov     eax, 2Ch 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 0ACh
mov     eax, 5Ah 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401C39
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_401C39
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_401C39
mov     eax, offset sub_401000
push    eax
mov     eax, 2Eh 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401C39
call    sub_401097
cmp     eax, 0
jnz     loc_401C38
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_401C38
jmp     loc_401C39

loc_401C38:
nop

loc_401C39:
nop
nop
mov     eax, 4Bh 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401CA7
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_401CA7
call    sub_401097
cmp     eax, 0
jz      loc_401CA7
movsx   eax, byte_405054
push    eax
mov     eax, 30h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401CA7
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_401CA7
jmp     loc_401C39

loc_401CA7:
jmp     loc_401D6B

loc_401CAC:
mov     eax, 62h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401D6B
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_401D6B
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_401D6B
mov     eax, [ebp+8]
movsx   ecx, byte ptr [eax]
push    ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+0Ch]
movsx   edx, byte ptr [ecx]
push    edx
mov     [ebp-0Ch], eax
call    sub_40100D
add     esp, 4
mov     ecx, [ebp-0Ch]
cmp     ecx, eax
mov     eax, 0
setnle  al
push    eax
mov     eax, 32h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401D6B
call    sub_401097
cmp     eax, 0
jnz     loc_401D4D
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_401D4D
jmp     loc_401D6B

loc_401D4D:
movsx   eax, byte_405054
movsx   ecx, byte_405055
add     eax, ecx
push    eax
call    sub_40100D
add     esp, 4
jmp     locret_401DB4

loc_401D6B:
movsx   eax, byte_405055
push    eax
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+8]
add     ecx, eax
mov     [ebp+8], ecx
movsx   eax, byte_405056
movsx   ecx, byte_405055
sub     eax, ecx
push    eax
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+0Ch]
add     ecx, eax
mov     [ebp+0Ch], ecx
jmp     loc_40159E

loc_401DA9:
nop
cpuid
nop
movsx   eax, byte_405054

locret_401DB4:
leave
retn

sub_401DB6 proc near

var_4= dword ptr -4
arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 4
nop
movsx   eax, byte_405054
mov     [ebp+var_4], eax

loc_401DCA:
mov     eax, 4Fh 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_401E79
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_401E79
call    sub_401097
cmp     eax, 0
jz      loc_401E79
mov     eax, [ebp+arg_0]
movsx   ecx, byte ptr [eax]
cmp     ecx, 0
mov     eax, 0
setnz   al
push    eax
mov     eax, 34h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_401E79
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_401E79
movsx   eax, byte_405055
push    eax
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_4]
add     ecx, eax
mov     [ebp+var_4], ecx
movsx   eax, byte_405056
movsx   ecx, byte_405055
sub     eax, ecx
push    eax
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+arg_0]
add     ecx, eax
mov     [ebp+arg_0], ecx
jmp     loc_401DCA

loc_401E79:
nop
cpuid
nop
mov     eax, [ebp+var_4]
movsx   ecx, byte_405054
add     eax, ecx
push    eax
call    sub_40100D
add     esp, 4
leave
retn
sub_401DB6 endp

sub_401E94 proc near

Source= byte ptr -5Ch
var_3C= dword ptr -3Ch
var_38= dword ptr -38h
var_34= dword ptr -34h
var_30= dword ptr -30h
var_2C= dword ptr -2Ch
var_25= byte ptr -25h
hModule= dword ptr -24h
Buffer= byte ptr -20h
arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 5Ch
nop
mov     eax, 6Ah 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402149
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402149
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402149
mov     eax, dword_405060
cmp     eax, 0
mov     eax, 0
setz    al
push    eax
mov     eax, 36h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402149
call    sub_401097
cmp     eax, 0
jnz     loc_401F22
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_401F22
jmp     loc_402149

loc_401F22:
movsx   eax, byte_40505A
push    eax
call    sub_4012BF
add     esp, 4
mov     ecx, offset Source 
push    ecx             
push    eax             
call    strcat
add     esp, 8
movsx   ecx, byte_405056
push    ecx
movsx   ecx, byte_405057
push    ecx
movsx   ecx, byte_40503F
push    ecx
movsx   ecx, byte_405038
push    ecx
movsx   ecx, byte_405041
push    ecx
movsx   ecx, byte_405045
push    ecx
movsx   ecx, byte_405038
push    ecx
movsx   ecx, byte_40503E
push    ecx
push    eax             
lea     eax, [ebp+Buffer]
push    eax             
call    sprintf
add     esp, 28h
lea     eax, [ebp+Buffer]
push    eax             
call    sub_40157B
add     esp, 4
mov     [ebp+hModule], eax
mov     eax, 6Eh 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402149
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402149
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402149
mov     eax, [ebp+hModule]
cmp     eax, 0
mov     eax, 0
setnz   al
push    eax
mov     eax, 38h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402149
call    sub_401097
cmp     eax, 0
jnz     loc_40201E
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_40201E
jmp     loc_402149

loc_40201E:
nop
cpuid
nop
movsx   eax, byte_40504F
mov     [ebp+var_25], al
movsx   eax, byte_405054
push    eax
call    sub_401000
add     esp, 4
movsx   eax, byte_405058
mov     ecx, offset byte_4053E4
add     ecx, eax
movsx   eax, byte_405054
push    eax
mov     [ebp+var_2C], ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_2C]
mov     [ecx], al
movsx   eax, byte_405057
mov     ecx, offset byte_4053E4
add     ecx, eax
movsx   eax, byte_405037
push    eax
mov     [ebp+var_30], ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_30]
mov     [ecx], al
movsx   eax, byte_405056
mov     ecx, offset byte_4053E4
add     ecx, eax
movsx   eax, byte_405034
push    eax
mov     [ebp+var_34], ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_34]
mov     [ecx], al
movsx   eax, byte_405055
mov     ecx, offset byte_4053E4
add     ecx, eax
movsx   eax, byte_405042
push    eax
mov     [ebp+var_38], ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_38]
mov     [ecx], al
movsx   eax, byte_405054
mov     ecx, offset byte_4053E4
add     ecx, eax
movsx   eax, byte_40504F
push    eax
mov     [ebp+var_3C], ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_3C]
mov     [ecx], al
nop
cpuid
nop
mov     eax, offset aC  
push    eax             
mov     eax, offset Destination 
push    eax             
call    strcat
add     esp, 8
movsx   ecx, byte_405050
push    ecx
push    eax             
lea     eax, [ebp+Source]
push    eax             
call    sprintf
add     esp, 0Ch
lea     eax, [ebp+Source]
push    eax             
mov     eax, offset byte_4053E4
push    eax             
call    strcat
add     esp, 8
push    eax             
mov     eax, [ebp+hModule]
push    eax             
call    sub_40155C
mov     dword_405060, eax

loc_402149:
mov     eax, 72h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4021DE
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4021DE
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_4021DE
mov     eax, dword_405060
cmp     eax, 0
mov     eax, 0
setnz   al
push    eax
mov     eax, 3Ah 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4021DE
call    sub_401097
cmp     eax, 0
jnz     loc_4021CD
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4021CD
jmp     loc_4021DE

loc_4021CD:
mov     eax, [ebp+arg_0]
push    eax
mov     eax, dword_405060
call    eax 
jmp     locret_4021E3

loc_4021DE:
mov     eax, 0

locret_4021E3:
leave
retn
sub_401E94 endp

sub_4021E5 proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+arg_0]
push    eax
call    sub_401E94
add     esp, 4
leave
retn
sub_4021E5 endp

sub_4021FD proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+arg_0]
push    eax
call    sub_4021E5
add     esp, 4
leave
retn
sub_4021FD endp

sub_402215 proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+arg_0]
push    eax
call    sub_4021FD
add     esp, 4
leave
retn
sub_402215 endp

sub_40222D proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+arg_0]
push    eax
call    sub_402215
add     esp, 4
leave
retn
sub_40222D endp

sub_402245 proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+arg_0]
push    eax
call    sub_40222D
add     esp, 4
leave
retn
sub_402245 endp

sub_40225D proc near

arg_0= dword ptr  8

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+arg_0]
push    eax
call    sub_402245
add     esp, 4
leave
retn
sub_40225D endp

sub_402275 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, 3Ch 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 0ECh
mov     eax, 7Ah 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40230C
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_40230C
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_40230C
mov     eax, offset sub_401000
push    eax
mov     eax, 3Eh 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40230C
call    sub_401097
cmp     eax, 0
jnz     loc_40230B
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_40230B
jmp     loc_40230C

loc_40230B:
nop

loc_40230C:
nop
nop
mov     eax, 5Bh 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40237A
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_40237A
call    sub_401097
cmp     eax, 0
jz      loc_40237A
movsx   eax, byte_405054
push    eax
mov     eax, 40h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40237A
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_40237A
jmp     loc_40230C

loc_40237A:
mov     eax, 42h 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 104h
mov     eax, 86h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402407
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402407
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402407
mov     eax, offset sub_401000
push    eax
mov     eax, 44h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402407
call    sub_401097
cmp     eax, 0
jnz     loc_402406
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_402406
jmp     loc_402407

loc_402406:
nop

loc_402407:
nop
nop
mov     eax, 61h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402475
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_402475
call    sub_401097
cmp     eax, 0
jz      loc_402475
movsx   eax, byte_405054
push    eax
mov     eax, 46h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402475
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_402475
jmp     loc_402407

loc_402475:
call    IsDebuggerPresent
leave
retn
sub_402275 endp

sub_40247C proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
int     3               
leave
retn
sub_40247C endp

sub_402489 proc near

var_1C= dword ptr -1Ch
var_18= dword ptr -18h
var_14= dword ptr -14h
var_10= qword ptr -10h
var_8= dword ptr -8
var_4= dword ptr -4

push    ebp
mov     ebp, esp
sub     esp, 18h
nop
mov     eax, 48h 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 11Ch
mov     eax, 92h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402520
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402520
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402520
mov     eax, offset sub_401000
push    eax
mov     eax, 4Ah 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402520
call    sub_401097
cmp     eax, 0
jnz     loc_40251F
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_40251F
jmp     loc_402520

loc_40251F:
nop

loc_402520:
nop
nop
mov     eax, 67h 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40258E
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_40258E
call    sub_401097
cmp     eax, 0
jz      loc_40258E
movsx   eax, byte_405054
push    eax
mov     eax, 4Ch 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40258E
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_40258E
jmp     loc_402520

loc_40258E:
movsx   eax, byte_405054
movsx   ecx, byte_405057
add     eax, ecx
push    eax
call    sub_401000
add     esp, 4
movsx   eax, byte_405057
movsx   ecx, byte_405056
sub     eax, ecx
push    eax
call    sub_401000
add     esp, 4
mov     eax, 9Ah
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40270F
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_40270F
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_40270F
call    IsDebuggerPresent
cmp     eax, 0
jnz     loc_40266F
call    sub_402275
cmp     eax, 0
jz      loc_40261B
mov     eax, 0
jmp     short loc_402620

loc_40261B:
mov     eax, 1

loc_402620:
mov     [ebp+var_4], eax
movsx   eax, byte_405054
mov     ecx, [ebp+var_4]
cdq
idiv    ecx
movsx   ecx, byte_405055
movsx   edx, byte_405054
add     ecx, edx
movsx   edx, byte_405055
add     ecx, edx
imul    eax, ecx
movsx   ecx, byte_405056
cdq
idiv    ecx
push    eax
call    sub_40100D
add     esp, 4
cmp     eax, 0
jnz     loc_40266F
mov     eax, 0
jmp     short loc_402674

loc_40266F:
mov     eax, 1

loc_402674:
push    eax
mov     eax, 4Eh 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40270F
call    sub_401097
cmp     eax, 0
jnz     loc_4026AF
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4026AF
jmp     loc_40270F

loc_4026AF:
call    sub_40247C
int     3               
movsx   eax, byte_405054
movsx   ecx, byte_405054
cdq
idiv    ecx
movsx   eax, byte_405055
movsx   ecx, byte_405054
cdq
idiv    ecx
movsx   eax, byte_405056
movsx   ecx, byte_405054
cdq
idiv    ecx
movsx   eax, byte_405057
movsx   ecx, byte_405054
cdq
idiv    ecx
movsx   eax, byte_405058
movsx   ecx, byte_405054
cdq
idiv    ecx
jmp     loc_402BC7

loc_40270F:
mov     eax, 0A0h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4027A7
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4027A7
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_4027A7
movsx   eax, byte_405054
cmp     eax, 4Fh 
mov     eax, 0
setnle  al
push    eax
mov     eax, 51h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4027A7
call    sub_401097
cmp     eax, 0
jnz     loc_402794
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_402794
jmp     loc_4027A7

loc_402794:
mov     eax, 29h 
push    eax
call    sub_401000
add     esp, 4
jmp     loc_402BC7

loc_4027A7:
mov     eax, 0AAh
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402837
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402837
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402837
mov     eax, 0
push    eax
mov     eax, 56h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402837
call    sub_401097
cmp     eax, 0
jnz     loc_40281F
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_40281F
jmp     loc_402837

loc_40281F:
movsx   eax, byte_405057
sub     eax, 57h 
push    eax
call    sub_40100D
add     esp, 4
jmp     loc_402BC7

loc_402837:
mov     eax, 0B2h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4029D4
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4029D4
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_4029D4
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 108h
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
push    eax
mov     eax, 5Ah 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4029D4
call    sub_401097
cmp     eax, 0
jnz     loc_4028D4
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4028D4
jmp     loc_4029D4

loc_4028D4:
mov     eax, 5Ch 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 16Ch
mov     eax, 0BAh
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402961
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402961
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402961
mov     eax, offset sub_401000
push    eax
mov     eax, 5Eh 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402961
call    sub_401097
cmp     eax, 0
jnz     loc_402960
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_402960
jmp     loc_402961

loc_402960:
nop

loc_402961:
nop
nop
mov     eax, 7Bh 
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4029CF
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_4029CF
call    sub_401097
cmp     eax, 0
jz      loc_4029CF
movsx   eax, byte_405054
push    eax
mov     eax, 60h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4029CF
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_4029CF
jmp     loc_402961

loc_4029CF:
jmp     loc_402BC7

loc_4029D4:
mov     eax, 0C4h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402BAB
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402BAB
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402BAB
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 123h
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
movsx   ecx, byte_405057
push    ecx
mov     [ebp+var_8], eax
call    sub_40100D
add     esp, 4
cmp     eax, 0
jz      loc_402A57
jmp     loc_402A63

loc_402A57:
movsx   eax, byte_405054
jmp     loc_402A6A

loc_402A63:
movsx   eax, byte_405056

loc_402A6A:
mov     ecx, [ebp+var_8]
imul    ecx, eax
push    ecx
mov     eax, 63h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402BAB
call    sub_401097
cmp     eax, 0
jnz     loc_402AAB
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_402AAB
jmp     loc_402BAB

loc_402AAB:
mov     eax, 65h 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 190h
mov     eax, 0CCh
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402B38
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402B38
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402B38
mov     eax, offset sub_401000
push    eax
mov     eax, 67h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402B38
call    sub_401097
cmp     eax, 0
jnz     loc_402B37
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_402B37
jmp     loc_402B38

loc_402B37:
nop

loc_402B38:
nop
nop
mov     eax, 84h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402BA6
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_402BA6
call    sub_401097
cmp     eax, 0
jz      loc_402BA6
movsx   eax, byte_405054
push    eax
mov     eax, 69h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402BA6
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_402BA6
jmp     loc_402B38

loc_402BA6:
jmp     loc_402BC7

loc_402BAB:
call    IsDebuggerPresent
push    eax
fild    [esp+1Ch+var_1C]
add     esp, 4
fst     [ebp+var_10]
fstp    st
fld     dbl_405078
fdiv    [ebp+var_10]
fstp    st

loc_402BC7:
movsx   eax, byte_40505B
push    eax             
call    malloc
add     esp, 4
mov     [ebp+var_14], eax
movsx   eax, byte_405057
movsx   ecx, byte_405056
add     eax, ecx
movsx   ecx, byte_405055
add     eax, ecx
mov     ecx, [ebp+var_14]
add     ecx, eax
mov     eax, 0
mov     [ecx], al
movsx   eax, byte_405055
movsx   ecx, byte_405056
add     eax, ecx
movsx   ecx, byte_405056
add     eax, ecx
mov     ecx, [ebp+var_14]
add     ecx, eax
movsx   eax, byte_405047
push    eax
mov     [ebp+var_18], ecx
call    sub_40100D
add     esp, 4
mov     ecx, [ebp+var_18]
mov     [ecx], al
movsx   eax, byte_405056
movsx   ecx, byte_405055
imul    eax, ecx
movsx   ecx, byte_405056
add     eax, ecx
mov     ecx, [ebp+var_14]
add     ecx, eax
movsx   eax, byte_405045
mov     [ecx], al
mov     eax, 6Bh 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 1A8h
mov     eax, 0D8h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402CEA
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402CEA
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402CEA
mov     eax, offset sub_401000
push    eax
mov     eax, 6Dh 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402CEA
call    sub_401097
cmp     eax, 0
jnz     loc_402CE9
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_402CE9
jmp     loc_402CEA

loc_402CE9:
nop

loc_402CEA:
nop
nop
mov     eax, 8Ah
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402D58
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_402D58
call    sub_401097
cmp     eax, 0
jz      loc_402D58
movsx   eax, byte_405054
push    eax
mov     eax, 6Fh 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402D58
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_402D58
jmp     loc_402CEA

loc_402D58:
movsx   eax, byte_405058
movsx   ecx, byte_405056
imul    eax, ecx
movsx   ecx, byte_405059
sub     eax, ecx
mov     ecx, [ebp+var_14]
add     ecx, eax
movsx   eax, byte_405036
mov     [ecx], al
movsx   eax, byte_405055
movsx   ecx, byte_405056
imul    eax, ecx
mov     ecx, [ebp+var_14]
add     ecx, eax
movsx   eax, byte_405049
mov     [ecx], al
movsx   eax, byte_405054
movsx   ecx, byte_405055
add     eax, ecx
mov     ecx, [ebp+var_14]
add     ecx, eax
movsx   eax, byte_405046
mov     [ecx], al
movsx   eax, byte_40505C
movsx   ecx, byte_405057
sub     eax, ecx
movsx   ecx, byte_405059
sub     eax, ecx
mov     ecx, [ebp+var_14]
add     ecx, eax
movsx   eax, byte_405040
mov     [ecx], al
mov     eax, 71h 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 1C0h
mov     eax, 0E4h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402E71
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_402E71
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_402E71
mov     eax, offset sub_401000
push    eax
mov     eax, 73h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402E71
call    sub_401097
cmp     eax, 0
jnz     loc_402E70
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_402E70
jmp     loc_402E71

loc_402E70:
nop

loc_402E71:
nop
nop
mov     eax, 90h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_402EDF
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_402EDF
call    sub_401097
cmp     eax, 0
jz      loc_402EDF
movsx   eax, byte_405054
push    eax
mov     eax, 75h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_402EDF
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_402EDF
jmp     loc_402E71

loc_402EDF:
mov     eax, [ebp+var_14]
leave
retn
sub_402489 endp

sub_402EE4 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402489
leave
retn
sub_402EE4 endp

sub_402EF5 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402EE4
leave
retn
sub_402EF5 endp

sub_402F06 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402EF5
leave
retn
sub_402F06 endp

sub_402F17 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F06
leave
retn
sub_402F17 endp

sub_402F28 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F17
leave
retn
sub_402F28 endp

sub_402F39 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F28
leave
retn
sub_402F39 endp

sub_402F4A proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F39
leave
retn
sub_402F4A endp

sub_402F5B proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F4A
leave
retn
sub_402F5B endp

sub_402F6C proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F5B
leave
retn
sub_402F6C endp

sub_402F7D proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F6C
leave
retn
sub_402F7D endp

sub_402F8E proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F7D
leave
retn
sub_402F8E endp

sub_402F9F proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F8E
leave
retn
sub_402F9F endp

sub_402FB0 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402F9F
leave
retn
sub_402FB0 endp

sub_402FC1 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402FB0
leave
retn
sub_402FC1 endp

sub_402FD2 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402FC1
leave
retn
sub_402FD2 endp

sub_402FE3 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402FD2
leave
retn
sub_402FE3 endp

sub_402FF4 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    sub_402FE3
leave
retn
sub_402FF4 endp

sub_403005 proc near

hConsoleOutput= dword ptr -410h
var_40C= dword ptr -40Ch
var_408= dword ptr -408h
ArgList= dword ptr -404h
Buffer= byte ptr -400h
arg_0= dword ptr  8
Format= dword ptr  0Ch

push    ebp
mov     ebp, esp
sub     esp, 410h
nop
mov     eax, 77h 
push    eax
call    sub_40100D
add     esp, 4
add     eax, 1D8h
mov     eax, 0F0h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40309C
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_40309C
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_40309C
mov     eax, offset sub_401000
push    eax
mov     eax, 79h 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40309C
call    sub_401097
cmp     eax, 0
jnz     loc_40309B
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_40309B
jmp     loc_40309C

loc_40309B:
nop

loc_40309C:
nop
nop
mov     eax, 96h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40310A
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_40310A
call    sub_401097
cmp     eax, 0
jz      loc_40310A
movsx   eax, byte_405054
push    eax
mov     eax, 7Bh 
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40310A
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_40310A
jmp     loc_40309C

loc_40310A:
movsx   eax, byte_405055
shl     eax, 2
movsx   ecx, byte_405056
imul    eax, ecx
movsx   ecx, byte_405058
imul    eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
movsx   ecx, byte_405056
shl     ecx, 2
movsx   edx, byte_405058
imul    ecx, edx
mov     [ebp+var_408], eax
mov     eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
mov     ecx, [ebp+var_408]
add     ecx, eax
movsx   eax, byte_405054
mov     edx, 0CAh
imul    eax, edx
add     ecx, eax
shr     ecx, 1
add     ecx, 3
and     ecx, 0FFFFFFFCh
lea     eax, [ebp+Format]
add     eax, ecx
mov     [ebp+ArgList], eax
movsx   eax, byte_405055
shl     eax, 0Ah
movsx   ecx, byte_405056
imul    eax, ecx
movsx   ecx, byte_405058
imul    eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
movsx   ecx, byte_405056
shl     ecx, 0Ah
movsx   edx, byte_405058
imul    ecx, edx
mov     [ebp+var_40C], eax
mov     eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
mov     ecx, [ebp+var_40C]
add     ecx, eax
movsx   eax, byte_405054
mov     edx, 0CBh
imul    eax, edx
add     ecx, eax
shr     ecx, 1
mov     eax, [ebp+ArgList]
push    eax             
mov     eax, [ebp+Format]
push    eax             
push    ecx             
lea     eax, [ebp+Buffer]
push    eax             
call    sub_403DEF
add     esp, 10h
mov     eax, 0FFFFFFF5h
push    eax
call    sub_40100D
add     esp, 4
push    eax
call    sub_40153A
add     esp, 4
push    eax
call    sub_40100D
add     esp, 4
mov     [ebp+hConsoleOutput], eax
mov     eax, [ebp+hConsoleOutput]
mov     ecx, 7Eh 
imul    eax, ecx
mov     ecx, [ebp+arg_0]
add     eax, ecx
push    eax
call    sub_401000
add     esp, 4
lea     eax, [ebp+Buffer]
push    eax
call    sub_401DB6
add     esp, 4
mov     ecx, 0
push    ecx             
mov     ecx, 0
push    ecx             
push    eax             
lea     eax, [ebp+Buffer]
push    eax             
mov     eax, [ebp+hConsoleOutput]
push    eax             
call    sub_401511
add     esp, 14h
leave
retn
sub_403005 endp

push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aScanf 
leave
retn

sub_403299 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aSprintf 
leave
retn
sub_403299 endp

push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aFclose 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aFopen 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aFread 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aFwrite 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aExit 
leave
retn

sub_403317 proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aStrcpy 
leave
retn
sub_403317 endp

push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aStrtok 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+0Ch]
movsx   ecx, byte_405055
imul    eax, ecx
mov     ecx, [ebp+10h]
push    ecx
push    eax
mov     eax, [ebp+8]
push    eax
call    memset
add     esp, 0Ch
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aMemcpy 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aStrchr 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aStrrchr 
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aRand 
leave
retn

sub_4033BF proc near
push    ebp
mov     ebp, esp
sub     esp, 0
nop
nop
cpuid
nop
mov     eax, offset aRealloc 
leave
retn
sub_4033BF endp

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+0Ch]
push    eax
mov     eax, [ebp+8]
push    eax
call    calloc
add     esp, 8
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 8
nop
call    sub_402FF4
push    eax
call    sub_40225D
add     esp, 4
mov     [ebp-4], eax
call    sub_4033BF
push    eax
mov     eax, [ebp-4]
push    eax
call    sub_40155C
mov     ecx, [ebp+0Ch]
push    ecx
mov     ecx, [ebp+8]
push    ecx
mov     [ebp-8], eax
mov     eax, [ebp-8]
call    eax
add     esp, 8
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+8]
push    eax
call    gets
add     esp, 4
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0Ch
nop
movsx   eax, byte_405055
shl     eax, 2
movsx   ecx, byte_405056
imul    eax, ecx
movsx   ecx, byte_405058
imul    eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
movsx   ecx, byte_405056
shl     ecx, 2
movsx   edx, byte_405058
imul    ecx, edx
mov     [ebp-8], eax
mov     eax, ecx
movsx   ecx, byte_40505C
xor     edx, edx
div     ecx
mov     ecx, [ebp-8]
add     ecx, eax
movsx   eax, byte_405054
mov     edx, 0CDh
imul    eax, edx
add     ecx, eax
shr     ecx, 1
add     ecx, 3
and     ecx, 0FFFFFFFCh
lea     eax, [ebp+10h]
add     eax, ecx
mov     [ebp-4], eax
mov     eax, [ebp-4]
push    eax
mov     eax, [ebp+10h]
push    eax
mov     eax, [ebp+0Ch]
push    eax
mov     eax, [ebp+8]
push    eax
call    sub_403DEF
add     esp, 10h
mov     [ebp-0Ch], eax
mov     eax, [ebp-0Ch]
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+10h]
push    eax
mov     eax, [ebp+0Ch]
push    eax
mov     eax, [ebp+8]
push    eax
call    vsprintf
add     esp, 0Ch
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+14h]
push    eax
mov     eax, [ebp+10h]
push    eax
mov     eax, [ebp+0Ch]
push    eax
mov     eax, [ebp+8]
push    eax
call    sub_403DEF
add     esp, 10h
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+8]
push    eax
call    getenv
add     esp, 4
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+8]
push    eax
call    system
add     esp, 4
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
call    abort
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+8]
push    eax
call    atexit
add     esp, 4
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+0Ch]
push    eax
mov     eax, [ebp+8]
push    eax
call    _getcwd
add     esp, 8
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+8]
push    eax
call    tolower
add     esp, 4
leave
retn
push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+8]
push    eax
call    toupper
add     esp, 4
leave
retn

sub_4035CE proc near

var_24= dword ptr -24h
var_20= dword ptr -20h
var_1C= dword ptr -1Ch
var_18= dword ptr -18h
var_14= dword ptr -14h
hModule= dword ptr -10h
Block= dword ptr -0Ch
var_8= dword ptr -8
var_4= dword ptr -4

push    ebp
mov     ebp, esp
sub     esp, 24h
nop
mov     eax, 0
mov     [ebp+var_4], eax
mov     eax, 0
mov     [ebp+var_4], eax

loc_4035E8:
mov     eax, [ebp+var_4]
cmp     eax, 5
jge     loc_403DAD
jmp     loc_4036A0

loc_4035F9:
mov     eax, [ebp+var_4]
mov     ecx, eax
inc     eax
mov     [ebp+var_4], eax
cmp     ecx, 0
jz      loc_40368B
movsx   eax, byte_40505D
movsx   ecx, byte_40505B
add     eax, ecx
movsx   ecx, byte_405054
shl     ecx, 8
add     eax, ecx
movsx   ecx, byte_40505C
cdq
idiv    ecx
movsx   ecx, byte_405055
sub     eax, ecx
mov     ecx, 8Ch
imul    eax, ecx
push    eax
call    sub_40100D
add     esp, 4
movsx   ecx, byte_405057
movsx   edx, byte_40505A
add     ecx, edx
movsx   edx, byte_405054
mov     [ebp+var_8], eax
mov     eax, 186h
imul    edx, eax
add     ecx, edx
movsx   eax, byte_40505D
sub     ecx, eax
mov     eax, [ebp+var_8]
add     eax, ecx
cmp     eax, 0
jz      loc_40368B
jmp     loc_40369B

loc_40368B:
movsx   eax, byte_405055
cmp     eax, 0
jnz     $+6

loc_40369B:
jmp     loc_4035E8

loc_4036A0:
mov     eax, 100h
push    eax
call    sub_40100D
add     esp, 4
push    eax             
call    malloc
add     esp, 4
mov     [ebp+Block], eax
call    sub_402FF4
push    eax
call    sub_40225D
add     esp, 4
mov     [ebp+hModule], eax
call    sub_403317
push    eax             
mov     eax, [ebp+hModule]
push    eax             
call    sub_40155C
mov     ecx, offset aDHelloWorld 
push    ecx
mov     ecx, [ebp+Block]
push    ecx
mov     [ebp+var_14], eax
mov     eax, [ebp+var_14]
call    eax
add     esp, 8
call    sub_402FF4
push    eax
call    sub_40225D
add     esp, 4
mov     [ebp+var_18], eax
call    sub_403299
push    eax             
mov     eax, [ebp+var_18]
push    eax             
call    sub_40155C
mov     ecx, [ebp+var_4]
inc     ecx
push    ecx
mov     ecx, [ebp+Block]
push    ecx
mov     ecx, [ebp+Block]
push    ecx
mov     [ebp+var_1C], eax
mov     eax, [ebp+var_1C]
call    eax
add     esp, 0Ch
mov     eax, 106h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40384D
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_40384D
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_40384D
mov     eax, [ebp+Block]
push    eax
mov     eax, 84h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40384D
call    sub_401097
cmp     eax, 0
jnz     loc_40379D
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_40379D
jmp     loc_40384D

loc_40379D:
mov     eax, 18Fh
movsx   ecx, byte_405054
cmp     eax, ecx
mov     eax, 0
setl    al
push    eax
call    sub_401000
add     esp, 4
mov     eax, [ebp+Block]
push    eax             
mov     eax, 86h
push    eax             
call    sub_403005
add     esp, 8
mov     eax, 0A4h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403848
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_403848
call    sub_401097
cmp     eax, 0
jz      loc_403848
movsx   eax, byte_405054
cmp     eax, 160h
mov     eax, 0
setnle  al
push    eax
mov     eax, 89h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403848
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_403848
jmp     loc_40379D

loc_403848:
jmp     loc_403D9C

loc_40384D:
mov     eax, 116h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_4038E8
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_4038E8
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_4038E8
movsx   eax, byte_405054
cmp     eax, 8Ah
mov     eax, 0
setnle  al
push    eax
mov     eax, 8Ch
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_4038E8
call    sub_401097
cmp     eax, 0
jnz     loc_4038D5
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_4038D5
jmp     loc_4038E8

loc_4038D5:
mov     eax, 46h 
push    eax
call    sub_401000
add     esp, 4
jmp     loc_403D9C

loc_4038E8:
mov     eax, 120h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_40397B
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_40397B
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_40397B
mov     eax, 0
push    eax
mov     eax, 91h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_40397B
call    sub_401097
cmp     eax, 0
jnz     loc_403960
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_403960
jmp     loc_40397B

loc_403960:
movsx   eax, byte_405057
sub     eax, 92h
push    eax
call    sub_40100D
add     esp, 4
jmp     loc_403D9C

loc_40397B:
mov     eax, 128h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403B18
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_403B18
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_403B18
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 1B9h
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
push    eax
mov     eax, 95h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403B18
call    sub_401097
cmp     eax, 0
jnz     loc_403A18
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_403A18
jmp     loc_403B18

loc_403A18:
mov     eax, 97h
push    eax
call    sub_40100D
add     esp, 4
add     eax, 258h
mov     eax, 130h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403AA5
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_403AA5
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_403AA5
mov     eax, offset sub_401000
push    eax
mov     eax, 99h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403AA5
call    sub_401097
cmp     eax, 0
jnz     loc_403AA4
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_403AA4
jmp     loc_403AA5

loc_403AA4:
nop

loc_403AA5:
nop
nop
mov     eax, 0B6h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403B13
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_403B13
call    sub_401097
cmp     eax, 0
jz      loc_403B13
movsx   eax, byte_405054
push    eax
mov     eax, 9Bh
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403B13
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_403B13
jmp     loc_403AA5

loc_403B13:
jmp     loc_403D9C

loc_403B18:
mov     eax, 13Ah
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403CEF
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_403CEF
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_403CEF
movsx   eax, byte_405057
movsx   ecx, byte_40505A
add     eax, ecx
movsx   ecx, byte_405054
mov     edx, 1D4h
imul    ecx, edx
add     eax, ecx
movsx   ecx, byte_40505D
sub     eax, ecx
movsx   ecx, byte_405057
push    ecx
mov     [ebp+var_20], eax
call    sub_40100D
add     esp, 4
cmp     eax, 0
jz      loc_403B9B
jmp     loc_403BA7

loc_403B9B:
movsx   eax, byte_405054
jmp     loc_403BAE

loc_403BA7:
movsx   eax, byte_405056

loc_403BAE:
mov     ecx, [ebp+var_20]
imul    ecx, eax
push    ecx
mov     eax, 9Eh
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403CEF
call    sub_401097
cmp     eax, 0
jnz     loc_403BEF
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_403BEF
jmp     loc_403CEF

loc_403BEF:
mov     eax, 0A0h
push    eax
call    sub_40100D
add     esp, 4
add     eax, 27Ch
mov     eax, 142h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403C7C
movsx   eax, byte_405055
cmp     eax, 0
jz      loc_403C7C
call    sub_401097
movsx   ecx, byte_405056
cmp     ecx, eax
jle     loc_403C7C
mov     eax, offset sub_401000
push    eax
mov     eax, 0A2h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403C7C
call    sub_401097
cmp     eax, 0
jnz     loc_403C7B
movsx   eax, byte_405054
cmp     eax, 0
jnz     loc_403C7B
jmp     loc_403C7C

loc_403C7B:
nop

loc_403C7C:
nop
nop
mov     eax, 0BFh
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403CEA
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_403CEA
call    sub_401097
cmp     eax, 0
jz      loc_403CEA
movsx   eax, byte_405054
push    eax
mov     eax, 0A4h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403CEA
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_403CEA
jmp     loc_403C7C

loc_403CEA:
jmp     loc_403D9C

loc_403CEF:
mov     eax, 1EFh
movsx   ecx, byte_405054
cmp     eax, ecx
mov     eax, 0
setl    al
push    eax
call    sub_401000
add     esp, 4
mov     eax, offset Format 
push    eax             
mov     eax, 0A6h
push    eax             
call    sub_403005
add     esp, 8
mov     eax, 0C4h
movsx   ecx, byte_405054
cmp     eax, ecx
jle     loc_403D9C
movsx   eax, byte_40505C
movsx   ecx, byte_405057
cmp     eax, ecx
jle     loc_403D9C
call    sub_401097
cmp     eax, 0
jz      loc_403D9C
movsx   eax, byte_405054
cmp     eax, 1A0h
mov     eax, 0
setnle  al
push    eax
mov     eax, 0A9h
push    eax
call    sub_4010EF
add     esp, 8
cmp     eax, 0
jz      loc_403D9C
movsx   eax, byte_405059
cmp     eax, 0
jz      loc_403D9C
jmp     loc_403CEF

loc_403D9C:
mov     eax, [ebp+Block]
push    eax             
call    free
add     esp, 4
jmp     loc_4035F9

loc_403DAD:
call    _getch
movsx   ecx, byte_405057
movsx   edx, byte_40505A
add     ecx, edx
movsx   edx, byte_405054
mov     [ebp+var_24], eax
mov     eax, 1FEh
imul    edx, eax
add     ecx, edx
movsx   eax, byte_40505D
sub     ecx, eax
mov     eax, [ebp+var_24]
add     eax, ecx
push    eax
call    sub_40100D
add     esp, 4
leave
retn
sub_4035CE endp

sub_403DEF proc near

Buffer= dword ptr  8
BufferCount= dword ptr  0Ch
Format= dword ptr  10h
ArgList= dword ptr  14h

push    ebp
mov     ebp, esp
sub     esp, 0
nop
mov     eax, [ebp+ArgList]
push    eax             
mov     eax, [ebp+Format]
push    eax             
mov     eax, [ebp+BufferCount]
push    eax             
mov     eax, [ebp+Buffer]
push    eax             
call    _vsnprintf
add     esp, 10h
leave
retn
sub_403DEF endp

align 4

public start
start proc near

var_24= dword ptr -24h
var_20= dword ptr -20h
var_1C= dword ptr -1Ch
var_18= byte ptr -18h

push    ebp
mov     ebp, esp
sub     esp, 24h
nop
lea     eax, [ebp+var_18]
push    eax
call    sub_403F89
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
mov     eax, dword_4053EC
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
call    sub_4035CE
add     esp, 0Ch
push    eax             
call    exit
start endp

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
call    sub_4035CE
add     esp, 0Ch
leave
retn
align 4
xchg    ebp, [esp]
push    ebp
lea     ebp, [esp+4]
push    ecx
mov     ecx, ebp

loc_403F23:
sub     ecx, 1000h
test    [ecx], eax
sub     eax, 1000h
cmp     eax, 1000h
jge     short loc_403F23
sub     ecx, eax
test    [ecx], eax
mov     eax, esp
mov     esp, ecx
mov     ecx, [eax]
jmp     dword ptr [eax+4]

sub_403F44 proc near
mov     eax, [ebp-14h]
retn
sub_403F44 endp

sub_403F48 proc near
call    sub_403F44
mov     eax, [eax]
mov     eax, [eax]
retn
sub_403F48 endp

call    sub_403F44
push    eax
call    sub_403F48
push    eax
call    _XcptFilter
add     esp, 8
retn
db 8Bh
dd 0D9E8E865h, 50FFFFFFh, 12BE8h, 0
dword_403F78 dd 0FFFFFFFFh, 403F52h, 403F67h

loc_403F84:
jmp     loc_4040A8

sub_403F89 proc near

arg_0= dword ptr  4
arg_4= byte ptr  8

push    ebp
mov     ebp, [esp+4+arg_0]
lea     eax, [esp+4+arg_4]
mov     [ebp+0], eax
xor     eax, eax
mov     [ebp+4], eax
mov     eax, large fs:0
mov     [ebp+8], eax
mov     eax, offset loc_403F84
mov     [ebp+0Ch], eax
mov     eax, offset dword_403F78
mov     [ebp+10h], eax
xor     eax, eax
mov     [ebp+14h], eax
lea     eax, [ebp+8]
mov     large fs:0, eax
pop     ebp
retn
sub_403F89 endp

align 8

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

align 4

align 10h

align 4

loc_4040A8:
jmp     ds:_except_handler3
align 200h
dd 380h dup(?)
_text ends

_data segment para public 'DATA' use32
assume cs:_data

aAbcdefghijklmn:
text "UTF-16LE", 'abcdefghijklmnopqrstu'
text "UTF-16LE", 'vwxyz'
byte_405034 db 61h
db  62h 
byte_405036 db 63h
byte_405037 db 64h
byte_405038 db 65h
db  66h 
db  67h 
db  68h 
db  69h 
db  6Ah 
byte_40503E db 6Bh
byte_40503F db 6Ch
byte_405040 db 6Dh
byte_405041 db 6Eh
byte_405042 db 6Fh
db  70h 
db  71h 
byte_405045 db 72h
byte_405046 db 73h
byte_405047 db 74h
db  75h 
byte_405049 db 76h
db  77h 
db  78h 
db  79h 
db  7Ah 
db  53h 
byte_40504F db 4Ch
byte_405050 db 41h
db  49h 
db  44h 
db  50h 
byte_405054 db 0
byte_405055 db 1
byte_405056 db 2
byte_405057 db 3
byte_405058 db 4
byte_405059 db 5
byte_40505A db 6
byte_40505B db 7
byte_40505C db 8
byte_40505D db 9
align 10h
dword_405060 dd 0

Source db '%d%d',0

Destination db 'Library',0

aC db '%c',0
align 8
dbl_405078 dq 0.0
aScanf db 'scanf',0
aSprintf db 'sprintf',0
aFclose db 'fclose',0
aFopen db 'fopen',0
aFread db 'fread',0
aFwrite db 'fwrite',0
aExit db 'exit',0
aStrcpy db 'strcpy',0
aStrtok db 'strtok',0
aMemcpy db 'memcpy',0
aStrchr db 'strchr',0
aStrrchr db 'strrchr',0
aRand db 'rand',0
aRealloc db 'realloc',0
aDHelloWorld db '%d) Hello, world!',0Ah,0

Format db 'Error!',0
align 10h
__IMPORT_DESCRIPTOR_kernel32 dd rva off_4051C4 
dd 0                    
dd 0                    
dd rva aKernel32Dll     
dd rva __imp_WriteConsoleA 
__IMPORT_DESCRIPTOR_msvcrt dd rva off_4051DC 
dd 0                    
dd 0                    
dd rva aMsvcrtDll       
dd rva __imp_strcat     
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

extrn __imp_WriteConsoleA:dword

extrn __imp_GetStdHandle:dword

extrn __imp_GetProcAddress:dword

extrn __imp_GetModuleHandleA:dword

extrn __imp_IsDebuggerPresent:dword

extrn __imp_strcat:dword

extrn __imp_sprintf:dword

extrn __imp_malloc:dword

extrn __imp_memset:dword

extrn __imp_calloc:dword
extrn __imp_gets:dword

extrn __imp_vsprintf:dword

extrn __imp_getenv:dword

extrn __imp_system:dword

extrn __imp_abort:dword

extrn __imp_atexit:dword

extrn __imp__getcwd:dword

extrn __imp_tolower:dword

extrn __imp_toupper:dword

extrn __imp_free:dword

extrn __imp__getch:dword

extrn __imp__vsnprintf:dword

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

_data segment para public 'DATA' use32
assume cs:_data

off_4051C4 dd rva word_405259
dd rva word_405269
dd rva word_405278
dd rva word_405289
dd rva word_40529C
dd 0

off_4051DC dd rva word_4052BB
dd rva word_4052C4
dd rva word_4052CE
dd rva word_4052D7
dd rva word_4052E0
dd rva word_4052E9
dd rva word_4052F0
dd rva word_4052FB
dd rva word_405304
dd rva word_40530D
dd rva word_405315
dd rva word_40531E
dd rva word_405328
dd rva word_405332
dd rva word_40533C
dd rva word_405343
dd rva word_40534C
dd rva word_405359
dd rva word_40536A
dd rva word_405377
dd rva word_405380
dd rva word_405389
dd rva word_405394
dd rva word_4053A4
dd rva word_4053AB
dd rva word_4053B9
dd rva word_4053C1
dd 0
aKernel32Dll db 'kernel32.dll',0
word_405259 dw 0
db 'WriteConsoleA',0
word_405269 dw 0
db 'GetStdHandle',0
word_405278 dw 0
db 'GetProcAddress',0
word_405289 dw 0
db 'GetModuleHandleA',0
word_40529C dw 0
db 'IsDebuggerPresent',0
aMsvcrtDll db 'msvcrt.dll',0
word_4052BB dw 0
db 'strcat',0
word_4052C4 dw 0
db 'sprintf',0
word_4052CE dw 0
db 'malloc',0
word_4052D7 dw 0
db 'memset',0
word_4052E0 dw 0
db 'calloc',0
word_4052E9 dw 0
db 'gets',0
word_4052F0 dw 0
db 'vsprintf',0
word_4052FB dw 0
db 'getenv',0
word_405304 dw 0
db 'system',0
word_40530D dw 0
db 'abort',0
word_405315 dw 0
db 'atexit',0
word_40531E dw 0
db '_getcwd',0
word_405328 dw 0
db 'tolower',0
word_405332 dw 0
db 'toupper',0
word_40533C dw 0
db 'free',0
word_405343 dw 0
db '_getch',0
word_40534C dw 0
db '_vsnprintf',0
word_405359 dw 0
db '__set_app_type',0
word_40536A dw 0
db '_controlfp',0
word_405377 dw 0
db '__argc',0
word_405380 dw 0
db '__argv',0
word_405389 dw 0
db '_environ',0
word_405394 dw 0
db '__getmainargs',0
word_4053A4 dw 0
db 'exit',0
word_4053AB dw 0
db '_XcptFilter',0
word_4053B9 dw 0
db '_exit',0
word_4053C1 dw 0
db '_except_handler3',0
unk_4053D4 db    0
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

byte_4053E4 db 8 dup(0)
dword_4053EC dd 0
align 1000h
_data ends

end start