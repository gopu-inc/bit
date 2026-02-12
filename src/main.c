#include "bit.h"

// Fonction de suggestion simple pour les fautes de frappe
void suggest_command(const char *input) {
    if (strncmp(input, "mer", 3) == 0 || strncmp(input, "meg", 3) == 0) 
        printf("💡 Did you mean: 'merge-create' or 'merge-now'?\n");
    else if (strncmp(input, "sw", 2) == 0)
        printf("💡 Did you mean: 'switch'?\n");
    else if (strncmp(input, "sen", 3) == 0)
        printf("💡 Did you mean: 'sends'?\n");
}

void print_usage() {
    printf("\033[1;34mBit - The Bitbucket CLI Tool\033[0m\n");
    printf("Usage: bit <command> [<args>] [--verbose]\n\n");
    
    printf("\033[1mSetup & Auth:\033[0m\n");
    printf("  login            Connect to Bitbucket\n");
    printf("  whoami           Show current user\n");
    printf("  copy <url>       Clone a repository\n\n");

    printf("\033[1mWorkflow:\033[0m\n");
    printf("  add <path>       Stage files (use '.' for all)\n");
    printf("  commit -m <msg>  Create a local commit\n");
    printf("  sends            Push changes to Bitbucket\n");
    printf("  pull             Download updates\n\n");

    printf("\033[1mBranches & Tags:\033[0m\n");
    printf("  create-branch <n> Create a new branch\n");
    printf("  switch <name>     Change current branch\n");
    printf("  tag <name>        Create a version tag\n\n");

    printf("\033[1mMerging (PR):\033[0m\n");
    printf("  merge-create <s> <d> <t>  Open a Pull Request\n");
    printf("  merge-now <id>            Merge a Pull Request\n\n");

    printf("\033[1mUtilities:\033[0m\n");
    printf("  mall             Show commit history\n");
    printf("  ci               Trigger CI/CD Pipeline\n");
    printf("  restore <file>   Recover a deleted file\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) { 
        print_usage(); 
        return 0; 
    }

    int verbose = 0;
    // Détection globale du mode verbose
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0) verbose = 1;
    }

    const char *cmd = argv[1];

    // --- LOGIQUE DE ROUTAGE ---
    if (strcmp(cmd, "login") == 0) {
        bit_login();
    } 
    else if (strcmp(cmd, "copy") == 0) {
        if (argc < 3) printf("Usage: bit copy <url>\n");
        else bit_copy(argv[2], verbose);
    }
    else if (strcmp(cmd, "whoami") == 0) {
        bit_whoami(verbose);
    }
    else if (strcmp(cmd, "add") == 0) {
        bit_add(argc < 3 ? "." : argv[2]);
    }
    else if (strcmp(cmd, "commit") == 0) {
        if (argc < 3) printf("Usage: bit commit -m \"message\"\n");
        else bit_commit(argv[argc-1]);
    }
    else if (strcmp(cmd, "sends") == 0) {
        bit_sends(verbose);
    }
    else if (strcmp(cmd, "pull") == 0) {
        bit_pull(verbose);
    }
    else if (strcmp(cmd, "switch") == 0) {
        if (argc < 3) printf("Usage: bit switch <branch>\n");
        else bit_switch(argv[2]);
    }
    else if (strcmp(cmd, "create-branch") == 0) {
        if (argc < 3) printf("Usage: bit create-branch <name>\n");
        else bit_branch_create(argv[2], verbose);
    }
    else if (strcmp(cmd, "merge-create") == 0) {
        if (argc < 5) printf("Usage: bit merge-create <source> <dest> <title>\n");
        else bit_pr_create(argv[2], argv[3], argv[4], verbose);
    }
    else if (strcmp(cmd, "merge-now") == 0) {
        if (argc < 3) printf("Usage: bit merge-now <pr_id>\n");
        else bit_pr_merge(atoi(argv[2]), verbose);
    }
    else if (strcmp(cmd, "tag") == 0) {
        if (argc < 3) printf("Usage: bit tag <v1.0.0>\n");
        else bit_tag(argv[2], verbose);
    }
    else if (strcmp(cmd, "mall") == 0) {
        bit_mall(verbose);
    }
    else if (strcmp(cmd, "ci") == 0) {
        bit_ci(verbose);
    }
    else if (strcmp(cmd, "restore") == 0) {
        if (argc < 3) printf("Usage: bit restore <filename>\n");
        else bit_restore(argv[2], verbose);
    }
    else if (strcmp(cmd, "sha") == 0) {
        if (argc < 3) printf("Usage: bit sha <file>\n");
        else bit_sha256(argv[2]);
    }
    else {
        fprintf(stderr, "\033[1;31mUnknown command: %s\033[0m\n", cmd);
        suggest_command(cmd);
        return 1;
    }

    return 0;
}
