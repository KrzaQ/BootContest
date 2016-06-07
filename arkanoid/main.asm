[bits 16]
[org 0x7C00]

; jmp word 0x0000:start

%macro sleep 1
  mov ah, 86h
  mov cx, (%1*1000) >> 16
  mov dx, (%1*1000) % 0FFh
  int 15h
%endmacro
  
  mov ax, 0B800h
  mov gs, ax
  ; mov gs, 0B800h

start:
  call getKeyIfAvailable
  ; test ax

  cmp ax, 0h
  jz .nope

  shr ax, 8

  call toHex

  mov [gs:0], dl
  mov byte [gs:1], 0x0F
  mov [gs:2], cl
  mov byte [gs:3], 0x0F


  .nope:
  jmp start

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

initGame:
  ret

toHex:
  mov bl, al
  and al, 0Fh
  call .letter
  xor cx, cx
  mov cl, al
  mov al, bl
  shr al, 4
  call .letter
  ; shl ax, 8
  ; add ax, cx
  mov dl, al
  ret
.letter:
  add al, '0'
  cmp al, 10+'0'
  jl .notHex
  add al, 7
  .notHex:
  ret

getKeyIfAvailable:
  mov ah, 1
  int 16h
  xor ax, ax
  jnz .nope
  int 16h
  shr ax, 8
  .nope:
  ret

writeChar:
  
  ret

; epilogue:
; %if ($ - $$) > 510
;   %fatal "Bootloader code exceed 512 bytes."
; %endif

; times 510 - ($ - $$) db 0
; db 0x55
; db 0xAA




