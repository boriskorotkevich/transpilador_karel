#ifndef LEXER_JAVA_H
#define LEXER_JAVA_H

#include "lexer_aux.h"
#include "error.h"
#include <cctype>
#include <string>
#include <map>

struct lexer_java : lexer_base{ 
   std::map<std::string, tipo_token> palabras = {
      {"class", CLASS},
      {"program", PROGRAM},
      {"define", DEFINE},
      {"void", VOID},
      {"return", RETURN},
      {"turnoff", APAGATE},
      {"turnleft", GIRA_IZQ},
      {"move", AVANZA},
      {"pickbeeper", COGE_ZUM},
      {"putbeeper", DEJA_ZUM},
      {"while", WHILE},
      {"iterate", ITERATE},
      {"pred", PRECEDE},
      {"succ", SUCEDE},
      {"iszero", ES_CERO},
      {"frontIsClear", FRENTE_LIB},
      {"frontIsBlocked", FRENTE_BLOQ},
      {"leftIsClear", IZQUIERDA_LIB},
      {"leftIsBlocked", IZQUIERDA_BLOQ},
      {"rightIsClear", DERECHA_LIB},
      {"rightIsBlocked", DERECHA_BLOQ},
      {"nextToABeeper", JUNTO_ZUM},
      {"notNextToABeeper", NJUNTO_ZUM},
      {"anyBeepersInBeeperBag", ALGUN_ZUM_BAG},
      {"noBeepersInBeeperBag", NINGUN_ZUM_BAG},
      {"facingNorth", ORIENTADO_NORTE},
      {"facingSouth", ORIENTADO_SUR},
      {"facingEast", ORIENTADO_ESTE},
      {"facingWest", ORIENTADO_OESTE},
      {"notFacingNorth", NORIENTADO_NORTE},
      {"notFacingSouth", NORIENTADO_SUR},
      {"notFacingEast", NORIENTADO_ESTE},
      {"notFacingWest", NORIENTADO_OESTE},
      {"else", ELSE},
      {"if", IF},
   };

   std::map<std::string, tipo_token> operadores = {
      {"!", NOT},
      {"||", OR},
      {"&&", AND},
      {"&", AND},
      {"(", PARENTESIS_IZQ},
      {")", PARENTESIS_DER},
      {"{", LLAVE_IZQ},
      {"}", LLAVE_DER},
      {";", PUNTO_COMA}
   };

   std::map<std::string, tipo_token>& get_palabras(){
      return palabras;
   }

   std::map<std::string, tipo_token>& get_operadores(){
      return operadores;
   }

   bool es_comentario_linea(char*& p){
      if(std::string(p, p + 2) == "//"){
         p += 2;
         while(*p != '\n'){
            ++p;
         }
         return true;
      }
      return false;
   }

   bool es_comentario_bloque(char*& p){
      if(std::string(p, p + 2) == "/*"){
         p += 2;
         while(std::string(p, p + 2) != "*/"){
            if(*p == '\0'){
               throw error("se esperaba */", p);
            }
            ++p;
         }
         p += 2;
         return true;
      }
      return false;
   }

   bool es_operador(char*& p){
      if(operadores.contains(std::string(p, p + 2))){
         p += 2;
         return true;
      }else if(operadores.contains(std::string(p, p + 1))){
         p += 1;
         return true;
      }

      return false;
   }

   void salta_espacios(char*& p){
      while(isspace(*p)){
         ++p;
      }
   }

   ~lexer_java(){ }
};


#endif
