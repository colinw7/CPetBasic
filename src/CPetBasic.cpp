#include <CPetBasic.h>
#include <CFileParse.h>
#include <CStrParse.h>
#include <CExpr.h>
#include <CReadLine.h>
#include <COSRand.h>

#include <algorithm>
#include <cmath>

int
main(int argc, char **argv)
{
  using FileNames = std::vector<std::string>;

  FileNames fileNames;
  bool      list  = false;
  bool      run   = false;
  bool      loop  = false;
  bool      debug = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if      (arg == "list")
        list = true;
      else if (arg == "run")
        run = true;
      else if (arg == "loop")
        loop = true;
      else if (arg == "debug")
        debug = true;
    }
    else
      fileNames.push_back(argv[i]);
  }

  CPetBasic basic;

  basic.setDebug(debug);

  for (const auto &fileName : fileNames)
    basic.loadFile(fileName);

  if (list)
    basic.list();

  if (run)
    basic.run();

  if (loop)
    basic.loop();

  return 0;
}

//---

namespace {

std::string toUpper(const std::string &str) {
  auto ustr = str;
  for (auto &c : ustr) c = char(toupper(c));
  return ustr;
}

}

//---

class CPetBasicExpr : public CExpr {
 public:
  using Inds = std::vector<uint>;

 public:
  CPetBasicExpr(CPetBasic *basic) : basic_(basic) { }

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

    auto l = long(s[0]);

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
//expr_->addFunction("DS"    , "s"    , new CPetBasicDsFunction    (this));
//expr_->addFunction("DS$"   , "s"    , new CPetBasicDssFunction   (this));
//expr_->addFunction("EXP"   , "r"    , new CPetBasicExpFunction   (this));
//expr_->addFunction("ERE"   , "s"    , new CPetBasicEreFunction   (this));
  expr_->addFunction("INT"   , "r"    , new CPetBasicIntFunction   (this));
  expr_->addFunction("LEFT$" , "s,i"  , new CPetBasicLeftFunction  (this));
  expr_->addFunction("LEN"   , "s"    , new CPetBasicLenFunction   (this));
  expr_->addFunction("LOG"   , "r"    , new CPetBasicLogFunction   (this));
  expr_->addFunction("MID$"  , "s,i,i", new CPetBasicMidFunction   (this));
  expr_->addFunction("PEEK"  , "i"    , new CPetBasicPeekFunction  (this));
//expr_->addFunction("POS"   , "s"    , new CPetBasicPosFunction   (this));
  expr_->addFunction("RIGHT$", "s,i"  , new CPetBasicRightFunction (this));
  expr_->addFunction("RND"   , "s"    , new CPetBasicRndFunction   (this));
//expr_->addFunction("SGN"   , "r"    , new CPetBasicSgnFunction   (this));
  expr_->addFunction("SIN"   , "s"    , new CPetBasicSinFunction   (this));
//expr_->addFunction("SPC"   , "s"    , new CPetBasicSpcFunction   (this));
//expr_->addFunction("SQR"   , "r"    , new CPetBasicSqrFunction   (this));
//expr_->addFunction("STATUS", "s"    , new CPetBasicStatusFunction(this));
//expr_->addFunction("STRS"  , "s"    , new CPetBasicStrsFunction  (this));
//expr_->addFunction("TAB"   , "s"    , new CPetBasicTabFunction   (this));
//expr_->addFunction("TAN"   , "r"    , new CPetBasicTanFunction   (this));
//expr_->addFunction("TIME$" , "s"    , new CPetBasicTimeFunction  (this));
//expr_->addFunction("USR"   , "s"    , new CPetBasicUsrFunction   (this));
  expr_->addFunction("VAL"   , "s"    , new CPetBasicValFunction   (this));

