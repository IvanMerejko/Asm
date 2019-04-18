//
// Created by ivan on 15.04.19.
//

#ifndef ASM_TYPES_H
#define ASM_TYPES_H
#include "assembler.h"
namespace assembler{


    constexpr auto MAX_DB_VALUE = static_cast<int>(std::numeric_limits<char>::max());
    constexpr auto MAX_DW_VALUE = static_cast<int>(std::numeric_limits<short>::max());
    constexpr auto MAX_DD_VALUE = std::numeric_limits<int>::max();


    enum class WordType{
        IDENTIFIER,
        COMMAND,
        CONSTANT,
        DIRECTIVE,
        INSTRUCTION,
        LABEL,
        ONE_SYMBOLE_LEXEM,
        REGISTER,
        NONE
    };


    enum class IdentifierType{
        DB = 1,
        DW = 2,
        DD = 4 ,
        INCORRECT_IDENTIFIER
    };
    enum class CommandName : int{
        MOV    = 1,
        IMUL   = 2,
        IDIV   = 3,
        OR     = 4,
        CMP    = 5,
        JNG    = 6,
        AND    = 7,
        ADD    = 8,
        CWDE    = 9,
        MODEL   =10,
    };
    using stringsVector = std::vector<std::string>;
    using lexem_type = std::vector<std::pair<std::string , WordType>>;
}
#endif //ASM_TYPES_H
