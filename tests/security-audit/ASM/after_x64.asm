RUNTIME_FUNCTION struc 

FunctionStart dd ?      
FunctionEnd dd ?        
UnwindInfo dd ?         
RUNTIME_FUNCTION ends

UNWIND_INFO_HDR struc 

Ver3_Flags db ?         
PrologSize db ?         
CntUnwindCodes db ?     
FrReg_FrRegOff db ?     
UNWIND_INFO_HDR ends

UNWIND_CODE struc 

PrologOff db ?          
OpCode_OpInfo db ?      
UNWIND_CODE ends

.686p
.mmx
.model flat

_text segment para public 'CODE' use64
assume cs:_text

assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

sub_401000 proc near

arg_0= qword ptr  10h
arg_8= qword ptr  18h
arg_10= qword ptr  20h
arg_18= qword ptr  28h

push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_40100B:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx
mov     [rbp+arg_10], r8
mov     [rbp+arg_18], r9
nop
leave
retn
sub_401000 endp

algn_40101E:
align 20h
stru_401020 UNWIND_INFO_HDR <1, 4, 2, 5>
UNWIND_CODE <4, 3>      
UNWIND_CODE <1, 50h>    

sub_401028 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_401033:
mov     [rbp+arg_0], rcx
movsx   eax, cs:byte_405057
mov     r11, rax
movsx   eax, cs:byte_405054
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401000
nop
cpuid
nop
mov     eax, dword ptr [rbp+arg_0]
movsx   ecx, cs:byte_405055
imul    eax, ecx
movsx   ecx, cs:byte_405058
movsx   edx, cs:byte_405056
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505C
sub     eax, ecx
leave
retn
sub_401028 endp

sub_401085 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_401090:
movq    [rbp+arg_0], xmm0
movsx   eax, cs:byte_40505A
mov     r11, rax
movsx   eax, cs:byte_405056
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401000
nop
cpuid
nop
movsx   eax, cs:byte_405055
cvtsi2sd xmm0, eax
movq    xmm1, [rbp+arg_0]
mulsd   xmm1, xmm0
movsd   xmm0, xmm1
leave
retn
sub_401085 endp

sub_4010D2 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_4010DD:
nop
cpuid
nop
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401124
movsx   eax, cs:byte_40505D
movsx   ecx, cs:byte_40505B
add     eax, ecx
movsx   ecx, cs:byte_40505C
cdq
idiv    ecx
movsx   ecx, cs:byte_405055
sub     eax, ecx
cmp     eax, 0
jz      loc_401124
mov     eax, 1
jmp     short locret_401129

loc_401124:
mov     eax, 0

locret_401129:
leave
retn
sub_4010D2 endp

sub_40112B proc near

var_10= qword ptr -10h
var_4= dword ptr -4
arg_0= qword ptr  10h
arg_8= qword ptr  18h

push    rbp
mov     rbp, rsp
sub     rsp, 30h

loc_401136:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx
mov     eax, dword ptr [rbp+arg_8]
movsx   ecx, cs:byte_405055
imul    eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
mov     [rbp+var_4], eax
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 3
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
mov     ecx, dword ptr [rbp+arg_0]
imul    eax, ecx
mov     ecx, [rbp+var_4]
cmp     ecx, eax
jnz     loc_4011E0
movsx   eax, cs:byte_405058
movsx   ecx, cs:byte_405056
imul    eax, ecx
movsx   ecx, cs:byte_40505C
sub     ecx, eax
cmp     ecx, 0
jz      loc_4011D1
call    sub_4010D2
cmp     eax, 0
jz      loc_4011CA
jmp     loc_4011D1

loc_4011CA:
mov     eax, 1
jmp     short loc_4011D6

loc_4011D1:
mov     eax, 0

loc_4011D6:
jmp     locret_401309
jmp     loc_4012F7

loc_4011E0:
movsx   eax, cs:byte_40505D
movsx   ecx, cs:byte_40505B
add     eax, ecx
movsx   ecx, cs:byte_405054
shl     ecx, 2
add     eax, ecx
movsx   ecx, cs:byte_40505C
cdq
idiv    ecx
movsx   ecx, cs:byte_405055
sub     eax, ecx
mov     ecx, [rbp+var_4]
cmp     ecx, eax
jnz     loc_4012F7
call    sub_4010D2
cmp     eax, 0
jnz     loc_401298
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 9
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
cmp     eax, 0
jnz     loc_401298
movsx   eax, cs:byte_40505D
movsx   ecx, cs:byte_40505B
add     eax, ecx
movsx   ecx, cs:byte_405054
shl     ecx, 3
add     eax, ecx
movsx   ecx, cs:byte_40505C
cdq
idiv    ecx
movsx   ecx, cs:byte_405055
sub     eax, ecx
cmp     eax, 0
jnz     loc_401298
jmp     loc_4012ED

loc_401298:
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 0Fh
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
movsx   ecx, cs:byte_405055
mov     r10, rcx
mov     [rbp+var_10], rax
mov     rcx, r10
call    sub_401028
mov     ecx, dword ptr [rbp+var_10]
add     ecx, eax
cmp     ecx, 0
jz      loc_4012ED
mov     eax, 1
jmp     short loc_4012F2

loc_4012ED:
mov     eax, 0

loc_4012F2:
jmp     locret_401309

loc_4012F7:
nop
cpuid
nop
mov     eax, dword ptr [rbp+arg_8]
mov     r10, rax
mov     rcx, r10
call    sub_401028

locret_401309:
leave
retn
sub_40112B endp

sub_40130B proc near

var_20= qword ptr -20h
var_18= qword ptr -18h
var_C= dword ptr -0Ch
var_8= qword ptr -8
arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 40h

loc_401316:
mov     [rbp+arg_0], rcx
mov     eax, 0Eh
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401460
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401460
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_401460
mov     eax, dword ptr [rbp+arg_0]
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_40140A
movsx   eax, cs:byte_405055
movsxd  rax, eax
shl     rax, 4
movsx   ecx, cs:byte_405056
movsxd  rcx, ecx
imul    rax, rcx
movsx   ecx, cs:byte_405058
movsxd  rcx, ecx
imul    rax, rcx
movsx   ecx, cs:byte_40505C
movsxd  rcx, ecx
xor     rdx, rdx
div     rcx
movsx   ecx, cs:byte_405056
movsxd  rcx, ecx
shl     rcx, 4
movsx   edx, cs:byte_405058
movsxd  rdx, edx
imul    rcx, rdx
movsx   edx, cs:byte_40505C
movsxd  rdx, edx
mov     [rbp+var_8], rax
mov     rax, rcx
mov     rcx, rdx
xor     rdx, rdx
div     rcx
mov     rcx, [rbp+var_8]
add     rcx, rax
movsx   eax, cs:byte_405054
mov     edx, 54h 
imul    eax, edx
movsxd  rax, eax
add     rcx, rax
shr     rcx, 1
mov     eax, dword ptr [rbp+arg_0]
movsxd  rax, eax
cmp     rax, rcx
jnb     loc_40140A
mov     eax, 0
jmp     short loc_40140F

loc_40140A:
mov     eax, 1

loc_40140F:
mov     r11, rax
mov     eax, 8
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401460
call    sub_4010D2
cmp     eax, 0
jnz     loc_401451
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_401451
jmp     loc_401460

loc_401451:
mov     rax, 0
jmp     locret_4015B2

loc_401460:
movsx   eax, cs:byte_405055
movsx   ecx, cs:byte_405059
imul    eax, ecx
movsx   ecx, cs:byte_405058
sub     eax, ecx
movsx   ecx, cs:byte_405055
add     eax, ecx
movsx   ecx, cs:byte_405056
sub     eax, ecx
mov     [rbp+var_C], eax
lea     rax, unk_405490
mov     [rbp+var_18], rax
movsx   eax, cs:byte_405054
mov     [rbp+var_C], eax

loc_4014A4:
mov     eax, [rbp+var_C]
mov     ecx, dword ptr [rbp+arg_0]
cmp     eax, ecx
jge     loc_40159C
jmp     loc_401569

loc_4014B7:
mov     eax, [rbp+var_C]
mov     rcx, rax
add     eax, 1
mov     [rbp+var_C], eax
cmp     ecx, 0
jz      loc_401554
movsx   eax, cs:byte_40505D
movsx   ecx, cs:byte_40505B
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 12h
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505C
cdq
idiv    ecx
movsx   ecx, cs:byte_405055
sub     eax, ecx
mov     ecx, 15h
imul    eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsx   ecx, cs:byte_405057
movsx   edx, cs:byte_40505A
add     ecx, edx
movsx   edx, cs:byte_405054
mov     [rbp+var_20], rax
mov     eax, 21h 
imul    edx, eax
add     ecx, edx
movsx   eax, cs:byte_40505D
sub     ecx, eax
mov     eax, dword ptr [rbp+var_20]
add     eax, ecx
cmp     eax, 0
jz      loc_401554
jmp     loc_401564

