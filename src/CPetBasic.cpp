#include <CPetBasic.h>
#include <CPetBasicTerm.h>
#include <CPetBasicRawTerm.h>
#include <CPetBasicUtil.h>
#include <CFileParse.h>
#include <CStrParse.h>
#include <CExpr.h>
#include <CReadLine.h>
#include <COSRand.h>
#include <COSRead.h>
#include <COSTime.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <termios.h>
#include <unistd.h>

//static int s_num_basic_tokens_created = 0;
//static int s_num_basic_tokens_deleted = 0;

class CPetBasicExpr : public CExpr {
 public:
  using Inds = std::vector<uint>;

 public:
  CPetBasicExpr(CPetBasic *basic) :
   basic_(basic) {
    setIgnoreCase(true);
  }

  CExprValuePtr variableSubscript(const std::string &name,
                                  const CExprValueArray &inds) const override {
    Inds inds1;

    for (const auto &ind : inds) {
      long i1;
      if (! ind->getIntegerValue(i1)) {
        std::cerr << "Invalid subscript\n";
        return CExprValuePtr();
      }

      inds1.push_back(uint(i1));
    }

    //---

    auto val = basic_->getVariableValue(name, inds1);

    if (! val) {
      auto *th = const_cast<CPetBasicExpr *>(this);

      val = th->createIntegerValue(0);
    }

    //std::cerr << "variableSubscript: " << name << " "; basic_->printInds(inds1);
    //std::cerr << " "; val->print(std::cerr); std::cerr << "\n";

    return val;
  }

  void errorMsg(const std::string &msg) const override {
    basic_->errorMsg(msg);
  }

 private:
  CPetBasic* basic_ { nullptr };
};

class CPetBasicFunction : public CExprFunctionObj {
 public:
  CPetBasicFunction(CPetBasic *basic) : basic_(basic) { }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) override {
    expr_ = expr;
    return exec(values);
  }

  virtual CExprValuePtr exec(const CExprValueArray &values) = 0;

 protected:
  CExprValuePtr errorMsg(const std::string &msg) {
    expr_->errorMsg(msg);
    return CExprValuePtr();
  }

 protected:
  CPetBasic *basic_ { nullptr };
  CExpr     *expr_  { nullptr };
};

class CPetBasicAbsFunction : public CPetBasicFunction {
 public:
  CPetBasicAbsFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto r1 = std::abs(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicAscFunction : public CPetBasicFunction {
 public:
  CPetBasicAscFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    std::string s;
    if (! values[0]->getStringValue(s))
      return errorMsg("Wrong argument type");

    uchar c(s[0]);

    auto l = long(c);

    return expr_->createIntegerValue(l);
  }
};

class CPetBasicAtnFunction : public CPetBasicFunction {
 public:
  CPetBasicAtnFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto r1 = std::atan(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicCosFunction : public CPetBasicFunction {
 public:
  CPetBasicCosFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto r1 = std::cos(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicChrFunction : public CPetBasicFunction {
 public:
  CPetBasicChrFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    long l;
    if (! values[0]->getIntegerValue(l))
      return errorMsg("Wrong argument type");

    auto c = uchar(l & 0xff);

    std::string s = " "; s[0] = c;

    return expr_->createStringValue(s);
  }
};

class CPetBasicExpFunction : public CPetBasicFunction {
 public:
  CPetBasicExpFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto r1 = std::exp(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicFreFunction : public CPetBasicFunction {
 public:
  CPetBasicFreFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &) override {
    return errorMsg("Unsupported command");
  }
};

class CPetBasicIntFunction : public CPetBasicFunction {
 public:
  CPetBasicIntFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto i = long(r);

    return expr_->createIntegerValue(i);
  }
};

class CPetBasicLeftFunction : public CPetBasicFunction {
 public:
  CPetBasicLeftFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 2) return errorMsg("Wrong number of arguments");

    std::string s; long n;
    if (! values[0]->getStringValue(s) || ! values[1]->getIntegerValue(n))
      return errorMsg("Wrong argument type");

    auto s1 = s.substr(0, n);

    return expr_->createStringValue(s1);
  }
};

class CPetBasicLenFunction : public CPetBasicFunction {
 public:
  CPetBasicLenFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    std::string s;
    if (! values[0]->getStringValue(s))
      return errorMsg("Wrong argument type");

    auto len = s.size();

    return expr_->createIntegerValue(len);
  }
};

class CPetBasicLogFunction : public CPetBasicFunction {
 public:
  CPetBasicLogFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    if (r <= 0)
      return errorMsg("Domain error");

    auto r1 = std::log(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicMidFunction : public CPetBasicFunction {
 public:
  CPetBasicMidFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 3) return errorMsg("Wrong number of arguments");

    std::string s; long i, n;
    if (! values[0]->getStringValue(s) ||
        ! values[1]->getIntegerValue(i) ||
        ! values[2]->getIntegerValue(n))
      return errorMsg("Wrong argument type for MID$");

    if (i < 1 || i > int(s.size()))
      return errorMsg("Outof range for MID$");

    auto s1 = s.substr(i - 1, n);

    return expr_->createStringValue(s1);
  }
};

class CPetBasicPeekFunction : public CPetBasicFunction {
 public:
  CPetBasicPeekFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    long addr;
    if (! values[0]->getIntegerValue(addr))
      return errorMsg("Wrong argument type");

    if (addr < 0 || addr >= 65536)
      return errorMsg("Invalid address");

    auto value = basic_->getMemory(uint(addr) & 0xFFFF);

    return expr_->createIntegerValue(value);
  }
};

class CPetBasicPosFunction : public CPetBasicFunction {
 public:
  CPetBasicPosFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    return expr_->createIntegerValue(basic_->term()->col());
  }
};

class CPetBasicRightFunction : public CPetBasicFunction {
 public:
  CPetBasicRightFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 2) return errorMsg("Wrong number of arguments");

    std::string s; long n;
    if (! values[0]->getStringValue(s) || ! values[1]->getIntegerValue(n))
      return errorMsg("Wrong argument type");

    auto s1 = s.substr(std::max(0, int(s.size() - n)));

    return expr_->createStringValue(s1);
  }
};

class CPetBasicRndFunction : public CPetBasicFunction {
 public:
  CPetBasicRndFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto r1 = COSRand::randIn(0.0, 1.0);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicSgnFunction : public CPetBasicFunction {
 public:
  CPetBasicSgnFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto i = (r != 0 ? (r < 0 ? -1 : 1) : 0);

    return expr_->createIntegerValue(i);
  }
};

class CPetBasicSinFunction : public CPetBasicFunction {
 public:
  CPetBasicSinFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto r1 = std::sin(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicSqrFunction : public CPetBasicFunction {
 public:
  CPetBasicSqrFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    if (r < 0)
      return errorMsg("Domain error");

    auto r1 = std::sqrt(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicStrsFunction : public CPetBasicFunction {
 public:
  CPetBasicStrsFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto s = std::to_string(r);

    return expr_->createStringValue(s);
  }
};

class CPetBasicSpcFunction : public CPetBasicFunction {
 public:
  CPetBasicSpcFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    long i;
    if (! values[0]->getIntegerValue(i))
      return errorMsg("Wrong argument type");

    std::string s;
    for ( ; i > 0; --i)
      s += " ";

    return expr_->createStringValue(s);
  }
};

class CPetBasicTabFunction : public CPetBasicFunction {
 public:
  CPetBasicTabFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    int currentPos = 0; // TODO

    long newPos;
    if (! values[0]->getIntegerValue(newPos))
      return errorMsg("Wrong argument type");

    std::string s;

    if (newPos < currentPos) {
      s += "\n";

      currentPos = 0;
    }

    for ( ; currentPos < newPos; ++currentPos)
      s += " ";

    return expr_->createStringValue(s);
  }
};

class CPetBasicTanFunction : public CPetBasicFunction {
 public:
  CPetBasicTanFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    double r;
    if (! values[0]->getRealValue(r))
      return errorMsg("Wrong argument type");

    auto r1 = std::tan(r);

    return expr_->createRealValue(r1);
  }
};

class CPetBasicUsrFunction : public CPetBasicFunction {
 public:
  CPetBasicUsrFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &) override {
    return errorMsg("Unsupported command");
  }
};

class CPetBasicValFunction : public CPetBasicFunction {
 public:
  CPetBasicValFunction(CPetBasic *basic) : CPetBasicFunction(basic) { }

  CExprValuePtr exec(const CExprValueArray &values) override {
    auto nv = values.size();
    if (nv != 1) return errorMsg("Wrong number of arguments");

    std::string s;
    if (! values[0]->getStringValue(s))
      return errorMsg("Wrong argument type");

    CStrParse parse(s);

    parse.skipSpace();

    std::string numStr;

    while (! parse.eof()) {
      if (! parse.isDigit())
        break;

      numStr += parse.readChar();
    }

    if (! parse.eof() && parse.isChar('.')) {
      numStr += parse.readChar();

      while (! parse.eof()) {
        if (! parse.isDigit())
          break;

        numStr += parse.readChar();
      }
    }

    if (numStr != "") {
      auto r = std::stod(numStr);
      auto i = std::stol(numStr);

      if (std::abs(r - double(i)) < 1E-6)
        return expr_->createIntegerValue(i);
      else
        return expr_->createRealValue(r);
    }
    else
      return expr_->createIntegerValue(0);
  }
};

//---

class CPetBasicDSVar : public CExprVariableObj {
 public:
  CPetBasicDSVar(CPetBasic *basic) : basic_(basic) { }

  CExprValuePtr get() const override {
    return basic_->expr()->createIntegerValue(0);
  }

  void set(CExprValuePtr) override { /* NOOP */ }

 private:
  CPetBasic *basic_ { nullptr };
};

class CPetBasicDSSVar : public CExprVariableObj {
 public:
  CPetBasicDSSVar(CPetBasic *basic) : basic_(basic) { }

  CExprValuePtr get() const override {
    return basic_->expr()->createStringValue("");
  }

  void set(CExprValuePtr) override { /* NOOP */ }

 private:
  CPetBasic *basic_ { nullptr };
};

class CPetBasicTIVar : public CExprVariableObj {
 public:
  CPetBasicTIVar(CPetBasic *basic) : basic_(basic) { }

  CExprValuePtr get() const override {
    auto l = long(time(nullptr));

    return basic_->expr()->createIntegerValue(l);
  }

  void set(CExprValuePtr) override { /* NOOP */ }

 private:
  CPetBasic *basic_ { nullptr };
};

class CPetBasicTISVar : public CExprVariableObj {
 public:
  CPetBasicTISVar(CPetBasic *basic) : basic_(basic) { }

  CExprValuePtr get() const override {
    return basic_->expr()->createStringValue("");
  }

  void set(CExprValuePtr) override { /* NOOP */ }

 private:
  CPetBasic *basic_ { nullptr };
};

class CPetBasicStatusVar : public CExprVariableObj {
 public:
  CPetBasicStatusVar(CPetBasic *basic) : basic_(basic) { }

  CExprValuePtr get() const override {
    return basic_->expr()->createIntegerValue(0);
  }

  void set(CExprValuePtr) override { /* NOOP */ }

 private:
  CPetBasic *basic_ { nullptr };
};

//---

CPetBasic::
CPetBasic()
{
  COSRand::srand();

  initExpr();

  term_ = new CPetBasicTerm(this);

  term_->init();
}

CPetBasic::
~CPetBasic()
{
  clearLines();

  delete term_;

//std::cerr << "Num Basic Tokens " << s_num_basic_tokens_created <<
//             " " << s_num_basic_tokens_deleted << "\n";
}

void
CPetBasic::
initExpr()
{
  expr_ = std::make_unique<CPetBasicExpr>(this);

  expr_->addFunction("ABS"   , "r"    , new CPetBasicAbsFunction   (this));
  expr_->addFunction("ASC"   , "s"    , new CPetBasicAscFunction   (this));
  expr_->addFunction("ATN"   , "r"    , new CPetBasicAtnFunction   (this));
  expr_->addFunction("CHR$"  , "i"    , new CPetBasicChrFunction   (this));
  expr_->addFunction("COS"   , "r"    , new CPetBasicCosFunction   (this));
  expr_->addFunction("EXP"   , "r"    , new CPetBasicExpFunction   (this));
  expr_->addFunction("FRE"   , "s"    , new CPetBasicFreFunction   (this));
  expr_->addFunction("INT"   , "r"    , new CPetBasicIntFunction   (this));
  expr_->addFunction("LEFT$" , "s,i"  , new CPetBasicLeftFunction  (this));
  expr_->addFunction("LEN"   , "s"    , new CPetBasicLenFunction   (this));
  expr_->addFunction("LOG"   , "r"    , new CPetBasicLogFunction   (this));
  expr_->addFunction("MID$"  , "s,i,i", new CPetBasicMidFunction   (this));
  expr_->addFunction("PEEK"  , "i"    , new CPetBasicPeekFunction  (this));
  expr_->addFunction("POS"   , "s"    , new CPetBasicPosFunction   (this));
  expr_->addFunction("RIGHT$", "s,i"  , new CPetBasicRightFunction (this));
  expr_->addFunction("RND"   , "s"    , new CPetBasicRndFunction   (this));
  expr_->addFunction("SGN"   , "r"    , new CPetBasicSgnFunction   (this));
  expr_->addFunction("SIN"   , "s"    , new CPetBasicSinFunction   (this));
  expr_->addFunction("SPC"   , "i"    , new CPetBasicSpcFunction   (this));
  expr_->addFunction("SQR"   , "r"    , new CPetBasicSqrFunction   (this));
  expr_->addFunction("STR$"  , "r"    , new CPetBasicStrsFunction  (this));
  expr_->addFunction("TAB"   , "i"    , new CPetBasicTabFunction   (this));
  expr_->addFunction("TAN"   , "r"    , new CPetBasicTanFunction   (this));
  expr_->addFunction("USR"   , "s"    , new CPetBasicUsrFunction   (this));
  expr_->addFunction("VAL"   , "s"    , new CPetBasicValFunction   (this));

  expr_->createUserVariable("DS"    , new CPetBasicDSVar    (this));
  expr_->createUserVariable("DS$"   , new CPetBasicDSSVar   (this));
  expr_->createUserVariable("TI"    , new CPetBasicTIVar    (this));
  expr_->createUserVariable("TI$"   , new CPetBasicTISVar   (this));
  expr_->createUserVariable("STATUS", new CPetBasicStatusVar(this));
}

bool
CPetBasic::
loadFile(const std::string &fileName)
{
  fileName_ = fileName;

  CFileParse fileParse(fileName_);

  fileParse.setStream(true);

  uint lineInd = 0;

  auto readLine = [&]() {
    std::string line;

    while (! fileParse.eof()) {
      if (fileParse.isChar('\n'))
        break;

      if (! fileParse.eof())
        line += fileParse.readChar();
    }

    if (fileParse.isChar('\n'))
      fileParse.skipChar();

    ++lineInd;

    return line;
  };

  //---

  uint lastLineNum = 0;

  forStack_ .clear();
  nextStack_.clear();

  while (! fileParse.eof()) {
    auto line = readLine();

    LineData lineData;

    if (! parseLine(lineInd, line, lineData))
      continue;

    if (lineData.lineN > 0 && lineData.lineN < lastLineNum)
      warnMsg("Non sequential line numbers " +
              std::to_string(lastLineNum) + " " + std::to_string(lineData.lineN));

    if (lineData.lineN > 0) {
      auto pl = lines_.find(lineData.lineN);

      if (pl != lines_.end())
        warnMsg("Overwriting line " + std::to_string(lineData.lineN));

      lines_[lineData.lineN] = lineData;
    }

    lastLineNum = lineData.lineN;
  }

  //---

#if 0
  for (const auto &forData : forStack_)
    std::cerr << "FOR " << forData.varName << " @ " <<
      forData.line.lineNum << ":" << forData.line.statementNum << "\n";

  for (const auto &nextData : nextStack_)
    std::cerr << "NEXT " << nextData.varName << " @ " <<
      nextData.line.lineNum << ":" << nextData.line.statementNum << "\n";
#endif

  //---

  // split line into statements
  for (auto &pl : lines_) {
    auto &lineData = pl.second;

    processLineData(lineData);
  }

  runDataValid_ = false;

  initRunData();

  notifyLinesChanged();

  return true;
}

void
CPetBasic::
processLineData(LineData &lineData) const
{
  Statement statement;

  auto flushStatement = [&]() {
    if (! statement.tokens.empty()) {
      lineData.statements.push_back(statement);

      statement = Statement();
    }
  };

  for (auto *token : lineData.tokens) {
    if (isSeparator(token, SeparatorType::COLON)) {
      flushStatement();
      continue;
    }

    statement.tokens.push_back(token);
  }

  flushStatement();
}

