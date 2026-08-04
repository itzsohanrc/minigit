#ifndef MINIGIT_REPOSITORY_HPP
#define MINIGIT_REPOSITORY_HPP

#include <filesystem>
#include <optional>
#include <string>

using namespace std;

namespace minigit {

namespace fs = filesystem;

class Repository {
private:
    fs::path worktree_;
    fs::path gitdir_;

public:
    Repository(fs::path worktree, bool force = false);

    static Repository create(const fs::path& path);
    static optional<Repository> find(const fs::path& path = fs::current_path(), bool required = true);

    fs::path repo_path(const fs::path& subpath) const;
    fs::path repo_file(const fs::path& subpath, bool mkdir = false) const;
    fs::path repo_dir(const fs::path& subpath, bool mkdir = false) const;

    const fs::path& worktree() const { return worktree_; }
    const fs::path& gitdir() const { return gitdir_; }
};

} // namespace minigit

#endif