#include "bit.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

// Génère un hash SHA256 (Imite l'ID d'objet de Git)
void compute_file_hash(const char *filename, char *out_hash) {
    char cmd[512];
    FILE *fp;
    // On utilise sha256sum pour identifier si le contenu a changé
    snprintf(cmd, sizeof(cmd), "sha256sum %s 2>/dev/null | awk '{print $1}'", filename);
    fp = popen(cmd, "r");
    if (fp) {
        if (fgets(out_hash, 65, fp)) {
            out_hash[strcspn(out_hash, "\n")] = 0;
        }
        pclose(fp);
    } else {
        strcpy(out_hash, "00000000000000000000000000000000");
    }
}

// Fonction récursive pour scanner les dossiers (src, include, etc.)
void scan_recursive(const char *base_path, FILE *new_index) {
    DIR *d = opendir(base_path);
    if (!d) return;

    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        // Ignorer les dossiers cachés (.bit, .git, etc.)
        if (dir->d_name[0] == '.') continue;

        char path[1024];
        if (strcmp(base_path, ".") == 0) 
            snprintf(path, sizeof(path), "%s", dir->d_name);
        else 
            snprintf(path, sizeof(path), "%s/%s", base_path, dir->d_name);

        struct stat st;
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                // Si c'est un dossier, on entre dedans
                scan_recursive(path, new_index);
            } else if (S_ISREG(st.st_mode)) {
                // Si c'est un fichier, on vérifie si on doit l'ignorer
                if (strstr(path, ".o") || strcmp(path, "bit") == 0) continue;

                char hash[65];
                compute_file_hash(path, hash);
                fprintf(new_index, "%s %s\n", path, hash);
            }
        }
    }
    closedir(d);
}

// Commande : bit add <path>
void bit_add(const char* path) {
    system("mkdir -p .bit");
    FILE *new_index = fopen(".bit/index.new", "w");
    if (!new_index) return;

    printf("[BIT] Scanning for changes (recursive)...\n");
    
    // 1. Scan tout le projet à partir de là où on est
    scan_recursive(".", new_index);

    // 2. Détection des suppressions (comparaison avec l'ancien état)
    FILE *old_index = fopen(".bit/index", "r");
    if (old_index) {
        char old_name[256], old_hash[65];
        while (fscanf(old_index, "%255s %64s", old_name, old_hash) == 2) {
            if (access(old_name, F_OK) == -1) {
                // Le fichier existait mais n'est plus là -> Marqué DELETE pour sends.c
                fprintf(new_index, "%s DELETE\n", old_name);
                printf("[BIT] Marked for removal: %s\n", old_name);
            }
        }
        fclose(old_index);
    }

    fclose(new_index);
    rename(".bit/index.new", ".bit/index");
    printf("[BIT] Index updated successfully.\n");
}

// Commande : bit commit -m "message"
void bit_commit(const char* message) {
    if (!message || strlen(message) == 0) {
        printf("Error: Please provide a commit message.\n");
        return;
    }
    FILE *f = fopen(BIT_COMMIT_MSG, "w");
    if (f) {
        fprintf(f, "%s", message);
        fclose(f);
        printf("[master] %s\n", message);
    }
}

// Commande : bit restore <file> (Via API + JQ)
void bit_restore(const char* filename, int verbose) {
    char *remote = get_stored_remote();
    char *token = get_token(verbose);
    if (!remote || !token) return;

    printf("[BIT] Restoring '%s' from Bitbucket...\n", filename);

    char cmd[2048];
    // On utilise JQ pour choper le dernier hash de commit
    snprintf(cmd, sizeof(cmd), 
        "HASH=$(curl -s -H \"Authorization: Bearer %s\" "
        "\"https://api.bitbucket.org/2.0/repositories/%s/commits\" | jq -r '.values[0].hash'); "
        "curl -s -H \"Authorization: Bearer %s\" "
        "\"https://api.bitbucket.org/2.0/repositories/%s/src/$HASH/%s\" -o %s",
        token, remote, token, remote, filename, filename);
    
    if (system(cmd) == 0) {
        printf("✅ Restored %s successfully.\n", filename);
    } else {
        printf("❌ Failed to restore %s.\n", filename);
    }
}
