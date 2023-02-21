#ifndef CPetBasic_H
#define CPetBasic_H

#include <CExprTypes.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <memory>
#include <cassert>

class CPetBasicExpr;
class CPetBasicTerm;

using uchar = unsigned char;

//---

class CPetBasic;

enum class CPetBasicTokenType {
  NONE,
  KEYWORD,
  VARIABLE,
  OPERATOR,
  SEPARATOR,
  STRING,
  NUMBER
};

class CPetBasicToken {
 public:
  using TokenType = CPetBasicTokenType;

 public:
  CPetBasicToken(const CPetBasic *b, const TokenType &type=TokenType::NONE,
                 const std::string &str="");

  virtual ~CPetBasicToken() { }

  const TokenType &type() const { return type_; }

  const std::string &str() const { return str_; }

  virtual std::string toString() const {
    return str_;
  }

  virtual std::string listString() const {
    return str_;
  }

  virtual std::string exprString() const {
    return str_;
  }

  virtual long   toInteger() const { return 0; }
  virtual double toReal   () const { return 0.0; }
  virtual bool   isReal   () const { return false; }

  virtual void printEsc(std::ostream &os) const {
    os << toString();
  }

  virtual void print(std::ostream &os) const {
    os << toString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CPetBasicToken &token) {
    token.print(os);
    return os;
  }

 protected:
  const CPetBasic* basic_ { nullptr };
  TokenType        type_  { TokenType::NONE };
  std::string      str_;
};

//---

class CPetsciChar {
 public:
  CPetsciChar() { }

  explicit CPetsciChar(uchar c) :
   c_(c) {
  }

  uchar c() const { return c_; }

  void shift() { assert(c_ < 196); c_ += 64; }

  bool isReversed() const { return c_ >= 128; }

  void reverse() { assert(! isReversed()); c_ += 128; }
  void unreverse() { assert(isReversed()); c_ -= 128; }

 private:
  uchar c_ { 0 };

};

class CAsciiChar {
 public:
  CAsciiChar() { }

  explicit CAsciiChar(uchar c) :
   c_(c) {
  }

  uchar c() const { return c_; }

 private:
  uchar c_ { 0 };
};

class CPetDrawChar {
 public:
  CPetDrawChar() { }

  explicit CPetDrawChar(uchar c, ulong utf=0, bool reverse=false) :
   c_(c), utf_(utf), reverse_(reverse) {
  }

  bool isSet() const { return c_ != 0 || utf_ != 0; }

  uchar c() const { return c_; }
  void setC(uchar c) { c_ = c; }

  ulong utf() const { return utf_; }
  void setUtf(ulong utf) { utf_ = utf; }

  bool isReverse() const { return reverse_; }
  void setReverse(bool b) { reverse_ = b; }

 private:
  uchar c_       { 0 };     // ascii char to draw
  ulong utf_     { 0 };     // utf-8 char to draw
  bool  reverse_ { false }; // is reversed
};

//---

class CPetBasic {
 public:
  using TokenType = CPetBasicTokenType;

  enum class KeywordType {
    NONE,
    APPEND,
    BACKUP,
    CLOSE,
    CLR,
    CMD,
    COLLECT,
    CONCAT,
    CONT,
    COPY,
    DATA,
    DCLOSE,
    DEF,
    DELAY, // custom command
    DIM,
    DIRECTORY,
    DLOAD,
    DOPEN,
    DSAVE,
    END,
    FN,
    FOR,
    GET,
    GOSUB,
    GOTO,
    HEADER,
    IF,
    INPUT,
    LET,
    LIST,
    LOAD,
    NEW,
    NEXT,
    ON,
    OPEN,
    PEEK,
    POKE,
    PRINT,
    READ,
    RECORD,
    REM,
    RENAME,
    RESTORE,
    RETURN,
    RUN,
    SAVE,
    SCRATCH,
    STEP,
    STOP,
    SYS,
    THEN,
    TO,
    USR,
    VERIFY,
    WAIT
  };

  enum class SeparatorType {
    NONE,
    COLON,
    COMMA,
    SEMI_COLON,
    OPEN_RBRACKET,
    CLOSE_RBRACKET
  };

  enum class OperatorType {
    NONE,
    HASH,
    ASSIGN,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    POWER,
    LESS,
    LESS_EQUAL,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    OR,
    AND,
    NOT
  };

  using Inds = std::vector<uint>;

  using Token  = CPetBasicToken;
  using Tokens = std::vector<Token *>;

  struct Statement {
    Tokens tokens;
  };

