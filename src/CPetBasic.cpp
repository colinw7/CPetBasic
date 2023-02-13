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

#include <algorithm>
#include <cmath>
#include <sstream>
#include <termios.h>
#include <unistd.h>

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

    return val;
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
      return errorMsg("Wrong argument type");

    auto s1 = s.substr(i, n);

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

//---

CPetBasic::
CPetBasic()
{
  COSRand::srand();

  expr_ = std::make_unique<CPetBasicExpr>(this);

  expr_->addFunction("ABS"   , "r"    , new CPetBasicAbsFunction   (this));
  expr_->addFunction("ASC"   , "s"    , new CPetBasicAscFunction   (this));
  expr_->addFunction("ATN"   , "r"    , new CPetBasicAtnFunction   (this));
  expr_->addFunction("CHR$"  , "i"    , new CPetBasicChrFunction   (this));
  expr_->addFunction("COS"   , "r"    , new CPetBasicCosFunction   (this));
  expr_->addFunction("EXP"   , "r"    , new CPetBasicExpFunction   (this));
//expr_->addFunction("FRE"   , "s"    , new CPetBasicFreFunction   (this));
  expr_->addFunction("INT"   , "r"    , new CPetBasicIntFunction   (this));
  expr_->addFunction("LEFT$" , "s,i"  , new CPetBasicLeftFunction  (this));
  expr_->addFunction("LEN"   , "s"    , new CPetBasicLenFunction   (this));
  expr_->addFunction("LOG"   , "r"    , new CPetBasicLogFunction   (this));
  expr_->addFunction("MID$"  , "s,i,i", new CPetBasicMidFunction   (this));
  expr_->addFunction("PEEK"  , "i"    , new CPetBasicPeekFunction  (this));
//expr_->addFunction("POS"   , "s"    , new CPetBasicPosFunction   (this));
  expr_->addFunction("RIGHT$", "s,i"  , new CPetBasicRightFunction (this));
  expr_->addFunction("RND"   , "s"    , new CPetBasicRndFunction   (this));
  expr_->addFunction("SGN"   , "r"    , new CPetBasicSgnFunction   (this));
  expr_->addFunction("SIN"   , "s"    , new CPetBasicSinFunction   (this));
  expr_->addFunction("SPC"   , "i"    , new CPetBasicSpcFunction   (this));
  expr_->addFunction("SQR"   , "r"    , new CPetBasicSqrFunction   (this));
  expr_->addFunction("STR$"  , "r"    , new CPetBasicStrsFunction  (this));
  expr_->addFunction("TAB"   , "s"    , new CPetBasicTabFunction   (this));
  expr_->addFunction("TAN"   , "r"    , new CPetBasicTanFunction   (this));
//expr_->addFunction("USR"   , "s"    , new CPetBasicUsrFunction   (this));
  expr_->addFunction("VAL"   , "s"    , new CPetBasicValFunction   (this));

//expr_->createUserVariable("DS"    , new CPetBasicDSVar    (this));
//expr_->createUserVariable("DS$"   , new CPetBasicDSSVar   (this));
  expr_->createUserVariable("TI"    , new CPetBasicTIVar    (this));
//expr_->createUserVariable("TI$"   , new CPetBasicTISVar   (this));
//expr_->createUserVariable("STATUS", new CPetBasicStatusVar(this));

  term_ = new CPetBasicTerm(this);

  term_->init();
}

CPetBasic::
~CPetBasic()
{
  delete term_;
}

