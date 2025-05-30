@echo off
rem Set TCC directory
set "path=%path%;C:\tcc"

set "FLAGS=-w -D NO_CFLOW -D ANTIDEBUG_V2 -D FAKE_SIGNS -D VIRT"

for %%F in (
    dll.c
    fib.c
    hello_dll.c
    hello_win.c
    sorter.c
    virtualmachine.c
    virtualmachine_unit.c
) do (
    tcc "%%F" %FLAGS%
)
