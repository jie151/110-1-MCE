;define control registers (with address) 
XBR2        equ        0e3h
P1MDIN      equ        0adh ;button的位置
P2MDOUT     equ        0a6h ;LED的位置
WDTCN       equ        0ffh
SFRPAGE     equ        084h
P1          equ        090h
P2          equ        0a0h

TMOD    equ     089h
CKCON   equ     0x8e
IE      equ     0A8h
TL0     equ     08Ah
TH0     equ     08Ch
TCON    equ     088h

;define control words
CONFIG_PAGE     equ     0fh 
LEGACY_PAGE     equ     00h

org 0h
ljmp main
org 0bh
ljmp Timer0_ISR

org 0100h
main:
    lcall Port_Config
    lcall wait_start
    lcall Timer_Config
    mov R0, #4
    mov R1, #80h
    mov R3, #5
    mov R5, #00011111b
    mov R4, #0

loop:
    mov P2, R1
    sjmp loop

Port_Config:
    ;turn-off the watch-dog timer
    mov WDTCN, #0deh
    mov WDTCN, #0adh
   
    ;setup port configuration
    mov SFRPAGE, #CONFIG_PAGE
    mov XBR2, #0c0h
    mov P1MDIN, #0ffh
    mov P2MDOUT, #0ffh
    mov SFRPAGE, #LEGACY_PAGE
    ret
Timer_Config:
    mov TMOD, #01h ;mode 1
    mov TCON, #010h ;TR0(bit4) = 1
    mov CKCON, #010h ;
    mov IE, #082h ;EA(bit7)=1, ET0(bit1) =1
    mov TL0,#0C6h ;#C6h (1734= 0x6C6)
    mov TH0,#06h ;#6h
    ret
init_suspend:
    mov R1, #00h
    jmp suspend_loop
suspend_loop:
    mov A, R4 //complement carry
    CPL A
    ANL A, R5
    mov P2, A
    mov R4, A
    jb P1.2, running_init //resume button
    lcall delay
    sjmp suspend_loop

wait_start:
    JNB P1.0, wait_start

Timer0_ISR:
    DJNZ R0, reset_timer ;跑四次
    mov R0, #4 
    djnz R3, countDown
    mov P2, #0h
    ljmp running_init

restart:
    ljmp main
init_terminate:
    mov P2, #0h
terminate:
    jb P1.4, restart
    sjmp terminate

running_init:
    mov A, #0
    lcall delay
    mov P2, A
    lcall delay
    sjmp running_loop
running_loop:
    add A, #08h
    mov P2, A
    jb P1.1, init_suspend
    jb P1.3, init_terminate
    lcall delay
    sjmp running_loop

delay:
    mov r1, #10
delay1:
    mov R2, #245
delay2:
    djnz R2, delay2
    djnz R1, delay1
    djnz R0, delay
    ret


countDown:
    mov A, R1
    ADD A, #1
    RR A ;1000 -> 10001 -> 11000
    mov R1, A

reset_timer:
    mov TL0, #0C6h ;#C6h (1734= 0x6C6)
    mov TH0, #06h ;#6h
    reti
    end