loc_401554:
movsx   eax, cs:byte_405055
cmp     eax, 0
jnz     $+6

loc_401564:
jmp     loc_4014A4

loc_401569:
mov     rax, [rbp+var_18]
mov     rcx, rax
add     rax, 1
mov     [rbp+var_18], rax
mov     eax, 25h 
mov     [rcx], al
mov     rax, [rbp+var_18]
mov     rcx, rax
add     rax, 1
mov     [rbp+var_18], rax
movsx   eax, cs:byte_405036
mov     [rcx], al
jmp     loc_4014B7

loc_40159C:
mov     rax, [rbp+var_18]
mov     ecx, 0
mov     [rax], cl
nop
cpuid
nop
lea     rax, unk_405490

locret_4015B2:
leave
retn
sub_40130B endp

sub_4015B4 proc near

lpReserved= qword ptr -10h
hConsoleOutput= qword ptr  10h
lpBuffer= qword ptr  18h
nNumberOfCharsToWrite= dword ptr  20h
lpNumberOfCharsWritten= qword ptr  28h
arg_20= qword ptr  30h

push    rbp
mov     rbp, rsp
sub     rsp, 30h

loc_4015BF:
mov     [rbp+hConsoleOutput], rcx
mov     [rbp+lpBuffer], rdx
mov     qword ptr [rbp+nNumberOfCharsToWrite], r8
mov     [rbp+lpNumberOfCharsWritten], r9
nop
cpuid
nop
mov     rax, [rbp+arg_20]
mov     [rsp+30h+lpReserved], rax 
mov     rax, [rbp+lpNumberOfCharsWritten]
mov     r9, rax         
mov     eax, [rbp+nNumberOfCharsToWrite]
mov     r8, rax         
mov     rax, [rbp+lpBuffer]
mov     r11, rax
mov     rax, [rbp+hConsoleOutput]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    WriteConsoleA
leave
retn
sub_4015B4 endp

sub_401604 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40160F:
mov     [rbp+arg_0], rcx
nop
cpuid
nop
mov     eax, dword ptr [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    sub_401028
mov     r10, rax
mov     rcx, r10        
call    GetStdHandle
leave
retn
sub_401604 endp

sub_401632 proc near

hModule= qword ptr  10h
lpProcName= qword ptr  18h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40163D:
mov     [rbp+hModule], rcx
mov     [rbp+lpProcName], rdx
nop
cpuid
nop
mov     rax, [rbp+lpProcName]
mov     r11, rax
mov     rax, [rbp+hModule]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    GetProcAddress
leave
retn
sub_401632 endp

sub_401664 proc near

lpModuleName= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40166F:
mov     [rbp+lpModuleName], rcx
nop
cpuid
nop
mov     rax, [rbp+lpModuleName]
mov     r10, rax
mov     rcx, r10        
call    GetModuleHandleA
leave
retn
sub_401664 endp

sub_401688 proc near

var_18= qword ptr -18h
var_10= qword ptr -10h
var_8= qword ptr -8
arg_0= qword ptr  10h
arg_8= qword ptr  18h

push    rbp
mov     rbp, rsp
sub     rsp, 40h

loc_401693:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx

loc_40169B:
mov     eax, 28h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401F3A
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_401F3A
call    sub_4010D2
cmp     eax, 0
jz      loc_401F3A
mov     rax, [rbp+arg_0]
movsx   ecx, byte ptr [rax]
cmp     ecx, 0
jnz     loc_4016FA
mov     rax, [rbp+arg_8]
movsx   ecx, byte ptr [rax]
cmp     ecx, 0
jnz     loc_4016FA
mov     eax, 0
jmp     short loc_4016FF

loc_4016FA:
mov     eax, 1

loc_4016FF:
mov     r11, rax
mov     eax, 0Dh
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401F3A
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_401F3A
mov     eax, 0Fh
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 38h 
mov     eax, 20h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4017C3
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4017C3
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4017C3
lea     rax, sub_401000
mov     r11, rax
mov     eax, 11h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4017C3
call    sub_4010D2
cmp     eax, 0
jnz     loc_4017C2
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4017C2
jmp     loc_4017C3

loc_4017C2:
nop

loc_4017C3:
nop
nop
mov     eax, 2Eh 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401838
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_401838
call    sub_4010D2
cmp     eax, 0
jz      loc_401838
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 13h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401838
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_401838
jmp     loc_4017C3

loc_401838:
mov     eax, 28h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_40193A
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_40193A
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_40193A
mov     rax, [rbp+arg_0]
movsx   ecx, byte ptr [rax]
mov     r10, rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+arg_8]
movsx   edx, byte ptr [rcx]
mov     r10, rdx
mov     [rbp+var_8], rax
mov     rcx, r10
call    sub_401028
mov     ecx, dword ptr [rbp+var_8]
cmp     ecx, eax
jge     loc_4018C5
movsx   eax, cs:byte_405055
mov     r10, rax
mov     rcx, r10
call    sub_401028
cmp     eax, 0
jz      loc_4018C5
mov     eax, 1
jmp     short loc_4018CA

loc_4018C5:
mov     eax, 0

loc_4018CA:
mov     r11, rax
mov     eax, 15h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_40193A
call    sub_4010D2
cmp     eax, 0
jnz     loc_40190C
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_40190C
jmp     loc_40193A

loc_40190C:
movsx   eax, cs:byte_405056
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsx   ecx, cs:byte_405056
cdq
idiv    ecx
mov     ecx, 0FFFFFFFFh
imul    eax, ecx
jmp     locret_401F45
jmp     loc_401EEC

loc_40193A:
mov     eax, 2Eh 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4019DB
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4019DB
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4019DB
movsx   eax, cs:byte_405054
cmp     eax, 16h
mov     eax, 0
setnle  al
mov     r11, rax
mov     eax, 18h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4019DB
call    sub_4010D2
cmp     eax, 0
jnz     loc_4019C6
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4019C6
jmp     loc_4019DB

loc_4019C6:
mov     eax, 0Ch
mov     r10, rax
mov     rcx, r10
call    sub_401000
jmp     loc_401EEC

loc_4019DB:
mov     eax, 38h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401A74
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401A74
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_401A74
mov     eax, 0
mov     r11, rax
mov     eax, 1Dh
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401A74
call    sub_4010D2
cmp     eax, 0
jnz     loc_401A5A
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_401A5A
jmp     loc_401A74

loc_401A5A:
movsx   eax, cs:byte_405057
sub     eax, 1Eh
mov     r10, rax
mov     rcx, r10
call    sub_401028
jmp     loc_401EEC

loc_401A74:
mov     eax, 40h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401C2A
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401C2A
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_401C2A
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 5Dh 
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
mov     r11, rax
mov     eax, 21h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401C2A
call    sub_4010D2
cmp     eax, 0
jnz     loc_401B18
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_401B18
jmp     loc_401C2A

loc_401B18:
mov     eax, 23h 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 88h
mov     eax, 48h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401BB0
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401BB0
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_401BB0
lea     rax, sub_401000
mov     r11, rax
mov     eax, 25h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401BB0
call    sub_4010D2
cmp     eax, 0
jnz     loc_401BAF
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_401BAF
jmp     loc_401BB0

loc_401BAF:
nop

loc_401BB0:
nop
nop
mov     eax, 42h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401C25
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_401C25
call    sub_4010D2
cmp     eax, 0
jz      loc_401C25
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 27h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401C25
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_401C25
jmp     loc_401BB0

loc_401C25:
jmp     loc_401EEC

loc_401C2A:
mov     eax, 52h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401E1D
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401E1D
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_401E1D
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 78h 
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
movsx   ecx, cs:byte_405057
mov     r10, rcx
mov     [rbp+var_10], rax
mov     rcx, r10
call    sub_401028
cmp     eax, 0
jz      loc_401CB0
jmp     loc_401CBC

loc_401CB0:
movsx   eax, cs:byte_405054
jmp     loc_401CC3

loc_401CBC:
movsx   eax, cs:byte_405056

loc_401CC3:
mov     ecx, dword ptr [rbp+var_10]
imul    ecx, eax
mov     r11, rcx
mov     eax, 2Ah 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401E1D
call    sub_4010D2
cmp     eax, 0
jnz     loc_401D0B
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_401D0B
jmp     loc_401E1D

