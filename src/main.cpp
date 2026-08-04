#include "minigit/commands.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: minigit <command> [<args>]\n";
        return 1;
    }

    string command = argv[1];
    vector<string> args(argv + 2, argv + argc);

    if (command == "init") return minigit::commands::cmd_init(args);
    if (command == "hash-object") return minigit::commands::cmd_hash_object(args);
    if (command == "cat-file") return minigit::commands::cmd_cat_file(args);
    if (command == "ls-files") return minigit::commands::cmd_ls_files(args);
    if (command == "add") return minigit::commands::cmd_add(args);
    if (command == "commit") return minigit::commands::cmd_commit(args);
    if (command == "log") return minigit::commands::cmd_log(args);

    cerr << "Unknown command: " << command << "\n";
    return 1;
}