;*****************************************************************
; devuelve en BX la cantidad de caracteres que tiene un string
; DS:SI apunta al string.
;*****************************************************************

STRLEN PROC
	mov bx,0
STRL01:
	cmp BYTE PTR [SI+BX],'$'
	je STREND
	inc BX
	jmp STRL01
STREND:
	ret
STRLEN ENDP

;*****************************************************************
; copia DS:SI a ES:DI; busca la cantidad de caracteres
;*****************************************************************

COPIAR PROC
	call STRLEN				; busco la cantidad de caracteres
	cmp bx,MAXSTRSIZE
	jle COPIARSIZEOK
	mov bx,MAXSTRSIZE
COPIARSIZEOK:
	mov cx,bx				; la copia se hace de ’CX’ caracteres
	cld						; cld es para que la copia se realice hacia adelante 
	rep movsb				; copia la cadea
	mov al,'$'				; carÆcter terminador
	mov BYTE PTR [DI],al	; el registro DI quedo apuntando al final
	ret
COPIAR ENDP

;*****************************************************************
; concatena DS:SI al final de ES:DI.
; busco el size del primer string
; sumo el size del segundo string
; si la suma excede MAXSTRSIZE, copio solamente MAXSTRSIZE caracteres
; si la suma NO excede MAXSTRSIZE, copio el total de caracteres que tiene el segundo string 
;*****************************************************************

CONCAT PROC
	push ds
	push si
	call STRLEN				; busco la cantidad de caracteres del 2do string
	mov dx,bx				; guardo en DX la cantidad de caracteres en el origen.
	mov si,di
	push es
	pop ds
	call STRLEN				; tamaño del 1er string
	add di,bx				; DI ya queda apuntando al final del primer string
	add bx,dx
	cmp bx,MAXSTRSIZE		; excede el tama#o maximo?
	jg CONCATSIZEMAL
CONCATSIZEOK:				; La suma no excede el maximo, copio todos los caracteres del segundo string.
	mov cx,dx
	jmp CONCATSIGO
CONCATSIZEMAL:				; La suma de caracteres de los 2 strings exceden el maximo
	sub bx,MAXSTRSIZE
	sub dx,bx
	mov cx,dx				; copio lo maximo permitido, el resto se pierde.
CONCATSIGO:
	push ds
	pop es
	pop si
	pop ds
	cld						; cld es para que la copia se realice hacia adelante
	rep movsb				; copia la cadena
	mov al,'$'				; caracter terminador
	mov BYTE PTR [DI],al	; el registro DI quedo apuntando al final
	ret
CONCAT ENDP