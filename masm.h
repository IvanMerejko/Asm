//
// Created by ivan on 10.03.19.
//

#ifndef CURS_MASM_H
#define CURS_MASM_H

#include <memory>
#include <map>
#include "assembler.h"

class masm {
private:
    struct info{
        bool isErrorInLine;
        int address;
        assembler::WordType type;
    };
public:
    std::string asmFileName;
    std::string listFileName;

    assembler::data _data;
    assembler::code _code;

    assembler::stringsVector wordsInString;
    std::string_view lastOpenedSegment;
    bool endOfFile{false};

    size_t line{0};
    std::map<int , info> infoAboutLines{};
public:
    explicit masm(const std::string &);
    void createListingFile() ;

private:
    void createAddress();
    bool pushIdentifier(assembler::identifier&& new_identifier)   ;
    void workWithIdentifier() ;
    void workWithCommand() ;
    void firstView();
    void secondView();
    bool takeLabelsFromLine();
    inline bool isLabel(std::string_view  word);
    void printLexems(const assembler::lexem_type& lexems);
    std::string getStringByType(const std::string& word , assembler::WordType type);
    void checkAllUsedButNotDeclaredIdentifiers();
    void currentAddressEqualsToPreviousAddress() ;
    size_t getBytesForUserIdetifier(const std::string& ident);
    size_t getBytesForRegister(const std::string& reg);
    bool isDD_Identifier(const std::string& ident);
};


#endif //CURS_MASM_H