  expr_->createUserVariable("TI", new CPetBasicTIVar(this));
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
        token = createString(tokenStr);
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
        if (parse.isChar('\\')) {
          parse.skipChar();

          if (! parse.eof())
            tokenStr += parse.readChar();
          else
            tokenStr += "/";
        }
        else if (parse.isChar('[')) {
          parse.skipChar();

          std::string str1;

          while (! parse.eof()) {
            if (parse.isChar(']'))
              break;

            str1 += parse.readChar();
          }

          if (parse.isChar(']'))
            parse.skipChar();

          std::string str2;

          if (replaceEmbedded(str1, str2))
            tokenStr += str2;
          else
            tokenStr += "[" + str1 + "]";
        }
        else {
          if (parse.isChar('"')) {
            parse.skipChar();
            break;
          }

          tokenStr += parse.readChar();
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

bool
CPetBasic::
replaceEmbedded(const std::string &str1, std::string &str2) const
{
  str2 = "";

  auto parseNumeric = [&](const std::string &name, const std::string &value, long &n) {
    auto len = name.size();

    if      (str1 == name) {
      n = 1;

      if (value != "")
        str2 += value;

      return true;
    }
    else if (str1.size() > len && str1.substr(str1.size() - len, len) == name) {
      CStrParse parse(str1);

      if (! parse.readInteger(&n) && n > 0)
        return false;

      if (value != "") {
        for (long i = 0; i < n; ++i)
          str2 += value;
      }

      return true;
    }
    else
      return false;
  };

  long n;

  if      (str1 == "CLS") { // 47
    str2 += "[2]";
    return true;
  }
  else if (str1 == "REV") { // 18
    str2 += "[7m";
    return true;
  }
  else if (str1 == "OFF") { // 146
    str2 += "[0m";
    return true;
  }
  else if (str1 == "HOM") { // 19
    str2 += "[0,0H";
    return true;
  }
  else if (parseNumeric("CU", "", n)) {  // 145
    str2 += "[" + std::to_string(n) + "A";
    return true;
  }
  else if (parseNumeric("CD", "", n)) { // 17
    str2 += "[" + std::to_string(n) + "B";
    return true;
  }
  else if (parseNumeric("CL", "", n)) { // 157
    str2 += "[" + std::to_string(n) + "D";
    return true;
  }
  else if (parseNumeric("CR", "", n)) { // 29
    str2 += "[" + std::to_string(n) + "C";
    return true;
  }
  else if (parseNumeric("SPC", " ", n)) { // 32
    return true;
  }
  else if (parseNumeric("^SPC", " ", n)) { // 160
    return true;
  }
  else if (parseNumeric("^V", "\u2715", n)) { // 214
    return true;
  }

  return false;
}

CPetBasic::KeywordType
CPetBasic::
lookupKeyword(const std::string &str) const
{
  auto ustr = toUpper(str);

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
//  addKeyword(KeywordType::CLR      , "CLR"      );
//  addKeyword(KeywordType::CMD      , "CMD"      );
//  addKeyword(KeywordType::COLLECT  , "COLLECT"  );
//  addKeyword(KeywordType::CONCAT   , "CONCAT"   );
    addKeyword(KeywordType::CONT     , "CONT"     );
//  addKeyword(KeywordType::COPY     , "COPY"     );
    addKeyword(KeywordType::DATA     , "DATA"     );
//  addKeyword(KeywordType::DCLOSE   , "DCLOSE"   );
//  addKeyword(KeywordType::DEF      , "DEF"      );
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
    printLine(pl.second);
}

void
CPetBasic::
printLine(const LineData &lineData) const
{
  int ns = int(std::log10(lineData.lineN) + 1);

  int is = 0;

  for (const auto &statement : lineData.statements) {
    if (is == 0)
      std::cout << lineData.lineN;
    else {
      for (int i = 0; i < ns; ++i)
        std::cout << " ";
    }

    for (auto *token : statement.tokens) {
      std::cout << " ";

      token->print(std::cout);
    }

    std::cout << "\n";

    ++is;
  }
}

bool
CPetBasic::
run()
{
  if (lines_.empty())
    return true;

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

  lineInd_ = 0;

  while (lineInd_ >= 0 && uint(lineInd_) < lineNums_.size()) {
    auto lineNum = lineNums_[lineInd_];

    auto pl = lines_.find(lineNum);
    assert(pl != lines_.end());

    if (! runLine((*pl).second)) {
      warnMsg("Error: " + errorMsg_);
      return false;
    }

    if (stopped_)
      break;
  }

  stopped_ = false;

  return true;
}

void
CPetBasic::
loop()
{
  CReadLine   readline;
  std::string lineBuffer;

  auto setPrompt = [&](bool ext=false) {
    readline.setPrompt(ext ? "pet+> " : "pet> ");
  };

  setPrompt();

  auto line = readline.readLine();

  while (! readline.eof()) {
    auto len = line.size();

    if (len > 0 && line[len - 1] == '\\') {
      setPrompt(true);

      lineBuffer += line.substr(0, len - 1);
    }
    else {
      lineBuffer += line;

      if (lineBuffer == "")
        break;

      LineData lineData;

      if (parseLine(0, lineBuffer, lineData)) {
        processLineData(lineData);

        if (lineData.lineN > 0)
          lines_[lineData.lineN] = lineData;
        else {
          if (! runLine(lineData)) {
            if (errorMsg_ != "")
              warnMsg("Invalid Line: " + lineBuffer + " (" + errorMsg_ + ")");
            else
              warnMsg("Invalid Line: " + lineBuffer);
          }
        }
      }
      else
        warnMsg("Invalid Line: " + lineBuffer);

      lineBuffer = "";

      setPrompt();

      if (stopped_)
        break;
    }

    line = readline.readLine();
  }
}

bool
CPetBasic::
runLine(const LineData &lineData)
{
  errorMsg_= "";

  if (isDebug())
    printLine(lineData);

  bool nextLine = true;

  for (const auto &statement : lineData.statements) {
    if (! runTokens(lineData.lineN, statement.tokens, nextLine))
      return false;

    if (! nextLine)
      break;
  }

  if (nextLine)
    ++lineInd_;

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
      case KeywordType::CONT:
        rc = contStatement(tokenList);
        break;
      case KeywordType::DATA:
        rc = dataStatement(token->str());
        break;
      case KeywordType::DIM:
        rc = dimStatement(tokenList);
        break;
      case KeywordType::END:
        rc = endStatement(tokenList);
        break;
      case KeywordType::IF:
        rc = ifStatement(lineN, tokenList, nextLine);
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
      case KeywordType::INPUT:
        rc = inputStatement(tokenList);
        break;
      case KeywordType::LET:
        rc = letStatement(tokenList);
        break;
      case KeywordType::LIST:
        rc = listStatement(tokenList);
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
      case KeywordType::POKE:
        rc = pokeStatement(tokenList);
        break;
      case KeywordType::PRINT:
        rc = printStatement(tokenList);
        break;
      case KeywordType::READ:
        rc = readStatement(tokenList);
        break;
      case KeywordType::RETURN:
        rc = returnStatement(tokenList);
        break;
      case KeywordType::RESTORE:
        rc = restoreStatement(tokenList);
        break;
      case KeywordType::REM:
        rc = true;
        break;
      case KeywordType::RUN:
        rc = runStatement(tokenList);
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
      return errorMsg("Invalid token '" + token1->str() + "'");
    }
  }
  else
    return errorMsg("Invalid run token " + token->listString());

