#include "encrypt.hpp"

int Encrypt::initAes(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx)
{
  int i, nrounds = 5;
  unsigned char key[32], iv[32];
  i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
  if (i != 32) {
    printf("Key size is %d bits - should be 256 bits\n", i);
    return -1;
  }
  EVP_CIPHER_CTX_init(e_ctx);
  EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
  EVP_CIPHER_CTX_init(d_ctx);
  EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
  return 0;
}

unsigned char* Encrypt::encryptAes(unsigned char *plaintext, int *len)
{
  int c_len = *len + AES_BLOCK_SIZE, f_len = 0;
  unsigned char *ciphertext = new unsigned char[c_len];
  EVP_EncryptInit_ex(encryptEvp, NULL, NULL, NULL, NULL);
  EVP_EncryptUpdate(encryptEvp, ciphertext, &c_len, plaintext, *len);
  EVP_EncryptFinal_ex(encryptEvp, ciphertext+c_len, &f_len);
  *len = c_len + f_len;
  return ciphertext;
}

unsigned char* Encrypt::decryptAes(unsigned char *ciphertext, int *len)
{
  int p_len = *len, f_len = 0;
  unsigned char *plaintext = new unsigned char[p_len];
  EVP_DecryptInit_ex(decryptEvp, NULL, NULL, NULL, NULL);
  EVP_DecryptUpdate(decryptEvp, plaintext, &p_len, ciphertext, *len);
  EVP_DecryptFinal_ex(decryptEvp, plaintext+p_len, &f_len);
  *len = p_len + f_len;
  return plaintext;
}

std::string Encrypt::decode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
        return c == '\0';
    });
}

std::string Encrypt::encode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}

Encrypt::Encrypt(Config config) : nipoLog(config)
{
  nipoConfig = config;
  salt[0] = nipoConfig.config.users[0].salt;
  salt[1] = nipoConfig.config.users[0].salt;
  token = (unsigned char *)nipoConfig.config.users[0].token.c_str();
  tokenLen = strlen(nipoConfig.config.users[0].token.c_str());
  initAes(token, tokenLen, (unsigned char *)&salt, encryptEvp, decryptEvp);
}

Encrypt::~Encrypt(){}