loc_401D0B:
mov     eax, 2Ch 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 0ACh
mov     eax, 5Ah 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401DA3
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401DA3
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_401DA3
lea     rax, sub_401000
mov     r11, rax
mov     eax, 2Eh 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401DA3
call    sub_4010D2
cmp     eax, 0
jnz     loc_401DA2
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_401DA2
jmp     loc_401DA3

loc_401DA2:
nop

loc_401DA3:
nop
nop
mov     eax, 4Bh 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401E18
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_401E18
call    sub_4010D2
cmp     eax, 0
jz      loc_401E18
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 30h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401E18
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_401E18
jmp     loc_401DA3

loc_401E18:
jmp     loc_401EEC

loc_401E1D:
mov     eax, 62h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_401EEC
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_401EEC
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_401EEC
mov     rax, [rbp+arg_0]
movsx   ecx, byte ptr [rax]
mov     r10, rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+arg_8]
movsx   edx, byte ptr [rcx]
mov     r10, rdx
mov     [rbp+var_18], rax
mov     rcx, r10
call    sub_401028
mov     ecx, dword ptr [rbp+var_18]
cmp     ecx, eax
mov     eax, 0
setnle  al
mov     r11, rax
mov     eax, 32h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_401EEC
call    sub_4010D2
cmp     eax, 0
jnz     loc_401ECC
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_401ECC
jmp     loc_401EEC

loc_401ECC:
movsx   eax, cs:byte_405054
movsx   ecx, cs:byte_405055
add     eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
jmp     locret_401F45

loc_401EEC:
movsx   eax, cs:byte_405055
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsxd  rax, eax
mov     rcx, [rbp+arg_0]
add     rcx, rax
mov     [rbp+arg_0], rcx
movsx   eax, cs:byte_405056
movsx   ecx, cs:byte_405055
sub     eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsxd  rax, eax
mov     rcx, [rbp+arg_8]
add     rcx, rax
mov     [rbp+arg_8], rcx
jmp     loc_40169B

loc_401F3A:
nop
cpuid
nop
movsx   eax, cs:byte_405054

locret_401F45:
leave
retn
sub_401688 endp

sub_401F47 proc near

var_8= qword ptr -8
arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 30h

loc_401F52:
mov     [rbp+arg_0], rcx
movsx   eax, cs:byte_405054
movsxd  rax, eax
mov     [rbp+var_8], rax

loc_401F64:
mov     eax, 4Fh 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_40202B
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_40202B
call    sub_4010D2
cmp     eax, 0
jz      loc_40202B
mov     rax, [rbp+arg_0]
movsx   ecx, byte ptr [rax]
cmp     ecx, 0
mov     eax, 1
setnz   al
mov     r11, rax
mov     eax, 34h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_40202B
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_40202B
movsx   eax, cs:byte_405055
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsxd  rax, eax
mov     rcx, [rbp+var_8]
add     rcx, rax
mov     [rbp+var_8], rcx
movsx   eax, cs:byte_405056
movsx   ecx, cs:byte_405055
sub     eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsxd  rax, eax
mov     rcx, [rbp+arg_0]
add     rcx, rax
mov     [rbp+arg_0], rcx
jmp     loc_401F64

loc_40202B:
nop
cpuid
nop
movsx   eax, cs:byte_405054
movsxd  rax, eax
mov     rcx, [rbp+var_8]
add     rcx, rax
mov     r10, rcx
mov     rcx, r10
call    sub_401028
movsxd  rax, eax
leave
retn
sub_401F47 endp

sub_402050 proc near

var_B0= qword ptr -0B0h
var_A8= qword ptr -0A8h
var_A0= qword ptr -0A0h
var_98= qword ptr -98h
var_90= qword ptr -90h
var_88= qword ptr -88h
Source= byte ptr -78h
var_58= qword ptr -58h
var_50= qword ptr -50h
var_48= qword ptr -48h
var_40= qword ptr -40h
var_38= qword ptr -38h
var_29= byte ptr -29h
var_28= qword ptr -28h
Buffer= byte ptr -20h
arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 0D0h

loc_40205B:
mov     [rbp+arg_0], rcx
mov     eax, 6Ah 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4023AE
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4023AE
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4023AE
mov     rax, cs:qword_405060
cmp     rax, 0
mov     eax, 0
setz    al
mov     r11, rax
mov     eax, 36h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4023AE
call    sub_4010D2
cmp     eax, 0
jnz     loc_4020EC
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4020EC
jmp     loc_4023AE

loc_4020EC:
movsx   eax, cs:byte_40505A
mov     r10, rax
mov     rcx, r10
call    sub_40130B
lea     rcx, Source     
mov     r11, rcx
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    strcat
movsx   ecx, cs:byte_405056
mov     [rsp+0D0h+var_88], rcx
movsx   ecx, cs:byte_405057
mov     [rsp+0D0h+var_90], rcx
movsx   ecx, cs:byte_40503F
mov     [rsp+0D0h+var_98], rcx
movsx   ecx, cs:byte_405038
mov     [rsp+0D0h+var_A0], rcx
movsx   ecx, cs:byte_405041
mov     [rsp+0D0h+var_A8], rcx
movsx   ecx, cs:byte_405045
mov     [rsp+0D0h+var_B0], rcx
movsx   ecx, cs:byte_405038
mov     r9, rcx
movsx   ecx, cs:byte_40503E
mov     r8, rcx
mov     r11, rax
lea     rax, [rbp+Buffer]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    sprintf
lea     rax, [rbp+Buffer]
mov     r10, rax
mov     rcx, r10
call    sub_401664
mov     [rbp+var_28], rax
mov     eax, 6Eh 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4023AE
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4023AE
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4023AE
mov     rax, [rbp+var_28]
cmp     rax, 0
mov     eax, 1
setnz   al
mov     r11, rax
mov     eax, 38h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4023AE
call    sub_4010D2
cmp     eax, 0
jnz     loc_402224
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_402224
jmp     loc_4023AE

loc_402224:
nop
cpuid
nop
movsx   eax, cs:byte_40504F
mov     [rbp+var_29], al
movsx   eax, cs:byte_405054
mov     r10, rax
mov     rcx, r10
call    sub_401000
movsx   eax, cs:byte_405058
movsxd  rax, eax
lea     rcx, byte_4054A0
add     rcx, rax
movsx   eax, cs:byte_405054
mov     r10, rax
mov     [rbp+var_38], rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+var_38]
mov     [rcx], al
movsx   eax, cs:byte_405057
movsxd  rax, eax
lea     rcx, byte_4054A0
add     rcx, rax
movsx   eax, cs:byte_405037
mov     r10, rax
mov     [rbp+var_40], rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+var_40]
mov     [rcx], al
movsx   eax, cs:byte_405056
movsxd  rax, eax
lea     rcx, byte_4054A0
add     rcx, rax
movsx   eax, cs:byte_405034
mov     r10, rax
mov     [rbp+var_48], rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+var_48]
mov     [rcx], al
movsx   eax, cs:byte_405055
movsxd  rax, eax
lea     rcx, byte_4054A0
add     rcx, rax
movsx   eax, cs:byte_405042
mov     r10, rax
mov     [rbp+var_50], rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+var_50]
mov     [rcx], al
movsx   eax, cs:byte_405054
movsxd  rax, eax
lea     rcx, byte_4054A0
add     rcx, rax
movsx   eax, cs:byte_40504F
mov     r10, rax
mov     [rbp+var_58], rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+var_58]
mov     [rcx], al
nop
cpuid
nop
lea     rax, aC         
mov     r11, rax
lea     rax, Destination 
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    strcat
movsx   ecx, cs:byte_405050
mov     r8, rcx
mov     r11, rax
lea     rax, [rbp+Source]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    sprintf
lea     rax, [rbp+Source]
mov     r11, rax
lea     rax, byte_4054A0
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    strcat
mov     r11, rax
mov     rax, [rbp+var_28]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401632
mov     cs:qword_405060, rax

loc_4023AE:
mov     eax, 72h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402454
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402454
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402454
mov     rax, cs:qword_405060
cmp     rax, 0
mov     eax, 1
setnz   al
mov     r11, rax
mov     eax, 3Ah 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402454
call    sub_4010D2
cmp     eax, 0
jnz     loc_40243B
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_40243B
jmp     loc_402454

loc_40243B:
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
mov     r11, cs:qword_405060
call    r11 
jmp     locret_40245E

loc_402454:
mov     rax, 0

locret_40245E:
leave
retn
sub_402050 endp

sub_402460 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40246B:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    sub_402050
leave
retn
sub_402460 endp

sub_402480 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40248B:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    sub_402460
leave
retn
sub_402480 endp

sub_4024A0 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4024AB:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    sub_402480
leave
retn
sub_4024A0 endp

sub_4024C0 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4024CB:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    sub_4024A0
leave
retn
sub_4024C0 endp