bool
CPetBasic::
parseLine(uint lineInd, const std::string &line, LineData &lineData) const
{
  CStrParse parse(line);

  //---

  // read line number
  long n = 0;

  parse.skipSpace();

  if (! parse.eof() && parse.isDigit()) {
    std::string lineStr;

    while (! parse.eof()) {
      if (! parse.isDigit())
        break;

      lineStr += parse.readChar();
    }

    if (lineStr == "")
      return false;

    n = std::stol(lineStr);

    if (n < 0) {
      warnMsg("Invalid line number '" + lineStr + "' @" + std::to_string(lineInd));
      return false;
    }
  }

  //---

  // read code
  parse.skipSpace();

  std::string codeStr;

  while (! parse.eof())
    codeStr += parse.readChar();

  lineData.line    = line;
  lineData.lineNum = lineInd;
  lineData.lineN   = uint(n);

  parseLineTokens(codeStr, lineData);

  return true;
}

bool
CPetBasic::
parseLineTokens(const std::string &line, LineData &lineData) const
{
  std::string tokenStr;
  bool        embedded { false };

  CStrParse parse(line);

  TokenType   firstToken   = TokenType::NONE;
  KeywordType firstKeyword = KeywordType::NONE;
  int         statementNum = 0;
  KeywordType keywordType  = KeywordType::NONE;
  std::string keywordStr;

  auto flushToken = [&](TokenType type=TokenType::NONE) {
    if (type != TokenType::NONE || tokenStr != "") {
      Token *token = nullptr;

      if      (type == TokenType::KEYWORD) {
        assert(keywordType != KeywordType::NONE);

        token = createKeyword(keywordType, keywordStr);
      }
      else if (type == TokenType::VARIABLE)
        token = createVariable(tokenStr);
      else if (type == TokenType::OPERATOR)
        token = createOperator(tokenStr);
      else if (type == TokenType::SEPARATOR)
        token = createSeparator(tokenStr);
      else if (type == TokenType::STRING)
        token = createString(tokenStr, embedded);
      else if (type == TokenType::NUMBER)
        token = createNumber(tokenStr);
      else {
        warnMsg("Invalid token type for '" + tokenStr + "'");
        token = createToken(tokenStr);
      }

      lineData.tokens.push_back(token);

      if (firstToken == TokenType::NONE) {
        firstToken   = token->type();
        firstKeyword = (type == TokenType::KEYWORD ? keywordType : KeywordType::NONE);
      }

      keywordType = KeywordType::NONE;
      keywordStr  = "";

      tokenStr = "";
      embedded = false;
    }
  };

  while (! parse.eof()) {
    if      (parse.isChar(':')) {
      flushToken();

      tokenStr += parse.readChar();

      flushToken(TokenType::SEPARATOR);

      firstToken   = TokenType::NONE;
      firstKeyword = KeywordType::NONE;

      ++statementNum;
    }
    else if (parse.isOneOf(",;()")) {
      flushToken();

      tokenStr += parse.readChar();

      flushToken(TokenType::SEPARATOR);
    }
    else if (parse.isChar('#')) {
      flushToken();

      tokenStr += parse.readChar();

      flushToken(TokenType::OPERATOR);
    }
    else if (parse.isDigit() ||
             (parse.isChar('.') && parse.isDigitAt(1)) ||
             (parse.isChar('-') && parse.isDigitAt(1))) {
      flushToken();

      if (parse.isChar('-'))
        tokenStr += parse.readChar();

      if (parse.isChar('.'))
        tokenStr += parse.readChar();

      while (! parse.eof()) {
        if (! parse.isDigit())
          break;

        tokenStr += parse.readChar();
      }

      if (! parse.eof() && parse.isChar('.')) {
        tokenStr += parse.readChar();

        while (! parse.eof()) {
          if (! parse.isDigit())
            break;

          tokenStr += parse.readChar();
        }
      }

      flushToken(TokenType::NUMBER);
    }
    else if (parse.isOneOf("+-^*/")) {
      flushToken();

      tokenStr += parse.readChar();

      flushToken(TokenType::OPERATOR);
    }
    else if (parse.isOneOf("=<>")) {
      flushToken();

      tokenStr += parse.readChar();

      if      (tokenStr == "<" && parse.isOneOf("=>"))
        tokenStr += parse.readChar();
      else if (tokenStr == ">" && parse.isChar('='))
        tokenStr += parse.readChar();

      flushToken(TokenType::OPERATOR);
    }
    else if (parse.isString("OR")) {
      flushToken();

      tokenStr += parse.readChar();
      tokenStr += parse.readChar();

      flushToken(TokenType::OPERATOR);
    }
    else if (parse.isString("AND") || parse.isString("NOT")) {
      flushToken();

      tokenStr += parse.readChar();
      tokenStr += parse.readChar();
      tokenStr += parse.readChar();

      flushToken(TokenType::OPERATOR);
    }
    else if (parse.isChar('"')) {
      flushToken();

      parse.skipChar();

      while (! parse.eof()) {
        if      (parse.isChar('\\')) {
          parse.skipChar();

          if (! parse.eof())
            tokenStr += parse.readChar();
          else
            tokenStr += "/";
        }
        else if (parse.isChar('{')) {
          parse.skipChar();

          std::string str1;

          while (! parse.eof()) {
            if      (parse.isChar('{'))
              std::cerr << "Mismatched {\n";
            else if (parse.isChar('}'))
              break;

            str1 += parse.readChar();
          }

          if (parse.isChar('}'))
            parse.skipChar();

          std::string str2;

          if (replaceEmbedded(str1, str2)) {
            embedded = true;
            tokenStr += str2;
          }
          else
            tokenStr += "{" + str1 + "}";
        }
        else if (parse.isChar('}')) {
          std::cerr << "Mismatched }\n";
        }
        else {
          if (parse.isChar('"')) {
            parse.skipChar();
            break;
          }

          auto c = parse.readChar();

          tokenStr += char(c);
        }
      }

      flushToken(TokenType::STRING);
    }
    else if (parse.isAlpha()) {
      flushToken();

      while (! parse.eof()) {
        if (! parse.isAlpha() && ! parse.isChar('_') && ! parse.isDigit())
          break;

        tokenStr += parse.readChar();

        keywordType = lookupKeyword(tokenStr);

        if (keywordType != KeywordType::NONE)
          break;
      }

      keywordType = lookupKeyword(tokenStr);

      if (keywordType != KeywordType::NONE) {
        keywordStr = "";

        if (firstToken == TokenType::NONE) {
          if      (keywordType == KeywordType::REM) {
            parse.skipSpace();

            while (! parse.eof())
              keywordStr += parse.readChar();
          }
          else if (keywordType == KeywordType::DATA) {
            parse.skipSpace();

            while (! parse.eof())
              keywordStr += parse.readChar();

            addData(keywordStr);
          }
          else if (keywordType == KeywordType::FOR) {
            forStack_.emplace_back(LineRef(lineData.lineN, statementNum));
          }
          else if (keywordType == KeywordType::NEXT) {
            nextStack_.emplace_back(LineRef(lineData.lineN, statementNum));
          }
        }

        flushToken(TokenType::KEYWORD);
      }
      else {
        if (! parse.eof() && (parse.isChar('$') || parse.isChar('%')))
          tokenStr += parse.readChar();

        auto varName = tokenStr;

        flushToken(TokenType::VARIABLE);

        if      (firstKeyword == KeywordType::FOR) {
          assert(! forStack_.empty());

          forStack_.back().varName = varName;
        }
        else if (firstKeyword == KeywordType::NEXT) {
          assert(! nextStack_.empty());

          nextStack_.back().varName = varName;
        }
      }
    }
    else if (parse.isSpace()) {
      flushToken();

      parse.skipSpace();
    }
    else {
      auto c = parse.readChar();
      warnMsg("Unhandled '" + std::string(&c, 1) + "' @" + std::to_string(lineData.lineN));
      tokenStr += c;
    }
  }

  flushToken();

  return true;
}

// replace contents of {<str>} embedded char
bool
CPetBasic::
replaceEmbedded(const std::string &str1, std::string &str2) const
{
  std::string nstr1;

  for (const auto &c : str1) {
    if (isspace(c)) continue;
    nstr1 += char(std::toupper(c));
  }

  auto len1 = str1.size();

  str2 = "";

  auto matchStrs = [&](std::initializer_list<std::string> strs) {
    for (const auto &str : strs)
      if (nstr1 == str) return true;
    return false;
  };

  auto parseNumeric = [&](const std::string &name, long &n) {
    auto len = name.size();

    if      (str1 == name) {
      n = 1;

      return true;
    }
    else if (len1 > len && str1.substr(len1 - len, len) == name) {
      CStrParse parse(str1);

      if (! parse.readInteger(&n) && n > 0)
        return false;

      return true;
    }
    else
      return false;
  };

  auto parseShift = [&](long &n, std::string &rhs) {
    n = 1;

    uint pos1 = 0;

    if (isdigit(str1[pos1])) {
      ++pos1;

      while (pos1 < len1 && isdigit(str1[pos1]))
        ++pos1;

      n = std::stol(str1.substr(0, pos1));
    }

    if (pos1 >= len1 && str1[pos1] != '^')
      return false;

    ++pos1;

    rhs = str1.substr(pos1);

    return true;
  };

  auto appendN = [](std::string &str, long n, auto c) {
    for (long i = 0; i < n; ++i)
      str += c;
  };

  //---

  long        n;
  std::string sstr;

  // SHIFT +128
  // INVERSE +64

  bool rc = true;

  // values are pet chars when in string (other string chars are ascii)
  // reverse in string uses separate char so no characters in string are reversed)
  if      (str1 == "HOM") { // 19 (octal 23)
    str2 += encodeEmbedded("HOM");
  }
  else if (matchStrs({"CLS", "CLR"})) { // 147 (octal 223)
    str2 += encodeEmbedded("CLS");
  }
  else if (str1 == "PI") { // 255 (octal 377)
    str2 += encodeEmbedded("PI");
  }
  else if (matchStrs({"REV", "RVON"})) { // 18 (octal 22)
    str2 += encodeEmbedded("REV");
  }
  else if (matchStrs({"OFF", "RVOF"})) { // 146 (octal 222)
    str2 += encodeEmbedded("OFF");
  }
  else if (parseNumeric("CU", n)) {  // 145 (octal 221)
    appendN(str2, n, encodeEmbedded("CU"));
  }
  else if (parseNumeric("CD", n)) { // 17 (octal 21)
    appendN(str2, n, encodeEmbedded("CD"));
  }
  else if (parseNumeric("CL", n)) { // 157 (octal 235)
    appendN(str2, n, encodeEmbedded("CL"));
  }
  else if (parseNumeric("CR", n)) { // 29 (octal 35)
    appendN(str2, n, encodeEmbedded("CR"));
  }
  else if (parseNumeric("SPC", n)) { // 32 (octal 40)
    appendN(str2, n, ' ');
  }
  else if (parseNumeric("^SPC", n)) { // 160 (octal 240)
    appendN(str2, n, encodeEmbedded("^SPC"));
  }
  else if (parseNumeric("DQT", n)) {
    appendN(str2, n, "\"");
  }
  else if (parseNumeric("BSH", n)) {
    appendN(str2, n, "\\");
  }
  else if (parseNumeric("DEL", n)) {
    appendN(str2, n, "~");
  }
  else if (parseShift(n, sstr)) { // ^<char> (shifted character)
    uchar c = uchar(std::toupper(sstr[0])); c += 128;
    char c1(c);
    appendN(str2, n, std::string(&c1, 1));
  }
  else {
    std::cerr << "Invalid embedded code '" << str1 << "'\n";
    rc = false;
  }

  return rc;
}

std::string
CPetBasic::
encodeEmbedded(const std::string &name) const
{
  uchar c { 0 };

  if      (name == "HOM" ) c = 19;
  else if (name == "CLS" ) c = 147;
  else if (name == "PI"  ) c = 255;
  else if (name == "REV" ) c = 18;
  else if (name == "OFF" ) c = 146;
  else if (name == "CU"  ) c = 145;
  else if (name == "CD"  ) c = 17;
  else if (name == "CL"  ) c = 157;
  else if (name == "CR"  ) c = 29;
  else if (name == "^SPC") c = 160;

  else if (name.size() == 1) {
    CPetDrawChar drawChar(name[0]);

    auto pet = drawCharToPet(drawChar);

    pet.shift();

    c = pet.c();
  }

  std::string s = " ";
  s[0] = c;

  return s;
}

std::string
CPetBasic::
decodeEmbeddedStr(const std::string &s)
{
  std::string s1;

  for (const auto &c : s) {
    auto c1 = uchar(c);

    s1 += decodeEmbeddedChar(c1);
  }

  return s1;
}

std::string
CPetBasic::
decodeEmbeddedChar(uchar c)
{
  switch (c) {
    case  19: return "{HOM}";
    case 147: return "{CLS}";
    case 255: return "{PI}";
    case  18: return "{REV}";
    case 146: return "{OFF}";
    case 145: return "{CU}";
    case  17: return "{CD}";
    case 157: return "{CL}";
    case  29: return "{CR}";
    case 160: return "{^SPC}";
  }

  if      (c == '\"')
    return "{DQT}";
  else if (c == '\\')
    return "{BSH}";
  else if (c == '~')
    return "{DEL}";

  if (c >= 128) {
    c -= 128;

    char c1(c);

    return "{^" + std::string(&c1, 1) + "}";
  }
  else {
    char c1(c);

    return std::string(&c1, 1);
  }
}

//---

uint
CPetBasic::
numLines() const
{
  return uint(lines_.size());
}

uint
CPetBasic::
maxLine() const
{
  auto pl = lines_.rbegin();

  return (pl != lines_.rend() ? (*pl).second.lineN : 0);
}

//---

CPetBasic::KeywordType
CPetBasic::
lookupKeyword(const std::string &str) const
{
  auto ustr = CPetBasicUtil::toUpper(str);

  initKeywords();

  auto pk = nameKeywords_.find(ustr);

  if (pk == nameKeywords_.end())
    return KeywordType::NONE;

  return (*pk).second;
}

std::string
CPetBasic::
keywordName(KeywordType keywordType) const
{
  auto pk = keywordNames_.find(keywordType);

  if (pk == keywordNames_.end())
    return "";

  return (*pk).second;
}

void
CPetBasic::
initKeywords() const
{
  auto addKeyword = [&](KeywordType keywordType, const std::string &str) {
    nameKeywords_[str        ] = keywordType;
    keywordNames_[keywordType] = str;
  };

  if (nameKeywords_.empty()) {
    addKeyword(KeywordType::APPEND   , "APPEND"   );
#ifdef PET_EXTRA_KEYWORDS
    addKeyword(KeywordType::ASSERT   , "ASSERT"   );
#endif
    addKeyword(KeywordType::BACKUP   , "BACKUP"   );
    addKeyword(KeywordType::CLOSE    , "CLOSE"    );
    addKeyword(KeywordType::CLR      , "CLR"      );
    addKeyword(KeywordType::CMD      , "CMD"      );
    addKeyword(KeywordType::COLLECT  , "COLLECT"  );
    addKeyword(KeywordType::CONCAT   , "CONCAT"   );
    addKeyword(KeywordType::CONT     , "CONT"     );
    addKeyword(KeywordType::COPY     , "COPY"     );
    addKeyword(KeywordType::DATA     , "DATA"     );
    addKeyword(KeywordType::DCLOSE   , "DCLOSE"   );
    addKeyword(KeywordType::DEF      , "DEF"      );
#ifdef PET_EXTRA_KEYWORDS
    addKeyword(KeywordType::DELAY    , "DELAY"    );
#endif
    addKeyword(KeywordType::DIM      , "DIM"      );
    addKeyword(KeywordType::DIRECTORY, "DIRECTORY");
    addKeyword(KeywordType::DLOAD    , "DLOAD"    );
    addKeyword(KeywordType::DOPEN    , "DOPEN"    );
    addKeyword(KeywordType::DSAVE    , "DSAVE"    );
#ifdef PET_EXTRA_KEYWORDS
    addKeyword(KeywordType::ELAPSED  , "ELAPSED"  );
#endif
    addKeyword(KeywordType::END      , "END"      );
    addKeyword(KeywordType::FN       , "FN"       );
    addKeyword(KeywordType::FOR      , "FOR"      );
    addKeyword(KeywordType::GET      , "GET"      );
    addKeyword(KeywordType::GOSUB    , "GOSUB"    );
    addKeyword(KeywordType::GOTO     , "GOTO"     );
    addKeyword(KeywordType::HEADER   , "HEADER"   );
    addKeyword(KeywordType::IF       , "IF"       );
    addKeyword(KeywordType::INPUT    , "INPUT"    );
    addKeyword(KeywordType::LET      , "LET"      );
    addKeyword(KeywordType::LIST     , "LIST"     );
    addKeyword(KeywordType::LOAD     , "LOAD"     );
    addKeyword(KeywordType::NEW      , "NEW"      );
    addKeyword(KeywordType::NEXT     , "NEXT"     );
    addKeyword(KeywordType::ON       , "ON"       );
    addKeyword(KeywordType::OPEN     , "OPEN"     );
    addKeyword(KeywordType::PEEK     , "PEEK"     );
#ifdef PET_EXTRA_KEYWORDS
    addKeyword(KeywordType::PLOT     , "PLOT"     );
#endif
    addKeyword(KeywordType::POKE     , "POKE"     );
    addKeyword(KeywordType::PRINT    , "PRINT"    );
    addKeyword(KeywordType::READ     , "READ"     );
    addKeyword(KeywordType::RECORD   , "RECORD"   );
    addKeyword(KeywordType::REM      , "REM"      );
    addKeyword(KeywordType::RESTORE  , "RESTORE"  );
    addKeyword(KeywordType::RETURN   , "RETURN"   );
    addKeyword(KeywordType::RUN      , "RUN"      );
    addKeyword(KeywordType::SAVE     , "SAVE"     );
    addKeyword(KeywordType::STEP     , "STEP"     );
    addKeyword(KeywordType::STOP     , "STOP"     );
    addKeyword(KeywordType::SYS      , "SYS"      );
    addKeyword(KeywordType::THEN     , "THEN"     );
    addKeyword(KeywordType::TO       , "TO"       );
    addKeyword(KeywordType::USR      , "USR"      );
    addKeyword(KeywordType::VERIFY   , "VERIFY"   );
    addKeyword(KeywordType::WAIT     , "WAIT"     );
  }
}

