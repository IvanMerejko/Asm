
#include <iostream>
#include <fstream>
#include <iomanip>
#include "assembler.h"
#include "masm.h"
#include "Types.h"

masm::masm(const std::string &file)
        :asmFileName{file} {

};

void masm::createListingFile() {
    firstView();
    checkAllUsedButNotDeclaredIdentifiers();
    createAddress();
    secondView();
}
void masm::secondView() {
    std::ifstream asmFile(asmFileName );

    if(!asmFile.is_open()){
        std::cout << "No file";
    }
    size_t currentLine{0};
    std::string oneStringFromAsmFile;
    std::string tmp;
    while (std::getline(asmFile , oneStringFromAsmFile) && currentLine <= line){
        ++currentLine;
        tmp = oneStringFromAsmFile;



        oneStringFromAsmFile.erase(std::remove_if(oneStringFromAsmFile.begin() ,
                                                  oneStringFromAsmFile.end()   ,
                                                  [](const char symbol){ return symbol == ' ' || symbol == '\t';}),
                                   oneStringFromAsmFile.end());
        if(!oneStringFromAsmFile.empty()){
            std::cout << std::setw(6) << std::hex << std::uppercase << infoAboutLines[currentLine].address <<
                      std::setw(20) << std::left << tmp << '\n' ;
        } else {
            std::cout << '\n';
        }
        if(!oneStringFromAsmFile.empty() && infoAboutLines[currentLine].isErrorInLine){
            std::cout << "Error" << std::endl;
        }

    }
}
void masm::firstView() {
    std::ifstream asmFile(asmFileName);

    if(!asmFile.is_open()){
        std::cout << "No file";
    }


    std::string oneStringFromAsmFile;
    while (std::getline(asmFile , oneStringFromAsmFile) && !endOfFile){
        ++line;

        assembler::createVectorOfWordsFromString(oneStringFromAsmFile , wordsInString);
        if(wordsInString.empty()){
            infoAboutLines[line].isErrorInLine = false;
            infoAboutLines[line + 1].address = infoAboutLines[line].address;
            continue;
        };
        /*
         *              LEXEM PARSING FOR FIRST TASK
         * */
        /*auto lexem =  assembler::lexemParsing(wordsInString);
        std::cout << oneStringFromAsmFile << std::endl;
        printLexems(lexem);
        assembler::syntAnaliser(std::cout , lexem);*/

        /*
         * if we have error with label method return true
         * */
        if( (infoAboutLines[line].isErrorInLine = takeLabelsFromLine()) || wordsInString.empty()) {
            wordsInString.clear();
            infoAboutLines[line + 1].address = infoAboutLines[line].address;
            continue;
        }

        if(assembler::isIdentifier(wordsInString.front())){
            workWithIdentifier();
        } else {
            workWithCommand();
        }

        wordsInString.clear();

    }
    asmFile.close();
}
std::string masm::getStringByType(const std::string& word , const assembler::WordType type){
    switch (type){
        case assembler::WordType::REGISTER:
            return  "REGISTER" ;
        case assembler::WordType::DIRECTIVE:
            return  "DIRECTIVE" ;
        case assembler::WordType::COMMAND:
            return  "COMMAND" ;
        case assembler::WordType::IDENTIFIER:
            return  std::string{"IDENTIFIER   "} + (assembler::isDeclaredIdentifier(word , _data , _code) ? "DECLARED" : "UNDECLARED") ;
        case assembler::WordType::INSTRUCTION:
            return  "INSTRUCTION" ;
        case assembler::WordType::CONSTANT:
            return  "CONSTANT" ;
        case assembler::WordType::ONE_SYMBOLE_LEXEM:
            return  "ONE_SYMBOLE_LEXEM" ;
    }
}
void masm::printLexems(const assembler::lexem_type& lexems){
    size_t i = 0;
    for(const auto& [word , type] : lexems){
        std::cout << std::setw(4) << std::left << i++ << std::setw(8) << std::left << word << std::setw(6) << word.length() << std::setw(20) << getStringByType(word , type) << std::endl;

    }
}
void masm::workWithCommand() {
    /*
     * if data segment isn`t opened - open it
     * if one data segment is opened and tried open one more  - ERROR
     * */

    std::string tmp{wordsInString.front()};
    std::transform(tmp.begin() , tmp.end() , tmp.begin() , ::tolower);
    if(tmp == assembler::instructionsVector().front()){ // .data
        if((infoAboutLines[line].isErrorInLine =  _data.isOpen() )) {
            return;
        };
        _data.open();
        lastOpenedSegment = "data";
        infoAboutLines[line].address = 0;
        infoAboutLines[line + 1].address = 0;
        return;
    }
    /*
     * if code segment isn`t opened - open it
     * if one code segment is opened and tried open one more  - ERROR
     **/
    if(tmp == assembler::instructionsVector().at(1)){ // .code
        if( (infoAboutLines[line].isErrorInLine =  _code.isOpen() ) ) {
            return;
        }
        _code.open();
        lastOpenedSegment = "code";
        infoAboutLines[line].address = 0;
        infoAboutLines[line + 1].address = 0;
        return;
    }

    if(tmp == assembler::instructionsVector().at(2)){ // .end
        endOfFile = true;
        return;
    }
    if((infoAboutLines[line].isErrorInLine = !_code.isOpen()) && !assembler::isWordInVector({"model"} , wordsInString.front()) ){

        return;
    }



    std::string parameters{};
    for(size_t i = 1 ; i < wordsInString.size() ; ++i) parameters += wordsInString.at(i) + " ";
    auto command = assembler::getPointerForCommandByName(wordsInString.front() , parameters);
    infoAboutLines[line].isErrorInLine = !command->isCorrectOperands(line);
    auto value = command->getNumberOfByte(line);
    infoAboutLines[line + 1].address = infoAboutLines[line].address + value;
    // infoAboutLines[line + 1].address = infoAboutLines[line].address ;
}
void masm::workWithIdentifier() {



    if((infoAboutLines[line].isErrorInLine = ( !_data.isOpen() && !_code.isOpen()))){
        currentAddressEqualsToPreviousAddress();
        return;
    }


    if( (infoAboutLines[line].isErrorInLine = (wordsInString.size() != 3 ) ) ){
        currentAddressEqualsToPreviousAddress();
        return;
    }


    auto identifierType = assembler::identifierType(wordsInString.at(1));

    if( (infoAboutLines[line].isErrorInLine =  (identifierType == assembler::IdentifierType::INCORRECT_IDENTIFIER)) ){
        currentAddressEqualsToPreviousAddress();
        return;
    }

    typename assembler::identifier new_identifier (wordsInString.front() , identifierType , wordsInString.back());

    if( (infoAboutLines[line].isErrorInLine =  !new_identifier.isCorrectIdentifierValue())){
        currentAddressEqualsToPreviousAddress();
        return;
    }


    /*
     * if we can`t push identifier - ERROR
     * */
    infoAboutLines[line + 1].address = infoAboutLines[line].address + new_identifier.getNumberOfByte(line);
    infoAboutLines[line].isErrorInLine = pushIdentifier(std::move(new_identifier)) ;
}
bool masm::pushIdentifier(assembler::identifier&& new_identifier) {
    if(lastOpenedSegment == "data"){
        return _data.push(std::move(new_identifier));
    } else if ( lastOpenedSegment == "code") {
        return _code.push(std::move(new_identifier));
    }
    return false;
}
bool masm::takeLabelsFromLine() {
    //  ?
    if( !isLabel(wordsInString.front()) /* && (infoAboutLines[line].isErrorInLine =  !_code.isOpen())*/ )
        return false;

    while (!wordsInString.empty()  && isLabel(wordsInString.front()) ){
        /*
         * _code.pushLabel returns true if label with this name already located in code or it has command`s name
         * */
        wordsInString.front().erase(wordsInString.front().end() - 1);
        if(wordsInString.front().empty()){
            return true;
        }
        if( _code.pushLabel(std::move(assembler::label(wordsInString.front().substr(0 ,wordsInString.front().size()) , 0))) ){
            return true;
        };
        for(size_t i = 0 ; i < wordsInString.size() - 1 ; ++i) {
            wordsInString.at(i) = wordsInString.at(i+1);
        }
        wordsInString.pop_back();
    }
    return false;
}
bool masm::isLabel(std::string_view  word) {
    return word.back() == ':';
}
void masm::checkAllUsedButNotDeclaredIdentifiers(){


    for(const auto& [name , lines] : assembler::userIdentifiers::getUsedLabels()){
        if( !_code.isLabelDeclared(name)){
            for(const auto line : lines){
                infoAboutLines[line].isErrorInLine = true;
            }
        }
    }

    for(const auto& [name , lines] : assembler::userIdentifiers::getUsedIdentifiers()){
        if(!_data.isDeclaredIdentifier(name) && !_code.isDeclaredIdentifier(name)){
            for(const auto& line : lines){
                infoAboutLines[line].isErrorInLine = true;
            }
        }
    }


}
void masm::currentAddressEqualsToPreviousAddress() {
    infoAboutLines[line].address = infoAboutLines[line - 1].address;
}
/*
 * TODO
 * es:
 *
 * */