sub_4024E0 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4024EB:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    sub_4024C0
leave
retn
sub_4024E0 endp

sub_402500 proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40250B:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    sub_4024E0
leave
retn
sub_402500 endp

sub_402520 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40252B:
mov     eax, 3Ch 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 0ECh
mov     eax, 7Ah 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4025C3
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4025C3
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4025C3
lea     rax, sub_401000
mov     r11, rax
mov     eax, 3Eh 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4025C3
call    sub_4010D2
cmp     eax, 0
jnz     loc_4025C2
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4025C2
jmp     loc_4025C3

loc_4025C2:
nop

loc_4025C3:
nop
nop
mov     eax, 5Bh 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402638
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_402638
call    sub_4010D2
cmp     eax, 0
jz      loc_402638
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 40h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402638
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_402638
jmp     loc_4025C3

loc_402638:
mov     eax, 42h 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 104h
mov     eax, 86h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4026D0
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4026D0
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4026D0
lea     rax, sub_401000
mov     r11, rax
mov     eax, 44h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4026D0
call    sub_4010D2
cmp     eax, 0
jnz     loc_4026CF
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4026CF
jmp     loc_4026D0

loc_4026CF:
nop

loc_4026D0:
nop
nop
mov     eax, 61h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402745
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_402745
call    sub_4010D2
cmp     eax, 0
jz      loc_402745
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 46h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402745
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_402745
jmp     loc_4026D0

loc_402745:
call    IsDebuggerPresent
leave
retn
sub_402520 endp

sub_40274C proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_402757:             
int     3
leave
retn
sub_40274C endp

sub_40275A proc near

var_20= qword ptr -20h
var_18= qword ptr -18h
var_10= qword ptr -10h
var_8= qword ptr -8

push    rbp
mov     rbp, rsp
sub     rsp, 40h

loc_402765:
mov     eax, 48h 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 11Ch
mov     eax, 92h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4027FD
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4027FD
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4027FD
lea     rax, sub_401000
mov     r11, rax
mov     eax, 4Ah 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4027FD
call    sub_4010D2
cmp     eax, 0
jnz     loc_4027FC
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4027FC
jmp     loc_4027FD

loc_4027FC:
nop

loc_4027FD:
nop
nop
mov     eax, 67h 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402872
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_402872
call    sub_4010D2
cmp     eax, 0
jz      loc_402872
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 4Ch 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402872
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_402872
jmp     loc_4027FD

loc_402872:
movsx   eax, cs:byte_405054
movsx   ecx, cs:byte_405057
add     eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401000
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_405056
sub     eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401000
mov     eax, 9Ah
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402A01
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402A01
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402A01
call    IsDebuggerPresent
cmp     eax, 0
jnz     loc_40295A
call    sub_402520
cmp     eax, 0
jz      loc_402903
mov     eax, 0
jmp     short loc_402908

loc_402903:
mov     eax, 1

loc_402908:
mov     [rbp+var_8], rax
movsx   eax, cs:byte_405054
mov     ecx, dword ptr [rbp+var_8]
cdq
idiv    ecx
movsx   ecx, cs:byte_405055
movsx   edx, cs:byte_405054
add     ecx, edx
movsx   edx, cs:byte_405055
add     ecx, edx
imul    eax, ecx
movsx   ecx, cs:byte_405056
cdq
idiv    ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
cmp     eax, 0
jnz     loc_40295A
mov     eax, 0
jmp     short loc_40295F

loc_40295A:
mov     eax, 1

loc_40295F:
mov     r11, rax
mov     eax, 4Eh 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402A01
call    sub_4010D2
cmp     eax, 0
jnz     loc_4029A1
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4029A1
jmp     loc_402A01

loc_4029A1:
call    sub_40274C
int     3               
movsx   eax, cs:byte_405054
movsx   ecx, cs:byte_405054
cdq
idiv    ecx
movsx   eax, cs:byte_405055
movsx   ecx, cs:byte_405054
cdq
idiv    ecx
movsx   eax, cs:byte_405056
movsx   ecx, cs:byte_405054
cdq
idiv    ecx
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_405054
cdq
idiv    ecx
movsx   eax, cs:byte_405058
movsx   ecx, cs:byte_405054
cdq
idiv    ecx
jmp     loc_402EF9

loc_402A01:
mov     eax, 0A0h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402AA2
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402AA2
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402AA2
movsx   eax, cs:byte_405054
cmp     eax, 4Fh 
mov     eax, 0
setnle  al
mov     r11, rax
mov     eax, 51h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402AA2
call    sub_4010D2
cmp     eax, 0
jnz     loc_402A8D
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_402A8D
jmp     loc_402AA2

loc_402A8D:
mov     eax, 29h 
mov     r10, rax
mov     rcx, r10
call    sub_401000
jmp     loc_402EF9

loc_402AA2:
mov     eax, 0AAh
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402B3B
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402B3B
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402B3B
mov     eax, 0
mov     r11, rax
mov     eax, 56h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402B3B
call    sub_4010D2
cmp     eax, 0
jnz     loc_402B21
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_402B21
jmp     loc_402B3B

loc_402B21:
movsx   eax, cs:byte_405057
sub     eax, 57h 
mov     r10, rax
mov     rcx, r10
call    sub_401028
jmp     loc_402EF9

loc_402B3B:
mov     eax, 0B2h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402CF1
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402CF1
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402CF1
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 108h
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
mov     r11, rax
mov     eax, 5Ah 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402CF1
call    sub_4010D2
cmp     eax, 0
jnz     loc_402BDF
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_402BDF
jmp     loc_402CF1

loc_402BDF:
mov     eax, 5Ch 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 16Ch
mov     eax, 0BAh
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402C77
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402C77
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402C77
lea     rax, sub_401000
mov     r11, rax
mov     eax, 5Eh 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402C77
call    sub_4010D2
cmp     eax, 0
jnz     loc_402C76
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_402C76
jmp     loc_402C77

loc_402C76:
nop

loc_402C77:
nop
nop
mov     eax, 7Bh 
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402CEC
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_402CEC
call    sub_4010D2
cmp     eax, 0
jz      loc_402CEC
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 60h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402CEC
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_402CEC
jmp     loc_402C77

loc_402CEC:
jmp     loc_402EF9

loc_402CF1:
mov     eax, 0C4h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402EE4
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402EE4
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402EE4
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 123h
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
movsx   ecx, cs:byte_405057
mov     r10, rcx
mov     [rbp+var_10], rax
mov     rcx, r10
call    sub_401028
cmp     eax, 0
jz      loc_402D77
jmp     loc_402D83

loc_402D77:
movsx   eax, cs:byte_405054
jmp     loc_402D8A

loc_402D83:
movsx   eax, cs:byte_405056

loc_402D8A:
mov     ecx, dword ptr [rbp+var_10]
imul    ecx, eax
mov     r11, rcx
mov     eax, 63h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402EE4
call    sub_4010D2
cmp     eax, 0
jnz     loc_402DD2
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_402DD2
jmp     loc_402EE4

loc_402DD2:
mov     eax, 65h 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 190h
mov     eax, 0CCh
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402E6A
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_402E6A
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_402E6A
lea     rax, sub_401000
mov     r11, rax
mov     eax, 67h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402E6A
call    sub_4010D2
cmp     eax, 0
jnz     loc_402E69
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_402E69
jmp     loc_402E6A

loc_402E69:
nop

loc_402E6A:
nop
nop
mov     eax, 84h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_402EDF
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_402EDF
call    sub_4010D2
cmp     eax, 0
jz      loc_402EDF
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 69h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_402EDF
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_402EDF
jmp     loc_402E6A

loc_402EDF:
jmp     loc_402EF9

loc_402EE4:
call    IsDebuggerPresent
cvtsi2sd xmm0, eax
movq    xmm1, cs:qword_405078
divsd   xmm1, xmm0

loc_402EF9:
movsx   eax, cs:byte_40505B
movsxd  rax, eax
mov     r10, rax
mov     rcx, r10        
call    malloc
mov     [rbp+var_18], rax
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_405056
add     eax, ecx
movsx   ecx, cs:byte_405055
add     eax, ecx
movsxd  rax, eax
mov     rcx, [rbp+var_18]
add     rcx, rax
mov     eax, 0
mov     [rcx], al
movsx   eax, cs:byte_405055
movsx   ecx, cs:byte_405056
add     eax, ecx
movsx   ecx, cs:byte_405056
add     eax, ecx
movsxd  rax, eax
mov     rcx, [rbp+var_18]
add     rcx, rax
movsx   eax, cs:byte_405047
mov     r10, rax
mov     [rbp+var_20], rcx
mov     rcx, r10
call    sub_401028
mov     rcx, [rbp+var_20]
mov     [rcx], al
movsx   eax, cs:byte_405056
movsx   ecx, cs:byte_405055
imul    eax, ecx
movsx   ecx, cs:byte_405056
add     eax, ecx
movsxd  rax, eax
mov     rcx, [rbp+var_18]
add     rcx, rax
movsx   eax, cs:byte_405045
mov     [rcx], al
mov     eax, 6Bh 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 1A8h
mov     eax, 0D8h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403040
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_403040
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_403040
lea     rax, sub_401000
mov     r11, rax
mov     eax, 6Dh 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403040
call    sub_4010D2
cmp     eax, 0
jnz     loc_40303F
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_40303F
jmp     loc_403040

