# MiniGit

A lightweight Git-like version control system built with C++ and CMake. MiniGit implements core version control concepts such as repository initialization, object storage, staging, commits, and commit history.

---

## 📋 Prerequisites

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

### 1. Initialize a New Repository

```bash
./build/minigit init
```

### 2. Create a Sample File

```bash
echo "Hello, mini Git!" > file.txt
```

### 3. Compute the Object Hash (Optional)

```bash
./build/minigit hash-object file.txt
```

### 4. Stage the File

```bash
./build/minigit add file.txt
```

### 5. Commit the Changes

```bash
./build/minigit commit -m "Initial commit"
```

### 6. View Commit History

```bash
./build/minigit log
```

---

## 📂 Project Structure

```text
minigit/
├── CMakeLists.txt          # CMake configuration
├── .gitignore              # Ignored build & internal files
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