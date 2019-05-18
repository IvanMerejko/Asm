#include <string>
#include <vector>
#include <algorithm>
#include <string_view>
#include <set>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "assembler.h"
#include <map>
namespace assembler{

    std::string getHexCodForValue(int value){
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

    /*
     *  TODO
     *
     *  address expresion numbers of byte
     *
     * */
    int getOpCod(const std::string& reg){
        std::string tmp{reg};
        std::transform(tmp.begin() , tmp.end() , tmp.begin() , ::tolower);
        static std::map<std::string , int> map{
            {"al" , 0} , {"ax" , 0} , {"eax" , 0} ,
            {"cl" , 1} , {"cx" , 1} , {"ecx" , 1} ,
            {"dl" , 2} , {"dx" , 2} , {"edx" , 2} ,
            {"bl" , 3} , {"bx" , 3} , {"ebx" , 3} ,
            {"ah" , 4} , {"sp" , 4} , {"esp" , 4} ,
            {"ch" , 5} , {"bp" , 5} , {"ebp" , 5} ,
            {"dh" , 6} , {"si" , 6} , {"esi" , 6} ,
            {"bh" , 7} , {"di" , 7} , {"edi" , 7} ,

        };
        return map[reg];
    }
    std::string fromDecToHex(int value){
        switch (value){
            case 1:
                return "1";
            case 2:
                return "2";
            case 3:
                return "3";
            case 4:
                return "4";
            case 5:
                return "5";
            case 6:
                return "6";
            case 7:
                return "7";
            case 8:
                return "8";
            case 9:
                return "9";
            case 10:
                return "A";
            case 11:
                return "B";
            case 12:
                return "C";
            case 13:
                return "D";
            case 14:
                return "E";
            case 15:
                return "F";

        }
    }
    int fromBinaryToDec(const std::string& value){
        return std::stoi(value , nullptr , 2);
    }
    int fromHexToDec(const std::string& value){
        return std::stoi(value , nullptr , 16);
    }

    void identifier::setValue(const std::string &value) {
        this->value = value;
    }
    std::string identifier::getName() const {
        return name;
    }
    std::string identifier::getValue() const {
        return value;
    }
    int identifier::getNumberOfByte(size_t line) const {
        switch (type){
            case assembler::IdentifierType::DB:
                return 1;
            case IdentifierType::DW:
                return 2;
            case IdentifierType::DD:
                return 3;
        }
    }
    std::string getNameOfIdentifierType(const identifier& cur_ident){
        switch (cur_ident.getType()){
            case IdentifierType::DB:
                return "DB";
            case IdentifierType::DW:
                return "DW";
            case IdentifierType::DD:
                return "DD";
        }
    }
    double identifier::getValueToInt() const {
        auto lastLetter = static_cast <char> ( std::tolower(static_cast<unsigned char> (value.back())));
        switch (lastLetter) {
            case 'h':

                return std::stod(value.substr(0 , value.size() - 1) , nullptr );
            case 'b':

                return std::stod(value.substr(0 , value.size() - 1) , nullptr );
            case 'd':
                return std::stod(value.substr(0 , value.size() - 1) , nullptr );
            default:
                return std::stod(value.substr(0 , value.size()) , nullptr );

        }

    }
    identifier segment::getIdentifier(const std::string &name) const {
        return *std::find_if(identifiers.begin() , identifiers.end(), [&name](const identifier& curIdent){
            return curIdent.getName() == name;
        });
    }
    IdentifierType identifier::getType() const {
        return type;
    }
    bool identifier::isCorrectIdentifierValue() const {
        auto positionOfMinusInValue = std::find(value.begin(), value.end(), '-');
        auto tmp = positionOfMinusInValue == value.begin() ? value.substr(1, value.size()) : value;
        auto lastLetter = static_cast <char> ( std::tolower(static_cast<unsigned char> (value.back())));
        bool isOneDot = false;
        if(tmp.size() > 15){
            return false;
        }
        switch (lastLetter) {
            case 'h':
                if(tmp.front() != '0'){
                    return false;
                }
                for (const auto &it : tmp.substr(0, tmp.size() - 1)) {

                    if (auto one_value = static_cast<int>(it);  one_value < 48 ||
                                                                (one_value > 57 && one_value < 65) ||
                                                                (one_value > 70 && one_value < 97) ||
                                                                one_value > 102) {
                        return false;
                    }
                    if(!isCorrectRangesForType(this->type , std::to_string(fromHexToDec(tmp)))){
                        return false;
                    }
                }
                break;
            case 'b':
                for (const auto &it : tmp.substr(0, tmp.size() - 2)) {
                    if( static_cast<int>(it) == 46){
                        if(!isOneDot){
                            isOneDot = true;
                            continue;
                        } else {
                            return false;
                        }
                    }
                    if (auto one_value = static_cast<int>(it); one_value < 48 || one_value > 49) {
                        return false;
                    }
                    if(!isCorrectRangesForType(this->type , std::to_string(fromBinaryToDec(tmp)))){
                        return false;
                    }

                }
                break;
            case 'd':
            default:
                if (tmp.empty()) return false;
                for (const auto &it : tmp) {
                    if( static_cast<int>(it) == 46){
                        if(!isOneDot){
                            isOneDot = true;
                            continue;
                        } else {
                            return false;
                        }
                    }
                    if (auto one_value = static_cast<int>(it);  one_value < 48 || one_value > 57) {
                        return false;
                    }
                }
                if(!isCorrectRangesForType(this->type , tmp)){
                    return false;
                }
                break;
        }
        return true;
    }
    bool identifier::isCorrectRangesForType(IdentifierType type , const std::string &value) const {
        switch (type){
            case IdentifierType::DB:
                return std::stoi(value) < MAX_DB_VALUE;
            case IdentifierType::DW:
                return std::stoi(value) < MAX_DW_VALUE;
            case IdentifierType::DD:
                return std::stoi(value) < MAX_DD_VALUE;

        }
    }
    void identifier::print() const {}

    bool segment::isOpen() const {
        return isCreate;
    }
    void segment::open() {
        isCreate = true;
    }
/*    const std::set<identifier>& segment::getAllIdentifiers() const &{
        return this->identifiers;
    }*/
    bool segment::push(assembler::identifier &&identifier1) {
        auto it = identifiers.insert(std::move(identifier1));
        return !it.second;
    }
    bool segment::isDeclaredIdentifier(const std::string &new_identifier) const {
        return std::find_if(identifiers.cbegin() , identifiers.cend() , [&new_identifier](const identifier& currentIdentifier){
            return currentIdentifier.name == new_identifier;
        }) != identifiers.cend();
    }
    void segment::printIdentifiers() {
        for(const auto &it : identifiers){
            it.print();
        }
    }

    size_t segment::getIdentifierNumbersOfByte(const std::string &id) {
        return std::find_if(identifiers.begin() , identifiers.end() , [&id](const identifier& cur_iden){
            return cur_iden.getName() == id;
        })->getType() == IdentifierType::DD ? 3 : 2;
    }
    const std::set<identifier>& segment::getAllIdentifiers() const {
        return identifiers;
    }
    /*                  TODO
     *
     *
     * */
    const label& code::getLabelByName(const std::string &label_name) const {
        auto it = std::find_if(labels.begin() , labels.end() , [&label_name](const label& curLabel){
            return curLabel.name == label_name;
        });
        return *it;
    }
    bool code::isLabelDeclared(const std::string &label_name) const {
        return std::find_if(labels.cbegin() , labels.cend(), [&label_name](const label& currectLabel){
            return currectLabel.name == label_name;
        }) != labels.cend();
    }
    bool code::pushLabel(assembler::label &&label) {
        auto it = labels.insert(std::move(label));
        return !it.second;
    }

    bool operator <(const identifier& lft , const identifier& rht){
        return lft.name < rht.name;
    }
    bool operator <(const label& lft , const label& rht){
        return lft.name < rht.name;
    }


    const  stringsVector commandsVector() {
        static const  stringsVector commands{
                "mov",
                "imul",
                "idiv",
                "or",
                "cmp",
                "jng",
                "and",
                "add",
                "cwde",
                "model"

        };
        return commands;
    }
    const  stringsVector directivesVector() {
        static const  stringsVector directives{
                "db",
                "dw",
                "dd"
        };
        return directives;
    }
    const  stringsVector instructionsVector(){
        static const stringsVector instructions{
                ".data",
                ".code",
                "end",
                "model",
                "small"
        };
        return instructions;
    }
    const  stringsVector registers32Vector() {
        static const  stringsVector registers32{
                "eax",
                "ebx",
                "edx",
                "ecx",
                "esi",
                "edi"
        };
        return registers32;
    }
    const  stringsVector registers16Vector() {
        static const  stringsVector registers16{
                "ax",
                "bx",
                "cx",
                "dx",
                "si",
                "di"
        };
        return registers16;
    }
    const  stringsVector registers8Vector() {
        static const  stringsVector registers8{
                "ah",
                "al",
                "bh",
                "bl",
                "ch",
                "cl"
        };
        return registers8;
    }
    const  stringsVector segmentRegisters(){
        static const  stringsVector segmentsRegisters{
                "cs",
                "ds",
                "es",
                "ss",
        };
        return segmentsRegisters;
    }

    bool isWordInVector(const stringsVector &vector, std::string_view word) {
        std::string tmp{word};
        removeSpacesAndTabs(tmp);
        for(const auto &it : vector){
            std::transform(tmp.begin() , tmp.end() ,tmp.begin(), ::tolower );
            if(tmp == it) {
                return true;
            }
        }
        return false;
    }
    bool isCommand(std::string_view word) {
        return isWordInVector(commandsVector() , word);
    }
    bool isDirective(std::string_view word) {
        return isWordInVector(directivesVector() , word);
    }
    bool isInstruction(std::string_view word) {
        return isWordInVector(instructionsVector() , word);
    }
    bool isRegister(const std::string &string) {
        return isWordInVector(registers8Vector()  , string) ||
               isWordInVector(registers16Vector() , string) ||
               isWordInVector(registers32Vector() , string);
    }
    bool isReservedWord(std::string_view word) {
        return isWordInVector(registers16Vector() , word)       ||
               isWordInVector(registers8Vector() , word)        ||
               isWordInVector(registers32Vector() , word)       ||
               isWordInVector(segmentRegisters() , word)       ||
               isCommand(word) || isDirective(word) || isInstruction(word) ;
    }



    void createVectorOfWordsFromString(const std::string& string , stringsVector& wordsInString) {
        std::istringstream inputStringStream(string);
        std::string tmp;
        while (inputStringStream >> tmp){
            wordsInString.push_back(tmp);

        }
    }

    bool isIdentifier(std::string_view word) {
        return !(isCommand(word) || isDirective(word) || isInstruction(word));

    }

    size_t directivePosition(std::string_view directive) {
        auto size = directivesVector().size();
        for(size_t i = 0 ; i < size; ++i){
            if( directive == directivesVector().at(i) ){
                return i;
            }
        }
        return directivesVector().size();
    }
    IdentifierType identifierType(std::string_view identifier_type) {
        auto directive_pos = directivePosition(identifier_type);
        switch(directive_pos){
            case 0:
                return IdentifierType::DB;
            case 1:
                return IdentifierType::DW;
            case 2:
                return IdentifierType::DD;
            default:
                return IdentifierType::INCORRECT_IDENTIFIER;
        }

    }





    bool isDeclaredIdentifier(const std::string& identifier , const data& _data , const code& _code){
        return _data.isDeclaredIdentifier(identifier) ||  _code.isDeclaredIdentifier(identifier) || _code.isLabelDeclared(identifier);
    }



    /************   Mov     *************/
    bool Mov::isCorrectOperands(size_t line) {
        if(operands.size() != 3){
            return false;
        }
        return isCorrectFirstOperand() && isCorrectSecondOperand();
    };
    bool Mov::isCorrectFirstOperand() {
        return isRegister(operands.front());
    }
    std::string Mov::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /*
     *  TODO
     *
     *  check range of value
     *
     * */
    bool Mov::isCorrectSecondOperand() {
        auto type = isWordInVector(registers8Vector()  , operands.front()) ?  IdentifierType::DB :
                    isWordInVector(registers16Vector() , operands.front()) ? IdentifierType::DW :
                    isWordInVector(registers32Vector() , operands.front()) ? IdentifierType::DD :
                    IdentifierType::INCORRECT_IDENTIFIER;
        return identifier{"" , type , operands.back()}.isCorrectIdentifierValue();
    }
    int Mov::getNumberOfByte(size_t line) {
        size_t tmp = isWordInVector(registers8Vector()  , operands.front()) ?  1 :
                     isWordInVector(registers16Vector() , operands.front()) ? 2 : 5;

        return 1 + tmp;
    }
    /************   Mov     *************/

    /************   Imul     *************/
    bool Imul::isCorrectOperands(size_t line) {
        return operands.size() == 1 && isRegister(operands.front());
    };
    int Imul::getNumberOfByte(size_t line) {
        return isWordInVector(registers32Vector() , operands.front()) ? 3 : 2;
    }
    std::string Imul::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        std::string returnValue;
        if(isWordInVector(registers8Vector() , operands[0])){
            returnValue = "F6 E";
        } else {
            returnValue = "F7 E" ;
        }
        returnValue += fromDecToHex(8 + getOpCod(operands[0]));

        return returnValue;
    }
    /************   Imul     *************/

