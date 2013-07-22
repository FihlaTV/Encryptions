#include "SEED.h"

uint32_t SEED::G(uint32_t X){
    X %= mod32;
    uint8_t X3 = (X >> 24) & 255;
    uint8_t X2 = (X >> 16) & 255;
    uint8_t X1 = (X >> 8) & 255;
    uint8_t X0 = X & 255;
    uint8_t m0 = 0xFC, m1 = 0xF3, m2 = 0xCF, m3 = 0x3F;
    uint32_t Z0 = (SEED_S0[X0] & m0) ^ (SEED_S1[X1] & m1) ^ (SEED_S0[X2] & m2) ^ (SEED_S1[X3] & m3);
    uint32_t Z1 = (SEED_S0[X0] & m1) ^ (SEED_S1[X1] & m2) ^ (SEED_S0[X2] & m3) ^ (SEED_S1[X3] & m0);
    uint32_t Z2 = (SEED_S0[X0] & m2) ^ (SEED_S1[X1] & m3) ^ (SEED_S0[X2] & m0) ^ (SEED_S1[X3] & m1);
    uint32_t Z3 = (SEED_S0[X0] & m3) ^ (SEED_S1[X1] & m0) ^ (SEED_S0[X2] & m1) ^ (SEED_S1[X3] & m2);
    return (Z3 << 24) + (Z2 << 16) + (Z1 << 8) + Z0;
}

uint64_t SEED::F(uint64_t & right, std::pair <uint32_t, uint32_t> & K){
    uint64_t R0 = right >> 32;
    uint64_t R1 = right & mod32;
    uint64_t r = G(G(G((R0 ^ K.first) ^ (R1 ^ K.second)) + (R0 ^ K.first)) + G((R0 ^ K.first) ^ (R1 ^ K.second))) & mod32;
    return ((r + (G(G((R0 ^ K.first) ^ (R1 ^ K.second)) + (R0 ^ K.first)))) << 32) + r;
}

std::string SEED::run(std::string & DATA){
    if (!keyset)
        error(1);
    uint64_t L = toint(DATA.substr(0, 8), 256), R = toint(DATA.substr(8, 8), 256);
    for(uint8_t i = 0; i < 16; i++){
        uint64_t temp = L;
        L = R;
        R = temp ^ F(R, k[i]);
    }
    return unhexlify(makehex(R, 16) + makehex(L, 16));
}

SEED::SEED(){
    keyset = false;
}

SEED::SEED(std::string KEY){
    keyset = false;
    setkey(KEY);
}

void SEED::setkey(std::string KEY){
    if (keyset){
        error(2);
    }
    KEY = hexlify(KEY.substr(0, std::min((int) KEY.size(), 16)));
    integer key(KEY, 16);
    uint64_t K0 = (uint32_t) (key >> 96);
    uint64_t K1 = (uint32_t) (key >> 64);
    uint64_t K2 = (uint32_t) (key >> 32);
    uint64_t K3 = (uint32_t) key;
    uint64_t T;
    for(uint8_t i = 1; i < 17; i++){
        k[i - 1].first = G((K0 + K2 - SEED_KC[i - 1]));
        k[i - 1].second = G((K1 - K3 + SEED_KC[i - 1]));
        if (i % 2){
            T = ROR(((K0 << 32) + K1) & mod64, 8, 64);
            K0 = T >> 32; K1 = T & mod32;
        }
        else{
            T = ROL(((K2 << 32) + K3) & mod64, 8, 64);
            K2 = T >> 32; K3 = T & mod32;
        }
    }
    keyset = true;
}

std::string SEED::encrypt(std::string DATA){
    return run(DATA);
}

std::string SEED::decrypt(std::string DATA){
    std::reverse(k, k + 16);
    std::string out = run(DATA);
    std::reverse(k, k + 16);
    return out;
}

unsigned int SEED::blocksize(){
    return 128;
}