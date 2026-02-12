// work file
#include <stdio.h>
#include <stdlib.h>
#include "bit.h"

void bit_work_check() {
    printf("[BIT] Scanning .bitbucket/workflows/ for YAML files...\n");

    // On utilise system() pour lancer la commande shell depuis le C
    // Cette commande liste les fichiers et les affiche proprement
    system("ls .bitbucket/workflows/*.yml 2>/dev/null | xargs -I {} echo \"[BIT] Found workflow: {}\"");
    
    // Petite vérification : si le dossier est vide
    system("[ \"$(ls -A .bitbucket/workflows/*.yml 2>/dev/null)\" ] || echo \"[BIT] ⚠️ No .yml files found in .bitbucket/workflows/\"");
}

