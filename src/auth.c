// auth.c
#include "bit.h"

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

char* get_json_value(const char* json, const char* key) {
    char search[128]; sprintf(search, "\"%s\"", key);
    char *pos = strstr(json, search);
    if (!pos) return NULL;
    pos = strstr(pos, ":"); pos = strstr(pos, "\""); pos++;
    char *end = strchr(pos, '\"');
    size_t len = end - pos;
    char *val = malloc(len + 1); strncpy(val, pos, len); val[len] = '\0';
    return val;
}

void bit_login() {
    char key[128], secret[128];
    printf("Client Key: "); scanf("%127s", key);
    printf("Client Secret: "); scanf("%127s", secret);
    save_credentials(key, secret);
}

int save_credentials(const char* key, const char* secret) {
    mkdir(BIT_DIR, 0700);
    FILE *f = fopen(BIT_CONFIG, "w");
    if (!f) return -1;
    fprintf(f, "KEY=%s\nSECRET=%s\n", key, secret);
    fclose(f);
    return 0;
}

char* get_token(int verbose) {
    static char cached[2048] = {0};
    char key[128], secret[128];
    FILE *f = fopen(BIT_CONFIG, "r");
    if (!f || fscanf(f, "KEY=%s\nSECRET=%s", key, secret) < 2) return NULL;
    fclose(f);

    CURL *curl = curl_easy_init();
    struct MemoryStruct chunk = {malloc(1), 0};
    curl_easy_setopt(curl, CURLOPT_URL, "https://bitbucket.org/site/oauth2/access_token");
    curl_easy_setopt(curl, CURLOPT_USERNAME, key);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, secret);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "grant_type=client_credentials");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    if (curl_easy_perform(curl) == CURLE_OK) {
        char *t = get_json_value(chunk.memory, "access_token");
        if (t) { strcpy(cached, t); free(t); }
    }
    free(chunk.memory); curl_easy_cleanup(curl);
    return cached;
}

void bit_whoami() {
    char *token = get_token(0);
    if (!token) return;
    CURL *curl = curl_easy_init();
    struct MemoryStruct chunk = {malloc(1), 0};
    struct curl_slist *h = NULL;
    char auth[2100]; sprintf(auth, "Authorization: Bearer %s", token);
    h = curl_slist_append(h, auth);
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.bitbucket.org/2.0/user");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    if (curl_easy_perform(curl) == CURLE_OK) printf("User: %s\n", chunk.memory);
    free(chunk.memory); curl_slist_free_all(h); curl_easy_cleanup(curl);
}

