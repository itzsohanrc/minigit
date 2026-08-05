#!/bin/bash
set -e

REPO="itzsohanrc/minigit"
BINARY_NAME="minigit"
ARCHIVE_NAME="minigit-linux-x86_64.tar.gz"

echo "🔍 Checking for the latest release of $BINARY_NAME..."

# 1. Try fetching tag from latest Release
LATEST_TAG=$(curl -sH "User-Agent: Mozilla/5.0" "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')

# 2. Fallback: If no Release object exists, fetch top Git Tag directly
if [ -z "$LATEST_TAG" ]; then
    LATEST_TAG=$(curl -sH "User-Agent: Mozilla/5.0" "https://api.github.com/repos/$REPO/tags" | grep '"name":' | head -n 1 | sed -E 's/.*"([^"]+)".*/\1/')
fi

# 3. Verify tag was found
if [ -z "$LATEST_TAG" ] || [ "$LATEST_TAG" = "null" ]; then
    echo "❌ Error: Could not detect any releases or tags in $REPO."
    echo "Please ensure at least one release or tag exists at: https://github.com/$REPO/releases"
    exit 1
fi

echo "📦 Found version: $LATEST_TAG"
echo "⬇️ Downloading $ARCHIVE_NAME..."

DOWNLOAD_URL="https://github.com/$REPO/releases/download/$LATEST_TAG/$ARCHIVE_NAME"

# Check if download asset exists
if ! curl --output /dev/null --silent --head --fail "$DOWNLOAD_URL"; then
    echo "❌ Error: Binary asset not found at $DOWNLOAD_URL"
    echo "Check if the GitHub Action finished uploading $ARCHIVE_NAME to release $LATEST_TAG."
    exit 1
fi

curl -sLO "$DOWNLOAD_URL"

echo "📂 Extracting archive..."
tar -xzf "$ARCHIVE_NAME"

echo "🚀 Installing $BINARY_NAME to /usr/local/bin..."

if [ -w "/usr/local/bin" ]; then
    mv "$BINARY_NAME" /usr/local/bin/
else
    echo "🔑 Sudo permission required to copy binary into /usr/local/bin:"
    sudo mv "$BINARY_NAME" /usr/local/bin/
fi

sudo chmod +x "/usr/local/bin/$BINARY_NAME"
rm -f "$ARCHIVE_NAME"

echo ""
echo "✨ Success! $BINARY_NAME ($LATEST_TAG) has been installed."
echo "Try running: minigit --help"