void
CPetBasic::
list()
{
  list(-1, -1);
}

void
CPetBasic::
list(long startNum, long endNum)
{
  //if (startNum < 0) std::cerr << "Start: " << startNum << "\n";
  //if (endNum   < 0) std::cerr << "End: "   << endNum   << "\n";

  // start num to end
  if      (startNum > 0 && endNum < 0) {
    for (const auto &pl : lines_) {
      const auto &lineData = pl.second;

      if (long(lineData.lineN) >= startNum)
        listLine(lineData);
    }
  }
  // start to end num
  else if (startNum < 0 && endNum > 0) {
    for (const auto &pl : lines_) {
      const auto &lineData = pl.second;

      if (long(lineData.lineN) <= endNum)
        listLine(lineData);
    }
  }
  // start num to end num
  else if (startNum > 0 && endNum > 0) {
    for (const auto &pl : lines_) {
      const auto &lineData = pl.second;

      if (long(lineData.lineN) >= startNum && long(lineData.lineN) <= endNum)
        listLine(lineData);
    }
  }
  // all
  else {
    for (const auto &pl : lines_) {
      const auto &lineData = pl.second;

      listLine(lineData);
    }
  }
}

void
CPetBasic::
listLine(const LineData &lineData) const
{
  if (lineData.lineN > 0)
    printString(std::to_string(lineData.lineN) + " ");

  auto str = lineToString(lineData, isListHighlight());

  if (isListHighlight())
    std::cout << str << "\n";
  else
    printString(str + "\n");
}

const CPetBasic::LineData *
CPetBasic::
getLineIndData(uint lineInd) const
{
  int lineNum = lineIndNum(int(lineInd));
  if (lineNum < 0) return nullptr;

  auto p = lines_.find(lineNum);
  if (p == lines_.end()) return nullptr;

  return &(*p).second;
}

std::string
CPetBasic::
lineToString(const LineData &lineData, bool highlight) const
{
  int ns = int(std::log10(lineData.lineN) + 1);

  std::string str;

  uint is = 0;

  for (const auto &statement : lineData.statements) {
    char lastChar = ' ';

    if (is > 0) {
      if (isSplitStatements()) {
        str += "\n";

        for (int i = 0; i < ns; ++i)
          str += " ";
      }
      else {
        str += ":";

        lastChar = ':';
      }
    }

    //---

    str += statementToString(statement, lastChar, highlight);

    ++is;
  }

  return str;
}

std::string
CPetBasic::
statementToString(const Statement &statement, char lastChar, bool highlight) const
{
  static std::string sepCharsL = ",;:()=+-";
  static std::string sepCharsR = ",;:()=+-";

  char lastChar1 = lastChar;

  std::string str, str1;

  auto lastType = TokenType::NONE;

  for (auto *token : statement.tokens) {
    std::string s, s1;

    if (token->type() == TokenType::STRING) {
      auto *strToken = static_cast<StringToken *>(token);

      if (strToken->isEmbedded()) {
        auto estr = decodeEmbeddedStr(strToken->toString());

        if (highlight)
          s1 = "\033[35m\"" + estr + "\"\033[0m";

        s = "\"" + estr + "\"";
      }
    }

    if (s.empty()) {
      std::stringstream ss, ss1;

      if (highlight)
        token->printEsc(ss1);

      token->print(ss);

      s  = ss .str();
      s1 = ss1.str();
    }

    bool needsSpace = true;

    if (lastType != TokenType::KEYWORD) {
      auto firstChar = s[0];

      if (sepCharsL.find(firstChar) != std::string::npos)
        needsSpace = false;

      if (sepCharsR.find(lastChar1) != std::string::npos)
        needsSpace = false;
    }

    if (needsSpace) {
      str  += " ";
      str1 += " ";
    }

    str  += s;
    str1 += s1;

    lastChar1 = s[s.size() - 1];
    lastType  = token->type();
  }

  if (highlight)
    return str1;
  else
    return str;
}

bool
CPetBasic::
run()
{
  initRunData();

  setLineInd(0, 0);

  breakLineNum_ = -1;

  bool rc = contRun();

  if (term()->col() > 0)
    term_->enter();

  return rc;
}

bool
CPetBasic::
step()
{
  initRunData();

  breakLineNum_ = -1;

  stopped_ = true;

  auto rc = contRun();

  stopped_ = false;

  return rc;
}

void
CPetBasic::
initRunData()
{
  if (! runDataValid_) {
    initRunState();

    //---

    // build lines vector
    lineNums_.clear();
    lineInds_.clear();

    int lineInd = 0;

    for (const auto &pl : lines_) {
      auto lineNum = pl.first;

      lineNums_.push_back(lineNum);

      lineInds_[lineNum] = uint(lineInd);

      ++lineInd;
    }

    //---

    setLineInd(0, 0);

    runDataValid_ = true;
  }
}

int
CPetBasic::
currentLineNum() const
{
  return lineIndNum(lineInd_);
}

int
CPetBasic::
lineIndNum(int lineInd) const
{
  if (lineInd < 0 || uint(lineInd) >= lineNums_.size())
    return -1;

  return lineNums_[lineInd];
}

bool
CPetBasic::
contRunTo(uint lineNum)
{
  initRunData();

  if (int(lineNum) > currentLineNum())
    breakLineNum_ = int(lineNum);
  else
    breakLineNum_ = -1;

  return contRun();
}

bool
CPetBasic::
contRun()
{
  initRunData();

  auto lineNum = currentLineNum();

  while (lineNum > 0) {
    if (lineNum == breakLineNum_) {
      setStopped(true);
      break;
    }

    auto pl = lines_.find(lineNum);
    assert(pl != lines_.end());

    if (! runLine((*pl).second)) {
      if (errorMsg_ != "")
        warnMsg("Error: " + errorMsg_ + " @" + std::to_string(lineNum));
      else
        warnMsg("Error: " + (*pl).second.line + " @" + std::to_string(lineNum));
      return false;
    }

    if (isStopped())
      break;

    lineNum = currentLineNum();
  }

  setStopped(false);

  return true;
}

//---

void
CPetBasic::
setRaw(bool b)
{
  delete term_;

  if (b)
    term_ = new CPetBasicRawTerm(this);
  else
    term_ = new CPetBasicTerm(this);

  term_->init();
}

//---

void
CPetBasic::
loop()
{
  term_->loop();
}

bool
CPetBasic::
inputLine(const std::string &lineBuffer)
{
  LineData lineData;

  if (parseLine(0, lineBuffer, lineData)) {
    processLineData(lineData);

    if (lineData.lineN > 0) {
      lines_[lineData.lineN] = lineData;

      runDataValid_ = false;

      notifyLinesChanged();
    }
    else {
      if (! runLine(lineData)) {
        if (errorMsg_ != "")
          warnMsg("Invalid Line: " + lineBuffer + " (" + errorMsg_ + ")");
        else
          warnMsg("Invalid Line: " + lineBuffer);
        return false;
      }
    }
  }
  else {
    warnMsg("Invalid Line: " + lineBuffer);
    return false;
  }

  return true;
}

bool
CPetBasic::
runLine(LineData &lineData)
{
  notifyRunLine(lineData.lineN);

  errorMsg_= "";

  if (isDebug())
    listLine(lineData);

  bool nextLine = true;

  auto numStatements = lineData.statements.size();

  while (statementNum_ < numStatements) {
    auto &statement = lineData.statements[statementNum_];

    if (! statement.compiled) {
      if (! compileTokens(statement.tokens, statement.hasCompiled, statement.compiledTokens))
        return false;

      statement.compiled = true;
    }

    LineRef lineRef(lineData.lineN, statementNum_);

    if (! runTokens(lineRef, statement.compiledTokens, nextLine))
      return false;

    if (! nextLine)
      break;

    ++statementNum_;
  }

  if (nextLine)
    setLineInd(lineInd_ + 1, 0);

  return true;
}

bool
CPetBasic::
compileTokens(const Tokens &tokens, bool &compiled, Tokens &compiledTokens)
{
  compiled = false;

  auto nt = tokens.size();
  assert(nt >= 1);

  uint it = 0;

  auto *token = tokens[it++];

  if (token->type() == TokenType::KEYWORD) {
    compiledTokens.push_back(token);

    TokenList tokenList(tokens);

    tokenList.skipToken(); // keyword

    auto *keyword = static_cast<KeywordToken *>(token);

    switch (keyword->keywordType()) {
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::ASSERT:
        compiled = compileAssertStatement(tokenList, compiledTokens);
        break;
#endif
      case KeywordType::CLR:
        break;
      case KeywordType::CONT:
        break;
      case KeywordType::DATA:
        break;
      case KeywordType::DEF:
        compiled = compileDefStatement(tokenList, compiledTokens);
        break;
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::DELAY:
        compiled = compileDelayStatement(tokenList, compiledTokens);
        break;
#endif
      case KeywordType::DIM:
        compiled = compileDimStatement(tokenList, compiledTokens);
        break;
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::ELAPSED:
        break;
#endif
      case KeywordType::FOR:
        compiled = compileForStatement(tokenList, compiledTokens);
        break;
      case KeywordType::GET:
        break;
      case KeywordType::GOSUB:
        compiled = compileGosubStatement(tokenList, compiledTokens);
        break;
      case KeywordType::GOTO:
        compiled = compileGotoStatement(tokenList, compiledTokens);
        break;
      case KeywordType::IF:
        compiled = compileIfStatement(tokenList, compiledTokens);
        break;
      case KeywordType::INPUT:
        break;
      case KeywordType::LET:
        compiled = compileLetStatement(tokenList, compiledTokens);
        break;
      case KeywordType::LIST:
        break;
      case KeywordType::NEXT:
        compiled = compileNextStatement(tokenList, compiledTokens);
        break;
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::PLOT:
        compiled = compilePlotStatement(tokenList, compiledTokens);
        break;
#endif
      case KeywordType::POKE:
        compiled = compilePokeStatement(tokenList, compiledTokens);
        break;
      case KeywordType::PRINT:
        compiled = compilePrintStatement(tokenList, compiledTokens);
        break;
      case KeywordType::READ:
        compiled = compileReadStatement(tokenList, compiledTokens);
        break;
      case KeywordType::REM:
        break;
      case KeywordType::RETURN:
        break;
      case KeywordType::RUN:
        break;
      case KeywordType::STOP:
        break;
      default:
        std::cerr << "No compile for " << keyword->exprString() << "\n";
        break;
    }
  }
  else if (token->type() == TokenType::VARIABLE) {
    // get variable token
    auto *varToken = static_cast<VariableToken *>(token);

    auto *token1 = (it < nt ? tokens[it++] : nullptr);

    //---

    // get optional index tokens
    Tokens indExprs;

    if (token1 && isSeparator(token1, SeparatorType::OPEN_RBRACKET)) {
      auto *token2 = (it < nt ? tokens[it++] : nullptr);

      Tokens indexTokens;

      auto addInd = [&]() {
        if (indexTokens.empty())
          return errorMsg("No indices for array variable");

        ExprData exprData;
        if (! tokensToExpr(indexTokens, exprData))
          return false;

        auto *exprToken = createExpr(exprData);

        indExprs.push_back(exprToken);

        return true;
      };

      while (token2 && ! isSeparator(token2, SeparatorType::CLOSE_RBRACKET)) {
        if (isSeparator(token2, SeparatorType::COMMA)) {
          if (! addInd())
            return false;

          indexTokens.clear();
        }
        else {
          indexTokens.push_back(token2);
        }

        token2 = (it < nt ? tokens[it++] : nullptr);
      }

      if (! isSeparator(token2, SeparatorType::CLOSE_RBRACKET))
        return errorMsg("Missing close bracket for variable index");

      if (! addInd())
        return false;

      //--

      token1 = (it < nt ? tokens[it++] : nullptr);
    }

    auto *indexToken = createTokenList(indExprs);

    // get assign tokens
    ExprToken *exprToken = nullptr;

    if (token1 && isOperator(token1, OperatorType::ASSIGN)) {
      Tokens assignTokens;

      for (uint i = it; i < nt; ++i)
        assignTokens.push_back(tokens[i]);

      ExprData exprData;
      if (! tokensToExpr(assignTokens, exprData))
        return false;

      exprToken = createExpr(exprData);
    }
    else {
      if (token1)
        return errorMsg("Invalid token '" + token1->str() + "'");
      else
        return errorMsg("Invalid token");
    }

    compiledTokens.push_back(varToken);
    compiledTokens.push_back(indexToken);
    compiledTokens.push_back(exprToken);

    compiled = true;
  }
  else
    return errorMsg("Invalid compile token " + token->listString());

  if (! compiled)
    compiledTokens = tokens;

  return true;
}

