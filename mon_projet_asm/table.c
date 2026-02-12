section .data
    ; Simulation d'une table : ID (4 octets), Nom (12 octets)
    table_users db 1, 0, 0, 0, "Mauricio    "
                db 2, 0, 0, 0, "Alice       "
                db 3, 0, 0, 0, "Bob         "
    row_size    equ 16    ; 4 (int) + 12 (string)
    row_count   equ 3

    msg_tab     db "  | ID | NOM         |", 0xa, 0
    msg_line    db "  |----|-------------|", 0xa, 0