  using Statements = std::vector<Statement>;

  struct LineData {
    std::string line;
    uint        lineNum { 0 };
    uint        lineN   { 0 };
    Tokens      tokens;
    Statements  statements;
  };

  using Lines = std::map<uint, LineData>;

  struct FunctionData {
    std::string              name;
    std::vector<std::string> args;
    Tokens                   tokens;
  };

  using Functions = std::map<std::string, FunctionData>;
 public:
  CPetBasic();

  virtual ~CPetBasic();

  //---

  bool isListHighlight() const { return listHighlight_; }
  void setListHighlight(bool b) { listHighlight_ = b; }

  bool isSplitStatements() const { return splitStatements_; }
  void setSplitStatements(bool b) { splitStatements_ = b; }

  // get/set debug
  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  //---

  CPetBasicTerm *term() const { return term_; }
  void setTerm(CPetBasicTerm *term);

  //---

  CPetBasicExpr *expr() const;

  //---

  void initExpr();

  bool loadFile(const std::string &fileName);

  void list();

  bool run();

  bool step();

  bool contRun();
  bool contRunTo(uint lineNum);

  void setRaw(bool b);

  void loop();

  bool inputLine(const std::string &lineBuffer);

  int lineInd() const { return lineInd_; }

  int currentLineNum() const;

  int lineIndNum(int lineInd) const;

  //---

  bool isStopped() const { return stopped_; }
  virtual void setStopped(bool b);

  bool isReverse() const { return reverse_; }
  virtual void setReverse(bool b);

  bool isShift() const { return shift_; }
  virtual void setShift(bool b);

  //---

  virtual void resize(uint nr, uint nc);

  uint numRows() const { return nr_; }
  uint numCols() const { return nc_; }

  uchar getMemory(uint addr) const;
  void setMemory(uint addr, uchar value);

  virtual bool getScreenMemory(uint r, uint c, uchar& value) const;
  virtual void setScreenMemory(uint r, uint c, uchar value);

  //---

  virtual void notifyRunLine(uint /*n*/) const { }

  //---

  std::string keywordName(KeywordType keywordType) const;

  //---

  uint numLines() const;
  uint maxLine() const;

  const Lines &getLines() const { return lines_; }

  const LineData *getLineIndData(uint lineInd) const;

  std::string lineToString(const LineData &lineData, bool highlight) const;

  virtual void notifyLinesChanged() { }

  virtual void notifyLineNumChanged() { }

  //---

  CExprVariablePtr addVariable(const std::string &name, const CExprValuePtr &value);

  CExprVariablePtr getVariable(const std::string &name) const;

  CExprValuePtr getVariableValue(const std::string &name) const;
  bool setVariableValue(const std::string &name, const CExprValuePtr &value);

  //---

  void dimVariable(const std::string &name, const Inds &inds);

  bool hasArrayVariable(const std::string &name) const;

  void addArrayVariable(const std::string &name, int ndim);

  CExprValuePtr getVariableValue(const std::string &name, const Inds &inds);
  bool setVariableValue(const std::string &name, const Inds &inds, const CExprValuePtr &value);

  void clearArrayVariables();

  //---

  void getVariableNames(std::vector<std::string> &names,
                        std::vector<std::string> &arrayNames) const;

  virtual void notifyVariablesChanged() { }

  //---

  void defineFunction(const std::string &fnName, const std::vector<std::string> &args,
                      const Tokens &tokens);

  bool getFunction(const std::string &fnName, FunctionData &data) const;

  //---

  static CPetsciChar drawCharToPet(const CPetDrawChar &drawChar);
  static CPetDrawChar petToDrawChar(const CPetsciChar &pet);

  static std::string decodeEmbeddedStr(const std::string &s);
  static std::string decodeEmbeddedChar(uchar c);

 private:
  bool replaceEmbedded(const std::string &str1, std::string &str2) const;

  std::string encodeEmbedded(const std::string &name) const;

  KeywordType lookupKeyword(const std::string &str) const;

  void initKeywords() const;

 private:
  class KeywordToken : public CPetBasicToken {
   public:
    KeywordToken(const CPetBasic *b, const KeywordType &keywordType, const std::string &str) :
     CPetBasicToken(b, TokenType::KEYWORD, str), keywordType_(keywordType) {
    }

    const KeywordType &keywordType() const { return keywordType_; }

    std::string exprString() const override {
      return basic_->keywordName(keywordType_);
    }

