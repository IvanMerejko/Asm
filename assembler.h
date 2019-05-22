//
// Created by ivan on 10.03.19.
//

#ifndef CURS_ASSEMBLER_H
#define CURS_ASSEMBLER_H
//
// Created by ivan on 10.03.19.
//

#include <string>
#include <vector>
#include <algorithm>
#include <string_view>
#include <set>
#include <memory>
#include <map>
#include <numeric>
#include "Types.h"
namespace assembler{


    template  <typename T>
    std::string getHexCodForValue(T value){
        auto ptr = (unsigned char *) &value;
        std::stringstream stream;
        int oneSymbol;
        for (int i = 0; i < 4; i++){
            oneSymbol = *(ptr + i);
            stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << oneSymbol ;
        }
        std::string hexCode;
        stream >> hexCode;

        return hexCode;
    }
    std::string fromDecToBinary(int value);
    std::string  getOpCod(const std::string& reg);
    int fromHexToDec(const std::string& value);
    std::string  fromDecToHex(int value);
    int fromBinaryToDec(const std::string& value);
    struct identifier {
    public:
        friend bool operator<(const identifier &lft, const identifier &rht);

    public:
        int address;
        IdentifierType type;
        std::string name;
        std::string value;
        std::string valueInDec;

        identifier(std::string name, IdentifierType type, std::string value , int _address = 0)
                : name{std::move(name)}, type{type}, value{std::move(value)} , valueInDec{} , address{_address} {};

        identifier(std::string name, IdentifierType type)
                : name{std::move(name)}, type{type}, value{""} {};

        void setValue(const std::string &value);
        std::string getName() const;
        std::string getValue() const;
        std::string getValueInDec() const;
        IdentifierType getType() const;

        int  getNumberOfByte(size_t) const;
        bool isCorrectIdentifierValue() ;
        bool isCorrectRangesForType(IdentifierType type , const std::string& value) const;
        void print() const;

    };
    struct label{
        friend bool operator <(const label& lft , const label& rht);
        std::string name;
        int position;
        label(std::string name , int pos)
                :name{std::move(name)} , position{pos}{};
        void addValueToPosition(int value);
    };
    std::string getNameOfIdentifierType(const identifier& cur_ident);
    struct segment{

    protected:
        std::set<identifier> identifiers;
        bool isCreate = false;
    public:
        virtual bool isOpen() const;
        virtual void open();
        virtual bool push(identifier&& identifier1);
        identifier getIdentifier(const std::string& name) const;
        virtual void printIdentifiers();
        virtual bool isDeclaredIdentifier(const std::string& identifier)const;
        virtual size_t getIdentifierNumbersOfByte(const std::string& id);
        const std::set<identifier>&  getAllIdentifiers() const ;
    };
    struct data : public segment{
    };
    struct code : public segment{
    private:
        std::set<label> labels;
    public:
        code() = default;
        bool pushLabel(label&& label) ;
        bool isLabelDeclared(const std::string& label_name) const;
        void changePositionOfAllLabels (int start , int value);
        const label& getLabelByName(const std::string& label_name) const;
    };

    bool isDeclaredIdentifier(const std::string& identifier , const data& _data , const code& _code);

    const  stringsVector commandsVector();
    const  stringsVector directivesVector();
    const  stringsVector instructionsVector();
    const  stringsVector registers32Vector();
    const  stringsVector registers16Vector();
    const  stringsVector registers8Vector();
    const  stringsVector segmentRegisters();

    bool isWordInVector(const stringsVector &vector, std::string_view word);
    bool isCommand(std::string_view word);
    bool isDirective(std::string_view word);
    bool isInstruction(std::string_view word);
    bool isIdentifier(std::string_view word);
    bool isRegister(const std::string& string);
    bool isReservedWord(std::string_view word);


    void createVectorOfWordsFromString(const std::string&  string , stringsVector& wordsInString)  ;





    size_t directivePosition(std::string_view directive);
    IdentifierType identifierType(std::string_view identifier_type);







    void removeSpacesAndTabs(std::string& string);
    WordType getTypeOfOperand(const std::string&  operand );

    void syntAnaliser(std::ostream& os , const lexem_type& vectorLexems);
    lexem_type  lexemParsing(const stringsVector& vectorOfOperands );

    int  getNumberOfByteForAddressExpr(const stringsVector& exp);


