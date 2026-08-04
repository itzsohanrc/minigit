#include "minigit/repository.hpp"
#include <fstream>
#include <stdexcept>

using namespace std;

namespace minigit {

Repository::Repository(fs::path worktree, bool force)
    : worktree_(std::move(worktree)), gitdir_(worktree_ / ".minigit") {
    if (!force && !fs::is_directory(gitdir_)) {
        throw runtime_error("Not a MiniGit repository: " + worktree_.string());
    }
}

fs::path Repository::repo_path(const fs::path& subpath) const {
    return gitdir_ / subpath;
}

fs::path Repository::repo_dir(const fs::path& subpath, bool mkdir) const {
    fs::path p = repo_path(subpath);
    if (fs::exists(p)) {
        if (fs::is_directory(p)) return p;
        throw runtime_error("Not a directory: " + p.string());
    }
    if (mkdir) {
        fs::create_directories(p);
        return p;
    }
    return "";
}

fs::path Repository::repo_file(const fs::path& subpath, bool mkdir) const {
    if (!subpath.has_parent_path() || repo_dir(subpath.parent_path(), mkdir) != "") {
        return repo_path(subpath);
    }
    return "";
}

Repository Repository::create(const fs::path& path) {
    Repository repo(path, true);

    if (fs::exists(repo.worktree_)) {
        if (!fs::is_directory(repo.worktree_)) {
            throw runtime_error(path.string() + " is not a directory!");
        }
    } else {
        fs::create_directories(repo.worktree_);
    }

    repo.repo_dir("objects", true);
    repo.repo_dir("refs/tags", true);
    repo.repo_dir("refs/heads", true);

    ofstream head_file(repo.repo_file("HEAD"));
    head_file << "ref: refs/heads/main\n";

    return repo;
}

optional<Repository> Repository::find(const fs::path& path, bool required) {
    fs::path canonical_path = fs::canonical(path);

    if (fs::is_directory(canonical_path / ".minigit")) {
        return Repository(canonical_path);
    }

    fs::path parent = canonical_path.parent_path();
    if (parent == canonical_path) {
        if (required) throw runtime_error("No MiniGit repository found.");
        return nullopt;
    }

    return find(parent, required);
}

} // namespace minigit