    std::string listString() const override {
      std::string str = exprString();

      if (str_ != "")
        str += " " + str_;

      return str;
    }

    void printEsc(std::ostream &os) const override {
      if (keywordType_ == KeywordType::REM) {
        os << "\033[43mREM " << str_ << "\033[0m";
      }
      else {
        os << "\033[33m" << basic_->keywordName(keywordType_) << "\033[0m";

        if (str_ != "")
          os << " " << str_;
      }
    }

    void print(std::ostream &os) const override {
      if (keywordType_ == KeywordType::REM) {
        os << "REM " << str_;
      }
      else {
        os << basic_->keywordName(keywordType_);

        if (str_ != "")
          os << " " << str_;
      }
    }

   private:
    KeywordType keywordType_;
  };

  class VariableToken : public CPetBasicToken {
   public:
    VariableToken(const CPetBasic *b, const std::string &str) :
     CPetBasicToken(b, TokenType::VARIABLE, str) {
    }

    void printEsc(std::ostream &os) const override {
      os << "\033[31m" << str_ << "\033[0m";
    }

    void print(std::ostream &os) const override {
      os << str_;
    }
  };

  class OperatorToken : public CPetBasicToken {
   public:
    OperatorToken(const CPetBasic *b, const std::string &str) :
     CPetBasicToken(b, TokenType::OPERATOR, str) {
      if      (str_ == "#"  ) operatorType_ = OperatorType::HASH;
      else if (str_ == "="  ) operatorType_ = OperatorType::ASSIGN;
      else if (str_ == "+"  ) operatorType_ = OperatorType::PLUS;
      else if (str_ == "-"  ) operatorType_ = OperatorType::MINUS;
      else if (str_ == "*"  ) operatorType_ = OperatorType::TIMES;
      else if (str_ == "/"  ) operatorType_ = OperatorType::DIVIDE;
      else if (str_ == "^"  ) operatorType_ = OperatorType::POWER;
      else if (str_ == "<"  ) operatorType_ = OperatorType::LESS;
      else if (str_ == "<=" ) operatorType_ = OperatorType::LESS_EQUAL;
      else if (str_ == ">"  ) operatorType_ = OperatorType::GREATER;
      else if (str_ == ">=" ) operatorType_ = OperatorType::GREATER_EQUAL;
      else if (str_ == "<>" ) operatorType_ = OperatorType::NOT_EQUAL;
      else if (str_ == "OR" ) operatorType_ = OperatorType::OR;
      else if (str_ == "AND") operatorType_ = OperatorType::AND;
      else if (str_ == "NOT") operatorType_ = OperatorType::NOT;
      else std::cerr << "Invalid Op: " << str << "\n";
    }

    const OperatorType &operatorType() const { return operatorType_; }

    void printEsc(std::ostream &os) const override {
      os << "\033[32m" << str_ << "\033[0m";
    }

    void print(std::ostream &os) const override {
      os << str_;
    }

   private:
    OperatorType operatorType_ { OperatorType::NONE };
  };

  class SeparatorToken : public CPetBasicToken {
   public:
    SeparatorToken(const CPetBasic *b, const std::string &str) :
     CPetBasicToken(b, TokenType::SEPARATOR, str) {
      assert(str_.size() == 1);

      switch (str_[0]) {
        case ':': separatorType_ = SeparatorType::COLON; break;
        case ',': separatorType_ = SeparatorType::COMMA; break;
        case ';': separatorType_ = SeparatorType::SEMI_COLON; break;
        case '(': separatorType_ = SeparatorType::OPEN_RBRACKET; break;
        case ')': separatorType_ = SeparatorType::CLOSE_RBRACKET; break;
        default: assert(false); break;
      }
    }

    const SeparatorType &separatorType() const { return separatorType_; }

    void printEsc(std::ostream &os) const override {
      os << "\033[34m" << str_ << "\033[0m";
    }

    void print(std::ostream &os) const override {
      os << str_;
    }

   private:
    SeparatorType separatorType_ { SeparatorType::NONE };
  };

  class StringToken : public CPetBasicToken {
   public:
    StringToken(const CPetBasic *b, const std::string &str, bool embedded) :
     CPetBasicToken(b, TokenType::STRING, str), embedded_(embedded) {
    }

    bool isEmbedded() { return embedded_; }

    std::string listString() const override {
      return "\"" + str_ + "\"";
    }

    void printEsc(std::ostream &os) const override {
      os << "\033[35m\"" << str_ << "\"\033[0m";
    }

    void print(std::ostream &os) const override {
      os << "\"" << str_ << "\"";
    }

