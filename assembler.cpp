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
namespace assembler{

    /*
     *  TODO
     *
     *  address expresion numbers of byte
     *
     * */
    std::string fromBinaryToDec(const std::string& value){
        int result{0};
        for(int i {static_cast<int>(value.size() - 2)} , j{0} ; i >= 0 ; --i , ++j){
            result += static_cast<int>(std::pow(2 , j) * (static_cast<int>(value[i]) - 48));
        }
        return std::to_string(result);
    }
    std::string fromHexToDec(const std::string& value){
        std::string number{value.substr(0 , value.size() - 1)};
        size_t length = number.size();
        std::transform(number.begin() , number.end() , number.begin() , ::tolower);
        int result{0};
        for(int i = length - 1 , j {0} ; i >= 0  ; --i , ++j){
            switch (number[i]){
                case 'a':
                    result += static_cast<int>(std::pow(16 , j)) * 10;
                    break;
                case 'b':
                    result += static_cast<int>(std::pow(16 , j)) * 11;
                    break;
                case 'c':
                    result += static_cast<int>(std::pow(16 , j)) * 12;
                    break;
                case 'd':
                    result += static_cast<int>(std::pow(16 , j)) * 13;
                    break;
                case 'e':
                    result += static_cast<int>(std::pow(16 , j)) * 14;
                    break;
                case 'f':
                    result += static_cast<int>(std::pow(16 , j)) * 15;
                    break;
                default:
                    result += static_cast<int>(std::pow(16 , j)) *  (static_cast<int>(number[i]) - 48) ;

            }
        }
        return std::to_string(result);
    }

    void identifier::setValue(const std::string &value) {
        this->value = value;
    };
    std::string identifier::getName() const {
        return name;
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
                    if(!isCorrectRangesForType(this->type , fromHexToDec(tmp))){
                        return false;
                    }
                }
                break;
            case 'b':
                for (const auto &it : tmp.substr(0, tmp.size() - 2)) {
                    if (auto one_value = static_cast<int>(it); one_value < 48 || one_value > 49) {
                        return false;
                    }
                    if(!isCorrectRangesForType(this->type , fromBinaryToDec(tmp))){
                        return false;
                    }

                }
                break;
            case 'd':
            default:
                if (tmp.empty()) return false;
                for (const auto &it : tmp) {
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
    };
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
    };
    void segment::open() {
        isCreate = true;
    }
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
    /*                  TODO
     *
     *
     * */
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
    };
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

        return  2;
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

        return 4;
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
        userIdentifiers::getMapOfAdressExpression()[line] = operands.front();
        int value = 2 + (std::tolower(operands.back().back()) == 'h' ? 4 : 2);
        return value;
    }
    /************   Add     *************/

    /************   Cwde     *************/
    bool Cwde::isCorrectOperands(size_t line) {
        return operands.empty();
    };
    int Cwde::getNumberOfByte(size_t line) {
        return 2;
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
            case 4:
                if(operands[1] == "[" && operands.back() == "]" && isRegister(operands[2]) ){
                    userIdentifiers::pushIdentifier(operands.front() ,line);
                    return true;
                } else {
                    return false;
                }
            case 6:

                if(isWordInVector(segmentRegisters(), operands.front())){
                    if(operands[3] == "[" && operands.back() == "]" && isRegister(operands[4]) ) {
                        userIdentifiers::pushIdentifier(operands[2], line);
                        return true;
                    }
                } else {
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
               (isWordInVector(registers16Vector() , expressionOperands[1]) || isWordInVector(registers32Vector() , expressionOperands[1])) &&
               expressionOperands[2] == "*" && isWordInVector({"2" , "4" , "8" , "16"} , expressionOperands[3]);
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