loc_40303F:
nop

loc_403040:
nop
nop
mov     eax, 8Ah
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4030B5
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_4030B5
call    sub_4010D2
cmp     eax, 0
jz      loc_4030B5
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 6Fh 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4030B5
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_4030B5
jmp     loc_403040

loc_4030B5:
movsx   eax, cs:byte_405058
movsx   ecx, cs:byte_405056
imul    eax, ecx
movsx   ecx, cs:byte_405059
sub     eax, ecx
movsxd  rax, eax
mov     rcx, [rbp+var_18]
add     rcx, rax
movsx   eax, cs:byte_405036
mov     [rcx], al
movsx   eax, cs:byte_405055
movsx   ecx, cs:byte_405056
imul    eax, ecx
movsxd  rax, eax
mov     rcx, [rbp+var_18]
add     rcx, rax
movsx   eax, cs:byte_405049
mov     [rcx], al
movsx   eax, cs:byte_405054
movsx   ecx, cs:byte_405055
add     eax, ecx
movsxd  rax, eax
mov     rcx, [rbp+var_18]
add     rcx, rax
movsx   eax, cs:byte_405046
mov     [rcx], al
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
sub     eax, ecx
movsx   ecx, cs:byte_405059
sub     eax, ecx
movsxd  rax, eax
mov     rcx, [rbp+var_18]
add     rcx, rax
movsx   eax, cs:byte_405040
mov     [rcx], al
mov     eax, 71h 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 1C0h
mov     eax, 0E4h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4031ED
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4031ED
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4031ED
lea     rax, sub_401000
mov     r11, rax
mov     eax, 73h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4031ED
call    sub_4010D2
cmp     eax, 0
jnz     loc_4031EC
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4031EC
jmp     loc_4031ED

loc_4031EC:
nop

loc_4031ED:
nop
nop
mov     eax, 90h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403262
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_403262
call    sub_4010D2
cmp     eax, 0
jz      loc_403262
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 75h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403262
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_403262
jmp     loc_4031ED

loc_403262:
mov     rax, [rbp+var_18]
leave
retn
sub_40275A endp

sub_403268 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403273:
call    sub_40275A
leave
retn
sub_403268 endp

sub_40327A proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403285:
call    sub_403268
leave
retn
sub_40327A endp

sub_40328C proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403297:
call    sub_40327A
leave
retn
sub_40328C endp

sub_40329E proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4032A9:
call    sub_40328C
leave
retn
sub_40329E endp

sub_4032B0 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4032BB:
call    sub_40329E
leave
retn
sub_4032B0 endp

sub_4032C2 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4032CD:
call    sub_4032B0
leave
retn
sub_4032C2 endp

sub_4032D4 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4032DF:
call    sub_4032C2
leave
retn
sub_4032D4 endp

sub_4032E6 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4032F1:
call    sub_4032D4
leave
retn
sub_4032E6 endp

sub_4032F8 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403303:
call    sub_4032E6
leave
retn
sub_4032F8 endp

sub_40330A proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403315:
call    sub_4032F8
leave
retn
sub_40330A endp

sub_40331C proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403327:
call    sub_40330A
leave
retn
sub_40331C endp

sub_40332E proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403339:
call    sub_40331C
leave
retn
sub_40332E endp

sub_403340 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40334B:
call    sub_40332E
leave
retn
sub_403340 endp

sub_403352 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40335D:
call    sub_403340
leave
retn
sub_403352 endp

sub_403364 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40336F:
call    sub_403352
leave
retn
sub_403364 endp

sub_403376 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403381:
call    sub_403364
leave
retn
sub_403376 endp

sub_403388 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403393:
call    sub_403376
leave
retn
sub_403388 endp

sub_40339A proc near

var_430= qword ptr -430h
var_418= dword ptr -418h
var_410= qword ptr -410h
var_408= qword ptr -408h
var_400= dword ptr -400h
arg_0= qword ptr  10h
arg_8= qword ptr  18h
arg_10= qword ptr  20h
arg_18= qword ptr  28h

push    rbp
mov     rbp, rsp
sub     rsp, 450h

loc_4033A5:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx
mov     [rbp+arg_10], r8
mov     [rbp+arg_18], r9
mov     eax, 77h 
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 1D8h
mov     eax, 0F0h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_40344D
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_40344D
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_40344D
lea     rax, sub_401000
mov     r11, rax
mov     eax, 79h 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_40344D
call    sub_4010D2
cmp     eax, 0
jnz     loc_40344C
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_40344C
jmp     loc_40344D

loc_40344C:
nop

loc_40344D:
nop
nop
mov     eax, 96h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4034C2
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_4034C2
call    sub_4010D2
cmp     eax, 0
jz      loc_4034C2
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 7Bh 
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4034C2
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_4034C2
jmp     loc_40344D

loc_4034C2:
lea     rax, [rbp+arg_10]
mov     [rbp+var_408], rax
movsx   eax, cs:byte_405055
movsxd  rax, eax
shl     rax, 0Ah
movsx   ecx, cs:byte_405056
movsxd  rcx, ecx
imul    rax, rcx
movsx   ecx, cs:byte_405058
movsxd  rcx, ecx
imul    rax, rcx
movsx   ecx, cs:byte_40505C
movsxd  rcx, ecx
xor     rdx, rdx
div     rcx
movsx   ecx, cs:byte_405056
movsxd  rcx, ecx
shl     rcx, 0Ah
movsx   edx, cs:byte_405058
movsxd  rdx, edx
imul    rcx, rdx
movsx   edx, cs:byte_40505C
movsxd  rdx, edx
mov     [rbp+var_410], rax
mov     rax, rcx
mov     rcx, rdx
xor     rdx, rdx
div     rcx
mov     rcx, [rbp+var_410]
add     rcx, rax
movsx   eax, cs:byte_405054
mov     edx, 0CAh
imul    eax, edx
movsxd  rax, eax
add     rcx, rax
shr     rcx, 1
mov     rax, [rbp+var_408]
mov     r9, rax
mov     rax, [rbp+arg_8]
mov     r8, rax
mov     r11, rcx
lea     rax, [rbp+var_400]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_404303
mov     eax, 0FFFFFFF5h
mov     r10, rax
mov     rcx, r10
call    sub_401028
mov     r10, rax
mov     rcx, r10
call    sub_401604
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsxd  rax, eax
mov     qword ptr [rbp+var_418], rax
mov     eax, [rbp+var_418]
mov     ecx, 7Dh 
imul    eax, ecx
mov     ecx, dword ptr [rbp+arg_0]
add     eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401000
lea     rax, [rbp+var_400]
mov     r10, rax
mov     rcx, r10
call    sub_401F47
mov     rcx, 0
mov     [rsp+450h+var_430], rcx 
mov     rcx, 0
mov     r9, rcx         
mov     r8, rax         
lea     rax, [rbp+var_400]
mov     r11, rax
mov     rax, qword ptr [rbp+var_418]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    sub_4015B4
leave
retn
sub_40339A endp

sub_40362C proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_403637:
nop
cpuid
nop
lea     rax, aScanf     
leave
retn
sub_40362C endp

sub_403644 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_40364F:
nop
cpuid
nop
lea     rax, aSprintf   
leave
retn
sub_403644 endp

sub_40365C proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_403667:
nop
cpuid
nop
lea     rax, aFclose    
leave
retn
sub_40365C endp

sub_403674 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_40367F:
nop
cpuid
nop
lea     rax, aFopen     
leave
retn
sub_403674 endp

sub_40368C proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_403697:
nop
cpuid
nop
lea     rax, aFread     
leave
retn
sub_40368C endp

sub_4036A4 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_4036AF:
nop
cpuid
nop
lea     rax, aFwrite    
leave
retn
sub_4036A4 endp

sub_4036BC proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_4036C7:
nop
cpuid
nop
lea     rax, aExit      
leave
retn
sub_4036BC endp

sub_4036D4 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_4036DF:
nop
cpuid
nop
lea     rax, aStrcpy    
leave
retn
sub_4036D4 endp