bool
CPetBasic::
runTokens(const LineRef &lineRef, const Tokens &tokens, bool &nextLine)
{
  auto nt = tokens.size();
  assert(nt >= 1);

  uint it = 0;

  auto *token = tokens[it++];

  if (token->type() == TokenType::KEYWORD) {
    TokenList tokenList(tokens);

    tokenList.skipToken(); // keyword

    auto *keyword = static_cast<KeywordToken *>(token);

    bool rc = false;

    switch (keyword->keywordType()) {
      case KeywordType::APPEND:
        rc = appendStatement(tokens);
        break;
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::ASSERT:
        rc = assertStatement(tokens);
        break;
#endif
      case KeywordType::BACKUP:
        rc = backupStatement(tokens);
        break;
      case KeywordType::CLOSE:
        rc = closeStatement(tokens);
        break;
      case KeywordType::CLR:
        rc = clrStatement(tokens);
        break;
      case KeywordType::CMD:
        rc = cmdStatement(tokens);
        break;
      case KeywordType::COLLECT:
        rc = collectStatement(tokens);
        break;
      case KeywordType::CONCAT:
        rc = concatStatement(tokens);
        break;
      case KeywordType::CONT:
        rc = contStatement(tokens);
        break;
      case KeywordType::COPY:
        rc = copyStatement(tokens);
        break;
      case KeywordType::DATA:
        rc = dataStatement(token->str());
        break;
      case KeywordType::DCLOSE:
        rc = dcloseStatement(tokens);
        break;
      case KeywordType::DEF:
        rc = defStatement(tokens);
        break;
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::DELAY:
        rc = delayStatement(tokens);
        break;
#endif
      case KeywordType::DIM:
        rc = dimStatement(tokens);
        break;
      case KeywordType::DIRECTORY:
        rc = directoryStatement(tokens);
        break;
      case KeywordType::DLOAD:
        rc = dloadStatement(tokens);
        break;
      case KeywordType::DOPEN:
        rc = dopenStatement(tokens);
        break;
      case KeywordType::DSAVE:
        rc = dsaveStatement(tokens);
        break;
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::ELAPSED:
        rc = elapsedStatement(tokens);
        break;
#endif
      case KeywordType::END:
        rc = endStatement(tokens);
        break;
      case KeywordType::FOR:
        rc = forStatement(lineRef, tokens);
        break;
      case KeywordType::GET:
        rc = getStatement(tokenList);
        break;
      case KeywordType::GOSUB:
        rc = gosubStatement(tokens);
        nextLine = false;
        break;
      case KeywordType::GOTO:
        rc = gotoStatement(tokens);
        nextLine = false;
        break;
      case KeywordType::HEADER:
        rc = headerStatement(tokens);
        break;
      case KeywordType::IF:
        rc = ifStatement(lineRef, tokens, nextLine);
        break;
      case KeywordType::INPUT:
        rc = inputStatement(tokenList);
        break;
      case KeywordType::LET:
        rc = letStatement(tokens);
        break;
      case KeywordType::LIST:
        rc = listStatement(tokenList);
        break;
      case KeywordType::LOAD:
        rc = loadStatement(tokens);
        break;
      case KeywordType::NEW:
        rc = newStatement(tokens);
        break;
      case KeywordType::NEXT:
        rc = nextStatement(lineRef, tokens);
        nextLine = false;
        break;
      case KeywordType::ON:
        rc = onStatement(tokenList);
        nextLine = false;
        break;
      case KeywordType::OPEN:
        rc = openStatement(tokens);
        break;
#ifdef PET_EXTRA_KEYWORDS
      case KeywordType::PLOT:
        rc = plotStatement(tokens);
        break;
#endif
      case KeywordType::POKE:
        rc = pokeStatement(tokens);
        break;
      case KeywordType::PRINT:
        rc = printStatement(tokens);
        break;
      case KeywordType::READ:
        rc = readStatement(tokens);
        break;
      case KeywordType::RECORD:
        rc = recordStatement(tokens);
        break;
      case KeywordType::REM:
        rc = true;
        break;
      case KeywordType::RENAME:
        rc = renameStatement(tokens);
        break;
      case KeywordType::RESTORE:
        rc = restoreStatement(tokens);
        break;
      case KeywordType::RETURN:
        rc = returnStatement(tokens);
        nextLine = false;
        break;
      case KeywordType::RUN:
        rc = runStatement(tokens);
        break;
      case KeywordType::SAVE:
        rc = saveStatement(tokens);
        break;
      case KeywordType::SCRATCH:
        rc = scratchStatement(tokens);
        break;
      case KeywordType::STOP:
        rc = stopStatement(tokens);
        break;
      case KeywordType::SYS:
        rc = sysStatement(tokens);
        break;
      case KeywordType::VERIFY:
        rc = verifyStatement(tokens);
        break;
      case KeywordType::WAIT:
        rc = waitStatement(tokens);
        break;
      default:
        warnMsg("Unhandled keyword " + token->listString());
        break;
    }

    if (! rc || errorMsg_ != "")
      return errorMsg(errorMsg_ != "" ? errorMsg_ : "Command failed");
  }
  else if (token->type() == TokenType::VARIABLE) {
    // <var> <inds> <expr>
    assert(tokens.size() == 3);
    auto *varToken = static_cast<VariableToken *>(token);

    auto varName = varToken->str();

    //---

    assert(tokens[1]->type() == TokenType::TOKEN_LIST);
    auto *indTokens = static_cast<TokenListToken *>(tokens[1]);

    Inds inds;

    if (! indTokens->tokens().empty()) {
      for (const auto *itoken : indTokens->tokens()) {
        assert(itoken->type() == TokenType::EXPR);
        const auto *exprIToken = static_cast<const ExprToken *>(itoken);

        CExprValuePtr val;
        if (! exprIToken->eval(val))
          return false;

        long i;
        if (! val->getIntegerValue(i) || i < 0)
          return errorMsg("Invalid variable index value");

        inds.push_back(uint(i));
      };
    }

    //---

    assert(tokens[2]->type() == TokenType::EXPR);
    auto *exprToken = static_cast<ExprToken *>(tokens[2]);

    CExprValuePtr val;
    if (! exprToken->eval(val))
      return false;

    if (! inds.empty()) {
      if (! setVariableValue(varName, inds, val))
        return errorMsg("Failed to set variable value for '" + varName + "'");
    }
    else {
      if (! setVariableValue(varName, val))
        return errorMsg("Failed to set variable value for '" + varName + "'");
    }
  }
  else
    return errorMsg("Invalid run token " + token->listString());

  return true;
}

//---

void
CPetBasic::
setReverse(bool b)
{
  reverse_ = b;
}

void
CPetBasic::
setShift(bool b)
{
  shift_ = b;
}

void
CPetBasic::
setStopped(bool b)
{
  stopped_ = b;
}

//---

void
CPetBasic::
resize(uint nr, uint nc)
{
  nr_ = nr;
  nc_ = nc;
}

uchar
CPetBasic::
getMemory(uint addr) const
{
  if (addr >= 0x8000 && addr <= 0x87ff) {
    uint pos = addr - 0x8000;

    uint r = pos/nc_;
    uint c = pos - r*nc_;
    assert(r < nr_ && c < nc_);

    uchar value;

    if (getScreenMemory(r, c, value))
      return value;
  }

  auto p = memory_.find(addr);
  if (p == memory_.end()) return 0;

  return (*p).second;
}

void
CPetBasic::
setMemory(uint addr, uchar value)
{
  memory_[addr] = value;

  if (addr >= 0x8000 && addr <= 0x87ff) {
    uint pos = addr - 0x8000;

    uint r = pos/nc_;
    uint c = pos - r*nc_;
    assert(r < nr_ && c < nc_);

    setScreenMemory(r, c, value);
  }
}

bool
CPetBasic::
getScreenMemory(uint r, uint c, uchar &petsci) const
{
  // screen memory is ascii, need to return petsci
  auto drawChar = term_->getChar(r, c);

  auto pet = drawCharToPet(drawChar);

  petsci = pet.c();

  return true;
}

void
CPetBasic::
setScreenMemory(uint r, uint c, uchar petsci)
{
  // value is in petsci, screen memory is ascii
  auto drawChar = petToDrawChar(CPetsciChar(petsci));

  term_->setChar(r, c, drawChar);

  term_->update();
}

//---

void
CPetBasic::
addData(const std::string &dataStr) const
{
  auto *th = const_cast<CPetBasic *>(this);

  CStrParse parse(dataStr);

#if 0
  Tokens tokens;
  parseLine(dataStr, 0, tokens);

  for (const auto *token : tokens) {
    if      (token->type() == TokenType::STRING)
      th->addDataValue(expr_->createStringValue(token->toString()));
    else if (token->type() == TokenType::NUMBER) {
      if (token->isReal())
        th->addDataValue(expr_->createRealValue(token->toReal()));
      else
        th->addDataValue(expr_->createIntegerValue(token->toInteger()));
    }
    else if (token->type() == TokenType::VARIABLE)
      th->addDataValue(expr_->createStringValue(token->toString()));
    else if (token->type() == TokenType::SEPARATOR)
      continue;
    else
      warnMsg("Invalid token in addData '" + token->toString() + "'");
  }
#else
  auto readWord = [&]() {
    parse.skipSpace();

    std::string word;

    while (! parse.eof()) {
      if      (parse.isChar(',')) {
        parse.skipChar();
        return word;
      }
      else if (parse.isChar('"')) {
        parse.skipChar();

        while (! parse.eof()) {
          if (parse.isChar('\\')) {
            parse.skipChar();

            if (! parse.eof())
              word += parse.readChar();
            else
              word += "/";
          }
          else {
            if (parse.isChar('"')) {
              parse.skipChar();
              break;
            }

            word += parse.readChar();
          }
        }
      }
      else
        word += parse.readChar();
    }

    return word;
  };

  //---

  while (! parse.eof()) {
    auto word = readWord();

    CStrParse parse1(word);

    parse1.skipSpace();

    if ((parse1.isChar('-') && parse1.isDigitAt(1)) || parse1.isDigit()) {
      bool isReal = false;

      std::string str;

      if (parse1.isChar('-'))
        str += parse1.readChar();

      while (! parse1.eof()) {
        if (! parse1.isDigit())
          break;

        str += parse1.readChar();
      }

      if (! parse1.eof() && parse1.isChar('.')) {
        isReal = true;

        str += parse1.readChar();

        while (! parse1.eof()) {
          if (! parse1.isDigit())
            break;

          str += parse1.readChar();
        }
      }

      parse1.skipSpace();

      if (parse1.eof()) {
        if (! isReal) {
          auto i = std::stol(str);

          th->addDataValue(expr_->createIntegerValue(i));
        }
        else {
          auto r = std::stod(str);

          th->addDataValue(expr_->createRealValue(r));
        }
      }
      else {
        th->addDataValue(expr_->createStringValue(word));
      }
    }
    else {
      th->addDataValue(expr_->createStringValue(word));
    }
  }
#endif
}

//---

bool
CPetBasic::
appendStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
compileAssertStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  Tokens exprTokens;

  auto *token = tokenList.nextToken();

  while (token) {
    exprTokens.push_back(token);

    token = tokenList.nextToken();
  }

  ExprData exprData;
  if (! tokensToExpr(exprTokens, exprData))
    return false;

  auto *exprToken = createExpr(exprData);

  compiledTokens.push_back(exprToken);

  return true;
}

#ifdef PET_EXTRA_KEYWORDS
bool
CPetBasic::
assertStatement(const Tokens &tokens)
{
  // ASSERT <expr>
  auto nt = tokens.size();
  assert(nt == 2);

  assert(tokens[1]->type() == TokenType::EXPR);
  auto *exprToken = static_cast<ExprToken *>(tokens[1]);

  CExprValuePtr val;
  if (! exprToken->eval(val))
    return false;

  long i;
  if (! val->getIntegerValue(i))
    return errorMsg("Invalid assert expression");

  std::cerr << "assert: " << i << "\n";

  return true;
}
#endif

bool
CPetBasic::
backupStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
closeStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
clrStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra CLR tokens");

  variableNames_.clear();

  clearArrayVariables();

  initExpr();

  notifyVariablesChanged();

  return true;
}

bool
CPetBasic::
cmdStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
collectStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
concatStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
contStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra CONT tokens");

  if (isStopped()) {
    setStopped(false);

    contRun();
  }

  return true;
}

bool
CPetBasic::
copyStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
dataStatement(const std::string &)
{
  // ignored on run
  return true;
}

bool
CPetBasic::
dcloseStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
compileDefStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  auto *token = tokenList.nextToken();

  if (! isKeyword(token, KeywordType::FN))
    return errorMsg("Missing FN for DEF");

  // get variable name
  auto *varToken = tokenList.nextToken();

  if (! varToken || varToken->type() != TokenType::VARIABLE)
    return errorMsg("Invalid variable for DEF");

  token = tokenList.nextToken();

  //---

  // get arguments if specified
  auto lastType = TokenType::NONE;

  Tokens argTokens;

  if (token && isSeparator(token, SeparatorType::OPEN_RBRACKET)) {
    auto *token1 = tokenList.nextToken();

    while (token1 && ! isSeparator(token1, SeparatorType::CLOSE_RBRACKET)) {
      if (isSeparator(token1, SeparatorType::COMMA)) {
        if (lastType != TokenType::VARIABLE)
          return errorMsg("Invalid comma in argument list");
      }
      else {
        if (token1->type() != TokenType::VARIABLE)
          return errorMsg("Invalid argument list type");

        if (lastType == TokenType::VARIABLE)
          return errorMsg("Invalid extra variable in argument list");

        argTokens.push_back(token1);
      }

      lastType = token1->type();

      token1 = tokenList.nextToken();
    }

    if (! isSeparator(token1, SeparatorType::CLOSE_RBRACKET))
      return errorMsg("Missing close bracket for argument list");

    //--

    token = tokenList.nextToken();
  }

  if (argTokens.empty())
    return errorMsg("No arguments to function");

  //---

  // ensure we have assign operator
  if (! token || ! isOperator(token, OperatorType::ASSIGN))
    return errorMsg("Missing equals for DEF");

  //---

  // get function tokens
  Tokens fnTokens;

  token = tokenList.nextToken();

  while (token) {
    fnTokens.push_back(token);

    token = tokenList.nextToken();
  }

  //---

  auto *argsTokenList = createTokenList(argTokens);
  auto *fnTokenList   = createTokenList(fnTokens);

  compiledTokens.push_back(varToken);
  compiledTokens.push_back(argsTokenList);
  compiledTokens.push_back(fnTokenList);

  return true;
}

bool
CPetBasic::
defStatement(const Tokens &tokens)
{
  // DEF <var> <args> <tokens>
  auto nt = tokens.size();
  assert(nt == 4);

  auto *varToken  = tokens[1];
  auto *argsToken = tokens[2];
  auto *fnToken   = tokens[3];

  assert(varToken ->type() == TokenType::VARIABLE);
  assert(argsToken->type() == TokenType::TOKEN_LIST);
  assert(fnToken  ->type() == TokenType::TOKEN_LIST);

  //---

  auto varName = varToken->str();

  std::vector<std::string> args;

  for (auto *argToken : static_cast<TokenListToken *>(argsToken)->tokens())
    args.push_back(argToken->str());

  defineFunction(varName, args, static_cast<TokenListToken *>(fnToken)->tokens());

  return true;
}

bool
CPetBasic::
compileDelayStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  Tokens exprTokens;

  auto *token = tokenList.nextToken();

  while (token) {
    exprTokens.push_back(token);

    token = tokenList.nextToken();
  }

  ExprData exprData;
  if (! tokensToExpr(exprTokens, exprData))
    return false;

  auto *exprToken = createExpr(exprData);

  compiledTokens.push_back(exprToken);

  return true;
}

#ifdef PET_EXTRA_KEYWORDS
bool
CPetBasic::
delayStatement(const Tokens &tokens)
{
  // DELAY <expr>
  auto nt = tokens.size();
  assert(nt == 2);

  assert(tokens[1]->type() == TokenType::EXPR);
  auto *exprToken = static_cast<ExprToken *>(tokens[1]);

  CExprValuePtr val;
  if (! exprToken->eval(val))
    return false;

  long i;
  if (! val->getIntegerValue(i))
    return errorMsg("Invalid DELAY expression");

  term_->delay(i);

  return true;
}
#endif

bool
CPetBasic::
compileDimStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  auto *token = tokenList.currentToken();

  while (token) {
    Tokens tokens1;
    if (! compileVariable(tokenList, "DIM", tokens1))
      return false;

    if (tokens1.size() != 2)
      return errorMsg("Missing dimension for var");

    auto *varTokenList = createTokenList(tokens1);

    compiledTokens.push_back(varTokenList);

    //---

    token = tokenList.currentToken();

    if (token && ! isSeparator(token, SeparatorType::COMMA))
      return errorMsg("Missing comma for DIM");

    if (token)
      token = tokenList.nextToken();
  }

  return true;
}

bool
CPetBasic::
dimStatement(const Tokens &tokens)
{
  // DIM ( <var> <inds> , ...)
  auto nt = tokens.size();
  assert(nt >= 1);

  for (uint i = 1; i < nt; ++i) {
    auto *token = tokens[i];

    assert(token->type() == TokenType::TOKEN_LIST);
    auto *tokenList = static_cast<TokenListToken *>(token);

    // <var> <dims>
    const auto &tokens1 = tokenList->tokens();

    auto nt1 = tokens1.size();
    assert(nt1 == 2);

    auto *varToken = tokens1[0];
    auto varName = varToken->str();

    auto *indsToken = tokens1[1];
    assert(indsToken->type() == TokenType::TOKEN_LIST);
    auto *indsTokenList = static_cast<TokenListToken *>(indsToken);

    Inds inds;

    for (auto *itoken : indsTokenList->tokens()) {
      assert(itoken->type() == TokenType::EXPR);
      const auto *exprIToken = static_cast<const ExprToken *>(itoken);

      CExprValuePtr val;
      if (! exprIToken->eval(val))
        return false;

      long ival;
      if (! val->getIntegerValue(ival) || ival < 0)
        return errorMsg("Invalid variable index value");

      inds.push_back(uint(ival));
    }

    //---

    dimVariable(varName, inds);
  }

  return true;
}

bool
CPetBasic::
directoryStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
dloadStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
dopenStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
dsaveStatement(const Tokens &)
{
  assert(false);
  return false;
}

#ifdef PET_EXTRA_KEYWORDS
bool
CPetBasic::
elapsedStatement(const Tokens &)
{
  static CHRTime lastTime;
  static bool    lastTimeSet { false };

  auto t = COSTime::getHRTime();

  if (lastTimeSet) {
    auto dt = COSTime::diffHRTime(lastTime, t);

    std::cerr << "Elapsed: " << dt.usecs << "\n";
  }

  lastTime    = t;
  lastTimeSet = true;

  return true;
}
#endif