    /************   Idiv     *************/
    bool Idiv::isCorrectOperands(size_t line) {

        return isCorrectAddressExpression(operands , line);
    };
    int Idiv::getNumberOfByte(size_t line) {
        std::string string{};
        for(const auto& oneStr : operands){
            string += oneStr;
        }
        userIdentifiers::getMapOfAdressExpression()[line] = string;
        if( operands.size() > 3 ) {
            if(isWordInVector(segmentRegisters() , operands.front()) && isWordInVector(registers32Vector() , operands[4])){
                return 3;
            } else if(isWordInVector(registers32Vector() , operands[2])){
                return 3;
            }
        }
        return 2;

    }
    std::string Idiv::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /************   Idiv     *************/

    /************   Or     *************/
    bool Or::isCorrectOperands(size_t line) {
        if(operands.size() != 3){
            return false;
        }
        return  (isWordInVector(registers32Vector() , operands.front()) && isWordInVector(registers32Vector() , operands.back())) ||
                (isWordInVector(registers16Vector() , operands.front()) && isWordInVector(registers16Vector() , operands.back())) ||
                (isWordInVector(registers8Vector() , operands.front()) && isWordInVector(registers8Vector() , operands.back()));
    };
    bool Or::isCorrectFirstOperand() {
        return isRegister(operands.front());
    }
    bool Or::isCorrectSecondOperand() {
        return isRegister(operands.back());
    }
    int Or::getNumberOfByte(size_t line) {
        if(isWordInVector(registers32Vector(), operands.front()) &&
           isWordInVector(registers32Vector(), operands.back())){
            return 3;
        } else {
            return 2;
        }

    }
    std::string Or::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /************   Or     *************/

