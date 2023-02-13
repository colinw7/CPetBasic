#ifndef CPetBasicTerm_H
#define CPetBasicTerm_H

#include <CPetBasic.h>

using uint  = unsigned int;
using uchar = unsigned char;

class CPetBasic;

class CPetBasicTerm {
 public:
  CPetBasicTerm(CPetBasic *basic);

  virtual ~CPetBasicTerm();

  virtual void init();

  //---

  CPetBasic *basic() const { return basic_; }

  //---

  virtual bool isTty() const { return true; }
  virtual bool isRaw() const { return false; }

  //---

  virtual uint row() const { return r_; }
  virtual uint col() const { return c_; }

  virtual uint numRows() const { return nr_; }
  virtual uint numCols() const { return nc_; }

  //---

  virtual void resize(uint nr, uint nc);

  virtual void moveTo(uint r, uint c);

  virtual CPetDrawChar getChar(uint r, uint c) const;
  virtual void setChar(uint r, uint c, const CPetDrawChar &drawChar);

  uint encodeCharPos(uint r, uint c) const { return r*nc_ + c; }
  void decodeCharPos(uint pos, uint &r, uint &c) const { r = pos/nc_; c = pos - r*nc_; }

  //---

  virtual void loop();

  virtual std::string readString(const std::string &prompt) const;
  virtual char readChar() const;

  //---

  virtual void home();
  virtual void clear();

  virtual void enter();
  virtual void tab();

  virtual void cursorUp();
  virtual void cursorDown(bool force=false);
  virtual void cursorLeft();
  virtual void cursorRight(bool force=false);

  virtual void cursorLeftFull();

  virtual void scrollUp();

  //---

  virtual void del();

  //---

  virtual bool inQuotes() const;

  //---

  virtual bool drawChar(const uchar &c);
  virtual bool drawChar(const CPetDrawChar &drawChar);

  virtual void update();

  //---

  virtual void delay(long d);

  virtual void enterLine();

 protected:
  using Chars = std::vector<CPetDrawChar>;

  CPetBasic *basic_ { nullptr };
  uint       nr_    { 25 };
  uint       nc_    { 40 };
  uint       r_     { 0 };
  uint       c_     { 0 };
  Chars      chars_;

  std::string inputBuffer_;
};

#endif