   private:
    bool embedded_ { false };
  };

  class NumberToken : public CPetBasicToken {
   public:
    NumberToken(const CPetBasic *b, const std::string &str) :
     CPetBasicToken(b, TokenType::NUMBER, str) {
      rvalue_ = std::stod(str_);

      try {
        ivalue_ = std::stol(str_);
        isReal_ = (rvalue_ != double(ivalue_));
      }
      catch (...) {
        ivalue_ = int(rvalue_);
        isReal_ = true;
      }
    }

    long   toInteger() const override { return ivalue(); }
    double toReal   () const override { return rvalue(); }
    bool   isReal   () const override { return isReal_; }

    long   ivalue() const { return ivalue_; }
    double rvalue() const { return rvalue_; }

    void printEsc(std::ostream &os) const override {
      os << "\033[36m" << str_ << "\033[0m";
    }

    void print(std::ostream &os) const override {
      os << str_;
    }

   private:
    long   ivalue_ { 0 };
    double rvalue_ { 0.0 };
    bool   isReal_ { false };
  };

  class TokenList {
   public:
    TokenList(const Tokens &tokens) :
     tokens_(tokens) {
      nt_ = uint(tokens_.size());
    }

    void skipToken() { ++it_; }

    Token *nextToken() {
      if (atEnd()) return nullptr;
      return tokens_[it_++];
    }

    Token *prevToken() {
      if (atStart()) return nullptr;
      return tokens_[--it_];
    }

    Token *currentToken() {
      if (atEnd()) return nullptr;
      return tokens_[it_];
    }

    bool atStart() const { return (it_ == 0); }
    bool atEnd  () const { return (it_ >= nt_); }

   private:
    Tokens tokens_;
    uint   it_ { 0 };
    uint   nt_ { 0 };
  };

  struct LineRef {
    uint lineNum { 0 };
    uint statementNum { 0 };

    LineRef() { }

    LineRef(uint lineNum_, uint statementNum_=0) :
     lineNum(lineNum_), statementNum(statementNum_) {
    }

    int cmp(const LineRef &lineRef) const {
      if (lineNum < lineRef.lineNum) return -1;
      if (lineNum > lineRef.lineNum) return  1;
      if (statementNum < lineRef.statementNum) return -1;
      if (statementNum > lineRef.statementNum) return  1;
      return 0;
    }

    friend bool operator<(const LineRef &lhs, const LineRef &rhs) {
      return (lhs.cmp(rhs) < 0);
    }

    friend bool operator>(const LineRef &lhs, const LineRef &rhs) {
      return (lhs.cmp(rhs) > 0);
    }

    friend bool operator==(const LineRef &lhs, const LineRef &rhs) {
      return (lhs.cmp(rhs) == 0);
    }

    std::string toString() const {
     return std::to_string(lineNum) + ":" + std::to_string(statementNum);
    }
  };

 private:
  void processLineData(LineData &lineData) const;

  bool parseLineTokens(const std::string &line, LineData &lineData) const;

  void listLine(const LineData &lineData) const;

  std::string statementToString(const Statement &statement, char lastChar, bool highlight) const;

  //---

  KeywordToken *createKeyword(KeywordType keywordType, const std::string &str) const {
    auto *keyword = new KeywordToken(this, keywordType, str);

    return keyword;
  }

  VariableToken *createVariable(const std::string &str) const {
    auto *variable = new VariableToken(this, str);

    return variable;
  }

  OperatorToken *createOperator(const std::string &str) const {
    auto *op = new OperatorToken(this, str);

    return op;
  }

  SeparatorToken *createSeparator(const std::string &str) const {
    auto *sep = new SeparatorToken(this, str);

    return sep;
  }

  StringToken *createString(const std::string &str, bool embedded) const {
    auto *op = new StringToken(this, str, embedded);

    return op;
  }

  NumberToken *createNumber(const std::string &str) const {
    auto *op = new NumberToken(this, str);

    return op;
  }

  Token *createToken(const std::string &str) const {
    auto *token = new Token(this, TokenType::NONE, str);

    return token;
  }

  //---

  bool isKeyword(Token *token, const KeywordType &keywordType) const {
    if (token->type() != TokenType::KEYWORD) return false;

    auto *keyword = static_cast<KeywordToken *>(token);

    return (keyword->keywordType() == keywordType);
  }

  bool isSeparator(Token *token, const SeparatorType &separatorType) const {
    if (token->type() != TokenType::SEPARATOR) return false;

    auto *separator = static_cast<SeparatorToken *>(token);

    return (separator->separatorType() == separatorType);
  }

