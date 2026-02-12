// pull.c
#include "bit.h"
#include <stdio.h>
#include <curl/curl.h>
#include <unistd.h>

void bit_pull(int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote();
    if (!token || !remote) return;

    CURL *curl = curl_easy_init();
    if (curl) {
        char url[512];
        // Exemple simple : on pull le README.md pour tester la connexion
        snprintf(url, 512, "https://api.bitbucket.org/2.0/repositories/%s/src/master/README.md", remote);

        struct curl_slist *headers = NULL;
        char auth[2100];
        snprintf(auth, 2100, "Authorization: Bearer %s", token);
        headers = curl_slist_append(headers, auth);

        FILE *f = fopen("README.md", "wb");
        if (!f) return;

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);

        printf("[BIT] Pulling updates from %s...\n", remote);
        if (curl_easy_perform(curl) == CURLE_OK) {
            printf("Success: Files updated.\n");
        }
        
        fclose(f);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}


