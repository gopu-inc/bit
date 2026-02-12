section .data
    prompt    db "AMAX-SQL> ", 0
    msg_wel   db "Moteur AMAX SQL v0.1 (iSH x86)", 0xa, 0
    msg_sel   db "Commande SELECT detectee !", 0xa, 0
    msg_err   db "Commande inconnue.", 0xa, 0
    cmd_select db "SELECT", 0

section .bss
    buffer    resb 100    ; Espace pour stocker l'entrée utilisateur

section .text
    global _start

_start:
    ; Affichage du message de bienvenue
    push msg_wel
    call _print
    add esp, 4

main_loop:
    ; 1. Afficher le prompt
    push prompt
    call _print
    add esp, 4

    ; 2. Lire l'entrée (sys_read)
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin (clavier)
    mov ecx, buffer     ; où stocker
    mov edx, 100        ; taille max
    int 0x80

    ; Ajouter un zéro terminal à la place du 'Entrée' (\n)
    ; (Le sys_read renvoie le nombre d'octets lus dans EAX)
    mov byte [buffer + eax - 1], 0

    ; 3. Comparer avec "SELECT"
    push cmd_select
    push buffer
    call _strcmp
    add esp, 8

    cmp eax, 0          ; Si EAX = 0, les chaînes sont identiques
    je .is_select
    
    push msg_err
    call _print
    add esp, 4
    jmp main_loop

.is_select:
    push msg_sel
    call _print
    add esp, 4
    jmp main_loop

; --- FONCTION _print(addr) ---
_print:
    push ebp
    mov ebp, esp
    mov esi, [ebp+8]
    xor edx, edx
.lp:
    cmp byte [esi+edx], 0
    je .out
    inc edx
    jmp .lp
.out:
    mov ecx, esi
    mov ebx, 1
    mov eax, 4
    int 0x80
    pop ebp
    ret

; --- FONCTION _strcmp(s1, s2) ---
; Retourne 0 dans EAX si identique
_strcmp:
    push ebp
    mov ebp, esp
    mov esi, [ebp+8]  ; s1 (buffer)
    mov edi, [ebp+12] ; s2 (cmd_select)
.loop:
    mov al, [esi]
    mov bl, [edi]
    cmp al, bl
    jne .not_equal
    cmp al, 0
    je .equal
    inc esi
    inc edi
    jmp .loop
.not_equal:
    mov eax, 1
    jmp .done
.equal:
    xor eax, eax
.done:
    pop ebp
    ret

