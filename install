#!/bin/sh

# Couleurs pour la sortie
GREEN='\033[0;32m'
CYAN='\033[0;36m'
RED='\033[0;31m'
RESET='\033[0m'

echo "${CYAN}🦁 Installing BIT - The Bitbucket CLI Streamer...${RESET}"

# 1. Vérification des dépendances
echo "🔍 Checking dependencies..."
for cmd in gcc make curl jq; do
    if ! command -v $cmd >/dev/null 2>&1; then
        echo "${RED}❌ Error: $cmd is not installed.${RESET}"
        exit 1
    fi
done

# 2. Création d'un dossier temporaire
TMP_DIR=$(mktemp -d)
cd $TMP_DIR

# 3. Clonage léger (shallow clone)
echo "📥 Downloading source from GitHub..."
git clone --depth 1 https://github.com/gopu-inc/bit.git . > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "${RED}❌ Failed to download source.${RESET}"
    exit 1
fi

# 4. Compilation
echo "⚙️  Compiling..."
# On supprime le copy.c problématique avant de compiler au cas où il est encore là
rm -f src/copy.c 
make > /dev/null

if [ $? -ne 0 ]; then
    echo "${RED}❌ Compilation failed.${RESET}"
    exit 1
fi

# 5. Installation
echo "🚀 Installing to /usr/local/bin..."
if [ -w /usr/local/bin ]; then
    mv bin/bit /usr/local/bin/bit
else
    echo "🔑 Requesting sudo for installation..."
    sudo mv bin/bit /usr/local/bin/bit
fi

# 6. Nettoyage
cd ~
rm -rf $TMP_DIR

echo "${GREEN}✅ BIT has been installed successfully!${RESET}"
echo "Type ${CYAN}'bit'${RESET} to start."

