#ifndef CQPetBasic_H
#define CQPetBasic_H

#include <CPetBasic.h>

class CQPetBasicApp;

//---

class CQPetBasic : public CPetBasic {
 public:
  CQPetBasic(CQPetBasicApp *term);

  void resize(uint nr, uint nc) override;

  void setReverse(bool b) override;

  void setShift(bool b) override;

  void notifyRunLine(uint n) const override;

  void notifyLinesChanged() override;
  void notifyLineNumChanged() override;

  void notifyVariablesChanged() override;

 private:
  CQPetBasicApp *app_ { nullptr };
};

#endif