sub_4036EC proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_4036F7:
nop
cpuid
nop
lea     rax, aStrtok    
leave
retn
sub_4036EC endp

sub_403704 proc near

arg_0= qword ptr  10h
arg_8= qword ptr  18h
Size= qword ptr  20h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40370F:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx
mov     [rbp+Size], r8
mov     eax, dword ptr [rbp+arg_8]
movsx   ecx, cs:byte_405055
imul    eax, ecx
mov     rcx, [rbp+Size]
mov     r8, rcx         
mov     r11, rax
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    memset
leave
retn
sub_403704 endp

sub_403746 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_403751:
nop
cpuid
nop
lea     rax, aMemcpy    
leave
retn
sub_403746 endp

sub_40375E proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_403769:
nop
cpuid
nop
lea     rax, aStrchr    
leave
retn
sub_40375E endp

sub_403776 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_403781:
nop
cpuid
nop
lea     rax, aStrrchr   
leave
retn
sub_403776 endp

sub_40378E proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_403799:
nop
cpuid
nop
lea     rax, aRand      
leave
retn
sub_40378E endp

sub_4037A6 proc near
push    rbp
mov     rbp, rsp
sub     rsp, 0

loc_4037B1:
nop
cpuid
nop
lea     rax, aRealloc   
leave
retn
sub_4037A6 endp

sub_4037BE proc near

Count= qword ptr  10h
Size= qword ptr  18h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4037C9:
mov     [rbp+Count], rcx
mov     [rbp+Size], rdx
mov     rax, [rbp+Size]
mov     r11, rax
mov     rax, [rbp+Count]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    calloc
leave
retn
sub_4037BE endp

sub_4037EC proc near

var_10= qword ptr -10h
var_8= qword ptr -8
arg_0= qword ptr  10h
arg_8= qword ptr  18h

push    rbp
mov     rbp, rsp
sub     rsp, 30h

loc_4037F7:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx
call    sub_403388
mov     r10, rax
mov     rcx, r10
call    sub_402500
mov     [rbp+var_8], rax
call    sub_4037A6
mov     r11, rax
mov     rax, [rbp+var_8]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401632
mov     rcx, [rbp+arg_8]
mov     r11, rcx
mov     rcx, [rbp+arg_0]
mov     r10, rcx
mov     [rbp+var_10], rax
mov     rcx, r10
mov     rdx, r11
mov     r11, [rbp+var_10]
call    r11
leave
retn
sub_4037EC endp

sub_40384E proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403859:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
call    gets
leave
retn
sub_40384E endp

sub_40386E proc near

var_C= dword ptr -0Ch
var_8= qword ptr -8
arg_0= qword ptr  10h
arg_8= qword ptr  18h
arg_10= qword ptr  20h
arg_18= qword ptr  28h

push    rbp
mov     rbp, rsp
sub     rsp, 30h

loc_403879:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx
mov     [rbp+arg_10], r8
mov     [rbp+arg_18], r9
lea     rax, [rbp+arg_18]
mov     [rbp+var_8], rax
mov     rax, [rbp+var_8]
mov     r9, rax
mov     rax, [rbp+arg_10]
mov     r8, rax
mov     rax, [rbp+arg_8]
mov     r11, rax
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_404303
mov     [rbp+var_C], eax
mov     eax, [rbp+var_C]
leave
retn
sub_40386E endp

sub_4038C0 proc near

Buffer= qword ptr  10h
Format= qword ptr  18h
ArgList= qword ptr  20h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4038CB:
mov     [rbp+Buffer], rcx
mov     [rbp+Format], rdx
mov     [rbp+ArgList], r8
mov     rax, [rbp+ArgList]
mov     r8, rax         
mov     rax, [rbp+Format]
mov     r11, rax
mov     rax, [rbp+Buffer]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    vsprintf
leave
retn
sub_4038C0 endp

sub_4038F9 proc near

arg_0= qword ptr  10h
arg_8= qword ptr  18h
arg_10= qword ptr  20h
arg_18= qword ptr  28h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403904:
mov     [rbp+arg_0], rcx
mov     [rbp+arg_8], rdx
mov     [rbp+arg_10], r8
mov     [rbp+arg_18], r9
mov     rax, [rbp+arg_18]
mov     r9, rax
mov     rax, [rbp+arg_10]
mov     r8, rax
mov     rax, [rbp+arg_8]
mov     r11, rax
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_404303
leave
retn
sub_4038F9 endp

sub_40393D proc near

VarName= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403948:
mov     [rbp+VarName], rcx
mov     rax, [rbp+VarName]
mov     r10, rax
mov     rcx, r10        
call    getenv
leave
retn
sub_40393D endp

sub_40395D proc near

Command= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403968:
mov     [rbp+Command], rcx
mov     rax, [rbp+Command]
mov     r10, rax
mov     rcx, r10        
call    system
leave
retn
sub_40395D endp

sub_40397D proc near
push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403988:
call    abort
sub_40397D endp

leave
retn

sub_40398F proc near

arg_0= qword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40399A:
mov     [rbp+arg_0], rcx
mov     rax, [rbp+arg_0]
mov     r10, rax
mov     rcx, r10        
call    atexit
leave
retn
sub_40398F endp

sub_4039AF proc near

DstBuf= qword ptr  10h
SizeInBytes= dword ptr  18h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4039BA:
mov     [rbp+DstBuf], rcx
mov     qword ptr [rbp+SizeInBytes], rdx
mov     rax, qword ptr [rbp+SizeInBytes]
mov     r11, rax
mov     rax, [rbp+DstBuf]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    _getcwd
movsxd  rax, eax
leave
retn
sub_4039AF endp

sub_4039E0 proc near

C= dword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_4039EB:
mov     qword ptr [rbp+C], rcx
mov     eax, [rbp+C]
mov     r10, rax
mov     rcx, r10        
call    tolower
leave
retn
sub_4039E0 endp

sub_4039FF proc near

C= dword ptr  10h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_403A0A:
mov     qword ptr [rbp+C], rcx
mov     eax, [rbp+C]
mov     r10, rax
mov     rcx, r10        
call    toupper
leave
retn
sub_4039FF endp

sub_403A1E proc near

var_48= qword ptr -48h
var_40= qword ptr -40h
var_38= qword ptr -38h
var_30= qword ptr -30h
var_28= qword ptr -28h
var_20= qword ptr -20h
Block= qword ptr -18h
var_10= qword ptr -10h
var_4= dword ptr -4

push    rbp
mov     rbp, rsp
sub     rsp, 70h

loc_403A29:
mov     eax, 0
mov     [rbp+var_4], eax
mov     eax, 0
mov     [rbp+var_4], eax

loc_403A39:
mov     eax, [rbp+var_4]
cmp     eax, 5
jge     loc_4042BE
jmp     loc_403AFC

loc_403A4A:
mov     eax, [rbp+var_4]
mov     rcx, rax
add     eax, 1
mov     [rbp+var_4], eax
cmp     ecx, 0
jz      loc_403AE7
movsx   eax, cs:byte_40505D
movsx   ecx, cs:byte_40505B
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 0FCh
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505C
cdq
idiv    ecx
movsx   ecx, cs:byte_405055
sub     eax, ecx
mov     ecx, 8Ah
imul    eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsx   ecx, cs:byte_405057
movsx   edx, cs:byte_40505A
add     ecx, edx
movsx   edx, cs:byte_405054
mov     [rbp+var_10], rax
mov     eax, 180h
imul    edx, eax
add     ecx, edx
movsx   eax, cs:byte_40505D
sub     ecx, eax
mov     eax, dword ptr [rbp+var_10]
add     eax, ecx
cmp     eax, 0
jz      loc_403AE7
jmp     loc_403AF7

loc_403AE7:
movsx   eax, cs:byte_405055
cmp     eax, 0
jnz     $+6

loc_403AF7:
jmp     loc_403A39

