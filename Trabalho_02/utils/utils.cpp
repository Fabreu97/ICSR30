#include "utils.h"
#include <openssl/evp.h>
// RTT: Round Trip Time = mede o tempo total entre o envio de um pacote até o recebimento da resposta.
void update_timeout(struct timeval* timeout, struct timeval start, struct timeval end) {
    if (end.tv_usec < start.tv_usec) {
        // "empresta" 1 segundo (1.000.000 microssegundos)
        end.tv_sec -= 1;
        end.tv_usec += 1000000;
    }
    timeout->tv_sec = ((end.tv_sec - start.tv_sec) * ALPHA + timeout->tv_sec * (1 - ALPHA));
    timeout->tv_usec = ((end.tv_usec - start.tv_usec) * ALPHA) + (timeout->tv_usec * (float)(1 - ALPHA));
}

// hash_len é necessário para saber quantos bytes converter
std::string sha256_to_hex(const unsigned char* hash, unsigned int hash_len) {
    std::stringstream ss;
    for (unsigned int i = 0; i < hash_len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string get_file_sha256(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo: " << filePath << std::endl;
        return "";
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        std::cerr << "Erro: Falha ao criar contexto EVP_MD_CTX." << std::endl;
        return "";
    }

    if (!EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr)) {
        std::cerr << "Erro: Falha no EVP_DigestInit_ex." << std::endl;
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    const int bufferSize = 4096;
    std::vector<char> buffer(bufferSize);

    while (file.good()) {
        file.read(buffer.data(), bufferSize);
        std::streamsize bytesRead = file.gcount();
        if (bytesRead > 0) {
            if (!EVP_DigestUpdate(mdctx, buffer.data(), bytesRead)) {
                std::cerr << "Erro: Falha no EVP_DigestUpdate." << std::endl;
                EVP_MD_CTX_free(mdctx);
                return "";
            }
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;

    if (!EVP_DigestFinal_ex(mdctx, hash, &hash_len)) {
        std::cerr << "Erro: Falha no EVP_DigestFinal_ex." << std::endl;
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);
    return sha256_to_hex(hash, hash_len);  // ✅ chamada corrigida
}