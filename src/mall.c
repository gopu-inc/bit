// mall.c
#include "bit.h"

void bit_mall(int verbose) {
    char *token = get_token(verbose);
    char *remote = get_stored_remote();
    if (!token || !remote) return;

    // On utilise un pipe vers jq pour formater la sortie
    // .values[] | "\(.date | .[0:16]) | \(.author.raw | .[0:15]) | \(.message)"
    char *jq_cmd = "jq -r '.values[] | \"\\(.date | .[0:16]) | \\(.hash | .[0:7]) | \\(.message | .[0:50])\"'";
    
    FILE *jq_pipe = popen(jq_cmd, "w");
    if (!jq_pipe) {
        fprintf(stderr, "Error: jq is not installed.\n");
        return;
    }

    CURL *curl = curl_easy_init();
    if (curl) {
        char url[512];
        snprintf(url, 512, "https://api.bitbucket.org/2.0/repositories/%s/commits", remote);

        struct curl_slist *headers = NULL;
        char auth[2100];
        snprintf(auth, 2100, "Authorization: Bearer %s", token);
        headers = curl_slist_append(headers, auth);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Rediriger la sortie de CURL vers le pipe de jq
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, jq_pipe);

        printf("\n--- BIT MALL (Recent Activity) ---\n");
        printf("DATE             | HASH    | MESSAGE\n");
        printf("--------------------------------------\n");

        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(res));

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        pclose(jq_pipe);
        printf("--------------------------------------\n");
    }
}