loc_403AFC:
mov     eax, 100h
mov     r10, rax
mov     rcx, r10
call    sub_401028
movsxd  rax, eax
mov     r10, rax
mov     rcx, r10        
call    malloc
mov     [rbp+Block], rax
call    sub_403388
mov     r10, rax
mov     rcx, r10
call    sub_402500
mov     [rbp+var_20], rax
call    sub_4036D4
mov     r11, rax
mov     rax, [rbp+var_20]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401632
lea     rcx, aDHelloWorld 
mov     r11, rcx
mov     rcx, [rbp+Block]
mov     r10, rcx
mov     [rbp+var_28], rax
mov     rcx, r10
mov     rdx, r11
mov     r11, [rbp+var_28]
call    r11
call    sub_403388
mov     r10, rax
mov     rcx, r10
call    sub_402500
mov     [rbp+var_30], rax
call    sub_403644
mov     r11, rax
mov     rax, [rbp+var_30]
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_401632
mov     ecx, [rbp+var_4]
add     ecx, 1
mov     r8, rcx
mov     rcx, [rbp+Block]
mov     r11, rcx
mov     rcx, [rbp+Block]
mov     r10, rcx
mov     [rbp+var_38], rax
mov     rcx, r10
mov     rdx, r11
mov     r11, [rbp+var_38]
call    r11
mov     eax, 102h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403D02
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_403D02
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_403D02
mov     eax, dword ptr [rbp+Block]
mov     r11, rax
mov     eax, 82h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403D02
call    sub_4010D2
cmp     eax, 0
jnz     loc_403C41
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_403C41
jmp     loc_403D02

loc_403C41:
mov     eax, 189h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
mov     eax, 0
setl    al
mov     r10, rax
mov     rcx, r10
call    sub_401000
mov     rax, [rbp+Block]
mov     r11, rax
mov     eax, 84h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40339A
mov     eax, 0A2h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403CFD
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_403CFD
call    sub_4010D2
cmp     eax, 0
jz      loc_403CFD
movsx   eax, cs:byte_405054
cmp     eax, 15Ch
mov     eax, 0
setnle  al
mov     r11, rax
mov     eax, 87h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403CFD
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_403CFD
jmp     loc_403C41

loc_403CFD:
jmp     loc_4042AA

loc_403D02:
mov     eax, 112h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403DA6
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_403DA6
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_403DA6
movsx   eax, cs:byte_405054
cmp     eax, 88h
mov     eax, 0
setnle  al
mov     r11, rax
mov     eax, 8Ah
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403DA6
call    sub_4010D2
cmp     eax, 0
jnz     loc_403D91
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_403D91
jmp     loc_403DA6

loc_403D91:
mov     eax, 45h 
mov     r10, rax
mov     rcx, r10
call    sub_401000
jmp     loc_4042AA

loc_403DA6:
mov     eax, 11Ch
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403E42
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_403E42
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_403E42
mov     eax, 0
mov     r11, rax
mov     eax, 8Fh
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403E42
call    sub_4010D2
cmp     eax, 0
jnz     loc_403E25
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_403E25
jmp     loc_403E42

loc_403E25:
movsx   eax, cs:byte_405057
sub     eax, 90h
mov     r10, rax
mov     rcx, r10
call    sub_401028
jmp     loc_4042AA

loc_403E42:
mov     eax, 124h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403FF8
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_403FF8
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_403FF8
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 1B3h
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
mov     r11, rax
mov     eax, 93h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403FF8
call    sub_4010D2
cmp     eax, 0
jnz     loc_403EE6
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_403EE6
jmp     loc_403FF8

loc_403EE6:
mov     eax, 95h
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 250h
mov     eax, 12Ch
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403F7E
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_403F7E
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_403F7E
lea     rax, sub_401000
mov     r11, rax
mov     eax, 97h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403F7E
call    sub_4010D2
cmp     eax, 0
jnz     loc_403F7D
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_403F7D
jmp     loc_403F7E

loc_403F7D:
nop

loc_403F7E:
nop
nop
mov     eax, 0B4h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_403FF3
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_403FF3
call    sub_4010D2
cmp     eax, 0
jz      loc_403FF3
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 99h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_403FF3
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_403FF3
jmp     loc_403F7E

loc_403FF3:
jmp     loc_4042AA

loc_403FF8:
mov     eax, 136h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4041EB
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_4041EB
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_4041EB
movsx   eax, cs:byte_405057
movsx   ecx, cs:byte_40505A
add     eax, ecx
movsx   ecx, cs:byte_405054
mov     edx, 1CEh
imul    ecx, edx
add     eax, ecx
movsx   ecx, cs:byte_40505D
sub     eax, ecx
movsx   ecx, cs:byte_405057
mov     r10, rcx
mov     [rbp+var_40], rax
mov     rcx, r10
call    sub_401028
cmp     eax, 0
jz      loc_40407E
jmp     loc_40408A

loc_40407E:
movsx   eax, cs:byte_405054
jmp     loc_404091

loc_40408A:
movsx   eax, cs:byte_405056

loc_404091:
mov     ecx, dword ptr [rbp+var_40]
imul    ecx, eax
mov     r11, rcx
mov     eax, 9Ch
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4041EB
call    sub_4010D2
cmp     eax, 0
jnz     loc_4040D9
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_4040D9
jmp     loc_4041EB

loc_4040D9:
mov     eax, 9Eh
mov     r10, rax
mov     rcx, r10
call    sub_401028
add     eax, 274h
mov     eax, 13Eh
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_404171
movsx   eax, cs:byte_405055
cmp     eax, 0
jz      loc_404171
call    sub_4010D2
movsx   ecx, cs:byte_405056
cmp     ecx, eax
jle     loc_404171
lea     rax, sub_401000
mov     r11, rax
mov     eax, 0A0h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_404171
call    sub_4010D2
cmp     eax, 0
jnz     loc_404170
movsx   eax, cs:byte_405054
cmp     eax, 0
jnz     loc_404170
jmp     loc_404171

loc_404170:
nop

loc_404171:
nop
nop
mov     eax, 0BDh
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4041E6
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_4041E6
call    sub_4010D2
cmp     eax, 0
jz      loc_4041E6
movsx   eax, cs:byte_405054
mov     r11, rax
mov     eax, 0A2h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4041E6
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_4041E6
jmp     loc_404171

loc_4041E6:
jmp     loc_4042AA

loc_4041EB:
mov     eax, 1E9h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
mov     eax, 0
setl    al
mov     r10, rax
mov     rcx, r10
call    sub_401000
lea     rax, aError     
mov     r11, rax
mov     eax, 0A4h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40339A
mov     eax, 0C2h
movsx   ecx, cs:byte_405054
cmp     eax, ecx
jle     loc_4042AA
movsx   eax, cs:byte_40505C
movsx   ecx, cs:byte_405057
cmp     eax, ecx
jle     loc_4042AA
call    sub_4010D2
cmp     eax, 0
jz      loc_4042AA
movsx   eax, cs:byte_405054
cmp     eax, 19Ch
mov     eax, 0
setnle  al
mov     r11, rax
mov     eax, 0A7h
mov     r10, rax
mov     rcx, r10
mov     rdx, r11
call    sub_40112B
cmp     eax, 0
jz      loc_4042AA
movsx   eax, cs:byte_405059
cmp     eax, 0
jz      loc_4042AA
jmp     loc_4041EB

loc_4042AA:
mov     rax, [rbp+Block]
mov     r10, rax
mov     rcx, r10        
call    free
jmp     loc_403A4A

loc_4042BE:
call    _getch
movsx   ecx, cs:byte_405057
movsx   edx, cs:byte_40505A
add     ecx, edx
movsx   edx, cs:byte_405054
mov     [rbp+var_48], rax
mov     eax, 1F8h
imul    edx, eax
add     ecx, edx
movsx   eax, cs:byte_40505D
sub     ecx, eax
mov     eax, dword ptr [rbp+var_48]
add     eax, ecx
mov     r10, rax
mov     rcx, r10
call    sub_401028
leave
retn
sub_403A1E endp

sub_404303 proc near

Buffer= qword ptr  10h
BufferCount= qword ptr  18h
Format= qword ptr  20h
ArgList= qword ptr  28h

push    rbp
mov     rbp, rsp
sub     rsp, 20h

loc_40430E:
mov     [rbp+Buffer], rcx
mov     [rbp+BufferCount], rdx
mov     [rbp+Format], r8
mov     [rbp+ArgList], r9
mov     rax, [rbp+ArgList]
mov     r9, rax         
mov     rax, [rbp+Format]
mov     r8, rax         
mov     rax, [rbp+BufferCount]
mov     r11, rax
mov     rax, [rbp+Buffer]
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    _vsnprintf
leave
retn
sub_404303 endp

algn_404347:
align 8

public start
start proc near

var_30= qword ptr -30h
var_18= qword ptr -18h
var_10= qword ptr -10h
var_4= dword ptr -4

push    rbp
mov     rbp, rsp
sub     rsp, 50h

loc_404353:
mov     eax, 0
mov     [rbp+var_4], eax
mov     eax, 1
mov     r10, rax
mov     rcx, r10        
call    __set_app_type
mov     eax, 30000h
mov     r11, rax
mov     eax, 10000h
mov     r10, rax
mov     rcx, r10        
mov     rdx, r11        
call    _controlfp
mov     rax, cs:__argc
mov     rcx, cs:__argv
mov     rdx, cs:_environ
mov     [rbp+var_10], rax
lea     rax, [rbp+var_4]
mov     [rsp+50h+var_30], rax
mov     eax, cs:dword_4054A8
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
call    sub_403A1E
mov     r10, rax
mov     rcx, r10        
call    exit
start endp

