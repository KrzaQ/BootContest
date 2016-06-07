[bits 16]
[org 0x7C00]

; jmp word 0x0000:start

%macro sleep 1
  mov ah, 86h
  mov cx, (%1*1000) >> 16
  mov dx, (%1*1000) % 0FFh
  int 15h
%endmacro

start:
  call getKeyIfAvailable
  test ax


  ; sleep 1000
  ; ; Load from floppy stage 2.
  ; ; DL == already set by BIOS
  ; ; AX -- 16 bits, AH AL -- 8 bits
  ; ; EAX -- AX
  
  ; mov ax, 0x2000 ; 0x2000:0x0000
  ; mov es, ax
  ; xor bx, bx ; bx == 0

  ; mov ah, 2  ; read sectors into memory
  ; mov al, 3  ; 1337 stage2  3 * 512 
  ; mov ch, 0
  ; mov cl, 2  ; sectors start from 1, or so they say ;)
  ; mov dh, 0

  ; int 13h
  
  ; ; Jump to stage 2
  ; jmp word 0x2000:0x0000

  ; ; The CPU doesn't reach here. Ever. Hopefully.

; sleep:

toHex:
  mov bl, al
  and al, 0Fh
  
  ; cmp 


toHexLetter:
  cmp al, 10
  add al, '0'
  jl notHex
  add al, 7
  .notHex:
  ret

getKeyIfAvailable:
  mov ah, 1
  int 16h
  xor ax, ax
  jz nope
  int 16h
  mov ax, ah
  .nope:
  ret

; epilogue:
; %if ($ - $$) > 510
;   %fatal "Bootloader code exceed 512 bytes."
; %endif

; times 510 - ($ - $$) db 0
; db 0x55
; db 0xAA