    void splitByDelimiters(const std::string& delimiters  , stringsVector & vector);
    bool isCorrectAddressExpression(const stringsVector& operands , size_t line);
    bool isCorrectExpressionBetweenParentheses(const stringsVector& expressionOperands);
    class Command{
    protected:
        stringsVector operands;
    public:
        Command() = default;
        explicit Command(const std::string& string){
            std::string help{string};
            removeSpacesAndTabs(help);
            createVectorOfWordsFromString(help , operands);
            splitByDelimiters("{}," , operands);
        };
        virtual ~Command() = default;
        virtual bool isCorrectOperands(size_t line) = 0;
        virtual int  getNumberOfByte(size_t) = 0;
        virtual std::string getBites(const data& data_segment , const code& code_segment , int address = 0) = 0;
        //virtual std::string createByteCode() = 0;

    };
    class Mov : public Command{
    public:
        Mov() = default;
        explicit Mov(const std::string& string)
            :Command(string){};
        ~Mov() override = default ;
        bool isCorrectOperands(size_t line) override ;
        bool isCorrectFirstOperand();
        bool isCorrectSecondOperand();
        int  getNumberOfByte(size_t) override ;
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class Imul : public Command{
    public:
        Imul() = default;
        explicit Imul(const std::string& string)
                :Command(string){};
        ~Imul() override = default ;
        bool isCorrectOperands(size_t line) override;
        //std::string createByteCode() override ;
        int  getNumberOfByte(size_t) override ;
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class Idiv : public Command{
    public:
        Idiv() = default;
        explicit Idiv(const std::string& string)
                :Command(string){
            splitByDelimiters("[]*:" , operands);
        };
        ~Idiv() override = default ;
        bool isCorrectOperands(size_t line) override;
        int  getNumberOfByte(size_t) override ;
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class Or : public Command{
    public:
        Or() = default;
        explicit Or(const std::string& string)
                :Command(string){};
        ~Or() override = default ;
        bool isCorrectOperands(size_t line) override;
        int  getNumberOfByte(size_t) override ;
        bool isCorrectFirstOperand();
        bool isCorrectSecondOperand();
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class Cmp : public Command{
    public:
        Cmp() = default;
        explicit Cmp(const std::string& string)
                :Command(string){};
        ~Cmp() override = default ;
        bool isCorrectOperands(size_t line) override;
        int  getNumberOfByte(size_t) override ;
        bool isCorrectFirstOperand();
        bool isCorrectSecondOperand(size_t line);
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class Jng : public Command{
    public:
        Jng() = default;
        explicit Jng(const std::string& string)
                :Command(string){};
        ~Jng() override = default ;
        bool isCorrectOperands(size_t line) override ;
        int  getNumberOfByte(size_t) override ;
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class And : public Command{
    public:
        And() = default;
        explicit And(const std::string& string)
                :Command(string){};
        ~And() override = default ;
        bool isCorrectOperands(size_t line) override;
        int  getNumberOfByte(size_t) override ;
        bool isCorrectFirstOperand(size_t line);
        bool isCorrectSecondOperand();
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class Add : public Command{
    public:
        Add() = default;
        explicit Add(const std::string& string)
                :Command(string){};
        ~Add() override = default ;
        bool isCorrectOperands(size_t line) override;
        int  getNumberOfByte(size_t) override ;
        bool isCorrectFirstOperand(size_t line);
        bool isCorrectSecondOperand();
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;

    };
    class Cwde : public Command{
    public:
        Cwde() = default;
        explicit Cwde(const std::string& string)
                :Command(string){};
        ~Cwde() override = default ;
        bool isCorrectOperands(size_t line) override ;
        int  getNumberOfByte(size_t) override ;
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };
    class Model : public Command{
    public:
        Model() = default;
        explicit Model(const std::string& string)
                :Command(string){};
        ~Model() override = default ;
        bool isCorrectOperands(size_t line) override ;
        int  getNumberOfByte(size_t) override ;
        std::string getBites(const data& data_segment , const code& code_segment , int address = 0) override ;
    };


    std::unique_ptr<Command> getPointerForCommandByName(std::string_view command , const std::string& operands);

    namespace userIdentifiers{
        using  mapOfIdentifiers = std::map<std::string , std::vector<size_t>>;
        using  mapOfAdressExpression = std::map<size_t , std::string >;



        mapOfAdressExpression& getMapOfAdressExpression();
        void pushAdrExpr(size_t line ,const std::string& expr);


        mapOfIdentifiers& getUsedLabels();
        mapOfIdentifiers& getUsedIdentifiers();

        static void pushLabel(const std::string& name , size_t line);
        static void pushIdentifier(const std::string& name , size_t line);

    }





}
#endif //CURS_ASSEMBLER_H
