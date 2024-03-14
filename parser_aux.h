#ifndef PARSER_AUX_H
#define PARSER_AUX_H

#include "lexer.h"
#include <string_view>

struct control_vista {
   const token_anotado* ini;
   const token_anotado*& ref;

   control_vista(const token_anotado*& p)
   : ini(p), ref(p) {
   }

   explicit operator std::string_view( ) const {
      return { ini->vista.begin( ), size_t(ini == ref ? 0 : (ref - 1)->vista.end( ) - ini->vista.begin( )) };
   }
};

const token_anotado* espera(const token_anotado*& p, token esperado) {
   if (p->tipo != esperado) {
      throw error("Token inesperado", p->vista);
   }
   return p++;
}

bool es_operador_prefijo(token t) {
   //...
}

bool es_operador_binario(token t) {
   //...
}

bool es_comando(token t) {

}

bool es_inicio_expr(token t) {
   //...
}

int precedencia(token t) {
   //...
}

int asociatividad(token t) {
   // 0 para derecha, 1 para izquierda
}

#endif
