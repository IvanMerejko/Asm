#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "masm.h"
//template  <typename T>
//std::string getHexCodForValue(T value){
//    auto ptr = (unsigned char *) &value;
//    std::stringstream stream;
//    int oneSymbol;
//    for (int i = 0; i < 8; i++){
//        oneSymbol = *(ptr + i);
//        stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << oneSymbol ;
//    }
//    std::string hexCode;
//    stream >> hexCode;
//
//    return hexCode;
//}

int main() {
//    std::cout << getHexCodForValue(std::stoi("4312")) << '\n';
//    std::cout << getHexCodForValue(std::stod("4312")) << '\n';
   masm{"ts.asm"}.createListingFile();

    return 0;
}