void masm::createAddress() {
    for(const auto& [line , string] : assembler::userIdentifiers::getMapOfAdressExpression()){
        if(!infoAboutLines[line].isErrorInLine){
            assembler::stringsVector operands{string};
            assembler::splitByDelimiters(":[]*" , operands);
            int value {0};
            switch (operands.size()){
                case 1:
                    value = getBytesForUserIdetifier(operands.front());
                    break;

                case 8 :
                    value = 1;
                    value += getBytesForRegister(operands[4]);
                    if(isDD_Identifier(operands[2])){
                        value += 1;
                    }
                    break;
                case 4 :
                case 6 :
                    if(assembler::isWordInVector(assembler::segmentRegisters() , operands.front())){
                        value += getBytesForRegister(operands[4]);
                        if(isDD_Identifier(operands[2])){
                            value += 1;
                        }
                    } else {
                        value += getBytesForRegister(operands[2]);
                        if(isDD_Identifier(operands.front())){
                            value += 1;
                        }
                    }

                    break;
            }
            for(size_t i = line ; i < infoAboutLines.size() ; ++i){
                infoAboutLines[i + 1].address = infoAboutLines[i + 1].address+value;
            }
        } else {
            for(size_t i = line ; i < infoAboutLines.size() ; ++i){
                infoAboutLines[i + 1].address = infoAboutLines[i + 1].address+-2;
            }
            //infoAboutLines[line + 1].address = infoAboutLines[line].address;
        }

    }

}
size_t masm::getBytesForUserIdetifier(const std::string &ident) {
    return _data.isDeclaredIdentifier(ident) ? _data.getIdentifierNumbersOfByte(ident) :
           _code.getIdentifierNumbersOfByte(ident);
}
size_t masm::getBytesForRegister(const std::string &reg) {
    return assembler::isWordInVector(assembler::registers32Vector() , reg) ? 5 : 2;
}
bool masm::isDD_Identifier(const std::string &ident) {
    return _data.isDeclaredIdentifier(ident) ?
                _data.getIdentifier(ident).getType() == assembler::IdentifierType::DD  :
                 _code.getIdentifier(ident).getType() == assembler::IdentifierType::DD;
}