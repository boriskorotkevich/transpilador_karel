#ifndef LEXER_PASCAL_H
#define LEXER_PASCAL_H

#include "lexer_aux.h"
#include "error.h"
#include <string>
#include <map>

struct lexer_pascal : lexer_base{ 
   private:

   std::map<std::string, tipo_token> palabras = {
      {"iniciar-programa", INI_PROG},
      {"inicia-ejecucion", INI_EJE},
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
      {"inicio", INI},
      {"fin", FIN},
      {"precede", PRECEDE},
      {"sucede", SUCEDE},
      {"si-es-cero", IF_CERO},
      {"frente-libre", FRENTE_LIB},
      {"frente-bloqueado", FRENTE_BLOQ},
      {"izquierda-libre", IZQ_LIB},
      {"izquierda-bloqueda", IZQ_BLOQ},
      {"derecha-libre", DER_LIB},
      {"derecha-bloqueada", DER_BLOQ},
      {"junto-a-zumbador", JUNTO_ZUM},
      {"no-junto-a-zumbador", NJUNTO_ZUM},
      {"algun-zumbador-en-la-mochila", ALGUN_ZUM_BAG},
      {"ningun-zumbador-en-la-mochila", NINGUN_ZUM_BAG},
      {"orientado-al-norte", IF_N},
      {"orientado-al-sur", IF_S},
      {"orientado-al-este", IF_E},
      {"orientado-al-oeste", IF_O},
      {"no-orientado-al-norte", NIF_N},
      {"no-orientado-al-sur", NIF_S},
      {"no-orientado-al-este", NIF_E},
      {"no-orientado-al-oeste", NIF_O},
      {"sino", SINO},
      {"si-no", SINO},
      {"si", SI},
   };

   std::map<std::string, tipo_token> operadores = {
      {"no", NOT},
      {"o", OR},
      {"u", OR},
      {"y", AND},
      {"e", AND},
      {"(", PAR_IZQ},
      {")", PAR_DER},
      {";", PUNTO_COMA}
   };

   public:

   std::map<std::string, tipo_token> get_palabras(){
      return palabras;
   }

   std::map<std::string, tipo_token> get_operadores(){
      return operadores;
   }

   bool es_comentario_linea(char*& p){
      if(*p == '{'){
         p += 1;
         while(*p != '}'){
            if(*p == '\0'){
               throw error("se esperaba }", p);
            }
            ++p;
         }
         p += 1;
         return true;
      }
      return false;
   }
   
   bool es_comentario_bloque(char*& p){
      if(*p == '(' && *(p + 1) == '*'){
         p += 2;
         while(*p != '*' || *(p + 1) != ')'){
            if(*p == '\0'){
               throw error("se esperaba *)", p);
            }
            ++p;
         }
         p += 2;
         return true;
      }
      return false;
   }

   bool es_operador(char*& p){
      if(operadores.contains(std::string(p, p + 1))){
         p += 1;
         return true;
      }else if(operadores.contains(std::string(p, p + 2))){
         p += 2;
         return true;
      }
      return false;
   }

   void salta_espacios(char*& p){
      while(isspace(*p)){
         ++p;
      }
   }

   ~lexer_pascal(){ }
};

#endif
