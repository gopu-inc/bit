#include "bit.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- 1. CRÉER UNE PULL REQUEST ---
// Exemple: bit merge create feature-branch main "Titre de ma PR"
void bit_pr_create(const char* source, const char* destination, const char* title, int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote();
    if (!token || !remote) return;

    CURL *curl = curl_easy_init();
    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "https://api.bitbucket.org/2.0/repositories/%s/pullrequests", remote);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth[2100];
        snprintf(auth, sizeof(auth), "Authorization: Bearer %s", token);
        headers = curl_slist_append(headers, auth);

        char data[1024];
        snprintf(data, sizeof(data), 
            "{\"title\": \"%s\", \"source\": {\"branch\": {\"name\": \"%s\"}}, \"destination\": {\"branch\": {\"name\": \"%s\"}}}",
            title, source, destination);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        printf("[BIT] Creating Pull Request: %s -> %s...\n", source, destination);
        
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 201) printf("✅ PR Created successfully!\n");
            else printf("❌ Error creating PR (Code %ld)\n", http_code);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// --- 2. ACCEPTER (MERGE) UNE PULL REQUEST ---
// Note: Il faut l'ID de la PR (qu'on récupère via mall ou l'interface)
void bit_pr_merge(int pr_id, int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote();
    if (!token || !remote) return;

    CURL *curl = curl_easy_init();
    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "https://api.bitbucket.org/2.0/repositories/%s/pullrequests/%d/merge", remote, pr_id);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth[2100];
        snprintf(auth, sizeof(auth), "Authorization: Bearer %s", token);
        headers = curl_slist_append(headers, auth);

        // On envoie un objet vide pour confirmer le merge
        const char *data = "{}"; 

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        printf("[BIT] Merging Pull Request #%d...\n", pr_id);
        
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            printf("🚀 PR #%d merged and closed!\n", pr_id);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

