#include "minigit/utils.hpp"
#include <openssl/sha.h>
#include <zlib.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace minigit::utils {

string compute_sha1(const string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.data()), input.size(), hash);

    ostringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

string compress_data(const string& data) {
    uLongf compressed_size = compressBound(data.size());
    string compressed(compressed_size, '\0');

    if (compress(reinterpret_cast<Bytef*>(&compressed[0]), &compressed_size,
                 reinterpret_cast<const Bytef*>(data.data()), data.size()) != Z_OK) {
        throw runtime_error("Failed to compress object data using Zlib");
    }

    compressed.resize(compressed_size);
    return compressed;
}

string decompress_data(const string& compressed_data) {
    uLongf dest_size = compressed_data.size() * 4 + 1024;
    string decompressed(dest_size, '\0');

    while (true) {
        uLongf current_size = dest_size;
        int result = uncompress(reinterpret_cast<Bytef*>(&decompressed[0]), &current_size,
                                reinterpret_cast<const Bytef*>(compressed_data.data()), compressed_data.size());

        if (result == Z_OK) {
            decompressed.resize(current_size);
            return decompressed;
        } else if (result == Z_BUF_ERROR) {
            dest_size *= 2;
            decompressed.resize(dest_size);
        } else {
            throw runtime_error("Failed to decompress object data using Zlib");
        }
    }
}

KVLM parse_kvlm(const string& raw, size_t start) {
    KVLM dct;
    size_t pos = start;

    while (pos < raw.size()) {
        size_t spc = raw.find(' ', pos);
        size_t nl = raw.find('\n', pos);

        if (spc == string::npos || nl < spc) {
            if (nl == pos) {
                dct["msg"] = { raw.substr(pos + 1) };
            } else {
                dct["msg"] = { raw.substr(pos) };
            }
            break;
        }

        string key = raw.substr(pos, spc - pos);
        size_t end = pos;

        while (true) {
            end = raw.find('\n', end + 1);
            if (end == string::npos || end + 1 >= raw.size() || raw[end + 1] != ' ') {
                break;
            }
        }

        if (end == string::npos) end = raw.size();

        string value = raw.substr(spc + 1, end - (spc + 1));
        size_t space_pos = 0;
        while ((space_pos = value.find("\n ", space_pos)) != string::npos) {
            value.replace(space_pos, 2, "\n");
        }

        dct[key].push_back(value);
        pos = end + 1;
    }

    return dct;
}

string serialize_kvlm(const KVLM& kvlm, const string& message) {
    string result;
    for (const auto& [key, values] : kvlm) {
        if (key == "msg") continue;
        for (const auto& val : values) {
            string formatted_val = val;
            size_t pos = 0;
            while ((pos = formatted_val.find('\n', pos)) != string::npos) {
                formatted_val.replace(pos, 1, "\n ");
                pos += 2;
            }
            result += key + " " + formatted_val + "\n";
        }
    }
    result += "\n" + message;
    return result;
}

} // namespace minigit::utils