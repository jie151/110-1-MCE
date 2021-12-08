;define control registers (with address)
XBR2		equ        0e3h
P1MDIN		equ        0adh	;button的位置
P2MDOUT		equ        0a6h	;LED的位置
WDTCN		equ        0ffh
SFRPAGE		equ        084h
P1			equ        090h
P2			equ        0a0h
 
;define control words
CONFIG_PAGE		equ		0fh	
LEGACY_PAGE		equ		00h

;turn-off the watch-dog timer
mov		WDTCN, #0deh	;WDTCN的密碼，先放入0de再放入0ad就可以關掉
mov		WDTCN, #0adh
 
;setup port configuration
mov		SFRPAGE, #CONFIG_PAGE
mov		XBR2, #0c0h
mov		P1MDIN, #0ffh   ;把p1設為input
mov		P2MDOUT, #0ffh  ;p2設為output
mov		SFRPAGE, #LFGACY_PAGE        
                
mov	P2, #00000000B
mov R0, #10

wait_button_click :     ;按了P1才繼續做
    mov A, P1        
    JZ  wait_button_click        
MAIN:
    mov A, #00000001B   ;對應bit為1即亮 b=binary
Loop:
    mov     P2, A   ;output
    LCALL   Delay   ;call a function at label "Delay"
    RR      A   ; RR:rotate right
    LJMP    Loop        
 
    Delay:  MOV R1, #10 ;a nested loop to delay some time (10*10*245)
    Delay1: MOV R2, #245
    Delay2: DJNZ    R2, Delay2
    DJNZ    R1, Delay1
    DJNZ    R0, Delay
    RET
END
