#include "hash_utils.h"
#include <sodium.h>
#include <stdexcept>



std::string hashed(const std::string &pass) {
        char hashed_pass[crypto_pwhash_STRBYTES];
    
        if (crypto_pwhash_str(hashed_pass,pass.c_str(), pass.size(),crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            throw std::runtime_error("Password hashing failed");
        }
        
        return std::string(hashed_pass);
}

