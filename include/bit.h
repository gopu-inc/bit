// bit.h
#ifndef BIT_H
#define BIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Chemins
#define BIT_DIR ".bit"
#define BIT_CONFIG ".bit/config"
#define BIT_REMOTE ".bit/fetch/remote"
#define BIT_SHA ".bit/fetch/sha256"
#define BIT_INDEX ".bit/index"
#define BIT_COMMIT_MSG ".bit/COMMIT_MSG"

void bit_add(const char* path);
void bit_commit(const char* message);
void bit_pull(int verbose);
void bit_ci(int verbose);
void bit_work_check();
void bit_pr_create(const char* source, const char* destination, const char* title, int verbose);
void bit_pr_merge(int pr_id, int verbose);

struct MemoryStruct {
    char *memory;
    size_t size;
};

// --- Prototypes Core ---
int bit_copy(const char *url, int verbose);
void bit_login();
void bit_whoami();
void bit_mall();
void bit_sends();
void bit_sha256(const char* file);
void bit_branch_create(const char* name, int verbose);
void bit_switch(const char* name);
void bit_tag(const char* tag_name, int verbose);

// --- Utilitaires & Réseau ---
char* get_token(int verbose);
int download_repository(const char* url, const char* token, const char* output_file, int verbose);
char* get_stored_remote();
int save_credentials(const char* key, const char* secret);
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
char* get_json_value(const char* json, const char* key);

#endif


