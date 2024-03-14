#ifndef LEXER_PASCAL_H
#define LEXER_PASCAL_H

#include "error.h"
#include "lexer_aux.h"
#include <cctype>
#include <string>
#include <vector>

struct lexer_pascal : lexer_base{
   lexer_pascal( )
   : lexer_base(
   /* palabras */
   {
      {"iniciar-programa", INICIO_PROG},
      {"inicia-ejecucion", INICIO_EJE},
      {"termina-ejecucion", FIN_EJE},
      {"finalizar-programa", FIN_PROG},
      {"define-nueva-instruccion", DEFN_INS},
      {"define-prototipo-instruccion", DEFP_INS},
      {"sal-de-instruccion", SAL_INS},
      {"como", COMO},
      {"apagate", APAGATE},
      {"gira-izquierda", GIRA_IZQ},
      {"avanza", AVANZA},
      {"coge-zumbador", COGE_ZUM},
      {"deja_zumbador", DEJA_ZUM},
      {"entonces", ENTONCES},
      {"mientras", MIENTRAS},
      {"hacer", HACER},
      {"repetir", REPETIR},
      {"veces", VECES},
      {"inicio", INICIO},
      {"fin", FIN},
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
      {"si-no", SINO},
      {"si", SI},
      {"no", NOT},
      {"o", OR},
      {"u", OR},
      {"y", AND},
      {"e", AND}
   },
   /* símbolos */
   {
      {"(", PARENTESIS_IZQ},
      {")", PARENTESIS_DER},
      {";", PUNTO_COMA}
   }) {
      return;
   }

   bool es_comentario_linea(const char*& p) const{
      return false;
   }

   bool es_comentario_bloque(const char*& p) const{
      auto tipo_comentario_bloque = [&](const std::vector<std::string>& del){
         int tam = del[0].size();
         if(std::string(p, p + tam) == del[0]){
            p += tam;
            while(std::string(p, p + tam) != del[1]){
               if(*p == '\0'){
                  throw error("se esperaba " + del[1], p);
               }
               ++p;
            }
            p += tam;
            return true;
         }
         return false;
      };

      return tipo_comentario_bloque({ "(*", "*)" }) || tipo_comentario_bloque({ "{", "}" });
   }

   void salta_espacios(const char*& p) const{
      while(std::isspace(*p)){
         ++p;
      }
   }
};

#endif
