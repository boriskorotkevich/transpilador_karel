#ifndef LEXER_RUBY_H
#define LEXER_RUBY_H

#include "lexer_aux.h"
#include "error.h"
#include <cctype>
#include <string>
#include <map>

struct lexer_ruby : lexer_base{ 
   std::map<std::string, tipo_token> palabras = {
      {"def", DEF},
      {"apagate", APAGATE},
      {"sal-de-instruccion", SAL_INS},
      {"gira-izquierda", GIRA_IZQ},
      {"avanza", AVANZA},
      {"coge-zumbador", COGE_ZUM},
      {"deja_zumbador", DEJA_ZUM},
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
   };

   std::map<std::string, tipo_token> operadores = {
      {"no", NOT},
      {"o", OR},
      {"u", OR},
      {"y", AND},
      {"(", PARENTESIS_IZQ},
      {")", PARENTESIS_DER},
      {"\n", SALTO_LINEA}
   };

   std::map<std::string, tipo_token>& get_palabras(){
      return palabras;
   }

   std::map<std::string, tipo_token>& get_operadores(){
      return operadores;
   }

   bool es_comentario_linea(char*& p){
      if(*p == '#'){
         p += 1;
         while(*p != '\n'){
            ++p;
         }
         return true;
      }
      return false;
   }

   bool es_comentario_bloque(char*& p){
      return false;
   }

   bool es_operador(char*& p){
      if(islower(*p)){
         if(operadores.contains(std::string(p, p + 1)) && !isalnum(*(p + 1))){
            p += 1;
            return true;
         }else if(operadores.contains(std::string(p, p + 2)) && !isalnum(*(p + 2))){
            p += 2;
            return true;
         }
      }else if(operadores.contains(std::string(p, p + 1))){
         p += 1;
         return true;
      }

      return false;
   }

   void salta_espacios(char*& p){
      while(*p == ' ' || *p == '\t' || *p == '\v' || *p == '\f' || *p == '\r'){
         ++p;
      }
   }

   ~lexer_ruby(){ }
};

#endif
