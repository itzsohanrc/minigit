# MiniGit

A lightweight Git-like version control system built with C++ and CMake. MiniGit implements core version control concepts such as repository initialization, object storage, staging, commits, and commit history.

---

## ⚡ Quick Installation (Recommended)

Skip building from source! Install the pre-compiled binary directly using the one-line installer:

```bash
curl -fsSL https://raw.githubusercontent.com/itzsohanrc/minigit/main/install.sh | bash
```

After installation, verify it:

```bash
minigit --help
```

---

## 📋 Prerequisites (For Building from Source)

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev zlib1g-dev
```

### macOS (Homebrew)

```bash
brew install cmake openssl zlib
```

---

## 🔨 Building the Project

### Clone the Repository

```bash
git clone https://github.com/itzsohanrc/minigit.git
cd minigit
```

### Configure with CMake

```bash
cmake -B build
```

### Compile the Project

```bash
cmake --build build
```

The compiled executable will be available at:

```text
./build/minigit
```

---

## 🚀 Quick Start & Usage

If installed using the installer:

```bash
minigit init
```

If built from source:

```bash
./build/minigit init
```

### Example Workflow

```bash
# Initialize a repository
minigit init

# Create a file
echo "Hello, MiniGit!" > file.txt

# Compute object hash (optional)
minigit hash-object file.txt

# Stage file
minigit add file.txt

# Commit changes
minigit commit -m "Initial commit"

# View history
minigit log
```

For source builds, replace `minigit` with `./build/minigit`.

---

## 📂 Project Structure

```text
minigit/
├── CMakeLists.txt          # CMake configuration
├── .gitignore              # Ignored build & generated files
├── install.sh              # One-line installer script
├── include/
│   └── minigit/
│       ├── commands.hpp
│       ├── index.hpp
│       ├── object.hpp
│       ├── repository.hpp
│       └── utils.hpp
└── src/
    ├── commands.cpp
    ├── index.cpp
    ├── main.cpp
    ├── object.cpp
    ├── repository.cpp
    └── utils.cpp
```

---

## 🪪 License

This project is open-source and available under the **MIT License**.