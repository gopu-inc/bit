#include "bit.h"

// Télécharge le dépôt (utilisé par bit copy)
int download_repository(const char* url, const char* token, const char* output_file, int verbose) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    FILE *fp = fopen(output_file, "wb");
    struct curl_slist *h = NULL;
    if (token) {
        char auth[2048]; snprintf(auth, 2048, "Authorization: Bearer %s", token);
        h = curl_slist_append(h, auth);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h);
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    fclose(fp); if(h) curl_slist_free_all(h); curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? 0 : -1;
}

// Récupère le nom du repo stocké (user/repo)
char* get_stored_remote() {
    static char r[256];
    // Cherche le fichier remote dans .bit/fetch/remote
    FILE *f = fopen(".bit/fetch/remote", "r");
    if(!f) f = fopen(".bit/remote", "r"); // Fallback
    if(!f) return NULL;

    if(fscanf(f, "%255s", r) != 1) { fclose(f); return NULL; }
    fclose(f);
    return r;
}

// Crée une branche sur Bitbucket
void bit_branch_create(const char* name, int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote();
    if (!token || !remote) return;

    // Déterminer la branche source (HEAD locale)
    char source[128] = "main";
    FILE *head = fopen(".bit/HEAD", "r");
    if (head) { fscanf(head, "%127s", source); fclose(head); }

    CURL *curl = curl_easy_init();
    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "https://api.bitbucket.org/2.0/repositories/%s/refs/branches", remote);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth[2100];
        snprintf(auth, sizeof(auth), "Authorization: Bearer %s", token);
        headers = curl_slist_append(headers, auth);

        char data[512];
        // On crée la nouvelle branche en pointant sur la branche actuelle
        snprintf(data, sizeof(data), "{\"name\": \"%s\", \"target\": {\"hash\": \"%s\"}}", name, source);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        if (verbose) printf("[DEBUG] Creating branch %s from %s\n", name, source);
        
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) printf("✅ Branch '%s' created on Bitbucket.\n", name);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// Switch local
void bit_switch(const char* name) {
    mkdir(".bit", 0755);
    FILE *f = fopen(".bit/HEAD", "w");
    if (f) {
        fprintf(f, "%s", name);
        fclose(f);
        printf("🔄 Switched to branch '%s'\n", name);
    }
}

// Crée un Tag
void bit_tag(const char* tag_name, int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote();
    if (!token || !remote) return;

    char source[128] = "main";
    FILE *head = fopen(".bit/HEAD", "r");
    if (head) { fscanf(head, "%127s", source); fclose(head); }

    CURL *curl = curl_easy_init();
    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "https://api.bitbucket.org/2.0/repositories/%s/refs/tags", remote);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth[2100];
        snprintf(auth, sizeof(auth), "Authorization: Bearer %s", token);
        headers = curl_slist_append(headers, auth);

        char data[512];
        snprintf(data, sizeof(data), "{\"name\": \"%s\", \"target\": {\"hash\": \"%s\"}}", tag_name, source);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        if (curl_easy_perform(curl) == CURLE_OK) printf("🔖 Tag '%s' created on Bitbucket.\n", tag_name);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
