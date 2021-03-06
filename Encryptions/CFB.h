// Cipher Feedback
#ifndef __CFB__
#define __CFB__

#include "../common/includes.h"
#include "../common/integer.h"
#include "SymAlg.h"

class CFB{
  private:
    SymAlg * algo;
    std::string const_IV;
    uint8_t blocksize;

  public:
    CFB(SymAlg * instance, const std::string & iv = "");
    std::string encrypt(const std::string & data);
    std::string decrypt(const std::string & data);
};
#endif
