; src/main.asm
section .data
    msg db "Installation ASM sur iSH : Reussie !", 0xa, 0

section .text
    global _start

_start:
    push msg        ; Pousse l'adresse du message sur la pile
    call _print     ; Appelle notre fonction d'affichage
    add esp, 4      ; Nettoie la pile (1 argument de 4 octets)

    ; Sortie du programme
    mov eax, 1      ; sys_exit
    xor ebx, ebx    ; code de retour 0
    int 0x80

_print:
    push ebp
    mov ebp, esp
    mov esi, [ebp+8] ; Récupère l'adresse du message

    ; Calculer la longueur (strlen simple)
    xor ecx, ecx
.loop:
    cmp byte [esi + ecx], 0
    je .done
    inc ecx
    jmp .loop

.done:
    mov edx, ecx    ; Longueur
    mov ecx, esi    ; Adresse du texte
    mov ebx, 1      ; stdout
    mov eax, 4      ; sys_write
    int 0x80

    pop ebp
    ret