    /************   Cmp     *************/
    bool Cmp::isCorrectOperands(size_t line) {
        if(operands.size() != 3){
            return false;
        }
        return isCorrectFirstOperand() && isCorrectSecondOperand(line) && operands[1] == ",";
    };
    bool Cmp::isCorrectFirstOperand() {
        return isRegister(operands.front());
    }
    bool Cmp::isCorrectSecondOperand(size_t line) {
        stringsVector firstOperand{operands.back()};
        splitByDelimiters(":[*]" , firstOperand);
        return isCorrectAddressExpression(firstOperand , line);
    }
    int Cmp::getNumberOfByte(size_t line) {
        std::string string{};
        if(operands.size() > 3 && isWordInVector(registers32Vector() , operands[4])){
            operands[4] = operands[4].substr(1 , operands[4].size());
        }
        for(int i =2 ; i < operands.size() ; ++i){
            string += operands[i];
        }
        userIdentifiers::getMapOfAdressExpression()[line] = string;
        /*
         * for exp ident[edi*const] must return 3
         * for another exp - 2
         *
         * */
        stringsVector tmp{operands.back()};
        splitByDelimiters(":[]*" , tmp);
        return  tmp.size() > 5 ? 3 : 2;
    }
    std::string Cmp::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /************   Cmp     *************/