bool
CPetBasic::
loadFile(const std::string &fileName)
{
  fileName_ = fileName;

  CFileParse fileParse(fileName_);

  fileParse.setStream(true);

  uint lineNum = 0;

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

    ++lineNum;

    return line;
  };

  uint lastLineNum = 0;

  while (! fileParse.eof()) {
    auto line = readLine();

    LineData lineData;

    if (! parseLine(lineNum, line, lineData))
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
parseLine(uint lineNum, const std::string &line, LineData &lineData) const
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

    if (n <= 0) {
      warnMsg("Invalid line number '" + lineStr + "' @" + std::to_string(lineNum));
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
  lineData.lineNum = lineNum;
  lineData.lineN   = uint(n);

  parseLine(codeStr, lineNum, lineData.tokens);

  return true;
}

bool
CPetBasic::
parseLine(const std::string &line, uint lineNum, Tokens &tokens) const
{
  std::string tokenStr;
  bool        embedded { false };

  CStrParse parse(line);

  bool        firstToken  = true;
  KeywordType keywordType = KeywordType::NONE;
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

      tokens.push_back(token);

      firstToken  = false;
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

      firstToken = true;
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

          if (isReplaceEmbedded() && replaceEmbedded(str1, str2)) {
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
    else if (parse.isDigit()) {
      flushToken();

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
    else if (parse.isAlpha()) {
      flushToken();

      while (! parse.eof()) {
        if (! parse.isAlpha() && ! parse.isChar('_') && ! parse.isDigit())
          break;

        tokenStr += parse.readChar();
      }

      keywordType = lookupKeyword(tokenStr);

      if (keywordType != KeywordType::NONE) {
        keywordStr = "";

        if (firstToken) {
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
        }

        flushToken(TokenType::KEYWORD);
      }
      else {
        if (! parse.eof() && (parse.isChar('$') || parse.isChar('%')))
          tokenStr += parse.readChar();

        flushToken(TokenType::VARIABLE);
      }
    }
    else if (parse.isSpace()) {
      flushToken();

      parse.skipSpace();
    }
    else {
      auto c = parse.readChar();
      warnMsg("Unhandled '" + std::string(&c, 1) + "' @" + std::to_string(lineNum));
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
  auto len1 = str1.size();

  str2 = "";

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
  else if (str1 == "CLS") { // 147 (octal 223)
    str2 += encodeEmbedded("CLS");
  }
  else if (str1 == "PI") { // 255 (octal 377)
    str2 += encodeEmbedded("PI");
  }
  else if (str1 == "REV") { // 18 (octal 22)
    str2 += encodeEmbedded("REV");
  }
  else if (str1 == "OFF") { // 146 (octal 222)
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
  else if (parseNumeric("^DQT", n)) {
    appendN(str2, n, encodeEmbedded("\""));
  }
  else if (parseNumeric("^BSH", n)) {
    appendN(str2, n, encodeEmbedded("\\"));
  }
  else if (parseNumeric("^DEL", n)) {
    appendN(str2, n, encodeEmbedded("~"));
  }
  else if (parseShift(n, sstr)) { // ^<char> (shifted character)
    std::string c;
    if (isEmbeddedEscapes())
      appendN(str2, n, sstr[0]);
    else
      appendN(str2, n, encodeEmbedded(sstr));
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
  if (isEmbeddedEscapes()) {
    if      (name == "HOM" ) return "\033[0,0H";
    else if (name == "CLS" ) return "\033[2]";
    else if (name == "PI"  ) return "π";
    else if (name == "REV" ) return "\033[7m";
    else if (name == "OFF" ) return "\033[0m";
    else if (name == "CU"  ) return "\033[A";
    else if (name == "CD"  ) return "\033[B";
    else if (name == "CL"  ) return "\033[D";
    else if (name == "CR"  ) return "\033[C";
    else if (name == "^SPC") return " ";

    else if (name.size() == 1) return name;
  }
  else {
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

  std::cerr << "Invalid embedded code '" << name << "'\n";

  return "";
}

#if 0
CPetsciChar
CPetBasic::
decodeEmbedded(CAsciiChar c)
{
  switch (c.c()) {
    case  19: return CPetsciChar(c.c()); // HOM
    case 147: return CPetsciChar(c.c()); // CLS
    case 255: return CPetsciChar(c.c()); // PI
    case  18: return CPetsciChar(c.c()); // REV
    case 146: return CPetsciChar(c.c()); // OFF
    case 145: return CPetsciChar(c.c()); // CU
    case  17: return CPetsciChar(c.c()); // CD
    case 157: return CPetsciChar(c.c()); // CL
    case  29: return CPetsciChar(c.c()); // CR
    case 160: return CPetsciChar(c.c()); // ^SPC
    default:
      std::cerr << "Invalid embedded char '" << int(c.c()) << "'\n";
      break;
  }

  uchar c1 = c.c();

  bool shift = (c1 >= 128);

  if (shift)
    c1 -= 128;

  CPetDrawChar drawChar(c1);

  auto pet = drawCharToPet(drawChar);

  if (shift)
    pet.shift();

  return pet;
}
#endif

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
    default:
      std::cerr << "Invalid embedded char '" << int(c) << "'\n";
      break;
  }

  bool shift = (c >= 128);

  if (shift)
    c -= 128;

  char c1(c);

  if (shift)
    return "{^" + std::string(&c1, 1) + "}";
  else
    return std::string(&c1, 1);
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
//  addKeyword(KeywordType::APPEND   , "APPEND"   );
//  addKeyword(KeywordType::BACKUP   , "BACKUP"   );
    addKeyword(KeywordType::CLOSE    , "CLOSE"    );
    addKeyword(KeywordType::CLR      , "CLR"      );
//  addKeyword(KeywordType::CMD      , "CMD"      );
//  addKeyword(KeywordType::COLLECT  , "COLLECT"  );
//  addKeyword(KeywordType::CONCAT   , "CONCAT"   );
    addKeyword(KeywordType::CONT     , "CONT"     );
//  addKeyword(KeywordType::COPY     , "COPY"     );
    addKeyword(KeywordType::DATA     , "DATA"     );
//  addKeyword(KeywordType::DCLOSE   , "DCLOSE"   );
//  addKeyword(KeywordType::DEF      , "DEF"      );
    addKeyword(KeywordType::DELAY    , "DELAY"    );
    addKeyword(KeywordType::DIM      , "DIM"      );
//  addKeyword(KeywordType::DIRECTORY, "DIRECTORY");
//  addKeyword(KeywordType::DLOAD    , "DLOAD"    );
//  addKeyword(KeywordType::DOPEN    , "DOPEN"    );
//  addKeyword(KeywordType::DSAVE    , "DSAVE"    );
    addKeyword(KeywordType::END      , "END"      );
//  addKeyword(KeywordType::FN       , "FN"       );
    addKeyword(KeywordType::FOR      , "FOR"      );
    addKeyword(KeywordType::GET      , "GET"      );
    addKeyword(KeywordType::GOSUB    , "GOSUB"    );
    addKeyword(KeywordType::GOTO     , "GOTO"     );
//  addKeyword(KeywordType::HEADER   , "HEADER"   );
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
    addKeyword(KeywordType::POKE     , "POKE"     );
    addKeyword(KeywordType::PRINT    , "PRINT"    );
    addKeyword(KeywordType::READ     , "READ"     );
//  addKeyword(KeywordType::RECORD   , "RECORD"   );
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
  for (const auto &pl : lines_)
    listLine(pl.second);
}

void
CPetBasic::
listLine(const LineData &lineData) const
{
  if (lineData.lineN > 0)
    printString(std::to_string(lineData.lineN) + " ");

  auto str = lineToString(lineData, isListHighlight());

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

  std::string str;

  for (auto *token : statement.tokens) {
    std::string s;

    if (token->type() == TokenType::STRING) {
      auto *strToken = static_cast<StringToken *>(token);

      if (strToken->isEmbedded()) {
        auto estr = decodeEmbeddedStr(strToken->toString());

        if (highlight)
          s = "\033[35m\"" + estr + "\"\033[0m";
        else
          s = "\"" + estr + "\"";
      }
    }

    if (s.empty()) {
      std::stringstream ss;

      if (highlight)
        token->printEsc(ss);
      else
        token->print(ss);

      s = ss.str();
    }

    auto firstChar = s[0];

    bool needsSpace = true;

    if (sepCharsL.find(firstChar) != std::string::npos)
      needsSpace = false;

    if (sepCharsR.find(lastChar1) != std::string::npos)
      needsSpace = false;

    if (needsSpace)
      str += " ";

    str += s;

    lastChar1 = s[s.size() - 1];
  }

  return str;
}

bool
CPetBasic::
run()
{
  initRunData();

  setLineInd(0);

  breakLineNum_ = -1;

  return contRun();
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

    setLineInd(0);

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

  term_ = new CPetBasicRawTerm(this);

  term_->init();

  setReplaceEmbedded(b);
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
runLine(const LineData &lineData)
{
  notifyRunLine(lineData.lineN);

  errorMsg_= "";

  if (isDebug())
    listLine(lineData);

  bool nextLine = true;

  for (const auto &statement : lineData.statements) {
    if (! runTokens(lineData.lineN, statement.tokens, nextLine))
      return false;

    if (! nextLine)
      break;
  }

  if (nextLine)
    setLineInd(lineInd_ + 1);

  return true;
}

bool
CPetBasic::
runTokens(int lineN, const Tokens &tokens, bool &nextLine)
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
      case KeywordType::CLOSE:
        rc = closeStatement(tokenList);
        break;
      case KeywordType::CLR:
        rc = clrStatement(tokenList);
        break;
      case KeywordType::CONT:
        rc = contStatement(tokenList);
        break;
      case KeywordType::DATA:
        rc = dataStatement(token->str());
        break;
      case KeywordType::DELAY:
        rc = delayStatement(tokenList);
        break;
      case KeywordType::DIM:
        rc = dimStatement(tokenList);
        break;
      case KeywordType::END:
        rc = endStatement(tokenList);
        break;
      case KeywordType::FOR:
        rc = forStatement(lineN, tokenList);
        break;
      case KeywordType::GET:
        rc = getStatement(tokenList);
        break;
      case KeywordType::GOSUB:
        rc = gosubStatement(tokenList);
        nextLine = false;
        break;
      case KeywordType::GOTO:
        rc = gotoStatement(tokenList);
        nextLine = false;
        break;
      case KeywordType::IF:
        rc = ifStatement(lineN, tokenList, nextLine);
        break;
      case KeywordType::INPUT:
        rc = inputStatement(tokenList);
        break;
      case KeywordType::LET:
        rc = letStatement(tokenList);
        break;
      case KeywordType::LIST:
        rc = listStatement(tokenList);
        break;
      case KeywordType::LOAD:
        rc = loadStatement(tokenList);
        break;
      case KeywordType::NEW:
        rc = newStatement(tokenList);
        break;
      case KeywordType::NEXT:
        rc = nextStatement(tokenList);
        nextLine = false;
        break;
      case KeywordType::ON:
        rc = onStatement(tokenList);
        nextLine = false;
        break;
      case KeywordType::OPEN:
        rc = openStatement(tokenList);
        break;
      case KeywordType::POKE:
        rc = pokeStatement(tokenList);
        break;
      case KeywordType::PRINT:
        rc = printStatement(tokenList);
        break;
      case KeywordType::READ:
        rc = readStatement(tokenList);
        break;
      case KeywordType::REM:
        rc = true;
        break;
      case KeywordType::RESTORE:
        rc = restoreStatement(tokenList);
        break;
      case KeywordType::RETURN:
        rc = returnStatement(tokenList);
        break;
      case KeywordType::RUN:
        rc = runStatement(tokenList);
        break;
      case KeywordType::SAVE:
        rc = saveStatement(tokenList);
        break;
      case KeywordType::STOP:
        rc = stopStatement(tokenList);
        break;
      default:
        warnMsg("Unhandled keyword " + token->listString());
        break;
    }

    if (! rc)
      return errorMsg(errorMsg_ != "" ? errorMsg_ : "Command failed");
  }
  else if (token->type() == TokenType::VARIABLE) {
    auto *varToken = static_cast<VariableToken *>(token);

    auto *token1 = (it < nt ? tokens[it++] : nullptr);

    Inds inds;

    if (token1 && isSeparator(token1, SeparatorType::OPEN_RBRACKET)) {
      auto *token2 = (it < nt ? tokens[it++] : nullptr);

      Tokens indexTokens;

      auto addInd = [&]() {
        if (indexTokens.empty())
          return errorMsg("Invalid variable index");

        auto val = evalExpr(indexTokens);

        long i;
        if (! val->getIntegerValue(i) || i < 0)
          return errorMsg("Invalid variable index value");

        inds.push_back(uint(i));

        return true;
      };

      while (token2 && ! isSeparator(token2, SeparatorType::CLOSE_RBRACKET)) {
        if (isSeparator(token2, SeparatorType::COMMA)) {
          if (! addInd()) return false;

          indexTokens.clear();
        }
        else {
          indexTokens.push_back(token2);
        }

        token2 = (it < nt ? tokens[it++] : nullptr);
      }

      if (! isSeparator(token2, SeparatorType::CLOSE_RBRACKET))
        return errorMsg("Invalid variable index");

      if (! addInd())
        return false;

      //--

      token1 = (it < nt ? tokens[it++] : nullptr);
    }

    if (token1 && isOperator(token1, OperatorType::ASSIGN)) {
      Tokens assignTokens;

      for (uint i = it; i < nt; ++i)
        assignTokens.push_back(tokens[i]);

      auto val = evalExpr(assignTokens);

      auto varName = varToken->str();

      if (! inds.empty()) {
        if (! setVariableValue(varName, inds, val))
          return false;
      }
      else {
        if (! setVariableValue(varName, val))
          return false;
      }
    }
    else {
      if (token1)
        return errorMsg("Invalid token '" + token1->str() + "'");
      else
        return errorMsg("Invalid token");
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
closeStatement(TokenList &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
clrStatement(TokenList &tokenList)
{
  if (tokenList.atEnd())
    return errorMsg("Extra tokens");

  variableNames_.clear();

  clearArrayVariables();

  expr_ = std::make_unique<CPetBasicExpr>(this);

  notifyVariablesChanged();

  return true;
}

bool
CPetBasic::
contStatement(TokenList &tokenList)
{
  if (tokenList.atEnd())
    return errorMsg("Extra tokens");

  if (isStopped()) {
    setStopped(false);

    contRun();
  }

  return true;
}

bool
CPetBasic::
dataStatement(const std::string &)
{
  return true;
}

bool
CPetBasic::
delayStatement(TokenList &tokenList)
{
  Tokens exprTokens;

  auto *token = tokenList.nextToken();

  while (token) {
    exprTokens.push_back(token);

    token = tokenList.nextToken();
  }

  auto val = evalExpr(exprTokens);

  long i;
  if (! val->getIntegerValue(i))
    return errorMsg("Invalid DELAY expression");

  term_->delay(i);

  return true;
}

bool
CPetBasic::
dimStatement(TokenList &tokenList)
{
  auto *token = tokenList.currentToken();

  while (token) {
    std::string varName;
    Inds        inds;

    if (! readVariable(tokenList, "DIM", varName, inds))
      return false;

#if 0
    printVarDetails("DIM", varName, inds);
#endif

    //---

    dimVariable(varName, inds);

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
endStatement(TokenList &)
{
  setStopped(true);

  return true;
}

bool
CPetBasic::
ifStatement(int lineN, TokenList &tokenList, bool &nextLine)
{
  Tokens exprTokens;

  bool thenFound = false;

  auto *token = tokenList.nextToken();

  while (token) {
    if (isKeyword(token, KeywordType::THEN)) {
      thenFound = true;
      break;
    }

    exprTokens.push_back(token);

    token = tokenList.nextToken();
  }

  if (! thenFound)
    return errorMsg("No THEN for IF");

  //---

  auto val = evalExpr(exprTokens);

  long i;
  if (! val->getIntegerValue(i))
    return errorMsg("Invalid IF expression");

  if (! i) {
    nextLine = false;
    setLineInd(lineInd_ + 1);

    return true;
  }

  //---

  Tokens tokens;

  token = tokenList.nextToken();

  while (token) {
    tokens.push_back(token);

    token = tokenList.nextToken();
  }

  if (tokens.size() == 1) {
    auto *token1 = tokens[0];

    if (token1->type() == TokenType::NUMBER) {
      uint lineNum = uint(token1->toInteger());

      auto lineInd = getLineInd(lineNum);
      if (lineInd < 0) return errorMsg("Invalid IF THEN line");

      setLineInd(lineInd);
      nextLine = false;

      return true;
    }
  }

  if (! runTokens(lineN, tokens, nextLine))
    return false;

  return true;
}

bool
CPetBasic::
forStatement(uint lineNum, TokenList &tokenList)
{
  auto *varToken = tokenList.nextToken();
  if (! varToken || varToken->type() != TokenType::VARIABLE)
    return errorMsg("No FOR variable ");

  auto *eqToken = tokenList.nextToken();
  if (! eqToken || ! isOperator(eqToken, OperatorType::ASSIGN))
    return errorMsg("No FOR = operator ");

  Tokens fromTokens;

  auto *token = tokenList.nextToken();

  bool toFound = false;

  while (token) {
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

  Tokens toTokens;

  token = tokenList.nextToken();

  bool stepFound = false;

  while (token) {
    if (isKeyword(token, KeywordType::STEP)) {
      stepFound = true;
      break;
    }

    toTokens.push_back(token);

    token = tokenList.nextToken();
  }

  if (toTokens.empty())
    return errorMsg("No tokens for FOR ... TO ...");

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

  auto var = getVariable(varToken->str());

  auto fromVal = evalExpr(fromTokens);
  auto toVal   = evalExpr(toTokens);
  auto stepVal = (stepFound ? evalExpr(stepTokens) : CExprValuePtr());

  long toI;
  if (! toVal->getIntegerValue(toI))
    return errorMsg("No FOR ... TO value");

  if (stepFound) {
    long stepI;
    if (! stepVal->getIntegerValue(stepI))
      return errorMsg("Invalid FOR ... TO ... STEP value");
  }

  var->setValue(fromVal);

  forDatas_[varToken->str()] = ForData(varToken->str(), toVal, stepVal, lineNum);

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
gosubStatement(TokenList &tokenList)
{
  auto *token = tokenList.nextToken();
  if (! token) return errorMsg("Missing value");

  if (token->type() != TokenType::NUMBER)
    return errorMsg("Not a number");

  uint lineNum = uint(token->toInteger());
  //std::cout << "GOSUB " << lineNum << "\n";

  auto lineInd = getLineInd(lineNum);
  if (lineInd < 0) return errorMsg("Invalid GOSUB line");

  lineStack_.push_back(lineInd_);

  setLineInd(lineInd);

  return true;
}

bool
CPetBasic::
gotoStatement(TokenList &tokenList)
{
  auto *token = tokenList.nextToken();
  if (! token) return errorMsg("Missing value");

  if (token->type() != TokenType::NUMBER)
    return errorMsg("Not a number");

  uint lineNum = uint(token->toInteger());
  //std::cout << "GOTO " << lineNum << "\n";

  auto lineInd = getLineInd(lineNum);
  if (lineInd < 0) return errorMsg("Invalid GOTO line");

  setLineInd(lineInd);

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
letStatement(TokenList &tokenList)
{
  auto *token = tokenList.nextToken();

  if (! token || token->type() != TokenType::VARIABLE)
    return errorMsg("Invalid variable");

  auto varName = token->str();

  token = tokenList.nextToken();

  if (! token || ! isOperator(token, OperatorType::ASSIGN))
    return errorMsg("Missing equals");

  Tokens tokens;

  token = tokenList.nextToken();

  while (token) {
    tokens.push_back(token);

    token = tokenList.nextToken();
  }

  auto val = evalExpr(tokens);

  if (! setVariableValue(varName, val))
    return false;

  return true;
}

bool
CPetBasic::
listStatement(TokenList &)
{
  // TODO: list from line
  list();

  return true;
}

bool
CPetBasic::
loadStatement(TokenList &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
newStatement(TokenList &tokenList)
{
  if (tokenList.atEnd())
    return errorMsg("Extra tokens");

  initRunState();

  lines_.clear();

  setLineInd(-1);

  expr_ = std::make_unique<CPetBasicExpr>(this);

  // clear data
  dataValues_.clear();

  dataValuePos_ = 0;

  runDataValid_ = false;

  notifyLinesChanged();

  return true;
}

bool
CPetBasic::
nextStatement(TokenList &tokenList)
{
  auto *varToken = tokenList.nextToken();
  if (! varToken || varToken->type() != TokenType::VARIABLE)
    return errorMsg("No NEXT variable ");

  auto pf = forDatas_.find(varToken->str());

  if (pf == forDatas_.end())
    return errorMsg("No matching FOR");

  const auto &forData = (*pf).second;

  if (forData.varName_ == varToken->str()) {
    auto var = getVariable(varToken->str());
    auto val = var->value();

    long fromI;

    if (! val->getIntegerValue(fromI))
      return errorMsg("Invalid loop variable");

    long stepI = 1;
    if (forData.stepVal_) {
      if (! forData.stepVal_->getIntegerValue(stepI))
        return errorMsg("Invalid FOR STEP value");
    }

    if (stepI == 0)
      return errorMsg("Invalid FOR STEP value");

    fromI += stepI;

    val->setIntegerValue(fromI);

    var->setValue(val);

    long toI;
    if (! forData.toVal_->getIntegerValue(toI))
      return errorMsg("Invalid FOR TO value");

    if ((stepI > 0 && fromI <= toI) || (stepI < 0 && fromI >= toI)) { // not at end
      auto lineInd = getLineInd(forData.lineNum_);
      if (lineInd < 0) return errorMsg("Invalid GOTO line");

      setLineInd(lineInd + 1);
    }
    else { // at end
      forDatas_.erase(varToken->str());

      setLineInd(lineInd_ + 1);
    }
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

  auto value = evalExpr(tokens);

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

  auto lineInd = getLineInd(uint(lines[i - 1]));
  if (lineInd < 0) return errorMsg("Invalid ON line");

  if (gosubFlag)
    lineStack_.push_back(lineInd_);

  setLineInd(lineInd);

  return true;
}

bool
CPetBasic::
openStatement(TokenList &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
pokeStatement(TokenList &tokenList)
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

  auto addrValue  = evalExpr(tokens1);
  auto valueValue = evalExpr(tokens2);

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
printStatement(TokenList &tokenList)
{
  // , next tab stop (10), ; next pos
  enum class Spacer {
    NONE,
    COMMA,
    SEMI_COLON
  };

  Spacer nextSpacer { Spacer::NONE };

  while (true) {
    auto spacer = nextSpacer;

    Tokens tokens1;

    auto *token = tokenList.nextToken();
    if (! token) break;

    int brackets = 0;

    while (token) {
      if (brackets <= 0) {
        if      (isSeparator(token, SeparatorType::COMMA)) {
          nextSpacer = Spacer::COMMA;
          break;
        }
        else if (isSeparator(token, SeparatorType::SEMI_COLON)) {
          nextSpacer = Spacer::SEMI_COLON;
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

      token = tokenList.nextToken();
    }

    //---

    if (spacer == Spacer::COMMA)
      printString("\t");

    auto value = evalExpr(tokens1);

    if (value->isIntegerValue() || value->isRealValue()) {
      double r;

      if (value->getRealValue(r) && r >= 0)
        printString(" ");
    }

    std::string s;
    (void) value->getStringValue(s);

    printString(s);
  }

  printString("\n");

  return true;
}

bool
CPetBasic::
readStatement(TokenList &tokenList)
{
  auto *token = tokenList.currentToken();

  while (token) {
    std::string varName;
    Inds        inds;

    if (! readVariable(tokenList, "READ", varName, inds))
      return false;

#if 0
    printVarDetails("DIM", varName, inds);
#endif

    //---

    // store next data in variable
    if (dataValuePos_ >= dataValues_.size())
      return errorMsg("No data for READ");

    auto val = dataValues_[dataValuePos_++];

    if (! inds.empty()) {
      if (! setVariableValue(varName, inds, val))
        return false;
    }
    else {
      if (! setVariableValue(varName, val))
        return false;
    }

    //---

    token = tokenList.currentToken();

    if (token && ! isSeparator(token, SeparatorType::COMMA))
      return errorMsg("Missing READ comma");

    if (token)
      token = tokenList.nextToken();
  }

  return true;
}

bool
CPetBasic::
restoreStatement(TokenList &tokenList)
{
  if (! tokenList.atEnd())
    return errorMsg("Extra arguments ");

  dataValuePos_ = 0;

  return true;
}

bool
CPetBasic::
returnStatement(TokenList &tokenList)
{
  if (! tokenList.atEnd())
    return errorMsg("Extra arguments ");

  if (lineStack_.empty())
    return errorMsg("Empty line stack");

  auto lineInd = lineStack_.back();
  lineStack_.pop_back();

  setLineInd(lineInd);

  return true;
}

bool
CPetBasic::
runStatement(TokenList &tokenList)
{
  if (! tokenList.atEnd())
    return errorMsg("Extra arguments ");

  run();

  return true;
}

bool
CPetBasic::
saveStatement(TokenList &)
{
  assert(false);
  return false;
}

bool
CPetBasic::
stopStatement(TokenList &)
{
  setStopped(true);

  return true;
}

//---

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

    auto val1 = evalExpr(indexTokens);

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
initRunState()
{
  // init run data
  lineStack_.clear();
  forDatas_ .clear();

  setStopped(false);

  errorMsg_ = "";

  setLineInd(-1);
}

void
CPetBasic::
setLineInd(int ind)
{
  if (ind != lineInd_) {
    lineInd_ = ind;

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

        term_->drawChar(drawChar);

        term_->cursorRight(/*nl*/false);

        handled = true;
      }

      // handle normal char
      if (! handled) {
        // map to upper case
        if (islower(c))
          c = char(toupper(c));

        if (term_->drawChar(c))
          term_->cursorRight(/*nl*/false);
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

CExprValuePtr
CPetBasic::
evalExpr(const Tokens &tokens) const
{
  std::string str;
  bool        embedded = false;

  auto nt = tokens.size();

  for (uint i = 0; i < nt; ++i) {
    if (str != "")
      str += " ";

    auto *token = tokens[i];

    if      (token->type() == TokenType::STRING) {
      auto *strToken = static_cast<StringToken *>(token);
      if (strToken->isEmbedded()) embedded = true;

      str += "\"" + token->toString() + "\"";
    }
    else if (token->type() == TokenType::NUMBER)
      str += token->toString();
    else if (token->type() == TokenType::VARIABLE)
      str += token->toString();
    else
      str += token->exprString();
  }

  if (embedded && nt > 1)
    std::cerr << "Expression with embedded '" << str << "'\n";

  return evalExpr(str);
}

CExprValuePtr
CPetBasic::
evalExpr(const std::string &str) const
{
  CExprValuePtr value;

  if (! expr_->evaluateExpression(str, value)) {
    warnMsg("Invalid expr: '" + str + "'");
    return expr_->createIntegerValue(0);
  }

  if (! value) {
    warnMsg("Invalid expr: '" + str + "'");
    return expr_->createIntegerValue(0);
  }

  return value;
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

  auto val = value;

  if      (valueType == CExprValueType::INTEGER) {
    if (! val->isIntegerValue()) {
      long i;
      if (! val->getIntegerValue(i))
        return errorMsg("Invalid value type");
      val = expr_->createIntegerValue(i);
    }
  }
  else if (valueType == CExprValueType::STRING) {
    if (! val->isStringValue()) {
      std::string s;
      if (! val->getStringValue(s))
        return errorMsg("Invalid value type");
      val = expr_->createStringValue(s);
    }
  }

  auto var = getVariable(uname);

  if (! var) {
    var = addVariable(uname, val);
  }
  else {
    var->setValue(val);

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
    th->addArrayVariable(uname);

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
addArrayVariable(const std::string &uname)
{
  assert(uname == CPetBasicUtil::toUpper(uname));

  ArrayData arrayData;

  arrayData.resize(10);

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
    th->addArrayVariable(uname);

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

  //std::cerr << "setVariableValue: " << uname << " "; printInds(inds); std::cerr << "\n";

  auto *th = const_cast<CPetBasic *>(this);

  if (! hasArrayVariable(uname))
    th->addArrayVariable(uname);

  auto pv = th->arrayVariables_.find(uname);
  assert(pv != th->arrayVariables_.end());

  bool rc = (*pv).second.setValue(inds, value);

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
}
