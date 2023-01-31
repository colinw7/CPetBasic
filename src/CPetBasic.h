#ifndef CPetBasic_H
#define CPetBasic_H

#include <CExprTypes.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <cassert>

class CPetBasicExpr;

class CPetBasic {
 public:
  enum class TokenType {
    NONE,
    KEYWORD,
    VARIABLE,
    OPERATOR,
    SEPARATOR,
    STRING,
    NUMBER
  };

  enum class KeywordType {
    NONE,
    CLOSE,
    CONT,
    DATA,
    DIM,
    END,
    FOR,
    GET,
    GOSUB,
    GOTO,
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
    REM,
    RESTORE,
    RETURN,
    RUN,
    SAVE,
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

  using uchar = unsigned char;

  using Inds = std::vector<uint>;

 public:
  CPetBasic();

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  CPetBasicExpr *expr() const;

  bool loadFile(const std::string &fileName);

  void list();

  bool run();

  void loop();

  uchar getMemory(uint addr) { return memory_[addr]; }
  void setMemory(uint addr, uchar value) { memory_[addr] = value; }

  std::string keywordName(KeywordType keywordType) const;

  CExprValuePtr getVariableValue(const std::string &name, const Inds &inds);

 private:
  bool replaceEmbedded(const std::string &str1, std::string &str2) const;

  KeywordType lookupKeyword(const std::string &str) const;

  void initKeywords() const;

 private:
  class Token {
   public:
    Token(const CPetBasic *b, const TokenType &type=TokenType::NONE, const std::string &str="") :
     basic_(b), type_(type) {
      str_ = str;
    }

    virtual ~Token() { }

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

    virtual void print(std::ostream &os) const {
      os << toString();
    }

    friend std::ostream &operator<<(std::ostream &os, const Token &token) {
      token.print(os);
      return os;
    }

   protected:
    const CPetBasic* basic_ { nullptr };
    TokenType        type_  { TokenType::NONE };
    std::string      str_;
  };

  class KeywordToken : public Token {
   public:
    KeywordToken(const CPetBasic *b, const KeywordType &keywordType, const std::string &str) :
     Token(b, TokenType::KEYWORD, str), keywordType_(keywordType) {
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

    void print(std::ostream &os) const override {
      if (keywordType_ == KeywordType::REM) {
        os << "[43mREM " << str_ << "[0m";
      }
      else {
        os << "[33m" << basic_->keywordName(keywordType_) << "[0m";

        if (str_ != "")
          os << " " << str_;
      }
    }

   private:
    KeywordType keywordType_;
  };

  class VariableToken : public Token {
   public:
    VariableToken(const CPetBasic *b, const std::string &str) :
     Token(b, TokenType::VARIABLE, str) {
    }

    void print(std::ostream &os) const override {
      os << "[31m" << str_ << "[0m";
    }
  };

  class OperatorToken : public Token {
   public:
    OperatorToken(const CPetBasic *b, const std::string &str) :
     Token(b, TokenType::OPERATOR, str) {
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

    void print(std::ostream &os) const override {
      os << "[32m" << str_ << "[0m";
    }

   private:
    OperatorType operatorType_ { OperatorType::NONE };
  };

  class SeparatorToken : public Token {
   public:
    SeparatorToken(const CPetBasic *b, const std::string &str) :
     Token(b, TokenType::SEPARATOR, str) {
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

    void print(std::ostream &os) const override {
      os << "[34m" << str_ << "[0m";
    }

   private:
    SeparatorType separatorType_ { SeparatorType::NONE };
  };

  class StringToken : public Token {
   public:
    StringToken(const CPetBasic *b, const std::string &str) :
     Token(b, TokenType::STRING, str) {
    }

    std::string listString() const override {
      return "\"" + str_ + "\"";
    }

    void print(std::ostream &os) const override {
      os << "[35m\"" << str_ << "\"[0m";
    }
  };

  class NumberToken : public Token {
   public:
    NumberToken(const CPetBasic *b, const std::string &str) :
     Token(b, TokenType::NUMBER, str) {
      ivalue_ = std::stol(str_);
      rvalue_ = std::stod(str_);

      isReal_ = (rvalue_ != double(ivalue_));
    }

    long   toInteger() const override { return ivalue(); }
    double toReal   () const override { return rvalue(); }
    bool   isReal   () const override { return isReal_; }

