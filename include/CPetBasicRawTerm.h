#ifndef CPetBasicRawTerm_H
#define CPetBasicRawTerm_H

#include <CPetBasicTerm.h>

class CPetBasicRawTerm : public CPetBasicTerm {
 public:
  enum State {
    NONE,
    LOOP,
    READ_STRING,
    READ_CHAR,
  };

 public:
  CPetBasicRawTerm(CPetBasic *basic);

  virtual ~CPetBasicRawTerm();

  void init() override;

  //---

  bool isTty() const override { return true; }
  bool isRaw() const override { return raw_; }

  //---

  void moveTo(uint r, uint c) override;

  //---

  void loop() override;

  std::string readString(const std::string &prompt) const override;
  char readChar() const override;

  //---

  void clear() override;

  void enter() override;

  void cursorUp() override;
  void cursorDown(bool force=true) override;
  void cursorLeft() override;
  void cursorRight(bool force=true) override;

  void cursorLeftFull() override;

  //---

  bool drawChar(const uchar &c) override;
  bool drawChar(const CPetDrawChar &drawChar) override;

  void update() override;

  void delay(long t) override;

 protected:
  void setRaw(bool b);

 protected:
  bool raw_ { false };

  struct termios *ios_ { nullptr };

  int screenRows_ { 60 };
  int screenCols_ { 100 };

  State state_ { State::NONE };

  uint redrawCount_ { 0 };
  long delay_       { 0 };
};

#endif
