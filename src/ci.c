#include "bit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void bit_ci(int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote();
    if (!token || !remote) return;

    printf("\n\033[1;35m📡 LION STREAMER (DIAGNOSTIC MODE)\033[0m\n");

    char p_uuid[128] = "";
    char cmd[2048];

    // 1. On récupère le dernier pipeline
    snprintf(cmd, sizeof(cmd), 
        "curl -s -g -H \"Authorization: Bearer %s\" \"https://api.bitbucket.org/2.0/repositories/%s/pipelines/?sort=-created_on&pagelen=1\" | jq -r '.values[0].uuid' > .p_uuid", 
        token, remote);
    system(cmd);

    FILE *f = fopen(".p_uuid", "r");
    if(!f || fscanf(f, "%s", p_uuid) == EOF) { printf("❌ Error reading .p_uuid\n"); return; }
    fclose(f);

    if (strlen(p_uuid) < 5 || strcmp(p_uuid, "null") == 0) {
        printf("❌ No active pipeline found on Bitbucket.\n"); return;
    }
    printf("\033[1;32m✅ Pipeline detected: %s\033[0m\n", p_uuid);

    // 2. Boucle d'attente du STEP avec timeout
    char s_uuid[128] = "null";
    int attempts = 0;
    while (strcmp(s_uuid, "null") == 0 && attempts < 30) {
        // -g est CRUCIAL ici car p_uuid contient des { }
        snprintf(cmd, sizeof(cmd), 
            "curl -s -g -H \"Authorization: Bearer %s\" \"https://api.bitbucket.org/2.0/repositories/%s/pipelines/%s/steps/\" | jq -r '.values[0].uuid // \"null\"' > .s_uuid", 
            token, remote, p_uuid);
        system(cmd);

        f = fopen(".s_uuid", "r");
        if(f) { 
            if (fscanf(f, "%127s", s_uuid) == EOF) strcpy(s_uuid, "null");
            fclose(f); 
        }

        if (strcmp(s_uuid, "null") == 0) {
            printf("\033[1;33m⌛ Step not ready... (Attempt %d/30)\033[0m\r", attempts++);
            fflush(stdout);
            sleep(2);
        }
    }

    if (strcmp(s_uuid, "null") == 0) {
        printf("\n❌ Timeout: Bitbucket is taking too long to start the step.\n");
        return;
    }

    printf("\n\033[1;32m✅ Stream Synced! (Step: %s)\033[0m\n", s_uuid);
    // ... suite du code (streaming des logs)
}