leave
retn
algn_40440F:
align 10h
stru_404410 UNWIND_INFO_HDR <1, 4, 2, 5>
UNWIND_CODE <4, 3>      
UNWIND_CODE <1, 50h>    
push    rbp
mov     rbp, rsp
sub     rsp, 30h

sub_404423 proc near
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
mov     rcx, r10        
mov     rdx, r11        
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
call    sub_403A1E
leave
retn
sub_404423 endp

algn_40449B:
align 20h

align 8

align 10h

align 8

align 20h

align 8

align 10h

align 8

align 20h

align 8

align 10h

align 8

align 20h

align 8

align 10h

align 8

align 20h

align 8

align 10h

align 8

align 20h

align 8

align 10h

align 8

align 20h

align 8

align 100h
dq 140h dup(?)
_text ends

_data segment para public 'DATA' use64
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
align 20h
qword_405060 dq 0

Source db '%d%d',0

Destination db 'Library',0

aC db '%c',0
qword_405078 dq 0
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
aError db 'Error!',0
align 20h
__IMPORT_DESCRIPTOR_kernel32 dd rva off_405234 
dd 0                    
dd 0                    
dd rva aKernel32Dll     
dd rva __imp_WriteConsoleA 
__IMPORT_DESCRIPTOR_msvcrt dd rva off_405264 
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

extrn __imp_WriteConsoleA:qword

extrn __imp_GetStdHandle:qword

extrn __imp_GetProcAddress:qword

extrn __imp_GetModuleHandleA:qword

extrn __imp_IsDebuggerPresent:qword

extrn __imp_strcat:qword

extrn __imp_sprintf:qword

extrn __imp_malloc:qword

extrn __imp_memset:qword

extrn __imp_calloc:qword
extrn __imp_gets:qword

extrn __imp_vsprintf:qword

extrn __imp_getenv:qword

extrn __imp_system:qword

extrn __imp_abort:qword

extrn __imp_atexit:qword

extrn __imp__getcwd:qword

extrn __imp_tolower:qword

extrn __imp_toupper:qword

extrn __imp_free:qword

extrn __imp__getch:qword

extrn __imp__vsnprintf:qword

extrn __imp___set_app_type:qword

extrn __imp__controlfp:qword

extrn __argc:qword

extrn __argv:qword

extrn _environ:qword
extrn __imp___getmainargs:qword

extrn __imp_exit:qword

_data segment para public 'DATA' use64
assume cs:_data

off_405234 dq rva word_405339
dq rva word_405349
dq rva word_405358
dq rva word_405369
dq rva word_40537C
dq 0

off_405264 dq rva word_40539B
dq rva word_4053A4
dq rva word_4053AE
dq rva word_4053B7
dq rva word_4053C0
dq rva word_4053C9
dq rva word_4053D0
dq rva word_4053DB
dq rva word_4053E4
dq rva word_4053ED
dq rva word_4053F5
dq rva word_4053FE
dq rva word_405408
dq rva word_405412
dq rva word_40541C
dq rva word_405423
dq rva word_40542C
dq rva word_405439
dq rva word_40544A
dq rva word_405457
dq rva word_405460
dq rva word_405469
dq rva word_405474
dq rva word_405484
dq 0
aKernel32Dll db 'kernel32.dll',0
word_405339 dw 0
db 'WriteConsoleA',0
word_405349 dw 0
db 'GetStdHandle',0
word_405358 dw 0
db 'GetProcAddress',0
word_405369 dw 0
db 'GetModuleHandleA',0
word_40537C dw 0
db 'IsDebuggerPresent',0
aMsvcrtDll db 'msvcrt.dll',0
word_40539B dw 0
db 'strcat',0
word_4053A4 dw 0
db 'sprintf',0
word_4053AE dw 0
db 'malloc',0
word_4053B7 dw 0
db 'memset',0
word_4053C0 dw 0
db 'calloc',0
word_4053C9 dw 0
db 'gets',0
word_4053D0 dw 0
db 'vsprintf',0
word_4053DB dw 0
db 'getenv',0
word_4053E4 dw 0
db 'system',0
word_4053ED dw 0
db 'abort',0
word_4053F5 dw 0
db 'atexit',0
word_4053FE dw 0
db '_getcwd',0
word_405408 dw 0
db 'tolower',0
word_405412 dw 0
db 'toupper',0
word_40541C dw 0
db 'free',0
word_405423 dw 0
db '_getch',0
word_40542C dw 0
db '_vsnprintf',0
word_405439 dw 0
db '__set_app_type',0
word_40544A dw 0
db '_controlfp',0
word_405457 dw 0
db '__argc',0
word_405460 dw 0
db '__argv',0
word_405469 dw 0
db '_environ',0
word_405474 dw 0
db '__getmainargs',0
word_405484 dw 0
db 'exit',0
align 10h
unk_405490 db    0
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

byte_4054A0 db 8 dup(0)
dword_4054A8 dd 0
align 1000h
_data ends

_pdata segment para public 'DATA' use64
assume cs:_pdata

ExceptionDir RUNTIME_FUNCTION <rva loc_40100B, \
                  rva algn_40101E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_401033, \
                  rva sub_401085, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_401090, \
                  rva sub_4010D2, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4010DD, \
                  rva sub_40112B, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_401136, \
                  rva sub_40130B, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_401316, \
                  rva sub_4015B4, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4015BF, \
                  rva sub_401604, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40160F, \
                  rva sub_401632, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40163D, \
                  rva sub_401664, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40166F, \
                  rva sub_401688, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_401693, \
                  rva sub_401F47, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_401F52, \
                  rva sub_402050, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40205B, \
                  rva sub_402460, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40246B, \
                  rva sub_402480, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40248B, \
                  rva sub_4024A0, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4024AB, \
                  rva sub_4024C0, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4024CB, \
                  rva sub_4024E0, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4024EB, \
                  rva sub_402500, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40250B, \
                  rva sub_402520, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40252B, \
                  rva sub_40274C, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_402757, \
                  rva sub_40275A, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_402765, \
                  rva sub_403268, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403273, \
                  rva sub_40327A, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403285, \
                  rva sub_40328C, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403297, \
                  rva sub_40329E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4032A9, \
                  rva sub_4032B0, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4032BB, \
                  rva sub_4032C2, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4032CD, \
                  rva sub_4032D4, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4032DF, \
                  rva sub_4032E6, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4032F1, \
                  rva sub_4032F8, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403303, \
                  rva sub_40330A, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403315, \
                  rva sub_40331C, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403327, \
                  rva sub_40332E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403339, \
                  rva sub_403340, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40334B, \
                  rva sub_403352, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40335D, \
                  rva sub_403364, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40336F, \
                  rva sub_403376, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403381, \
                  rva sub_403388, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403393, \
                  rva sub_40339A, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4033A5, \
                  rva sub_40362C, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403637, \
                  rva sub_403644, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40364F, \
                  rva sub_40365C, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403667, \
                  rva sub_403674, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40367F, \
                  rva sub_40368C, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403697, \
                  rva sub_4036A4, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4036AF, \
                  rva sub_4036BC, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4036C7, \
                  rva sub_4036D4, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4036DF, \
                  rva sub_4036EC, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4036F7, \
                  rva sub_403704, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40370F, \
                  rva sub_403746, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403751, \
                  rva sub_40375E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403769, \
                  rva sub_403776, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403781, \
                  rva sub_40378E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403799, \
                  rva sub_4037A6, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4037B1, \
                  rva sub_4037BE, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4037C9, \
                  rva sub_4037EC, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4037F7, \
                  rva sub_40384E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403859, \
                  rva sub_40386E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403879, \
                  rva sub_4038C0, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4038CB, \
                  rva sub_4038F9, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403904, \
                  rva sub_40393D, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403948, \
                  rva sub_40395D, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403968, \
                  rva sub_40397D, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403988, \
                  rva sub_40398F, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40399A, \
                  rva sub_4039AF, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4039BA, \
                  rva sub_4039E0, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_4039EB, \
                  rva sub_4039FF, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403A0A, \
                  rva sub_403A1E, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_403A29, \
                  rva sub_404303, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_40430E, \
                  rva algn_404347, \
                  rva stru_401020>
RUNTIME_FUNCTION <rva loc_404353, \
                  rva algn_40440F, \
                  rva stru_404410>
RUNTIME_FUNCTION <rva sub_404423, \
                  rva algn_40449B, \
                  rva stru_404410>
align 1000h
_pdata ends

end start