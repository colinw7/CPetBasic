#ifndef CQPetBasic_H
#define CQPetBasic_H

#include <CPetBasic.h>

class CQPetBasicTerm;

//---

class CQPetBasic : public CPetBasic {
 public:
  CQPetBasic(CQPetBasicTerm *term);

  void printString(const std::string &s) const override;

  char getChar() const override;

  std::string getString(const std::string &prompt) const override;

  void resize(uint nr, uint nc) override;

  bool getScreenMemory(uint r, uint c, uchar &value) const override;
  void setScreenMemory(uint r, uint c, uchar value) override;

  void delay() override;

  void notifyRunLine(uint n) const override;

  void notifyLinesChanged() override;
  void notifyLineNumChanged() override;

 private:
  CQPetBasicTerm *term_ { nullptr };
};

#endif
