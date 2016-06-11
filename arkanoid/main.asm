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

  push ax
  call updateGame

  ; test ax

  ; mov ax, 7Fh
  cmp ax, 0h
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
  mov cx, rows
  .loop:
  ; save cx
  push cx
  ; push arg
  push cx
  call initRow
  pop cx
  dec cx
  test cx, cx
  jg .loop
  ret

initRow:
  pop ax
  push bx
  ; mul ax, columns
  mov bx, columns
  mul bx
  mov bx, ax
  mov cx, columns-1
  add bx, blocks
  add bx, cx
  .loop:
  ; mov byte [es:bx + cx], block_green
  mov byte [es: bx], block_green
  dec cx
  dec bx
  test cx, cx
  jnz .loop
  pop bx
  ret

updateGame:
  pop ax

; update pallet


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