  bool isOperator(Token *token, const OperatorType &operatorType) const {
    if (token->type() != TokenType::OPERATOR) return false;

    auto *op = static_cast<OperatorToken *>(token);

    return (op->operatorType() == operatorType);
  }

  //---

  bool parseLine(uint lineNum, const std::string &line, LineData &lineData) const;

  bool runLine(const LineData &lineData);
  bool runTokens(int lineN, const Tokens &tokens, bool &nextLine);

  void addData(const std::string &dataStr) const;

  //---

  bool appendStatement   (TokenList &tokenList);
  bool backupStatement   (TokenList &tokenList);
  bool closeStatement    (TokenList &tokenList);
  bool clrStatement      (TokenList &tokenList);
  bool cmdStatement      (TokenList &tokenList);
  bool collectStatement  (TokenList &tokenList);
  bool concatStatement   (TokenList &tokenList);
  bool contStatement     (TokenList &tokenList);
  bool copyStatement     (TokenList &tokenList);
  bool dataStatement     (const std::string &str);
  bool dcloseStatement   (TokenList &tokenList);
  bool defStatement      (TokenList &tokenList);
  bool delayStatement    (TokenList &tokenList);
  bool dimStatement      (TokenList &tokenList);
  bool directoryStatement(TokenList &tokenList);
  bool dloadStatement    (TokenList &tokenList);
  bool dopenStatement    (TokenList &tokenList);
  bool dsaveStatement    (TokenList &tokenList);
  bool endStatement      (TokenList &tokenList);
  bool forStatement      (const LineRef &lineRef, TokenList &tokenList);
  bool getStatement      (TokenList &tokenList);
  bool gosubStatement    (TokenList &tokenList);
  bool gotoStatement     (TokenList &tokenList);
  bool headerStatement   (TokenList &tokenList);
  bool ifStatement       (const LineRef &lineRef, TokenList &tokenList, bool &nextLine);
  bool inputStatement    (TokenList &tokenList);
  bool letStatement      (TokenList &tokenList);
  bool listStatement     (TokenList &tokenList);
  bool loadStatement     (TokenList &tokenList);
  bool newStatement      (TokenList &tokenList);
  bool nextStatement     (const LineRef &lineRef, TokenList &tokenList);
  bool onStatement       (TokenList &tokenList);
  bool openStatement     (TokenList &tokenList);
  bool pokeStatement     (TokenList &tokenList);
  bool printStatement    (TokenList &tokenList);
  bool readStatement     (TokenList &tokenList);
  bool recordStatement   (TokenList &tokenList);
  bool renameStatement   (TokenList &tokenList);
  bool restoreStatement  (TokenList &tokenList);
  bool returnStatement   (TokenList &tokenList);
  bool runStatement      (TokenList &tokenList);
  bool saveStatement     (TokenList &tokenList);
  bool scratchStatement  (TokenList &tokenList);
  bool stopStatement     (TokenList &tokenList);
  bool sysStatement      (TokenList &tokenList);
  bool verifyStatement   (TokenList &tokenList);
  bool waitStatement     (TokenList &tokenList);

  //---

  bool readVariable(TokenList &tokenList, const std::string &id,
                    std::string &varName, Inds &inds) const;

  void printInds(const Inds &inds) const;

  //---

  void gotoLine(const LineRef &lineRef);

  void pushLine(int lineNum);
  bool popLine();

  //---

  void removeForData(uint ind);

  //---

  void initRunData();
  void initRunState();

  void setLineInd(int lineInd, int statementNum);

  //---

 public:
  virtual void printString(const std::string &s) const;

  //---

 private:
  bool evalExpr(const Tokens &tokens, CExprValuePtr &value) const;
  bool evalExpr(const std::string &str, CExprValuePtr &value) const;

  int getLineInd(uint lineNum) const;

  //---

  void addDataValue(const CExprValuePtr &value);

  //---

 public:
  void warnMsg(const std::string &msg) const;
  bool errorMsg(const std::string &msg) const;

 private:
  using NameKeywordMap = std::map<std::string, KeywordType>;
  using KeywordNameMap = std::map<KeywordType, std::string>;

  using LineNums = std::vector<uint>;
  using LineInds = std::map<uint, uint>;

  class ForData {
   public:
    ForData() { }