bool
CPetBasic::
endStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra END tokens");

  setStopped(true);

  return true;
}

bool
CPetBasic::
compileForStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  // get loop variable
  auto *varToken = tokenList.nextToken();
  if (! varToken || varToken->type() != TokenType::VARIABLE)
    return errorMsg("No FOR variable ");

  //---

  // check for assign operator
  auto *eqToken = tokenList.nextToken();
  if (! eqToken || ! isOperator(eqToken, OperatorType::ASSIGN))
    return errorMsg("No FOR = operator ");

  //---

  // get from tokens (break at TO)
  Tokens fromTokens;

  auto *token = tokenList.nextToken();

  bool toFound = false;

  while (token) {
    // break at TO keyword
    if (isKeyword(token, KeywordType::TO)) {
      toFound = true;
      break;
    }

    fromTokens.push_back(token);

    token = tokenList.nextToken();
  }

  if (! toFound)
    return errorMsg("No FOR TO token");

  if (fromTokens.empty())
    return errorMsg("No tokens for FOR ... TO");

  //---

  // get to tokens (break at end or STEP)
  Tokens toTokens;

  token = tokenList.nextToken();

  bool stepFound = false;

  while (token) {
    // break at STEP keyword
    if (isKeyword(token, KeywordType::STEP)) {
      stepFound = true;
      break;
    }

    toTokens.push_back(token);

    token = tokenList.nextToken();
  }

  if (toTokens.empty())
    return errorMsg("No tokens for FOR ... TO ...");

  //---

  // get step tokens (if specified)
  Tokens stepTokens;

  if (stepFound) {
    token = tokenList.nextToken();

    while (token) {
      stepTokens.push_back(token);

      token = tokenList.nextToken();
    }

    if (stepTokens.empty())
      return errorMsg("No tokens for FOR ... TO ... STEP ...");
  }

  //---

  compiledTokens.push_back(varToken);

  //---

  ExprData fromExprData;
  if (! tokensToExpr(fromTokens, fromExprData))
    return false;

  auto *fromExprToken = createExpr(fromExprData);

  compiledTokens.push_back(fromExprToken);

  //---

  ExprData toExprData;
  if (! tokensToExpr(toTokens, toExprData))
    return false;

  auto *toExprToken = createExpr(toExprData);

  compiledTokens.push_back(toExprToken);

  //---

  if (stepFound) {
    ExprData stepExprData;
    if (! tokensToExpr(stepTokens, stepExprData))
      return false;

    auto *stepExprToken = createExpr(stepExprData);

    compiledTokens.push_back(stepExprToken);
  }

  return true;
}

bool
CPetBasic::
forStatement(const LineRef &lineRef, const Tokens &tokens)
{
  // FOR <var> <from> <to> [<step>]
  auto nt = tokens.size();
  assert(nt == 4 || nt <= 5);

  // get loop variable
  auto *varToken = tokens[1];

  //---

  // get from to and step tokens
  assert(tokens[2]->type() == TokenType::EXPR);
  auto *fromExpr = static_cast<ExprToken *>(tokens[2]);

  assert(tokens[3]->type() == TokenType::EXPR);
  auto *toExpr = static_cast<ExprToken *>(tokens[3]);

  ExprToken *stepExpr = nullptr;

  if (nt == 5) {
    assert(tokens[4]->type() == TokenType::EXPR);
    stepExpr = static_cast<ExprToken *>(tokens[4]);
  }

  //---

  // get/create loop variable
  auto var = getVariable(varToken->str());

  //---

  // calc from, to and (optional) step values
  CExprValuePtr fromVal;
  if (! fromExpr->eval(fromVal))
    return false;

  CExprValuePtr toVal;
  if (! toExpr->eval(toVal))
    return false;

  CExprValuePtr stepVal;
  if (stepExpr) {
    if (! stepExpr->eval(stepVal))
      return false;
  }

  //---

  // get to and step as integers (TODO: suppport real ?)
  long toI;
  if (! toVal->getIntegerValue(toI))
    return errorMsg("No FOR ... TO value");

  if (stepExpr) {
    long stepI;
    if (! stepVal->getIntegerValue(stepI))
      return errorMsg("Invalid FOR ... TO ... STEP value");
  }

  //---

  // set loop variable to from value
  auto fromVal1 = CExprValuePtr(fromVal->dup());
  var->setValue(fromVal1);

  //---

  auto varName = CPetBasicUtil::toUpper(varToken->str());

  // find next statement
  LineRef nextLineRef;
  bool    nextFound { false };

  for (const auto &nextData : nextStack_) {
    if (nextData.line.lineNum < lineRef.lineNum ||
        (nextData.line.lineNum == lineRef.lineNum &&
         nextData.line.statementNum < lineRef.statementNum))
      continue;

    if (nextData.varName != "" && nextData.varName != varName)
      continue;

//  std::cerr << "NEXT found at " << nextData.varName << " @ " <<
//    nextData.line.lineNum << ":" << nextData.line.statementNum << "\n";

    nextLineRef = nextData.line;
    nextFound   = true;

    break;
  }

  if (! nextFound)
    return errorMsg("No NEXT for FOR");

  //---

  // create for data structure
  forDatas_.emplace_back(varName, toVal, stepVal, lineRef, nextLineRef);

  return true;
}

bool
CPetBasic::
getStatement(TokenList &tokenList)
{
  std::string prompt;

  auto *token = tokenList.nextToken();

  std::string varName;

  if      (token->type() == TokenType::VARIABLE)
    varName = token->toString();
  else if (token->type() != TokenType::SEPARATOR)
    return errorMsg("Invalid GET token '" + token->str() + "'");

  auto c = term_->readChar();

  std::string s;
  if (c) { s += c; }

  auto val = expr_->createStringValue(s);

  if (! setVariableValue(varName, val))
    return false;

  return true;
}

bool
CPetBasic::
compileGosubStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  auto *token = tokenList.nextToken();
  if (! token) return errorMsg("Missing value");

  if (token->type() != TokenType::NUMBER)
    return errorMsg("Not a number");

  auto *numberToken = static_cast<NumberToken *>(token);

  compiledTokens.push_back(numberToken);

  return true;
}

bool
CPetBasic::
gosubStatement(const Tokens &tokens)
{
  // GOSUB <lineNum>
  auto nt = tokens.size();
  assert(nt == 2);

  auto *token = tokens[1];
  assert(token->type() == TokenType::NUMBER);

  auto lineNum = uint(token->toInteger());
  //std::cout << "GOSUB " << lineNum << "\n";

  auto lineInd = getLineInd(int(lineNum));
  if (lineInd < 0) return errorMsg("Invalid GOSUB line");

  pushLine(LineRef(lineNum));

  return true;
}

bool
CPetBasic::
compileGotoStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  auto *token = tokenList.nextToken();
  if (! token) return errorMsg("Missing value");

  if (token->type() != TokenType::NUMBER)
    return errorMsg("Not a number");

  auto *numberToken = static_cast<NumberToken *>(token);

  compiledTokens.push_back(numberToken);

  return true;
}

bool
CPetBasic::
gotoStatement(const Tokens &tokens)
{
  // GOTO <line>
  auto nt = tokens.size();
  assert(nt == 2);

  auto *token = tokens[1];
  assert(token->type() == TokenType::NUMBER);

  auto lineNum = uint(token->toInteger());
  //std::cout << "GOTO " << lineNum << "\n";

  auto lineInd = getLineInd(lineNum);
  if (lineInd < 0) return errorMsg("Invalid GOTO line");

  gotoLine(LineRef(lineNum));

  return true;
}

bool
CPetBasic::
headerStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
compileIfStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  // get if expression tokens (up to THEN or GOTO)
  Tokens exprTokens;

  KeywordToken *thenToken = nullptr;
  KeywordToken *gotoToken = nullptr;

  auto *token = tokenList.nextToken();

  while (token) {
    if      (isKeyword(token, KeywordType::THEN)) {
      thenToken = static_cast<KeywordToken *>(token);
      break;
    }
    else if (isKeyword(token, KeywordType::GOTO)) {
      gotoToken = static_cast<KeywordToken *>(token);
      break;
    }

    exprTokens.push_back(token);

    token = tokenList.nextToken();
  }

  //---

  // compile if exprssion
  ExprData exprData;
  if (! tokensToExpr(exprTokens, exprData))
    return false;

  auto *exprToken = createExpr(exprData);

  compiledTokens.push_back(exprToken);

  //---

  if      (thenToken)
    compiledTokens.push_back(thenToken);
  else if (gotoToken)
    compiledTokens.push_back(gotoToken);
  else
    return errorMsg("No THEN or GOTO for IF");

  //---

  // get tokens after then/goto
  Tokens tokens;

  token = tokenList.nextToken();

  while (token) {
    tokens.push_back(token);

    token = tokenList.nextToken();
  }

  //---

  if (thenToken) {
    // if single number after token then use as goto line number
    if (tokens.size() == 1) {
      auto *token1 = tokens[0];

      if (token1->type() == TokenType::NUMBER) {
        auto *numberToken = static_cast<NumberToken *>(token1);

        compiledTokens.push_back(numberToken);

        return true;
      }
    }

    //---

    // compile tokens after if
    Tokens compiledTokens1;
    bool   hasCompiled1;

    if (! compileTokens(tokens, hasCompiled1, compiledTokens1))
      return false;

    for (auto *ctoken : compiledTokens1)
      compiledTokens.push_back(ctoken);
  }
  else {
    // must be single number after goto
    if (tokens.size() != 1)
      return errorMsg("Invalid IF <expr> GOTO line");

    auto *token1 = tokens[0];

    if (token1->type() != TokenType::NUMBER)
      return errorMsg("Invalid IF <expr> GOTO line");

    auto *numberToken = static_cast<NumberToken *>(token1);

    compiledTokens.push_back(numberToken);
  }

  return true;
}

bool
CPetBasic::
ifStatement(const LineRef &lineRef, const Tokens &tokens, bool &nextLine)
{
  // IF <expr> THEN|GOTO <line>|<tokens>
  auto nt = tokens.size();
  assert(nt > 2);

  // get if expression
  assert(tokens[1]->type() == TokenType::EXPR);
  auto *expr = static_cast<ExprToken *>(tokens[1]);

  bool thenFound = false;
  bool gotoFound = false;

  if      (isKeyword(tokens[2], KeywordType::THEN))
    thenFound = true;
  else if (isKeyword(tokens[2], KeywordType::GOTO))
    gotoFound = true;

   assert(thenFound || gotoFound);

  //---

  // evaluate if expression
  CExprValuePtr val;
  if (! expr->eval(val))
    return false;

  long ival;
  if (! val->getIntegerValue(ival))
    return errorMsg("Invalid IF expression");

  //---

  // if expression false then goto next line
  if (! ival) {
    nextLine = false;
    setLineInd(lineInd_ + 1, 0);

    return true;
  }

  //---

  if (thenFound) {
    // if single number after token then use as goto line number
    if (nt == 4 && tokens[3]->type() == TokenType::NUMBER) {
      uint lineNum = uint(tokens[3]->toInteger());

      auto lineInd = getLineInd(lineNum);
      if (lineInd < 0) return errorMsg("Invalid IF THEN line");

      setLineInd(lineInd, 0);
      nextLine = false;

      return true;
    }

    //---

    // run tokens after if (and remaining statements on line)
    Tokens compiledTokens;

    for (uint i = 3; i < nt; ++i)
      compiledTokens.push_back(tokens[i]);

    if (! runTokens(lineRef, compiledTokens, nextLine))
      return false;
  }
  else {
    // must be single number after goto
    assert(nt == 4 && tokens[3]->type() == TokenType::NUMBER);

    auto lineNum = uint(tokens[3]->toInteger());
    //std::cout << "GOTO " << lineNum << "\n";

    auto lineInd = getLineInd(lineNum);
    if (lineInd < 0) return errorMsg("Invalid IF <expr> GOTO line");

    setLineInd(lineInd, 0);
    nextLine = false;
  }

  return true;
}

bool
CPetBasic::
inputStatement(TokenList &tokenList)
{
  std::string prompt;

  auto *token = tokenList.nextToken();

  if (token && token->type() == TokenType::STRING) {
    prompt = token->toString();

    token = tokenList.nextToken();
  }

  using VarNames = std::vector<std::string>;

  VarNames varNames;

  while (token) {
    if      (token->type() == TokenType::VARIABLE)
      varNames.push_back(token->toString());
    else if (token->type() != TokenType::SEPARATOR)
      return errorMsg("Invalid INPUT token '" + token->str() + "'");

    token = tokenList.nextToken();
  }

  for (const auto &varName : varNames) {
    auto line = term_->readString(prompt);

    auto val = expr_->createStringValue(line);

    if (! setVariableValue(varName, val))
      return false;
  }

  return true;
}

bool
CPetBasic::
compileLetStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  // get variable name
  auto *varToken = tokenList.nextToken();

  if (! varToken || varToken->type() != TokenType::VARIABLE)
    return errorMsg("Invalid variable for LET");

  //---

  // get optional index tokens
  Tokens indExprs;

  auto *token = tokenList.nextToken();

  if (token && isSeparator(token, SeparatorType::OPEN_RBRACKET)) {
    auto *token1 = tokenList.nextToken();

    Tokens indexTokens;

    auto addInd = [&]() {
      if (indexTokens.empty())
        return errorMsg("No indices for array variable");

      ExprData exprData;
      if (! tokensToExpr(indexTokens, exprData))
        return false;

      auto *exprToken = createExpr(exprData);

      indExprs.push_back(exprToken);

      return true;
    };

    while (token1 && ! isSeparator(token1, SeparatorType::CLOSE_RBRACKET)) {
      if (isSeparator(token1, SeparatorType::COMMA)) {
        if (! addInd())
          return false;

        indexTokens.clear();
      }
      else {
        indexTokens.push_back(token1);
      }

      token1 = tokenList.nextToken();
    }

    if (! isSeparator(token1, SeparatorType::CLOSE_RBRACKET))
      return errorMsg("Missing close bracket for variable index");

    if (! addInd())
      return false;

    //--

    token = tokenList.nextToken();
  }

  auto *indexToken = createTokenList(indExprs);

  //---

  // ensure we have assign operator
  if (! token || ! isOperator(token, OperatorType::ASSIGN))
    return errorMsg("Missing equals for LET");

  //---

  // get assign tokens
  Tokens assignTokens;

  token = tokenList.nextToken();

  while (token) {
    assignTokens.push_back(token);

    token = tokenList.nextToken();
  }

  ExprData exprData;
  if (! tokensToExpr(assignTokens, exprData))
    return false;

  auto *exprToken = createExpr(exprData);

  //---

  compiledTokens.push_back(varToken);
  compiledTokens.push_back(indexToken);
  compiledTokens.push_back(exprToken);

  return true;
}

bool
CPetBasic::
letStatement(const Tokens &tokens)
{
  // LET <var> <inds> <expr>
  auto nt = tokens.size();
  assert(nt == 4);

  // get variable name
  auto *varToken = tokens[1];
  assert(varToken->type() == TokenType::VARIABLE);

  auto varName = varToken->str();

  //---

  // get indices if specified
  assert(tokens[2]->type() == TokenType::TOKEN_LIST);
  auto *indTokens = static_cast<TokenListToken *>(tokens[2]);

  Inds inds;

  if (! indTokens->tokens().empty()) {
    for (const auto *itoken : indTokens->tokens()) {
      assert(itoken->type() == TokenType::EXPR);
      const auto *exprIToken = static_cast<const ExprToken *>(itoken);

      CExprValuePtr val;
      if (! exprIToken->eval(val))
        return false;

      long i;
      if (! val->getIntegerValue(i) || i < 0)
        return errorMsg("Invalid variable index value");

      inds.push_back(uint(i));
    };
  }

  //---

  // calc value to assign
  assert(tokens[3]->type() == TokenType::EXPR);
  auto *exprToken = static_cast<ExprToken *>(tokens[3]);

  CExprValuePtr val;
  if (! exprToken->eval(val))
    return false;

  //---

  // set variable
  if (! inds.empty()) {
    if (! setVariableValue(varName, inds, val))
      return errorMsg("Failed to set variable value for '" + varName + "'");
  }
  else {
    if (! setVariableValue(varName, val))
      return errorMsg("Failed to set variable value for '" + varName + "'");
  }

  return true;
}