    long   ivalue() const { return ivalue_; }
    double rvalue() const { return rvalue_; }

    void print(std::ostream &os) const override {
      os << "[36m" << str_ << "[0m";
    }

   private:
    long   ivalue_ { 0 };
    double rvalue_ { 0.0 };
    bool   isReal_ { false };
  };

  using Tokens = std::vector<Token *>;

  struct Statement {
    Tokens tokens;
  };

  using Statements = std::vector<Statement>;

  struct LineData {
    uint       lineNum { 0 };
    uint       lineN   { 0 };
    Tokens     tokens;
    Statements statements;
  };

  using Lines = std::map<uint, LineData>;

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

    bool atEnd() const { return (it_ >= nt_); }

   private:
    Tokens tokens_;
    uint   it_ { 0 };
    uint   nt_ { 0 };
  };

 private:
  void processLineData(LineData &lineData) const;

  bool parseLine(const std::string &line, uint lineNum, Tokens &tokens) const;

  void printLine(const LineData &lineData) const;

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

  StringToken *createString(const std::string &str) const {
    auto *op = new StringToken(this, str);

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

  bool dataStatement   (const std::string &str);
  bool dimStatement    (TokenList &tokenList);
  bool endStatement    (TokenList &tokenList);
  bool forStatement    (uint lineNum, TokenList &tokenList);
  bool getStatement    (TokenList &tokenList);
  bool gosubStatement  (TokenList &tokenList);
  bool gotoStatement   (TokenList &tokenList);
  bool ifStatement     (int lineN, TokenList &tokenList, bool &nextLine);
  bool inputStatement  (TokenList &tokenList);
  bool letStatement    (TokenList &tokenList);
  bool listStatement   (TokenList &tokenList);
  bool newStatement    (TokenList &tokenList);
  bool nextStatement   (TokenList &tokenList);
  bool onStatement     (TokenList &tokenList);
  bool readStatement   (TokenList &tokenList);
  bool pokeStatement   (TokenList &tokenList);
  bool printStatement  (TokenList &tokenList);
  bool returnStatement (TokenList &tokenList);
  bool restoreStatement(TokenList &tokenList);
  bool runStatement    (TokenList &tokenList);
  bool stopStatement   (TokenList &tokenList);

  //---

  void initRunState();

  //---

  CExprValuePtr evalExpr(const Tokens &tokens) const;
  CExprValuePtr evalExpr(const std::string &str) const;

  bool setVariableValue(const std::string &name, const Inds &inds, const CExprValuePtr &value);
  bool setVariableValue(const std::string &name, const CExprValuePtr &value);

  int getLineInd(uint lineNum) const;

  CExprVariablePtr getVariable(const std::string &name) const;

  //---

  void addDataValue(const CExprValuePtr &value);

  //---

  void warnMsg(const std::string &msg) const;
  bool errorMsg(const std::string &msg);

 private:
  using NameKeywordMap = std::map<std::string, KeywordType>;
  using KeywordNameMap = std::map<KeywordType, std::string>;

  using LineNums = std::vector<uint>;
  using LineInds = std::map<uint, uint>;

  struct ForData {
    std::string   varName_;
    CExprValuePtr toVal_;
    uint          lineNum_ { 0 };

    ForData() { }

    ForData(const std::string &varName, const CExprValuePtr &toVal, uint lineNum) :
     varName_(varName), toVal_(toVal), lineNum_(lineNum) {
    }
  };

  using ForDatas = std::map<std::string, ForData>;

  using LineStack = std::vector<uint>;

  using DataValues = std::vector<CExprValuePtr>;

  using Memory = std::map<uint, uchar>;

  using ExprP = std::unique_ptr<CPetBasicExpr>;

  //---

  std::string fileName_;
  bool        debug_;

  mutable NameKeywordMap nameKeywords_;
  mutable KeywordNameMap keywordNames_;

  Lines lines_;

  LineNums lineNums_;
  LineInds lineInds_;
  int      lineInd_ { -1 };

  ExprP expr_;

  //---

  // Run State

  LineStack lineStack_;
  ForDatas  forDatas_;

  bool stopped_ = false;

  std::string errorMsg_;

  //--

  DataValues dataValues_;
  uint       dataValuePos_ { 0 };

  //--

  Memory memory_;

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
      dims_ = dims;

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
