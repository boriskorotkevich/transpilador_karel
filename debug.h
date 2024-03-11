#include <iostream>
#include "lexer_aux.h"

std::ostream& operator<<(std::ostream& os, const token_registrado& tr){
   os << tr.tipo << " " << tr.vista;
   return os;
}