    /************   Jng     *************/
    bool Jng::isCorrectOperands(size_t line) {
        if(operands.size() != 1){
            return false;
        }
        userIdentifiers::pushLabel(operands.front() , line);
        return true;
    };
    int Jng::getNumberOfByte(size_t line) {

        return 2;
    }
    std::string Jng::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /************   Jng     *************/

    /************   And     *************/
    bool And::isCorrectOperands(size_t line) {
        if(operands.size() != 3){
            return false;
        }
        return isCorrectFirstOperand(line) && isCorrectSecondOperand() && operands[1] == ",";
    };
    bool And::isCorrectFirstOperand(size_t line) {
        stringsVector firstOperand{operands.front()};
        splitByDelimiters(":[*]" , firstOperand);
        return isCorrectAddressExpression(firstOperand , line);
    }
    bool And::isCorrectSecondOperand() {
        return isRegister(operands.back());
    }
    int And::getNumberOfByte(size_t line) {

        userIdentifiers::getMapOfAdressExpression()[line] = operands.front();
        return 2;
    }
    std::string And::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /************   And     *************/

    /************   Add     *************/
    bool Add::isCorrectOperands(size_t line) {
        if(operands.size() != 3){
            return false;
        }
        return isCorrectFirstOperand(line) && isCorrectSecondOperand() && operands[1] == ",";

    };
    bool Add::isCorrectSecondOperand() {
        return identifier("" , IdentifierType::DD, operands.back()).isCorrectIdentifierValue();
    }
    bool Add::isCorrectFirstOperand(size_t line) {
        stringsVector firstOperand{operands.front()};
        splitByDelimiters(":[*]" , firstOperand);
        return isCorrectAddressExpression(firstOperand , line);
    }
    int Add::getNumberOfByte(size_t line) {
       /*
        * for add we must include symbol '#' because
        * number of byte depends on value (second operand of command)
        *
        *
        * */
        userIdentifiers::getMapOfAdressExpression()[line] = operands.front() + "#" + operands.back();

        return 2;
    }
    std::string Add::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /************   Add     *************/

