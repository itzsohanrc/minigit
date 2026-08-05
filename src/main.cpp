#include "minigit/commands.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2 || argv[1] == string("--help") || argv[1] == string("-h")) {
        cout << "##########################################################################################\n";
        cout << "Welcome to minigit\nDEVELOPED BY: Sohan Roy Chowdhury\nFollowing commands are available:\n";
        cout << "  init : to initialize a new MiniGit repository\n";
        cout << "  hash-object : to hash an object\n";
        cout << "  cat-file : to display the content of an object\n";
        cout << "  ls-files : to list all files in the index\n";
        cout << "  add : to add files to the index\n";
        cout << "  commit : to create a new commit\n";
        cout << "  log : to display commit history\n";
        cout << "Feel free to contact me at roychowdhurysohan2006@gmail.com\n";
        cout<< "Happy coding and enjoy using minigit!\n";
        cout << "##########################################################################################\n";
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