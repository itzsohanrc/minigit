#!/bin/bash
set -e

REPO="itzsohanrc/minigit"
BINARY_NAME="minigit"
ARCHIVE_NAME="minigit-linux-x86_64.tar.gz"

echo "🚀 Installing $BINARY_NAME..."
echo "⬇️ Downloading latest release asset from GitHub..."

# GitHub's native redirect URL for the latest release asset (No API needed!)
DOWNLOAD_URL="https://github.com/$REPO/releases/latest/download/$ARCHIVE_NAME"

# Download with -L (follow redirects) and -f (fail silently on HTTP errors)
if ! curl -sL -f -o "$ARCHIVE_NAME" "$DOWNLOAD_URL"; then
    echo "❌ Error: Failed to download $ARCHIVE_NAME."
    echo "Please ensure a published release containing $ARCHIVE_NAME exists at:"
    echo "https://github.com/$REPO/releases"
    exit 1
fi

echo "📂 Extracting archive..."
tar -xzf "$ARCHIVE_NAME"

echo "⚙️ Installing $BINARY_NAME to /usr/local/bin..."

if [ -w "/usr/local/bin" ]; then
    mv "$BINARY_NAME" /usr/local/bin/
else
    echo "🔑 Sudo permission required to install to /usr/local/bin:"
    sudo mv "$BINARY_NAME" /usr/local/bin/
fi

sudo chmod +x "/usr/local/bin/$BINARY_NAME"
rm -f "$ARCHIVE_NAME"

echo ""
echo "✨ Success! $BINARY_NAME has been installed successfully."
echo "Try running: minigit --help"