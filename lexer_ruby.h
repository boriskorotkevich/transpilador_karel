#ifndef LEXER_RUBY_H
#define LEXER_RUBY_H

#include "error.h"
#include "lexer.h"
#include <cctype>
#include <string>
#include <vector>

struct lexer_ruby : lexer_base{
   lexer_ruby( )
   : lexer_base(
   /* palabras */
   {
      {"def", DEF},
      {"apagate", APAGATE},
      {"sal-de-instruccion", SAL_INS},
      {"gira-izquierda", GIRA_IZQ},
      {"avanza", AVANZA},
      {"coge-zumbador", COGE_ZUM},
      {"deja-zumbador", DEJA_ZUM},
      {"fin", FIN},
      {"veces", VECES},
      {"mientras", MIENTRAS},
      {"precede", PRECEDE},
      {"sucede", SUCEDE},
      {"si-es-cero", ES_CERO},
      {"frente-libre", FRENTE_LIB},
      {"frente-bloqueado", FRENTE_BLOQ},
      {"izquierda-libre", IZQUIERDA_LIB},
      {"izquierda-bloqueda", IZQUIERDA_BLOQ},
      {"derecha-libre", DERECHA_LIB},
      {"derecha-bloqueada", DERECHA_BLOQ},
      {"junto-a-zumbador", JUNTO_ZUM},
      {"no-junto-a-zumbador", NJUNTO_ZUM},
      {"algun-zumbador-en-la-mochila", ALGUN_ZUM_BAG},
      {"ningun-zumbador-en-la-mochila", NINGUN_ZUM_BAG},
      {"orientado-al-norte", ORIENTADO_NORTE},
      {"orientado-al-sur", ORIENTADO_SUR},
      {"orientado-al-este", ORIENTADO_ESTE},
      {"orientado-al-oeste", ORIENTADO_OESTE},
      {"no-orientado-al-norte", NORIENTADO_NORTE},
      {"no-orientado-al-sur", NORIENTADO_SUR},
      {"no-orientado-al-este", NORIENTADO_ESTE},
      {"no-orientado-al-oeste", NORIENTADO_OESTE},
      {"sino", SINO},
      {"si", SI},
      {"no", NOT},
      {"o", OR},
      {"u", OR},
      {"y", AND}
   },
   /* símbolos */
   {
      {"(", PARENTESIS_IZQ},
      {")", PARENTESIS_DER},
      {"\n", SALTO_LINEA}
   }) {
      return;
   }

   bool es_comentario_linea(const char*& p) const{
      if(*p == '#'){
         p += 1;
         while(*p != '\n'){
            ++p;
         }
         return true;
      }
      return false;
   }

   bool es_comentario_bloque(const char*& p) const{
      return false;
   }

   void salta_espacios(const char*& p) const{
      while(std::isspace(*p) && *p != '\n'){
         ++p;
      }
   }
};

#endif