bool
CPetBasic::
listStatement(TokenList &tokenList)
{
  auto *token = tokenList.nextToken();

  long startNum    = -1;
  bool startNumSet = false;
  long endNum      = -1;
  bool endNumSet   = false;
  bool endRange    = false;

  if (token && token->type() != TokenType::NUMBER)
    return errorMsg("Invalid line number for LIST");

  if (token) {
    auto *numberToken = static_cast<NumberToken *>(token);

    startNum    = numberToken->toInteger();
    startNumSet = true;

    if (startNum < 0) {
      startNumSet = false;

      endNum    = -startNum;
      endNumSet = true;
    }

    token = tokenList.nextToken();

    if (! endNumSet) {
      if (token) {
        if (token->type() == TokenType::OPERATOR && token->str() == "-") {
          endRange = true;
          token    = nullptr;
        }
      }

      if (token && token->type() != TokenType::NUMBER)
        return errorMsg("Invalid line number for LIST");

      if (token) {
        auto *numberToken1 = static_cast<NumberToken *>(token);

        endNum    = numberToken1->toInteger();
        endNumSet = true;

        if (endNum < 0)
          endNum = -endNum;

        token = tokenList.nextToken();
      }
    }

    if (token)
      return errorMsg("Too many tokens for LIST");
  }

  // TODO: list from line
  if      (startNumSet && endNumSet)
    list(startNum, endNum);
  else if (startNumSet)
    list(startNum, (endRange ? -1 : startNum));
  else if (endNumSet)
    list(-1, endNum);
  else
    list(-1, -1);

  return true;
}

bool
CPetBasic::
loadStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
newStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra NEW tokens");

  initRunState();

  clearLines();

  setLineInd(-1, 0);

  expr_ = std::make_unique<CPetBasicExpr>(this);

  // clear data
  dataValues_.clear();

  dataValuePos_ = 0;

  runDataValid_ = false;

  notifyLinesChanged();

  return true;
}

void
CPetBasic::
clearLines()
{
  for (auto &pl : lines_) {
    auto &lineData = pl.second;

    for (auto *token : lineData.tokens)
      delete token;

    for (auto &statement : lineData.statements) {
      if (! statement.hasCompiled) continue;

      for (auto *ctoken : statement.compiledTokens) {
        if (ctoken->type() == TokenType::KEYWORD ||
            ctoken->type() == TokenType::VARIABLE ||
            ctoken->type() == TokenType::NUMBER)
          continue;

        delete ctoken;
      }
    }
  }

  lines_.clear();
}

bool
CPetBasic::
compileNextStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  // get var name
  std::string varName = "";

  auto *varToken = tokenList.nextToken();

  if (varToken && varToken->type() != TokenType::VARIABLE)
    return errorMsg("Invalid NEXT variable ");

  if (varToken)
    compiledTokens.push_back(varToken);

  return true;
}

bool
CPetBasic::
nextStatement(const LineRef &lineRef, const Tokens &tokens)
{
  auto nt = tokens.size();
  assert(nt == 1 || nt == 2);

  // get var name
  auto *varToken = (nt > 1 ? tokens[1] : nullptr);

  std::string varName;

  if (varToken)
    varName = CPetBasicUtil::toUpper(varToken->str());

  //---

  // get for data
  int forInd = -1;

  for (uint i = 0; i < forDatas_.size(); ++i) {
    if (forDatas_[i].nextLineNum() == int(lineRef.lineNum)) {
      forInd = int(i);
      break;
    }
  }

  if (forInd < 0)
    return errorMsg("No matching FOR");

  const auto &forData = forDatas_[forInd];

  //---

  // get loop variable and current value
  if (varName == "")
    varName = forData.varName();

  auto var = getVariable(varName);
  auto val = var->value();

  long fromI;

  if (! val->getIntegerValue(fromI))
    return errorMsg("Invalid loop variable");

  long stepI = 1;
  if (forData.stepVal()) {
    if (! forData.stepVal()->getIntegerValue(stepI))
      return errorMsg("Invalid FOR STEP value");
  }

  if (stepI == 0)
    return errorMsg("Invalid FOR STEP value");

  fromI += stepI;

  val->setIntegerValue(fromI);

  var->setValue(val);

  long toI;
  if (! forData.toVal()->getIntegerValue(toI))
    return errorMsg("Invalid FOR TO value");

  if ((stepI > 0 && fromI <= toI) || (stepI < 0 && fromI >= toI)) { // not at end
    auto lineInd = getLineInd(forData.lineNum());
    if (lineInd < 0) return errorMsg("Invalid GOTO line");

    auto pl = lines_.find(forData.lineNum());
    assert(pl != lines_.end());

    auto statementNum = forData.statementNum() + 1;

    if (statementNum >= int((*pl).second.statements.size())) {
      ++lineInd;

      statementNum = 0;
    }

    setLineInd(lineInd, statementNum);
  }
  else { // at end
    removeForData(forInd);

    setLineInd(lineInd_ + 1, 0);
  }

  return true;
}

bool
CPetBasic::
onStatement(TokenList &tokenList)
{
  Tokens tokens;

  auto *token = tokenList.nextToken();

  bool gotoFlag  = false;
  bool gosubFlag = false;

  while (token) {
    if      (isKeyword(token, KeywordType::GOTO)) {
      gotoFlag = true;
      break;
    }
    else if (isKeyword(token, KeywordType::GOSUB)) {
      gosubFlag = true;
      break;
    }
    else
      tokens.push_back(token);

    token = tokenList.nextToken();
  }

  if (! gotoFlag && ! gosubFlag)
    return errorMsg("Missing GOTO/GOSUB");

  CExprValuePtr value;
  if (! evalExpr(tokens, value))
    return false;

  long i;
  if (! value->getIntegerValue(i))
    return errorMsg("Invalid ON expression");

  std::vector<long> lines;

  token = tokenList.nextToken();

  while (token) {
    if      (token->type() == TokenType::NUMBER) {
      long l = token->toInteger();
      lines.push_back(l);
    }
    else if (token->type() != TokenType::SEPARATOR)
      return errorMsg("Invalid ON token '" + token->str() + "'");

    token = tokenList.nextToken();
  }

  if (i < 1 || uint(i) > lines.size())
    return errorMsg("Invalid line index");

  auto lineNum = uint(lines[i - 1]);

  auto lineInd = getLineInd(lineNum);
  if (lineInd < 0)
    return errorMsg("Invalid ON line '" + std::to_string(lineNum) + "'");

  if (gosubFlag)
    pushLine(LineRef(lineNum));
  else
    gotoLine(LineRef(lineNum));

  return true;
}

bool
CPetBasic::
openStatement(const Tokens &)
{
  assert(false);
  return false;
}

#ifdef PET_EXTRA_KEYWORDS
bool
CPetBasic::
compilePlotStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  using TokensArray = std::vector<Tokens>;

  TokensArray tokensArray;

  while (true) {
    Tokens tokens;

    auto *token = tokenList.nextToken();

    // get first expression
    int brackets = 0;

    while (token) {
      if (brackets <= 0) {
        if (isSeparator(token, SeparatorType::COMMA))
          break;
      }

      if      (isSeparator(token, SeparatorType::OPEN_RBRACKET)) {
        ++brackets;
      }
      else if (isSeparator(token, SeparatorType::CLOSE_RBRACKET)) {
        --brackets;
      }

      tokens.push_back(token);

      token = tokenList.nextToken();
    }

    tokensArray.push_back(tokens);

    if (! token)
      break;
  }

  if (tokensArray.size() != 3)
    return errorMsg("Invalid number of values for PLOT");

  for (const auto &tokens : tokensArray) {
    ExprData exprData;
    if (! tokensToExpr(tokens, exprData))
      return false;

    auto *exprToken = createExpr(exprData);

    compiledTokens.push_back(exprToken);
  }

  return true;
}
#endif

#ifdef PET_EXTRA_KEYWORDS
bool
CPetBasic::
plotStatement(const Tokens &tokens)
{
  // PLOT <x> <y> <color>
  auto nt = tokens.size();
  assert(nt == 4);

  using IValues = std::vector<long>;

  IValues ivalues;

  for (int i = 1; i < 4; ++i) {
    auto *token = tokens[i];

    assert(token->type() == TokenType::EXPR);
    const auto *exprToken = static_cast<const ExprToken *>(token);

    CExprValuePtr value;
    if (! exprToken->eval(value))
      return false;

    long ivalue;

    if (! value->getIntegerValue(ivalue))
      return errorMsg("Invalid PLOT argument");

    ivalues.push_back(ivalue);
  }

  term_->drawPoint(ivalues[0], ivalues[1], ivalues[2]);

  return true;
}
#endif

bool
CPetBasic::
compilePokeStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  Tokens tokens1;

  auto *token = tokenList.nextToken();

  // get first expression
  int brackets = 0;

  while (token) {
    if (brackets <= 0) {
      if (isSeparator(token, SeparatorType::COMMA))
        break;
    }

    if      (isSeparator(token, SeparatorType::OPEN_RBRACKET)) {
      ++brackets;
    }
    else if (isSeparator(token, SeparatorType::CLOSE_RBRACKET)) {
      --brackets;
    }

    tokens1.push_back(token);

    token = tokenList.nextToken();
  }

  if (! token || ! isSeparator(token, SeparatorType::COMMA))
    return errorMsg("Missing POKE comma expression separator");

  //---

  // get second expression
  Tokens tokens2;

  token = tokenList.nextToken();

  while (token) {
    tokens2.push_back(token);

    token = tokenList.nextToken();
  }

  //---

  ExprData addrExprData;
  if (! tokensToExpr(tokens1, addrExprData))
    return false;

  auto *addrExprToken = createExpr(addrExprData);

  compiledTokens.push_back(addrExprToken);

  ExprData valueExprData;
  if (! tokensToExpr(tokens2, valueExprData))
    return false;

  auto *valueExprToken = createExpr(valueExprData);

  compiledTokens.push_back(valueExprToken);

  return true;
}

bool
CPetBasic::
pokeStatement(const Tokens &tokens)
{
  // POKE <addr> <value>
  auto nt = tokens.size();
  assert(nt == 3);

  //---

  assert(tokens[1]->type() == TokenType::EXPR);
  const auto *addrToken = static_cast<const ExprToken *>(tokens[1]);

  CExprValuePtr addrValue;
  if (! addrToken->eval(addrValue))
    return false;

  assert(tokens[2]->type() == TokenType::EXPR);
  const auto *valueToken = static_cast<const ExprToken *>(tokens[2]);

  CExprValuePtr valueValue;
  if (! valueToken->eval(valueValue))
    return false;

  long addr, value;
  if (! addrValue->getIntegerValue(addr) || ! valueValue->getIntegerValue(value))
    return errorMsg("Invalid POKE arguments");

  if (addr < 0 || addr >= 65536)
    return errorMsg("Invalid POKE address");

  auto value1 = uint(value) & 0xFF;

  setMemory(uint(addr) & 0xFFFF, uchar(value1));

  return true;
}

bool
CPetBasic::
compilePrintStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  // , next tab stop (10)
  // ; next pos

  while (true) {
    Tokens tokens1;

    auto *token = tokenList.nextToken();
    if (! token) break;

    int brackets = 0;

    SeparatorToken *sep = nullptr;

    auto lastType = TokenType::NONE;

    while (token) {
      auto type = token->type();

      if (brackets <= 0) {
        if      (isSeparator(token, SeparatorType::COMMA)) {
          sep = static_cast<SeparatorToken *>(token);
          break;
        }
        else if (isSeparator(token, SeparatorType::SEMI_COLON)) {
          sep = static_cast<SeparatorToken *>(token);
          break;
        }

        bool endExpression = false;

        if      (lastType == TokenType::STRING && type != TokenType::OPERATOR)
          endExpression = true;
        else if (lastType != TokenType::NONE && lastType != TokenType::OPERATOR &&
                 type == TokenType::STRING)
          endExpression = true;
        else if (lastType == TokenType::SEPARATOR && type == TokenType::VARIABLE)
          endExpression = true;

        if (endExpression) {
          tokenList.prevToken();
          break;
        }
      }

      if      (isSeparator(token, SeparatorType::OPEN_RBRACKET)) {
        ++brackets;
      }
      else if (isSeparator(token, SeparatorType::CLOSE_RBRACKET)) {
        --brackets;
      }

      tokens1.push_back(token);

      lastType = type;

      token = tokenList.nextToken();
    }

    //---

    ExprData exprData;
    if (! tokensToExpr(tokens1, exprData))
      return false;

    auto *exprToken = createExpr(exprData);

    compiledTokens.push_back(exprToken);

    if (sep != nullptr)
      compiledTokens.push_back(sep);
  }

  return true;
}

bool
CPetBasic::
printStatement(const Tokens &tokens)
{
  // PRINT <expr> [;,] ...

  auto nt = tokens.size();

  // , next tab stop (10)
  // ; next pos

  if (tokens.empty())
    return true;

  for (uint i = 1; i < nt; ++i) {
    auto *token = tokens[i];

    if      (token->type() == TokenType::EXPR) {
      const auto *expr = static_cast<const ExprToken *>(token);

      CExprValuePtr value;
      if (! expr->eval(value))
        return false;

      if (term_->col() == 0) {
        if (value->isIntegerValue() || value->isRealValue()) {
          double r;

          if (value->getRealValue(r) && r >= 0)
            printString(" ");
        }
      }

      std::string s;
      (void) value->getStringValue(s);

      printString(s);
    }
    else if (token->type() == TokenType::SEPARATOR) {
      if      (isSeparator(token, SeparatorType::COMMA)) {
        printString("\t");
      }
      else if (isSeparator(token, SeparatorType::SEMI_COLON)) {
      }
      else
        assert(false);
    }
    else {
      assert(false);
    }
  }

  auto *lastToken = tokens.back();

  if (lastToken && isSeparator(lastToken, SeparatorType::SEMI_COLON)) {
    if (term_->col() >= int(term_->numCols()))
      term_->enter();
  }
  else
    term_->enter();

  return true;
}

bool
CPetBasic::
compileReadStatement(TokenList &tokenList, Tokens &compiledTokens)
{
  auto *token = tokenList.currentToken();

  while (token) {
    Tokens tokens1;
    if (! compileVariable(tokenList, "READ", tokens1))
      return false;

    auto *varTokenList = createTokenList(tokens1);

    compiledTokens.push_back(varTokenList);

    //---

    token = tokenList.currentToken();

    if (token && ! isSeparator(token, SeparatorType::COMMA))
      return errorMsg("Missing command for READ");

    if (token)
      token = tokenList.nextToken();
  }

  return true;
}

bool
CPetBasic::
readStatement(const Tokens &tokens)
{
  // READ ( <var> <inds> , ...)
  auto nt = tokens.size();
  assert(nt >= 1);

  for (uint i = 1; i < nt; ++i) {
    auto *token = tokens[i];

    assert(token->type() == TokenType::TOKEN_LIST);
    auto *tokenList = static_cast<TokenListToken *>(token);

    // <var> <dims>
    const auto &tokens1 = tokenList->tokens();

    auto nt1 = tokens1.size();
    assert(nt1 == 1 || nt1 == 2);

    auto *varToken = tokens1[0];
    auto varName = varToken->str();

    Inds inds;

    if (nt1 == 2) {
      auto *indsToken = tokens1[1];
      assert(indsToken->type() == TokenType::TOKEN_LIST);
      auto *indsTokenList = static_cast<TokenListToken *>(indsToken);

      for (auto *itoken : indsTokenList->tokens()) {
        assert(itoken->type() == TokenType::EXPR);
        const auto *exprIToken = static_cast<const ExprToken *>(itoken);

        CExprValuePtr val;
        if (! exprIToken->eval(val))
          return false;

        long ival;
        if (! val->getIntegerValue(ival) || ival < 0)
          return errorMsg("Invalid variable index value");

        inds.push_back(uint(ival));
      }
    }

    //---

    // store next data in variable
    if (dataValuePos_ >= dataValues_.size())
      return errorMsg("No data for READ");

    auto val = dataValues_[dataValuePos_++];

    if (! inds.empty()) {
      if (! setVariableValue(varName, inds, val))
        return errorMsg("Failed to set array variable '" + varName + "'");
    }
    else {
      if (! setVariableValue(varName, val))
        return errorMsg("Failed to set variable '" + varName + "'");
    }
  }

  return true;
}

bool
CPetBasic::
recordStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
renameStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
restoreStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra RESTORE tokens");

  dataValuePos_ = 0;

  return true;
}

bool
CPetBasic::
returnStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra RETURN tokens");

  if (! popLine())
    return errorMsg("RETURN: Empty line stack");

  return true;
}

bool
CPetBasic::
runStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra RUN tokens");

  run();

  return true;
}