  return true;
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
  return false;
}

bool
CPetBasic::
contStatement(TokenList &)
{
  return false;
}

bool
CPetBasic::
dataStatement(const std::string &)
{
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
  stopped_ = true;

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
    ++lineInd_;
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

      lineInd_ = lineInd;
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

  CReadLine readline;

  readline.setPrompt("? ");

  auto line = toUpper(readline.readLine());

  auto val = expr_->createStringValue(line);

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

  lineInd_ = lineInd;

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

  lineInd_ = lineInd;

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

  CReadLine readline;

  readline.setPrompt(prompt != "" ? prompt + " ? " : "? ");

  for (const auto &varName : varNames) {
    auto line = readline.readLine();

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
  list();

  return true;
}

bool
CPetBasic::
newStatement(TokenList &tokenList)
{
  if (tokenList.atEnd())
    return errorMsg("Extra tokens");

  initRunState();

  lines_.clear();

  lineInd_ = -1;

  expr_ = std::make_unique<CPetBasicExpr>(this);

  // clear data
  dataValues_.clear();

  dataValuePos_ = 0;

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

    if ((stepI > 0 && fromI < toI) || (stepI < 0 && fromI > toI)) { // not at end
      auto lineInd = getLineInd(forData.lineNum_);
      if (lineInd < 0) return errorMsg("Invalid GOTO line");

      lineInd_ = lineInd + 1;
    }
    else { // at end
      forDatas_.erase(varToken->str());

      ++lineInd_;
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

  lineInd_ = lineInd;

  return true;
}

bool
CPetBasic::
pokeStatement(TokenList &tokenList)
{
  Tokens tokens1;

  auto *token = tokenList.nextToken();

  while (token && ! isSeparator(token, SeparatorType::COMMA)) {
    tokens1.push_back(token);

    token = tokenList.nextToken();
  }

  if (! token || ! isSeparator(token, SeparatorType::COMMA))
    return errorMsg("Missing POKE comma");

  Tokens tokens2;

  token = tokenList.nextToken();

  while (token) {
    tokens2.push_back(token);

    token = tokenList.nextToken();
  }

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
  int numOut = 0;

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

    while (token) {
      if      (isSeparator(token, SeparatorType::COMMA)) {
        nextSpacer = Spacer::COMMA;
        break;
      }
      else if (isSeparator(token, SeparatorType::SEMI_COLON)) {
        nextSpacer = Spacer::SEMI_COLON;
        break;
      }

      tokens1.push_back(token);

      token = tokenList.nextToken();
    }

    auto value = evalExpr(tokens1);

    std::string s;
    (void) value->getStringValue(s);

    if      (spacer == Spacer::COMMA)
      printString("\t");
    else if (spacer == Spacer::SEMI_COLON)
      printString(" ");

    printString(s);

    ++numOut;
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
returnStatement(TokenList &tokenList)
{
  if (! tokenList.atEnd())
    return errorMsg("Extra arguments ");

  if (lineStack_.empty())
    return errorMsg("Empty line stack");

  lineInd_ = lineStack_.back();
  lineStack_.pop_back();

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
runStatement(TokenList &)
{
  run();

  return true;
}

bool
CPetBasic::
stopStatement(TokenList &)
{
  stopped_ = true;

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

  stopped_  = false;
  errorMsg_ = "";

  lineInd_ = -1;
}

//---

void
CPetBasic::
printString(const std::string &s) const
{
  for (const auto &c : s) {
    std::cout << c;
  }
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

  auto nt = tokens.size();

  for (uint i = 0; i < nt; ++i) {
    if (str != "")
      str += " ";

    auto *token = tokens[i];

    if      (token->type() == TokenType::STRING)
      str += "\"" + token->toString() + "\"";
    else if (token->type() == TokenType::NUMBER)
      str += token->toString();
    else if (token->type() == TokenType::VARIABLE)
      str += token->toString();
    else
      str += token->exprString();
  }

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

void
CPetBasic::
dimVariable(const std::string &name, const Inds &inds)
{
  auto uname = toUpper(name);

  //std::cerr << "dimVariable: " << uname << " "; printInds(inds); std::cerr << "\n";

  auto *th = const_cast<CPetBasic *>(this);

  if (! hasArrayVariable(uname))
    th->addArrayVariable(uname);

  auto pv = th->arrayVariables_.find(uname);
  assert(pv != th->arrayVariables_.end());

  return (*pv).second.resize(inds);
}

CExprValuePtr
CPetBasic::
getVariableValue(const std::string &name, const Inds &inds)
{
  auto uname = toUpper(name);

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
setVariableValue(const std::string &name, const CExprValuePtr &value)
{
  auto uname = toUpper(name);

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

  if (! var)
    var = expr_->createVariable(uname, val);
  else
    var->setValue(val);

  return true;
}

bool
CPetBasic::
setVariableValue(const std::string &name, const Inds &inds, const CExprValuePtr &value)
{
  auto uname = toUpper(name);

  //std::cerr << "setVariableValue: " << uname << " "; printInds(inds); std::cerr << "\n";

  auto *th = const_cast<CPetBasic *>(this);

  if (! hasArrayVariable(uname))
    th->addArrayVariable(uname);

  auto pv = th->arrayVariables_.find(uname);
  assert(pv != th->arrayVariables_.end());

  return (*pv).second.setValue(inds, value);
}

CExprVariablePtr
CPetBasic::
getVariable(const std::string &name) const
{
  auto uname = toUpper(name);

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

    var = expr_->createVariable(uname, val);
  }

  return var;
}

bool
CPetBasic::
hasArrayVariable(const std::string &uname) const
{
  assert(uname == toUpper(uname));

  auto pv = arrayVariables_.find(uname);

  return (pv != arrayVariables_.end());
}

void
CPetBasic::
addArrayVariable(const std::string &uname)
{
  assert(uname == toUpper(uname));

  ArrayData arrayData;

  arrayData.resize(10);

  arrayVariables_[uname] = arrayData;
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
