[bits 16]
[org 0x7C00]

; jmp word 0x0000:start

%macro sleep 1
  mov ah, 86h
  mov cx, (((%1)*1000) >> 16)
  mov dx, (((%1)*1000) % 0xFFFF)
  int 15h
%endmacro

columns equ 20
rows equ 10

points_w equ 0x0
lives_w equ points_w + 2
ball_x_dw equ lives_w + 2
ball_y_dw equ ball_x_dw + 4
ball_x_speed_dw equ ball_y_dw + 4
ball_y_speed_dw equ ball_x_speed_dw + 4
blocks equ ball_y_speed_dw + 4

block_empty equ 0
block_green equ 1
block_red equ 2

  
start:
  mov ax, 0xB800
  mov gs, ax
  mov ax, 0x8000
  mov es, ax

  call initGame

game:
  call getKeyIfAvailable

  ; push ax
  call updateGame


  ; mov ax, 7Fh
  cmp al, 0h
  jz .nope

  ; shr ax, 8

  ; call toHex

  ; mov [gs:0], dl
  ; mov byte [gs:1], 0x0F
  ; mov [gs:2], cl
  ; mov byte [gs:3], 0x0F


  .nope:
  sleep 50

  jmp start


initGame:
  mov cx, rows-1
  .loop:
  ; save cx
  push cx
  mov ax, cx
  call initRow
  pop cx
  dec cx
  jg .loop
  ret

initRow:
  ; expects row number in ax
  push bx
  mov bx, blocks
  mov cx, rows * columns
  .loop:
  mov byte [es: bx], block_green
  inc bx
  dec cx
  jnz .loop
  pop bx
  ret

updateGame:
  

  mov bx, blocks
  mov cx, rows * columns
  xor di, di

  .blockLoop:
  mov al, [es:bx]
  mov dl, 4
  .innerBlockLoop:
  cmp al, block_green
  je .drawGreen
  ; cmp al, block_red
  ; je .drawRed

  ; .drawBlank

  mov word [gs:di], 0x442B

  jmp .drawn

  .drawGreen:
  mov word [gs:di], 0x2220
  ; .drawRed:

  .drawn:
  inc di
  inc di
  dec dl
  jnz .innerBlockLoop
  
  inc bx
  dec cx
  jnz .blockLoop

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
  mov ax, 0x100
  ; mov ah, 1
  int 16h
  ; xor ax, ax
  jz .nope
  xor ax, ax
  int 16h
  shr ax, 8
  ret
  .nope:
  ; dec ah
  xor ax, ax
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




