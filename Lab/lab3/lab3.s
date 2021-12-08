;define control registers (with address) 
XBR2        equ        0e3h
P1MDIN      equ        0adh ;button的位置
P2MDOUT     equ        0a6h ;LED的位置
WDTCN       equ        0ffh
SFRPAGE     equ        084h
P1          equ        090h
P2          equ        0a0h

TMOD        equ        089h
CKCON       equ        08eh
IE          equ        0A8h
TL0         equ        08Ah
TH0         equ        08Ch
TCON        equ        088h

;define control words
CONFIG_PAGE equ     0fh 
LEGACY_PAGE equ     00h

org     0h
ljmp    main
org     0bh ;收到interrupt時會跳到的位置
ljmp    Timer0_ISR

org     0100h ;將程式碼放在100的位置
main:
    lcall Port_Config
    lcall Timer_Config
    mov R0, #4
    mov R1, #80h
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

Timer0_ISR:
    DJNZ R0, reset_timer ;跑四次

    mov R0, #4 
    mov A, R1
    RL A
    mov R1, A 
   
reset_timer:
    mov TL0, #0C6h ;#C6h (1734= 0x6C6)
    mov TH0, #06h ;#6h
    reti

end
