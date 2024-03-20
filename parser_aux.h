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

const token_registrado* espera(const token_registrado*& p, tipo_token esperado) {
   if (p->tipo != esperado) {
      throw error("Token inesperado", p->vista);
   }
   return p++;
}

const token_registrado* espera(const token_registrado*& p, auto predicado) {
   if (!predicado(p->tipo)) {
      throw error("Token inesperado", p->vista);
   }
   return p++;
}

const token_registrado* espera(const token_registrado*& p, const std::vector<tipo_token>& v){
   for(auto token_esperado : v){
      if(p->tipo != token_esperado){
         throw error("Token inesperado", p->vista);
      }
      ++p;
   }
   return p;
}

bool es_decl_funcion(tipo_token t) {
   return t == VOID || t == DEFINE;
}

bool es_comando(tipo_token t) {
   return t == AVANZA || t == APAGATE ||  t == GIRA_IZQ || t == COGE_ZUM || t == DEJA_ZUM;
}

bool es_operador_prefijo(tipo_token t) {
   return t == NOT;
}

bool es_operador_binario(tipo_token t) {
   return t == AND || t == OR;
}

bool es_booleano(tipo_token t){
   return t == IZQUIERDA_LIB || t == IZQUIERDA_BLOQ || t == FRENTE_LIB || t == FRENTE_BLOQ
      || t == DERECHA_LIB || t == DERECHA_BLOQ || t == JUNTO_ZUM || t == NJUNTO_ZUM || t == ALGUN_ZUM_BAG
      || t == NINGUN_ZUM_BAG || t == ORIENTADO_NORTE || t == ORIENTADO_SUR || t == ORIENTADO_ESTE
      || t == ORIENTADO_OESTE || t == NORIENTADO_NORTE || t == NORIENTADO_SUR || t == NORIENTADO_ESTE || t == NORIENTADO_OESTE;
}

bool es_termino(tipo_token t) {
   return es_booleano(t) || t == IDENTIFICADOR || t == LITERAL_ENTERA;
}

bool es_funcion_nativa(tipo_token t){
   return t == PRECEDE || t == SUCEDE || t == ES_CERO;
}

bool es_inicio_expr(tipo_token t){
   return es_termino(t) || es_operador_prefijo(t) || es_funcion_nativa(t) || t == PARENTESIS_IZQ;
}

int precedencia(tipo_token t) {
   return (t == OR ? 0 : ( t == AND ? 1 : -1 ));
}

int asociatividad(tipo_token t) {
   // 0 para derecha, 1 para izquierda
   return 1;
}

#endif