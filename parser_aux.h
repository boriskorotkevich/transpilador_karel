#ifndef PARSER_AUX_H
#define PARSER_AUX_H

#include "lexer.h"
#include <string_view>

struct control_vista {
   const token_registrado* ini;
   const token_registrado*& ref;

   control_vista(const token_registrado*& p)
   : ini(p), ref(p) {
   }

   explicit operator std::string_view( ) const {
      return { ini->vista.begin( ), size_t(ini == ref ? 0 : (ref - 1)->vista.end( ) - ini->vista.begin( )) };
   }
};

const token_registrado* espera(const token_registrado*& p, token esperado) {
   if (p->tipo != esperado) {
      throw error("Token inesperado", p->vista);
   }
   return p++;
}

bool es_operador_prefijo(token t) {
   return t == NOT;
}

bool es_operador_binario(token t) {
   return t == AND || t == OR;
}

bool es_comando(token t) {
   return t == AVANZA || t == APAGATE ||  t == GIRA_IZQ || t == COGE_ZUM || t == DEJA_ZUM;
}

bool es_predicado(token t){
   return t == IZQUIERDA_LIB || t == IZQUIERDA_BLOQ || t == FRENTE_LIB || t == FRENTE_BLOQ
      || t == DERECHA_LIB || t == DERECHA_BLOQ || t == JUNTO_ZUM || t == NJUNTO_ZUM || t == ALGUN_ZUM_BAG 
      || t == NINGUN_ZUM_BAG || t == ORIENTADO_NORTE || t == ORIENTADO_SUR || t == ORIENTADO_ESTE 
      || t == ORIENTADO_OESTE || t == NORIENTADO_NORTE || t == NORIENTADO_SUR || t == NORIENTADO_ESTE || t == NORIENTADO_OESTE;
}

bool es_funcion_nativa(token t){
   return t == PRECEDE || t == SUCEDE;
}

bool es_inicio_expr(token t){
   // ... 
}

int precedencia(token t) {
   return ( t == OR ? 0 : ( t == AND ? 1 : -1 ));
}

int asociatividad(token t) {
   // 0 para derecha, 1 para izquierda
   return 1;
}

#endif