bool
CPetBasic::
saveStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
scratchStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
stopStatement(const Tokens &tokens)
{
  if (tokens.size() != 1)
    return errorMsg("Extra STOP tokens");

  setStopped(true);

  return true;
}

bool
CPetBasic::
sysStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
verifyStatement(const Tokens &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
waitStatement(const Tokens &)
{
  assert(false);
  return false;
}

//---

// read variable name and optional indices from token list
// expect current token to be variable. Moves to token after close bracket
bool
CPetBasic::
compileVariable(TokenList &tokenList, const std::string &id, Tokens &tokens) const
{
  auto *token = tokenList.currentToken();

  if (token->type() != TokenType::VARIABLE)
    return errorMsg("Missing Variable token for " + id + " '" + token->str() + "'");

  auto *varToken = tokenList.nextToken();

  tokens.push_back(varToken);

  //---

  // just variable
  token = tokenList.currentToken();

  if (! token || ! isSeparator(token, SeparatorType::OPEN_RBRACKET))
    return true;

  token = tokenList.nextToken();

  //---

  // parse indices
  Tokens indexTokens;
  Tokens indExprs;

  auto addInd = [&]() {
    if (indexTokens.empty())
      return errorMsg("Invalid variable index for " + id);

    ExprData exprData;
    if (! tokensToExpr(indexTokens, exprData))
      return false;

    auto *exprToken = createExpr(exprData);

    indExprs.push_back(exprToken);

    return true;
  };

  token = tokenList.currentToken();

  while (token && ! isSeparator(token, SeparatorType::CLOSE_RBRACKET)) {
    if (isSeparator(token, SeparatorType::COMMA)) {
      if (! addInd())
        return false;

      indexTokens.clear();
    }
    else {
      indexTokens.push_back(token);
    }

    token = tokenList.nextToken();
    token = tokenList.currentToken();
  }

  if (! isSeparator(token, SeparatorType::CLOSE_RBRACKET))
    return errorMsg("Invalid variable index for " + id);

  token = tokenList.nextToken();

  if (! addInd())
    return false;

  auto *indexToken = createTokenList(indExprs);

  tokens.push_back(indexToken);

  return true;
}

#if 0
// read variable name and optional indices from token list
// expect current token to be variable. Moves to token after close bracket
bool
CPetBasic::
readVariable(TokenList &tokenList, const std::string &id,
             std::string &varName, Inds &inds) const
{
  auto *token = tokenList.currentToken();

  if (token->type() != TokenType::VARIABLE)
    return errorMsg("Missing Variable token for " + id + " '" + token->str() + "'");

  token = tokenList.nextToken();

  varName = token->str();

  //---

  // just variable
  token = tokenList.currentToken();

  if (! token || ! isSeparator(token, SeparatorType::OPEN_RBRACKET))
    return true;

  token = tokenList.nextToken();

  //---

  // parse indices
  Tokens indexTokens;

  auto addInd = [&]() {
    if (indexTokens.empty())
      return errorMsg("Invalid variable index for " + id);

    CExprValuePtr val1;
    if (! evalExpr(indexTokens, val1))
      return false;

    long i;
    if (! val1->getIntegerValue(i) || i < 0)
      return errorMsg("Invalid variable index value for " + id);

    inds.push_back(uint(i));

    return true;
  };

  token = tokenList.currentToken();

  while (token && ! isSeparator(token, SeparatorType::CLOSE_RBRACKET)) {
    if (isSeparator(token, SeparatorType::COMMA)) {
      if (! addInd()) return false;

      indexTokens.clear();
    }
    else {
      indexTokens.push_back(token);
    }

    token = tokenList.nextToken();
    token = tokenList.currentToken();
  }

  if (! isSeparator(token, SeparatorType::CLOSE_RBRACKET))
    return errorMsg("Invalid variable index for " + id);

  token = tokenList.nextToken();

  if (! addInd())
    return false;

  return true;
}
#endif

#if 0
void
CPetBasic::
printVarDetails(const std::string &cmd, const std::string &varName, const Inds &inds) const
{
  std::cerr << cmd << " " << varName;

  if (! inds.empty()) {
    std::cerr << "(";

    printInds(inds);

    std::cerr << ")";
  }

  std::cerr << "\n";
}
#endif

void
CPetBasic::
printInds(const Inds &inds) const
{
  bool output = false;

  for (const auto &ind : inds) {
    if (output) std::cerr << ",";

    std::cerr << ind;

    output = true;
  }
}

//---

void
CPetBasic::
gotoLine(const LineRef &lineRef)
{
  uint i = 0;

  while (i < forDatas_.size()) {
    const auto &forData = forDatas_[i];

    const auto &startLineRef = forData.lineRef();
    const auto &endLineRef   = forData.nextLineRef();

    if (lineRef < startLineRef || lineRef > endLineRef) {
      //std::cerr << "For data invalidate : GOTO " << lineRef.toString() <<
      //            " FOR " << startLineRef.toString() << " TO " << endLineRef.toString() << "\n";

      removeForData(i);

      i = 0;
    }
    else
      ++i;
  }

  //---

  auto lineInd = getLineInd(lineRef.lineNum);

  auto pl = lines_.find(lineRef.lineNum);
  assert(pl != lines_.end());

  auto statementNum = lineRef.statementNum;

  if (lineRef.statementNum >= (*pl).second.statements.size()) {
    ++lineInd;

    statementNum = 0;
  }

  setLineInd(lineInd, statementNum);
}

void
CPetBasic::
pushLine(const LineRef &lineRef)
{
  int retLineNum = lineIndNum(lineInd_);

//std::cerr << "GOSUB " << lineRef.lineNum << ":0" <<
//             " FROM " << retLineNum << ":" << statementNum_ << "\n";

  auto retLineRef = LineRef(retLineNum, statementNum_ + 1);

  lineStack_.push_back(retLineRef);

  gotoLine(lineRef);
}

bool
CPetBasic::
popLine()
{
  if (lineStack_.empty())
    return false;

  auto lineRef = lineStack_.back();

  lineStack_.pop_back();

//std::cerr << "RETURN TO " << lineIndNum(lineRef.lineNum) << ":" << lineRef.statementNum <<
//             " FROM " << lineIndNum(lineInd_) << ":" << statementNum_ << "\n";

  gotoLine(lineRef);

  return true;
}

void
CPetBasic::
removeForData(uint ind)
{
  assert(ind < forDatas_.size());

  for (uint i = ind + 1; i < forDatas_.size(); ++i)
    forDatas_[i - 1] = forDatas_[i];

  forDatas_.pop_back();
}

//---

void
CPetBasic::
initRunState()
{
  // init run data
  lineStack_.clear();

  forDatas_.clear();

  setStopped(false);

  errorMsg_ = "";

  setLineInd(-1, 0);
}

void
CPetBasic::
setLineInd(int lineInd, int statementNum)
{
  if (lineInd != lineInd_ || statementNum != int(statementNum_)) {
    lineInd_      = lineInd;
    statementNum_ = statementNum;

    notifyLineNumChanged();
  }
}

//---

void
CPetBasic::
printString(const std::string &str) const
{
#if 1
  auto *th = const_cast<CPetBasic *>(this);

  // ASCII with embedded escape codes for special chars
  uint i   = 0;
  auto len = str.length();

  //---

  while (i < len) {
    auto c  = str[i];
    auto c1 = uchar(c);

    // handle new line
    if      (c == '\n') {
      term_->enter();
    }

    // handle tab
    else if (c == '\t') {
      term_->tab();
    }

    else {
      // handle embedded escape codes
      bool handled = true;

      switch (c1) {
        case  19: term_->home(); break;
        case 147: term_->clear(); break;
        case 255: th->setStopped(true); break;
        case  18: th->setReverse(true); break;
        case 146: th->setReverse(false); break;
        case 145: term_->cursorUp(); break;
        case  17: term_->cursorDown(); break;
        case 157: term_->cursorLeft(); break;
        case  29: term_->cursorRight(); break;
        default: handled = false; break;
      }

      if (! handled && c1 >= 128) {
        c1 -= 128;

        CPetDrawChar drawChar(c1);

        auto pet = CPetBasic::drawCharToPet(drawChar);

        pet.shift();

        drawChar = CPetBasic::petToDrawChar(pet);

        drawChar.setReverse(isReverse());

        if (term_->col() >= int(term_->numCols()) - 1)
          term_->enter();

        if (term_->drawChar(drawChar))
          term_->cursorRight();

        handled = true;
      }

      // handle normal char
      if (! handled) {
        // map to upper case
        if (islower(c))
          c = char(toupper(c));

        if (term_->drawChar(c))
          term_->cursorRight();
      }
    }

    ++i;
  }

  term_->update();
#else
  for (const auto &c : s) {
    std::cout << c;
  }
#endif
}

//---

void
CPetBasic::
setTerm(CPetBasicTerm *term)
{
  delete term_;

  term_ = term;
}

//---

CPetBasicExpr *
CPetBasic::
expr() const
{
  return expr_.get();
}

bool
CPetBasic::
evalExpr(const Tokens &tokens, CExprValuePtr &value) const
{
  ExprData exprData;

  if (! tokensToExpr(tokens, exprData))
    return false;

  if (! evalExprData(exprData, value))
    return false;

  return true;
}

bool
CPetBasic::
tokensToExpr(const Tokens &tokens, ExprData &exprData) const
{
  bool embedded = false;

  auto nt = tokens.size();

  bool simple = (nt == 1);

  std::string varName;

  for (uint i = 0; i < nt; ++i) {
    if (exprData.str != "")
      exprData.str += " ";

    auto *token = tokens[i];

    if      (token->type() == TokenType::STRING) {
      auto *strToken = static_cast<StringToken *>(token);

      exprData.str += "\"";

      const auto &str1 = token->toString();

      if (strToken->isEmbedded()) {
        for (const auto &c : str1) {
          if (c == '\"')
            exprData.str += "\\\"";
          else
            exprData.str += c;
        }

        embedded = true;
      }
      else {
        exprData.str += str1;
      }

      exprData.str += "\"";
    }
    else if (token->type() == TokenType::NUMBER)
      exprData.str += token->toString();
    else if (token->type() == TokenType::VARIABLE) {
      varName = token->toString();

      exprData.str += varName;
    }
    else if (token->type() == TokenType::KEYWORD) {
      if (isKeyword(token, KeywordType::FN)) {
        ++i;

        if (i >= nt)
          return errorMsg("Missing name for FN");

        auto *token1 = tokens[i];

        if (token1->type() != TokenType::VARIABLE)
          return errorMsg("Missing name for FN");

        auto fnName = token1->toString();

        FunctionData functionData;

        if (! getFunction(fnName, functionData))
          return errorMsg("Invalid function '" + fnName + "'");

        ++i;

        if (i >= nt)
          return errorMsg("Missing args for FN");

        token1 = tokens[i];

        if (! isSeparator(token1, SeparatorType::OPEN_RBRACKET))
          return errorMsg("Missing args for FN");

        bool closed = false;

        Tokens                     argTokens;
        std::vector<CExprValuePtr> argValues;

        auto flushArgTokens = [&]() {
          if (argTokens.empty())
            return errorMsg("No values for index");

          CExprValuePtr val;
          if (! evalExpr(argTokens, val))
            return false;

          argValues.push_back(val);

          return true;
        };

        ++i;

        while (i < nt) {
          token1 = tokens[i];

          if (isSeparator(token1, SeparatorType::CLOSE_RBRACKET)) {
            closed = true;
            break;
          }
          if (isSeparator(token1, SeparatorType::COMMA)) {
            if (! flushArgTokens())
              return false;

            argTokens.clear();
          }
          else {
            argTokens.push_back(token1);
          }

          ++i;
        }

        if (! closed)
          return errorMsg("Missing args for FN");

        if (! flushArgTokens())
          return false;

        if (argValues.size() != functionData.args.size())
          return errorMsg("FN arg mismatch");

        exprData.str += "(";

        for (auto *ftoken :  functionData.tokens) {
          if (ftoken->type() == TokenType::VARIABLE) {
            auto fVarName = CPetBasicUtil::toUpper(ftoken->toString());

            bool found = false;

            for (uint find = 0; find < functionData.args.size(); ++find) {
              if (functionData.args[find] == fVarName) {
                exprData.str += argTokens[find]->exprString();
                found = true;
                break;
              }
            }

            if (! found)
              exprData.str += ftoken->exprString();
          }
          else
            exprData.str += ftoken->exprString();
        }

        exprData.str += ")";
      }
      else
        exprData.str += token->exprString();

      simple = false;
    }
    else if (token->type() == TokenType::OPERATOR) {
      exprData.str += token->exprString();
      simple = false;
    }
    else if (token->type() == TokenType::SEPARATOR) {
      exprData.str += token->exprString();
      simple = false;
    }
    else {
      std::cerr << "Invalid expr token '" << token->exprString() << "'\n";
      exprData.str += token->exprString();
      simple = false;
    }
  }

  if (embedded && nt > 1)
    warnMsg("Expression with embedded '" + exprData.str + "'");

  if (simple) {
    if (varName != "")
      exprData.varName = varName;
    else {
      //std::cerr << "Simple: " << exprData.str << "\n";

      if (! evalExpr(exprData.str, exprData.value))
        return errorMsg("Invalid expression '" + exprData.str + "'");
    }
  }
  else {
    auto pstack = expr_->parseLine(exprData.str);
    auto itoken = expr_->interpPTokenStack(pstack);

    exprData.cstack = new CExprTokenStack;

    *exprData.cstack = expr_->compileIToken(itoken);

    //std::cerr << "Complex: " << exprData.str << "\n";
  }

  return true;
}

bool
CPetBasic::
evalExpr(const std::string &str, CExprValuePtr &value) const
{
  if (! expr_->evaluateExpression(str, value)) {
    errorMsg("Invalid expr: '" + str + "'");
    value = expr_->createIntegerValue(0);
    return false;
  }

  if (! value) {
    errorMsg("Invalid expr: '" + str + "'");
    value = expr_->createIntegerValue(0);
    return false;
  }

  return true;
}

bool
CPetBasic::
evalExprData(const ExprData &exprData, CExprValuePtr &val) const
{
  if (exprData.value) {
    val = exprData.value;
  }
  else if (exprData.varName != "") {
    val = getVariableValue(exprData.varName);
  }
  else if (exprData.cstack) {
    CExprValueArray values;

    if (! expr_->executeCTokenStack(*exprData.cstack, values) || values.empty())
      return false;

    val = values.back();
  }
  else {
    if (! evalExpr(exprData.str, val))
      return false;
  }

#if 0
  std::cerr << "eval: ";
  print(std::cerr);
  std::cerr << " = ";
  std::string s;
  if (! val->getStringValue(s)) s = "<null>";
  std::cerr << s << "\n";
#endif

  return true;
}

//---

CExprVariablePtr
CPetBasic::
addVariable(const std::string &name, const CExprValuePtr &value)
{
  auto var = expr_->createVariable(name, value);

  variableNames_.insert(name);

  notifyVariablesChanged();

  return var;
}

CExprVariablePtr
CPetBasic::
getVariable(const std::string &name) const
{
  auto uname = CPetBasicUtil::toUpper(name);

  auto var = expr_->getVariable(uname);

  if (! var) {
    auto valueType = expr_->nameType(uname);

    CExprValuePtr val;

    if      (valueType == CExprValueType::INTEGER)
      val = expr_->createIntegerValue(0);
    else if (valueType == CExprValueType::STRING)
      val = expr_->createStringValue("");
    else
      val = expr_->createRealValue(0.0);

    auto *th = const_cast<CPetBasic *>(this);

    var = th->addVariable(uname, val);
  }

  return var;
}

CExprValuePtr
CPetBasic::
getVariableValue(const std::string &name) const
{
  return getVariable(name)->value();
}

bool
CPetBasic::
setVariableValue(const std::string &name, const CExprValuePtr &value)
{
  auto uname = CPetBasicUtil::toUpper(name);

  auto valueType = expr_->nameType(uname);

  auto value1 = value;

  if      (valueType == CExprValueType::INTEGER) {
    if (! value1->isIntegerValue()) {
      long i;
      if (! value1->getIntegerValue(i))
        return errorMsg("Invalid value type");
      value1 = expr_->createIntegerValue(i);
    }
  }
  else if (valueType == CExprValueType::STRING) {
    if (! value1->isStringValue()) {
      std::string s;
      if (! value1->getStringValue(s))
        return errorMsg("Invalid value type");
      value1 = expr_->createStringValue(s);
    }
  }
  else {
    value1 = CExprValuePtr(value1->dup());
  }

  auto var = getVariable(uname);

  if (! var) {
    var = addVariable(uname, value1);
  }
  else {
    var->setValue(value1);

    notifyVariablesChanged();
  }

  return true;
}

//---

void
CPetBasic::
dimVariable(const std::string &name, const Inds &inds)
{
  auto uname = CPetBasicUtil::toUpper(name);

  //std::cerr << "dimVariable: " << uname << " "; printInds(inds); std::cerr << "\n";

  auto *th = const_cast<CPetBasic *>(this);

  if (! hasArrayVariable(uname))
    th->addArrayVariable(uname, int(inds.size()));

  auto pv = th->arrayVariables_.find(uname);
  assert(pv != th->arrayVariables_.end());

  return (*pv).second.resize(inds);
}

bool
CPetBasic::
hasArrayVariable(const std::string &uname) const
{
  assert(uname == CPetBasicUtil::toUpper(uname));

  auto pv = arrayVariables_.find(uname);

  return (pv != arrayVariables_.end());
}

void
CPetBasic::
addArrayVariable(const std::string &uname, int ndim)
{
  assert(uname == CPetBasicUtil::toUpper(uname));

  assert(ndim >= 1);

  ArrayData arrayData;

  ArrayData::Dims dims;

  for (int i = 0; i < ndim; ++i)
    dims.push_back(10);

  arrayData.resize(dims);

  arrayVariables_[uname] = arrayData;

  notifyVariablesChanged();
}

CExprValuePtr
CPetBasic::
getVariableValue(const std::string &name, const Inds &inds)
{
  auto uname = CPetBasicUtil::toUpper(name);

  //std::cerr << "getVariableValue: " << uname << " "; printInds(inds); std::cerr << "\n";

  auto *th = const_cast<CPetBasic *>(this);

  if (! hasArrayVariable(uname))
    th->addArrayVariable(uname, int(inds.size()));

  auto pv = th->arrayVariables_.find(uname);
  assert(pv != th->arrayVariables_.end());

//std::cerr << uname << " " << inds.size() << "\n";

  return (*pv).second.value(inds);
}

bool
CPetBasic::
setVariableValue(const std::string &name, const Inds &inds, const CExprValuePtr &value)
{
  auto uname = CPetBasicUtil::toUpper(name);

  //std::cerr << "setVariableValue: " << uname << " "; printInds(inds);
  //std::cerr << " "; value->print(std::cerr); std::cerr << "\n";

  auto *th = const_cast<CPetBasic *>(this);

  if (! hasArrayVariable(uname))
    th->addArrayVariable(uname, int(inds.size()));

  auto pv = th->arrayVariables_.find(uname);
  assert(pv != th->arrayVariables_.end());

  auto value1 = CExprValuePtr(value->dup());

  bool rc = (*pv).second.setValue(inds, value1);

  notifyVariablesChanged();

  return rc;
}

void
CPetBasic::
clearArrayVariables()
{
  arrayVariables_.clear();
}

void
CPetBasic::
getVariableNames(std::vector<std::string> &names, std::vector<std::string> &arrayNames) const
{
  for (const auto &name : variableNames_)
    names.push_back(name);

  for (const auto &pa : arrayVariables_)
    arrayNames.push_back(pa.first);
}

//---

void
CPetBasic::
defineFunction(const std::string &fnName, const std::vector<std::string> &args,
               const Tokens &tokens)
{
  FunctionData data;

  data.name = CPetBasicUtil::toUpper(fnName);

  for (const auto &arg: args)
    data.args.push_back(CPetBasicUtil::toUpper(arg));

  data.tokens = tokens;

  functions_[data.name] = data;
}

bool
CPetBasic::
getFunction(const std::string &fnName, FunctionData &data) const
{
  auto fnName1 = CPetBasicUtil::toUpper(fnName);

  auto pf = functions_.find(fnName1);
  if (pf == functions_.end()) return false;

  data = (*pf).second;

  return true;
}

//---

int
CPetBasic::
getLineInd(uint lineNum) const
{
  auto pl = lineInds_.find(lineNum);

  if (pl == lineInds_.end())
    return -1;

  return (*pl).second;
}

//---

void
CPetBasic::
addDataValue(const CExprValuePtr &value)
{
  dataValues_.push_back(value);
}

//---

void
CPetBasic::
warnMsg(const std::string &msg) const
{
  std::cerr << msg << "\n";
}

bool
CPetBasic::
errorMsg(const std::string &msg) const
{
  errorMsg_ = msg;
  return false;
}

//---

CPetsciChar
CPetBasic::
drawCharToPet(const CPetDrawChar &drawChar)
{
  CPetsciChar pet;

  if (drawChar.utf() > 0) {
    auto utfToPet = [](ulong utf1) -> uchar {
      switch (utf1) {
        case 0x1fb79: return 64 ; // horizontal mid line
        case 0x2660 : return 65 ; // spades suit
        case 0x1fb72: return 66 ; // vertical line 1
        case 0x1fb78: return 67 ; // horizontal line 1
        case 0x1fb77: return 68 ; // horizontal line 2
        case 0x1fb76: return 69 ; // horizontal line 3
        case 0x1fb7a: return 70 ; // horizontal line 4
        case 0x1fb71: return 71 ; // vertical line 2
        case 0x1fb74: return 72 ; // vertical line 3
        case 0x256e : return 73 ; // round corner ll
        case 0x2570 : return 74 ; // round corner ur
        case 0x256f : return 75 ; // round corner ul
        case 0x1fb7c: return 76 ; // square corner ll
        case 0x2572 : return 77 ; // diagonal tl->br
        case 0x2571 : return 78 ; // diagonal bl->tr
        case 0x1fb7d: return 79 ; // square corner tl
        case 0x1fb7e: return 80 ; // square corner tr
        case 0x25cf : return 81 ; // white filled circle, black square
        case 0x1fb7b: return 82 ; // horizontal line 5
        case 0x2665 : return 83 ; // hearts suit
        case 0x1fb70: return 84 ; // vertical line 4
        case 0x256d : return 85 ; // round corner lr
        case 0x2573 : return 86 ; // cross
        case 0x25cb : return 87 ; // white stroked circle, black square
        case 0x2663 : return 88 ; // clubs suit
        case 0x1fb75: return 89 ; // vertical line 5
        case 0x2666 : return 90 ; // diamonds suit
        case 0x253c : return 91 ; // plus
        case 0x1fb8c: return 92 ; // hash fill left half
        case 0x2502 : return 93 ; // vertical line 5
        case 0x03c0 : return 94 ; // pi
        case 0x25e5 : return 95 ; // filled tr triangle
        case 0x00a0 : return 96 ; // space
        case 0x258c : return 97 ; // filled left side
        case 0x2584 : return 98 ; // filled bottom side
        case 0x2594 : return 99 ; // filled top side
        case 0x2581 : return 100; // horizontal line bottom
        case 0x258f : return 101; // vertical line left
        case 0x2592 : return 102; // hash fill
        case 0x2595 : return 103; // line top and right
        case 0x1fb8f: return 104; // hash fill bottom half
        case 0x25e4 : return 105; // filled tl triangle
        case 0x1fb87: return 106; // thick line right
        case 0x251c : return 107; // vertical line mid and to right
        case 0x2597 : return 108; // filled br quarter
        case 0x2514 : return 109; // stroked tr quarter
        case 0x2510 : return 110; // stroked bl quarter
        case 0x2582 : return 111; // thick line bottom
        case 0x250c : return 112; // stroked br quarter
        case 0x2534 : return 113; // horizontal line mid and to top
        case 0x252c : return 114; // horizontal line mid and to bottom
        case 0x2524 : return 115; // vertical line mid and to left
        case 0x258e : return 116; // thick line left
        case 0x258d : return 117; // double thick line left
        case 0x1fb88: return 118; // double thick line right
        case 0x1fb82: return 119; // double thick line top
        case 0x1fb83: return 120; // triple thick line top
        case 0x2583 : return 121; // quadruple thick line bottom
        case 0x1fb7f: return 122; // stroked br quarter
        case 0x2596 : return 123; // filled bl quarter
        case 0x259d : return 124; // filled tr quarter
        case 0x2518 : return 125; // stroked tl quarter
        case 0x2598 : return 126; // filled tl quarter
        case 0x259a : return 127; // filled tl and br quarter
        default     : return 32;
      }
    };

    pet = CPetsciChar(utfToPet(drawChar.utf()));
  }
  else {
    auto asciiToPet = [](uchar ascii1) -> uchar {
      switch (ascii1) {
        case '@': return 0;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
        case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
        case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
          return (ascii1 - 'A' + 1);
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
        case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
        case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
          return (ascii1 - 'a' + 1);
        case '[': return 27;
        case '\\': return 28;
        case ']': return 29;
        case '^': return 30; // up arrow
        case '~': return 31; // back arrow
        case ' ': return 32;
        case '!': return 33;
        case '"': return 34;
        case '#': return 35;
        case '$': return 36;
        case '%': return 37;
        case '&': return 38;
        case '\'': return 39;
        case '(': return 40;
        case ')': return 41;
        case '*': return 42;
        case '+': return 43;
        case ',': return 44;
        case '-': return 45;
        case '.': return 46;
        case '/': return 47;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          return (ascii1 - '0' + 48);
        case ':': return 58;
        case ';': return 59;
        case '<': return 60;
        case '=': return 61;
        case '>': return 62;
        case '?': return 63;
        // 64-127 : graphic characters
        default: return 32;
      }
    };

    pet = CPetsciChar(asciiToPet(drawChar.c()));
  }

  if (drawChar.isReverse())
    pet.reverse();

  return pet;
}

CPetDrawChar
CPetBasic::
petToDrawChar(const CPetsciChar &pet)
{
  CPetsciChar pet1 = pet;

  CPetDrawChar drawChar;

  if (pet1.isReversed()) {
    pet1.unreverse();

    drawChar.setReverse(true);
  }

  auto setC = [&](uchar c) {
    drawChar.setC(c);
    return drawChar;
  };

  // A-Z
  if (pet1.c() >= 1 && pet1.c() <= 26)
    return setC(uchar('A' + pet1.c() - 1));

  // 0-9
  if (pet1.c() >= 48 && pet1.c() <= 57)
    return setC('0' + pet1.c() - 48);

  // 0-63 normal characters
  if (pet1.c() < 64) {
    switch (pet1.c()) {
      case 0 : return setC('@');
      // A-Z (1-26)
      case 27: return setC('[');
      case 28: return setC('\\');
      case 29: return setC(']');
      case 30: return setC('^'); // up arrow
      case 31: return setC('~'); // left arrow
      case 32: return setC(' ');
      case 33: return setC('!');
      case 34: return setC('"');
      case 35: return setC('#');
      case 36: return setC('$');
      case 37: return setC('%');
      case 38: return setC('&');
      case 39: return setC('\'');
      case 40: return setC('(');
      case 41: return setC(')');
      case 42: return setC('*');
      case 43: return setC('+');
      case 44: return setC(',');
      case 45: return setC('-');
      case 46: return setC('.');
      case 47: return setC('/');
      // 0-9 (48-57)
      case 58: return setC(':');
      case 59: return setC(';');
      case 60: return setC('<');
      case 61: return setC('=');
      case 62: return setC('>');
      case 63: return setC('?');
    }
  }

  auto setUtf = [&](ulong utf) {
    drawChar.setUtf(utf);
    return drawChar;
  };

  // 64-127 : graphic characters
  switch (pet1.c()) {
    case 64 : return setUtf(0x1fb79); // horizontal mid line
    case 65 : return setUtf(0x2660 ); // spades suit
    case 66 : return setUtf(0x1fb72); // vertical line 1
    case 67 : return setUtf(0x1fb78); // horizontal line 1
    case 68 : return setUtf(0x1fb77); // horizontal line 2
    case 69 : return setUtf(0x1fb76); // horizontal line 3
    case 70 : return setUtf(0x1fb7a); // horizontal line 4
    case 71 : return setUtf(0x1fb71); // vertical line 2
    case 72 : return setUtf(0x1fb74); // vertical line 3
    case 73 : return setUtf(0x256e ); // round corner ll
    case 74 : return setUtf(0x2570 ); // round corner ur
    case 75 : return setUtf(0x256f ); // round corner ul
    case 76 : return setUtf(0x1fb7c); // square corner ll
    case 77 : return setUtf(0x2572 ); // diagonal tl->br
    case 78 : return setUtf(0x2571 ); // diagonal bl->tr
    case 79 : return setUtf(0x1fb7d); // square corner tl
    case 80 : return setUtf(0x1fb7e); // square corner tr
    case 81 : return setUtf(0x25cf ); // white filled circle, black square
    case 82 : return setUtf(0x1fb7b); // horizontal line 5
    case 83 : return setUtf(0x2665 ); // hearts suit
    case 84 : return setUtf(0x1fb70); // vertical line 4
    case 85 : return setUtf(0x256d ); // round corner lr
    case 86 : return setUtf(0x2573 ); // cross
    case 87 : return setUtf(0x25cb ); // white stroked circle, black square
    case 88 : return setUtf(0x2663 ); // clubs suit
    case 89 : return setUtf(0x1fb75); // vertical line 5
    case 90 : return setUtf(0x2666 ); // diamonds suit
    case 91 : return setUtf(0x253c ); // plus
    case 92 : return setUtf(0x1fb8c); // hash fill left half
    case 93 : return setUtf(0x2502 ); // vertical line 5
    case 94 : return setUtf(0x03c0 ); // pi
    case 95 : return setUtf(0x25e5 ); // filled tr triangle
    case 96 : return setUtf(0x00a0 ); // space
    case 97 : return setUtf(0x258c ); // filled left side
    case 98 : return setUtf(0x2584 ); // filled bottom side
    case 99 : return setUtf(0x2594 ); // filled top side
    case 100: return setUtf(0x2581 ); // horizontal line bottom
    case 101: return setUtf(0x258f ); // vertical line left
    case 102: return setUtf(0x2592 ); // hash fill
    case 103: return setUtf(0x2595 ); // line top and right
    case 104: return setUtf(0x1fb8f); // hash fill bottom half
    case 105: return setUtf(0x25e4 ); // filled tl triangle
    case 106: return setUtf(0x1fb87); // thick line right
    case 107: return setUtf(0x251c ); // vertical line mid and to right
    case 108: return setUtf(0x2597 ); // filled br quarter
    case 109: return setUtf(0x2514 ); // stroked tr quarter
    case 110: return setUtf(0x2510 ); // stroked bl quarter
    case 111: return setUtf(0x2582 ); // thick line bottom
    case 112: return setUtf(0x250c ); // stroked br quarter
    case 113: return setUtf(0x2534 ); // horizontal line mid and to top
    case 114: return setUtf(0x252c ); // horizontal line mid and to bottom
    case 115: return setUtf(0x2524 ); // vertical line mid and to left
    case 116: return setUtf(0x258e ); // thick line left
    case 117: return setUtf(0x258d ); // double thick line left
    case 118: return setUtf(0x1fb88); // double thick line right
    case 119: return setUtf(0x1fb82); // double thick line top
    case 120: return setUtf(0x1fb83); // triple thick line top
    case 121: return setUtf(0x2583 ); // quadruple thick line bottom
    case 122: return setUtf(0x1fb7f); // stroked br quarter
    case 123: return setUtf(0x2596 ); // filled bl quarter
    case 124: return setUtf(0x259d ); // filled tr quarter
    case 125: return setUtf(0x2518 ); // stroked tl quarter
    case 126: return setUtf(0x2598 ); // filled tl quarter
    case 127: return setUtf(0x259a ); // filled tl and br quarter
  }

  // TODO ?
  assert(false);

  return drawChar;
}

//---

CPetBasicToken::
CPetBasicToken(const CPetBasic *basic, const TokenType &type, const std::string &str) :
 basic_(basic), type_(type)
{
  str_ = str;

//++s_num_basic_tokens_created;
}

CPetBasicToken::
~CPetBasicToken()
{
//++s_num_basic_tokens_deleted;
}

//---

CPetBasic::ExprToken::
ExprToken(const CPetBasic *b, const ExprData &exprData) :
 CPetBasicToken(b, TokenType::EXPR, exprData.str), exprData_(exprData)
{
  if (exprData_.value) {
    assert(! exprData_.value->isConstant());

    exprData_.value->setConstant(true);
  }
}

void
CPetBasic::ExprToken::
print(std::ostream &os) const
{
  if (exprData_.value) {
    std::string s;

    if (! exprData_.value->getStringValue(s))
      s = "<null>";

    os << s;
  }
  else if (exprData_.varName != "")
    os << "Var: " << exprData_.varName;
  else
    os << exprData_.str;
}

bool
CPetBasic::ExprToken::
eval(CExprValuePtr &val) const
{
  return basic_->evalExprData(exprData_, val);
}
