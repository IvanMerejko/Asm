
#include <iostream>
#include <fstream>
#include <iomanip>
#include "assembler.h"
#include "masm.h"
#include "Types.h"

/*
 *
 *      TODO
 * */


masm::masm(const std::string &file)
        :asmFileName{file} {}

void masm::createListingFile() {
    firstView();
    checkAllUsedButNotDeclaredIdentifiers();
    createAddress();
    secondView();
    printAllIdentifiers();
}
void masm::secondView() {
    std::ifstream asmFile(asmFileName );
    size_t numberOfErrors{0};
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
            assembler::stringsVector vector;
            std::string help{tmp};
            assembler::createVectorOfWordsFromString(help , vector);

            std::cout << '\n';
            //assembler::splitByDelimiters("{}", vector);
            if(!assembler::isWordInVector({"end"} , oneStringFromAsmFile) && !assembler::isWordInVector({"model"} , vector.front())){

                std::cout << std::setw(6) << std::hex << std::uppercase << std::left <<infoAboutLines[currentLine].address;
                if(!infoAboutLines[currentLine].isErrorInLine){
                    std::string operands ;
                    for(int i = 1 ; i < vector.size() ; ++i){
                        operands += vector[i];
                    }

                    switch (infoAboutLines[currentLine].type){
                        case TypeOfLine::COMMAND:


                            std::cout << std::setw(6) << assembler::getPointerForCommandByName(vector[0] , operands)->getBites(_data , _code) ;
                            break;
                        case TypeOfLine::IDENTIFIER: {

                            auto type = assembler::identifierType(vector[1]);
                            auto identifier = assembler::identifier(vector.front(), type,
                                                                    vector.back());
                            identifier.isCorrectIdentifierValue();
                            std::string valueInDec = identifier.getValueInDec();
                            std::string hexCodeOfValue = assembler::getHexCodForValue(std::stoi( valueInDec,
                                                                                                nullptr));
                            std::string outValue;
                            switch (type) {
                                case assembler::IdentifierType::DB:
                                    outValue = hexCodeOfValue.substr(0, 2);
                                    std::cout <<  outValue;
                                    break;
                                case assembler::IdentifierType::DW:
                                    outValue = hexCodeOfValue.substr(0, 4);
                                    std::cout << outValue;
                                    break;
                                case assembler::IdentifierType::DD:
                                    outValue = hexCodeOfValue;
                                    for(int i = 0 ; i < outValue.size() ; i+=2){
                                        char letter = outValue[i];
                                        outValue[i] = outValue[i+1];
                                        outValue[i+1] = letter;
                                    }
                                    std::reverse(outValue.begin() , outValue.end());
                                    std::cout << outValue;
                                    break;

                            }
                            break;
                        }
                        case TypeOfLine::NO_IMPORTANT:
                            //std::cout << std::setw(6) << "NO_IMPORTANT";
                            break;
                    }
                }
                std::cout <<  std::setw(20) << std::left << tmp << std::endl;
            } else {
                std::cout  << tmp << std::endl ;
            }

        } else {
            std::cout << std::endl;
        }
        if(!oneStringFromAsmFile.empty() && infoAboutLines[currentLine].isErrorInLine){
            ++numberOfErrors;
            std::cout << "Error" << std::endl;
        }


    }
    std::cout << "\n----------------------------------------------------------\n";
    std::cout << "Number of Errors: " << numberOfErrors << '\n';
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
            infoAboutLines[line].type = TypeOfLine::NO_IMPORTANT;
            continue;
        }
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
            infoAboutLines[line].type = TypeOfLine::NO_IMPORTANT;
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
        }
        _data.open();
        lastOpenedSegment = "data";
        infoAboutLines[line].address = 0;
        infoAboutLines[line].type = TypeOfLine::NO_IMPORTANT;
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
        infoAboutLines[line].type = TypeOfLine::NO_IMPORTANT;
        infoAboutLines[line].address = 0;
        infoAboutLines[line + 1].address = 0;
        return;
    }

    if(tmp == assembler::instructionsVector().at(2)){ // .end
        endOfFile = true;
        infoAboutLines[line].address = -1;
        infoAboutLines[line].type = TypeOfLine::NO_IMPORTANT;
        return;
    }
    if((infoAboutLines[line].isErrorInLine = !_code.isOpen()) && !assembler::isWordInVector({"model"} , wordsInString.front()) ){

        return;
    }



    std::string parameters{};
    for(size_t i = 1 ; i < wordsInString.size() ; ++i) parameters += wordsInString.at(i) + " ";
    auto command = assembler::getPointerForCommandByName(wordsInString.front() , parameters);
    infoAboutLines[line].isErrorInLine = !command->isCorrectOperands(line);
    if(!infoAboutLines[line].isErrorInLine ){
        auto value = command->getNumberOfByte(line);
        infoAboutLines[line + 1].address = infoAboutLines[line].address + value;
        infoAboutLines[line].type = TypeOfLine::COMMAND;
    } else{
        infoAboutLines[line + 1].address = infoAboutLines[line].address ;
    }

    //
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
    if( (infoAboutLines[line].isErrorInLine = wordsInString.front().size() > 4)){
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
    auto numberOfBytes = new_identifier.getNumberOfByte(line);
    if(  !(infoAboutLines[line].isErrorInLine = pushIdentifier(std::move(new_identifier)))){
        infoAboutLines[line + 1].address = infoAboutLines[line].address + numberOfBytes;
        infoAboutLines[line].type = TypeOfLine::IDENTIFIER;
    }
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
        if( _code.pushLabel(std::move(assembler::label(wordsInString.front().substr(0 ,wordsInString.front().size()) , infoAboutLines[line-1].address))) ){
            return true;
        }
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
            for(const auto& tmp : lines){
                infoAboutLines[tmp].isErrorInLine = true;
            }
        } else{
            for(const auto& tmp : lines){

                const auto distance = infoAboutLines[tmp].address - _code.getLabelByName(name).position;
                std::cout <<  infoAboutLines[tmp].address << "           " << _code.getLabelByName(name).position  << '\n' ;

                if( distance >= 80 || distance <= 0){
                    addValueForALlAddressFromLine(tmp , 2);
                }
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
    for(auto& [line , string] : assembler::userIdentifiers::getMapOfAdressExpression()){
        if(!infoAboutLines[line].isErrorInLine){
            if(string.back() == '&'){
                string = string.substr(0 , string.size() - 1);
            }
            int value {0};
            auto add_symbol_pos = string.find('#');
            std::string add_value;
            if(add_symbol_pos != std::string::npos){
                add_value = string.substr(add_symbol_pos + 1);
                string = string.substr(0 , add_symbol_pos );

            }
            assembler::stringsVector operands{string};
            assembler::splitByDelimiters(":[]*" , operands);
            switch (operands.size()){
                case 1:
                    value = getBytesForUserIdetifier(operands.front());
                    if(add_symbol_pos != std::string::npos ){
                        value += getAddCommandSpecialBytes(operands.front() , fromStringToInt(add_value));
                    }
                    break;
                case 3:
                    value = 3;
                    if(isDD_Identifier(operands.back())){
                        value += 1;
                    }
                    if(add_symbol_pos != std::string::npos ){
                        value += getAddCommandSpecialBytes(operands.back() , fromStringToInt(add_value));
                    }
                    break;
                case 8 :
                    value = 1;
                    value += getBytesForRegister(operands[4]);
                    if(isDD_Identifier(operands[2])){
                        value += 1;
                    }
                    if(add_symbol_pos != std::string::npos ){
                        value += getAddCommandSpecialBytes(operands[2], fromStringToInt(add_value));
                        value += 1;
                    }
                    break;
                case 6 :
                    if(assembler::isWordInVector(assembler::segmentRegisters() , operands.front())){
                        value = 1;
                        value += getBytesForRegister(operands[4]);
                        if(isDD_Identifier(operands[2])){
                            value += 1;
                        }
                    } else {
                        value += 5;
                        if(isDD_Identifier(operands.front())){
                            value += 1;
                        }
                    }
                    if(add_symbol_pos != std::string::npos ){
                        value += getAddCommandSpecialBytes(operands.front(), fromStringToInt(add_value));
                        value += 1;
                    }
                    break;
            }
            addValueForALlAddressFromLine(line , value);
        } else {
            if(string.back() == '&'){
                addValueForALlAddressFromLine(line , -3);
            } else {
                addValueForALlAddressFromLine(line , -2);
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

void masm::printAllIdentifiers() const noexcept {
    std::cout << "data segmets: \n";
    for(const auto& it : _data.getAllIdentifiers()){
        std::cout << it.getName()                           << std::setw(10)
                  << assembler::getNameOfIdentifierType(it) << std::setw(10)
                  << it.getValue() << '\n';
    }

    std::cout << "code segmets: \n";
    for(const auto& it : _code.getAllIdentifiers()){
        std::cout << it.getName()                           << std::setw(10)
                  << assembler::getNameOfIdentifierType(it) << std::setw(10)
                  << it.getValue() << '\n';
    }
}
void masm::addValueForALlAddressFromLine(int start_line, int value) {
    for(size_t i = start_line ; i < infoAboutLines.size() ; ++i){
        infoAboutLines[i + 1].address += value;
    }
}
int masm::fromStringToInt(const std::string &string_Value) {
    switch(std::tolower(string_Value.back())){
        case 'h':
            return assembler::fromHexToDec(string_Value);
        case 'b':
            return assembler::fromBinaryToDec(string_Value);
        default :
            return std::stoi(string_Value);
    }
}
int masm::getAddCommandSpecialBytes(const std::string &name, int tmp) {
    int value{0};
    if(isDD_Identifier(name)){
        if( tmp > assembler::MAX_DB_VALUE){
            value += 4;
        } else {
            value += 1;
        }
    } else {
        if(tmp < assembler::MAX_DB_VALUE){
            value += 1;
        } else if(tmp < assembler::MAX_DW_VALUE) {
            value +=2;
        } else {
            value += 4;
        }
    }
    return value;
}