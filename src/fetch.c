// fetch.c
#include "bit.h"

int bit_copy(const char *url, int verbose) {
    char user[128] = {0}, repo[128] = {0};

    if (sscanf(url, "https://bitbucket.org/%[^/]/%[^.].bit", user, repo) != 2) {
        fprintf(stderr, "Error: Invalid URL format. Use https://bitbucket.org/user/repo.bit\n");
        return 1;
    }

    if (verbose) printf("[DEBUG] Target: User=%s, Repo=%s\n", user, repo);

    char *token = get_token(verbose);
    if (!token) {
        fprintf(stderr, "Error: Authentication failed. Please run 'bit login'.\n");
        return 1;
    }

    printf("Cloning into '%s'...\n", repo);

    // Try main then master branches
    const char *branches[] = {"main", "master"};
    char archive[256] = ".bit_download.tar.gz";
    int success = 0;

    for (int i = 0; i < 2; i++) {
        char fetch_url[512];
        snprintf(fetch_url, 512, "https://bitbucket.org/%s/%s/get/%s.tar.gz", user, repo, branches[i]);
        
        if (verbose) printf("[DEBUG] Fetching from: %s\n", fetch_url);

        if (download_repository(fetch_url, token, archive, verbose) == 0) {
            char cmd[512];
            mkdir(repo, 0755);
            snprintf(cmd, 512, "tar -xzf %s -C %s --strip-components=1", archive, repo);
            system(cmd);
            unlink(archive);
            success = 1;
            break;
        }
    }

    if (success) {
        // Setup internal .bit metadata for later 'sends'
        char bit_path[512];
        snprintf(bit_path, 512, "mkdir -p %s/.bit/fetch", repo);
        system(bit_path);

        snprintf(bit_path, 512, "%s/%s", repo, BIT_REMOTE);
        FILE *f = fopen(bit_path, "w");
        if (f) {
            fprintf(f, "%s/%s", user, repo);
            fclose(f);
        }
        printf("Success: Repository cloned to %s/\n", repo);
        return 0;
    }

    fprintf(stderr, "Error: Could not find a valid branch (main/master).\n");
    return 1;
}

