#!/bin/bash
set -e

# Repository configuration
REPO="itzsohanrc/minigit"
BINARY_NAME="minigit"
ARCHIVE_NAME="minigit-linux-x86_64.tar.gz"

echo "🔍 Checking for the latest release of $BINARY_NAME..."

# Fetch the latest release tag using GitHub's API
LATEST_TAG=$(curl -s "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$LATEST_TAG" ]; then
    echo "❌ Error: Failed to fetch the latest release tag from GitHub."
    exit 1
fi

echo "📦 Found latest release: $LATEST_TAG"
echo "⬇️ Downloading $ARCHIVE_NAME..."

# Download binary archive from release assets
DOWNLOAD_URL="https://github.com/$REPO/releases/download/$LATEST_TAG/$ARCHIVE_NAME"
curl -sLO "$DOWNLOAD_URL"

echo "📂 Extracting archive..."
tar -xzf "$ARCHIVE_NAME"

echo "🚀 Installing $BINARY_NAME to /usr/local/bin..."

# Check write permissions for /usr/local/bin
if [ -w "/usr/local/bin" ]; then
    mv "$BINARY_NAME" /usr/local/bin/
else
    echo "🔑 Sudo permission required to copy binary into /usr/local/bin:"
    sudo mv "$BINARY_NAME" /usr/local/bin/
fi

# Make binary executable
sudo chmod +x "/usr/local/bin/$BINARY_NAME"

# Clean up downloaded archive
rm -f "$ARCHIVE_NAME"

echo ""
echo "✨ Success! $BINARY_NAME ($LATEST_TAG) has been installed."
echo "Try running: minigit --help"