    /************   Cwde     *************/
    bool Cwde::isCorrectOperands(size_t line) {
        return operands.empty();
    };
    int Cwde::getNumberOfByte(size_t line) {
        return 2;
    }
    std::string Cwde::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "66| 98";
    }
    /************   Cwde     *************/
    /************   Model     *************/
    bool Model::isCorrectOperands(size_t line) {
        return operands[0] == "{" &&
               isWordInVector({"small"} , operands[1]) &&
               operands[2] == "}";
    };
    int Model::getNumberOfByte(size_t line) {
        return 0;
    }
    std::string Model::getBites(const assembler::data &data_segment, const assembler::code &code_segment) {
        return "";
    }
    /************   Model     *************/

    void removeSpacesAndTabs(std::string& string){
        string.erase(std::remove_if(string.begin() , string.end() , [](char s){
            return s == ' ' || s == '\t';
        }) , string.end()) ;
    }



    void syntAnaliser(std::ostream& os , const lexem_type& vectorLexems){

        auto commaPosition = std::find_if(vectorLexems.cbegin() , vectorLexems.cend() , [](const std::pair<std::string , WordType>& lexem){
            return lexem.first == ",";
        });

        auto commandPos = std::find_if(vectorLexems.cbegin() , vectorLexems.cend() , [](const std::pair<std::string , WordType>& lexem){
            return lexem.second == WordType::COMMAND;
        });
        auto first = vectorLexems.front();

        if(first.second == WordType::IDENTIFIER){
            if(commandPos == vectorLexems.cend()){
                auto directive = std::find_if(vectorLexems.cbegin() , vectorLexems.cend() , [](const std::pair<std::string , WordType>& lexem){
                    return lexem.second == WordType::DIRECTIVE;
                });
                if(directive == vectorLexems.end()){
                    os    << "mitkaIndex = 0, "
                          << "mnemokodIndex = -1"
                          << ", mnemokodCount =  -1"
                          << ", firstOperandIndex = -1"
                          << ", firstOperandCount = -1"
                          << ", secondOperandIndex = -1 , "
                          << "secondOperandCount = -1" << std::endl;
                } else {
                    os    << "mitkaIndex = 0, "
                          << "mnemokodIndex = 1"
                          << ", mnemokodCount =  1"
                          << ", firstOperandIndex = 2"
                          << ", firstOperandCount = 1"
                          << ", secondOperandIndex = -1 , "
                          << "secondOperandCount = -1" << std::endl;
                }
            } else {
                os    << "mitkaIndex = 0, "
                      << "mnemokodIndex = " << commandPos - vectorLexems.begin()
                      << ", mnemokodCount =  1"
                      << ", firstOperandIndex = " << commandPos - vectorLexems.begin() + 1
                      << ", firstOperandCount = " << commaPosition - commandPos - 1;
                if(commaPosition == vectorLexems.cend()){
                    os << ", secondOperandIndex = -1 , "
                       << "secondOperandCount = -1" << std::endl;
                } else {
                    os << ", secondOperandIndex = " << commaPosition - vectorLexems.begin() + 1
                       << " , secondOperandCount = "<< vectorLexems.cend() - commaPosition - 1 << std::endl;
                }
            }
        } else if (first.second == WordType::COMMAND){
            os << "mitkaIndex     = -1,  "
               << "mnemokodIndex  = 0, "
               << "mnemokodCount  = 1 , ";
            if(vectorLexems.size() == 1){
                os  << "firstOperandIndex = -1 , "
                    << "firstOperandCount = -1" ;
            } else {
                os  << "firstOperandIndex = 1 , "
                    << "firstOperandCount = " << commaPosition - commandPos - 1;
            }


            if(commaPosition == vectorLexems.cend()){
                os << ", secondOperandIndex = -1 , "
                   << "secondOperandCount = -1" << std::endl;
            } else {
                os << ", secondOperandIndex = " << commaPosition - commandPos + 1
                   << " , secondOperandCount = "<< vectorLexems.cend() - commaPosition - 1 << std::endl;
            }
        } else {
            os    << "mitkaIndex = -1,  "
                  << "mnemokodIndex = 0"
                  << ", mnemokodCount =  1";
            if(vectorLexems.size() == 1){
                os      << ", firstOperandIndex = -1"
                        << ", firstOperandCount = -1"
                        << ", secondOperandIndex = -1 , "
                        << "secondOperandCount = -1" << std::endl;
            } else {
                os      << ", firstOperandIndex = 2"
                        << ", firstOperandCount = 1"
                        << ", secondOperandIndex = -1 , "
                        << "secondOperandCount = -1" << std::endl;
            }

        }




    }


    WordType getTypeOfOperand(const std::string& operand){
        if(isWordInVector(commandsVector() , operand)){
            return WordType::COMMAND;
        } else if(isRegister(operand) ||
                  isWordInVector(segmentRegisters() , operand)  ){
            return WordType::REGISTER;
        } else if(isWordInVector(directivesVector() , operand)) {
            return WordType::DIRECTIVE;
        } else if (identifier{"" , IdentifierType::INCORRECT_IDENTIFIER , operand}.isCorrectIdentifierValue()){
            return WordType::CONSTANT;
        } else if (isWordInVector(instructionsVector() , operand)){
            return WordType::INSTRUCTION;
        } else {
            return WordType::IDENTIFIER;
        }
    }
    lexem_type  lexemParsing(const stringsVector& vectorOfOperands){
        lexem_type lexems;

        std::string one_lexem;
        std::string delimiters = "{}:,[]*-";
        for(const auto& word : vectorOfOperands){
            auto current_delimiter_position = std::find_first_of(word.cbegin() , word.cend() ,
                                                                 delimiters.cbegin() , delimiters.cend());
            decltype(current_delimiter_position) previous_delimiter_position = word.cbegin();
            /*
             * check if delimiter is the first symbol in line
             * */
            while(current_delimiter_position == previous_delimiter_position){
                lexems.push_back({std::string{*current_delimiter_position} , WordType::ONE_SYMBOLE_LEXEM});
                if( ++current_delimiter_position == word.cend()){
                    break;
                } else {
                    current_delimiter_position = std::find_first_of(current_delimiter_position  , word.cend() ,
                                                                    delimiters.cbegin() , delimiters.cend());
                    ++previous_delimiter_position;
                }

            }
            while ( current_delimiter_position != word.cend()  ){
                std::string current_operand {previous_delimiter_position , current_delimiter_position};
                removeSpacesAndTabs(current_operand);
                lexems.push_back(
                        {current_operand , getTypeOfOperand(current_operand)}
                );
                lexems.push_back(
                        {std::string{*current_delimiter_position} , WordType::ONE_SYMBOLE_LEXEM}
                );
                previous_delimiter_position = current_delimiter_position + 1;
                current_delimiter_position = std::find_first_of(current_delimiter_position + 1 , word.cend() ,
                                                                delimiters.cbegin() , delimiters.cend());
            }
            if(previous_delimiter_position != word.cend() && word.size() != 1 ){
                lexems.push_back(
                        {std::string{previous_delimiter_position , word.cend()} ,
                         getTypeOfOperand(std::string(previous_delimiter_position , word.cend()))}
                );
            }
            std::string oneValue = "1234567890";
            if(std::find(oneValue.begin() , oneValue.end() , word.front()) != oneValue.end() && word.size() == 1 ){
                lexems.push_back(
                        {std::string{previous_delimiter_position , word.cend()} ,
                         WordType ::CONSTANT}
                );
            }

        }

        return lexems;
    };



    void splitByDelimiters(const std::string& delimiters  , stringsVector& wordsInString){
        stringsVector newVector;
        for(const auto& word : wordsInString){
            auto current_delimiter_position = std::find_first_of(word.cbegin() , word.cend() ,
                                                                 delimiters.cbegin() , delimiters.cend());
            decltype(current_delimiter_position) previous_delimiter_position = word.cbegin();
            /*
             * check if delimiter is the first symbol in line
             * */
            while(current_delimiter_position == previous_delimiter_position){
                newVector.push_back(std::string{*current_delimiter_position} );

                if( ++current_delimiter_position == word.cend()){
                    break;
                } else {
                    current_delimiter_position = std::find_first_of(current_delimiter_position  , word.cend() ,
                                                                    delimiters.cbegin() , delimiters.cend());
                    ++previous_delimiter_position;
                }

            }
            while ( current_delimiter_position != word.cend()  ){
                std::string current_operand {previous_delimiter_position , current_delimiter_position};
                removeSpacesAndTabs(current_operand);
                newVector.push_back(current_operand);
                newVector.push_back(std::string{*current_delimiter_position});
                previous_delimiter_position = current_delimiter_position + 1;
                current_delimiter_position = std::find_first_of(current_delimiter_position + 1 , word.cend() ,
                                                                delimiters.cbegin() , delimiters.cend());
            }
            if(previous_delimiter_position != word.cend() && word.size() != 1 ){
                newVector.push_back(std::string{previous_delimiter_position , word.cend()});
            }
            std::string oneValue = "1234567890";
            if(std::find(oneValue.begin() , oneValue.end() , word.front()) != oneValue.end() && word.size() == 1 ){
                newVector.push_back(std::string{previous_delimiter_position , word.cend()});
            }
        }
        wordsInString = std::move(newVector);
    }
    bool isCorrectAddressExpression(const stringsVector& operands , size_t line){
        switch (operands.size()){
            case 1:
                userIdentifiers::pushIdentifier(operands.back() ,line);
                return true;
            case 3:
                if(isWordInVector(segmentRegisters() , operands.front()) && operands[1] == ":"){
                    userIdentifiers::pushIdentifier(operands.back() , line);
                    return true;
                } else {
                    return false;
                }
            case 6:

                        if(isCorrectExpressionBetweenParentheses({operands[1] ,
                                                                  operands[2] ,
                                                                  operands[3] ,
                                                                  operands[4] ,
                                                                  operands[5] })){
                            userIdentifiers::pushIdentifier(operands.front() , line);
                            return true;
                        } else {
                            return false;
                        }


            case 8:
                if(isWordInVector(segmentRegisters() , operands.front()) && operands[1] == ":" &&
                   isCorrectExpressionBetweenParentheses({operands[3] ,
                                                          operands[4] ,
                                                          operands[5] ,
                                                          operands[6] ,
                                                          operands[7] })){
                    userIdentifiers::pushIdentifier(operands[2] , line);
                    return true;
                } else {
                    return false;
                }
            default:
                return false;
        }


    }
    bool isCorrectExpressionBetweenParentheses(const stringsVector & expressionOperands){
        return expressionOperands[0] == "[" && expressionOperands.back() == "]" &&
               isWordInVector(registers32Vector() , expressionOperands[1]) &&
               expressionOperands[2] == "*" && isWordInVector({"1","2" , "4" , "8" , "16"} , expressionOperands[3]);
    }
    std::unique_ptr<Command> getPointerForCommandByName(std::string_view command , const std::string& operands){
        size_t numberOfCommand = 0;
        auto commands = commandsVector();
        for(size_t i = 0 ; i < commands.size() ; ++i){
            if(commands[i] == command){
                numberOfCommand = i;
                break;
            };
        }
        switch(static_cast<CommandName >(numberOfCommand + 1)){
            case CommandName::MOV:
                return std::make_unique<Mov>(operands);
            case CommandName::IMUL:
                return std::make_unique<Imul>(operands);
            case CommandName::IDIV:
                return std::make_unique<Idiv>(operands);
            case CommandName::OR:
                return std::make_unique<Or>(operands);
            case CommandName::CMP:
                return std::make_unique<Cmp>(operands);
            case CommandName::JNG:
                return std::make_unique<Jng>(operands);
            case CommandName::AND:
                return std::make_unique<And>(operands);
            case CommandName::ADD:
                return std::make_unique<Add>(operands);
            case CommandName::CWDE:
                return std::make_unique<Cwde>(operands);
            case CommandName::MODEL:
                return std::make_unique<Model>(operands);
        }
    }

    namespace userIdentifiers{

        mapOfAdressExpression& getMapOfAdressExpression(){
            static mapOfAdressExpression exp;
            return exp;
        }
        void pushAdrExpr(size_t line , const std::string& expr){
            getMapOfAdressExpression()[line] = expr;
        }


        mapOfIdentifiers& getUsedLabels(){
            static mapOfIdentifiers labels;
            return labels;
        }
        mapOfIdentifiers& getUsedIdentifiers(){
            static mapOfIdentifiers identifiers;
            return identifiers;
        };
        static void pushLabel(const std::string& name , size_t line){
            getUsedLabels()[name].push_back(line);
        }
        static void pushIdentifier(const std::string& name , size_t line){
            getUsedIdentifiers()[name].push_back(line);
        }
    }
}