    ForData(const std::string &varName, const CExprValuePtr &toVal,
            const CExprValuePtr &stepVal, const LineRef &lineRef,
            const LineRef &nextLineRef) :
     varName_(varName), toVal_(toVal), stepVal_(stepVal), lineRef_(lineRef),
     nextLineRef_(nextLineRef) {
    }

    const std::string &varName() const { return varName_; }

    const CExprValuePtr &toVal() const { return toVal_; }

    const CExprValuePtr &stepVal() const { return stepVal_; }

    const LineRef &lineRef() const { return lineRef_; }

    int lineNum     () const { return lineRef_.lineNum; }
    int statementNum() const { return lineRef_.statementNum; }

    const LineRef &nextLineRef() const { return nextLineRef_; }

    int nextLineNum     () const { return nextLineRef_.lineNum; }
    int nextStatementNum() const { return nextLineRef_.statementNum; }

   private:
    std::string   varName_;
    CExprValuePtr toVal_;
    CExprValuePtr stepVal_;
    LineRef       lineRef_;
    LineRef       nextLineRef_;
  };

  using ForNames = std::map<std::string, int>;
  using ForDatas = std::vector<ForData>;

  using LineStack = std::vector<LineRef>;

  //---

  struct ForRefData {
    LineRef     line;
    std::string varName;

    ForRefData() { }

    ForRefData(const LineRef &line_) : line(line_) { }
  };

  mutable std::vector<ForRefData> forStack_;
  mutable std::vector<ForRefData> nextStack_;

  //---

  using DataValues = std::vector<CExprValuePtr>;

  using Memory = std::map<uint, uchar>;

  using ExprP = std::unique_ptr<CPetBasicExpr>;

  //---

  std::string fileName_;
  bool        debug_           { false };
  bool        listHighlight_   { false };
  bool        splitStatements_ { false };

  mutable NameKeywordMap nameKeywords_;
  mutable KeywordNameMap keywordNames_;

  Lines lines_;

  LineNums lineNums_;
  LineInds lineInds_;
  int      lineInd_ { -1 };

  uint statementNum_ { 0 };

  int breakLineNum_ { -1 };

  //---

  CPetBasicTerm *term_ { nullptr };

  //---

  ExprP expr_;

  //---

  // Run State

  bool      runDataValid_ { false };
  LineStack lineStack_;
  ForDatas  forDatas_;

  bool stopped_ { false };
  bool reverse_ { false };
  bool shift_   { false };

  mutable std::string errorMsg_;

  //--

  DataValues dataValues_;
  uint       dataValuePos_ { 0 };

  //--

  uint nr_ { 25 };
  uint nc_ { 40 };

  Memory memory_;

  //---

  using VariableNames = std::set<std::string>;

  VariableNames variableNames_;

  //---


  Functions functions_;

  //---

  class ArrayData {
   public:
    using Dims = std::vector<uint>;

   public:
    ArrayData() { }

    void resize(uint n) {
      Dims dims;
      dims.push_back(n);
      resize(dims);
    }

    void resize(const Dims &dims) {
      dims_.clear();

      for (const auto &dim : dims)
        dims_.push_back(dim + 1); // 0 .. N

      uint n = 1;

      for (const auto &dim : dims_)
        n *= dim;

      values_.resize(n);
    }

    CExprValuePtr value(uint i) {
      Inds inds; inds.push_back(i);
      return value(inds);
    }

    CExprValuePtr value(const Inds &inds) {
      auto n = inds.size();
      assert(n > 0);

      if (n != dims_.size())
        return CExprValuePtr();

      uint i1 = 0;

      for (uint i = uint(n - 1); i > 1; --i) {
        i1 += inds[i]*dims_[i + 1];
      }

      i1 += inds[n - 1];

      if (i1 >= values_.size())
        return CExprValuePtr();

      return values_[i1];
    }

    bool setValue(uint i, const CExprValuePtr &value) {
      Inds inds; inds.push_back(i);
      return setValue(inds, value);
    }

    bool setValue(const Inds &inds, const CExprValuePtr &value) {
      auto n = inds.size();
      assert(n > 0);

      if (n != dims_.size())
        return false;

      uint i1 = 0;

      for (uint i = uint(n - 1); i > 1; --i) {
        i1 += inds[i]*dims_[i + 1];
      }

      i1 += inds[n - 1];

      if (i1 >= values_.size())
        return false;

      values_[i1] = value;

      return true;
    }

   private:
    CExprValueArray values_;
    Dims            dims_;
  };

  using ArrayVariables = std::map<std::string, ArrayData>;

  ArrayVariables arrayVariables_;
};

#endif
