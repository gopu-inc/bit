#include "bit.h"
#include <curl/curl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void bit_sends(int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote(); 
    
    if (!token || !remote) {
        fprintf(stderr, "❌ Error: Not logged in or remote not found. Run 'bit login' or 'bit copy'.\n");
        return;
    }

    CURL *curl = curl_easy_init();
    if (!curl) return;

    char url[512];
    snprintf(url, sizeof(url), "https://api.bitbucket.org/2.0/repositories/%s/src", remote);

    struct curl_slist *headers = NULL;
    char auth[2100];
    snprintf(auth, sizeof(auth), "Authorization: Bearer %s", token);
    headers = curl_slist_append(headers, auth);

    curl_mime *mime = curl_mime_init(curl);
    curl_mimepart *part;

    // --- 1. DÉTERMINATION DE LA BRANCHE CIBLE ---
    // On lit .bit/HEAD pour savoir vers quelle branche pousser
    char branch[128] = "main"; // Valeur par défaut
    FILE *head = fopen(".bit/HEAD", "r");
    if (head) {
        if (fscanf(head, "%127s", branch) != 1) strcpy(branch, "main");
        fclose(head);
    }
    
    // Ajout du paramètre branch au formulaire MIME
    part = curl_mime_addpart(mime);
    curl_mime_name(part, "branch");
    curl_mime_data(part, branch, CURL_ZERO_TERMINATED);

    // --- 2. RÉCUPÉRATION DU MESSAGE DE COMMIT ---
    char commit_msg[256] = "Bit sync update"; // Message par défaut
    FILE *fmsg = fopen(BIT_COMMIT_MSG, "r");
    if (fmsg) {
        if (fgets(commit_msg, 256, fmsg)) {
            commit_msg[strcspn(commit_msg, "\n")] = 0; // Nettoie le \n
        }
        fclose(fmsg);
    }
    
    part = curl_mime_addpart(mime);
    curl_mime_name(part, "message");
    curl_mime_data(part, commit_msg, CURL_ZERO_TERMINATED);

    // --- 3. TRAITEMENT DE L'INDEX (Uploads & Deletes) ---
    FILE *index = fopen(".bit/index", "r");
    if (!index) {
        printf("⚠️ Nothing to send. Did you run 'bit add .'?\n");
        curl_mime_free(mime);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return;
    }

    char filename[256], status_or_hash[65];
    int change_count = 0;

    while (fscanf(index, "%255s %64s", filename, status_or_hash) == 2) {
        if (strcmp(status_or_hash, "DELETE") == 0) {
            // Instruction de suppression pour Bitbucket
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "files");
            curl_mime_data(part, filename, CURL_ZERO_TERMINATED);
            if (verbose) printf("[DEBUG] Marking for remote deletion: %s\n", filename);
            change_count++;
        } else {
            // Vérification que le fichier existe localement avant upload
            if (access(filename, F_OK) == 0) {
                part = curl_mime_addpart(mime);
                curl_mime_name(part, filename); // Le nom du champ doit être le chemin du fichier
                curl_mime_filedata(part, filename);
                if (verbose) printf("[DEBUG] Preparing upload: %s\n", filename);
                change_count++;
            }
        }
    }
    fclose(index);

    if (change_count == 0) {
        printf("ℹ️ No changes detected to send.\n");
        curl_mime_free(mime);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return;
    }

    // --- 4. CONFIGURATION ET ENVOI ---
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    printf("[BIT] Connecting to %s (Branch: %s)...\n", remote, branch);
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        if (http_code >= 200 && http_code < 300) {
            printf("🚀 Success! Commit published: \"%s\"\n", commit_msg);
            // On supprime le message de commit après succès
            unlink(BIT_COMMIT_MSG);
        } else {
            printf("❌ API Error (%ld). Check if branch '%s' exists and your permissions.\n", http_code, branch);
        }
    } else {
        fprintf(stderr, "❌ Connection failed: %s\n", curl_easy_strerror(res));
    }

    // --- 5. NETTOYAGE ---
    curl_mime_free(mime);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}
