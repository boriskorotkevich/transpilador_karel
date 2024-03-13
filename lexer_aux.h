#ifndef LEXER_AUX_H
#define LEXER_AUX_H

#include <string_view>
#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "error.h"

enum tipo_token{ 
   /* Exclusivas de Ruby */

   DEF,
   SALTO_LINEA,

   /* Exclusivas de Java */

   CLASS,
   PROGRAM,
   RETURN,
   DEFINE,
   VOID,
   WHILE,
   ITERATE,
   LLAVE_IZQ,
   LLAVE_DER,
   IF,
   ELSE,

   /* Exclusivas de Pascal */

   INICIO_PROG,
   INICIO_EJE,
   FIN_PROG,
   FIN_EJE,
   COMO,
   INICIO,
   HACER,
   REPETIR,
   ENTONCES,
   DEFN_INS,
   DEFP_INS,

   /* Comunes entre Ruby y Pascal */

   MIENTRAS,
   SAL_INS,
   VECES,
   FIN,
   SINO,
   SI,

   /* Comunes entre Java y Pascal */

   PUNTO_COMA,

   /* Comunes entre todos */

   APAGATE,
   GIRA_IZQ,
   AVANZA,
   COGE_ZUM,
   DEJA_ZUM,
   PRECEDE,
   SUCEDE,
   ES_CERO,
   FRENTE_LIB,
   FRENTE_BLOQ,
   IZQUIERDA_LIB,
   IZQUIERDA_BLOQ,
   DERECHA_LIB,
   DERECHA_BLOQ,
   JUNTO_ZUM,
   NJUNTO_ZUM,
   ALGUN_ZUM_BAG,
   NINGUN_ZUM_BAG,
   ORIENTADO_NORTE,
   ORIENTADO_SUR,
   ORIENTADO_ESTE,
   ORIENTADO_OESTE,
   NORIENTADO_NORTE,
   NORIENTADO_SUR,
   NORIENTADO_ESTE,
   NORIENTADO_OESTE,
   NOT,
   OR,
   AND,
   PARENTESIS_DER,
   PARENTESIS_IZQ,
   LITERAL_ENTERA,
   IDENTIFICADOR,
   FIN_ARCHIVO
};

struct lexer_base{
   std::map<std::string, tipo_token> palabras, operadores;

   lexer_base() = default;
   virtual std::map<std::string, tipo_token>& get_operadores() = 0;
   virtual std::map<std::string, tipo_token>& get_palabras() = 0;
   virtual bool es_comentario_linea(char*&) = 0;
   virtual bool es_comentario_bloque(char*&) = 0;
   virtual bool es_operador(char*&) = 0;
   virtual void salta_espacios(char*&) = 0;

   bool es_literal_entera(char*& p) const{
      if(isdigit(*p)){
         do{
            ++p;
         }while(isdigit(*p));
         return true;
      }
      return false;
   }

   bool es_identificador(char*& p) const{
      if(isalpha(*p) || *p == '_'){
         do{
            ++p;
         }while(isalnum(*p) || *p == '_' || *p == '-');
         return true;
      }
      return false;
   }

   virtual ~lexer_base(){ }
};

struct token_registrado{
   tipo_token tipo;
   std::string_view vista;

   token_registrado() = default;
   token_registrado(tipo_token _tipo, const char* ini, const char* fin)
      : tipo(_tipo), vista(ini, fin - ini){
      }
};

tipo_token token_palabra(const std::map<std::string, tipo_token>& palabras, const std::string& s){
   auto itr = palabras.find(s);
   if(itr != palabras.end()){
      return itr->second;
   }else{
      return IDENTIFICADOR;
   }
}

tipo_token token_operador(const std::map<std::string, tipo_token>& operadores, const std::string& s){
   return operadores.find(s)->second;
}

std::vector<token_registrado> lexer(lexer_base* lb, std::string& s){
   std::vector<token_registrado> ans;
   auto ini = &s[0], fin = ini + s.size();

   while(true){
      lb->salta_espacios(ini);
      auto temp = ini;

      if(lb->es_comentario_linea(ini) || lb->es_comentario_bloque(ini)){
         continue;
      }else if(lb->es_literal_entera(ini)){
         ans.emplace_back(LITERAL_ENTERA, temp, ini);
      }else if(lb->es_operador(ini)){
         ans.emplace_back(token_operador(lb->get_operadores(), std::string(temp, ini)), temp, ini);
      }else if(lb->es_identificador(ini)){
         ans.emplace_back(token_palabra(lb->get_palabras(), std::string(temp, ini)), temp, ini);
      }else if(ini == fin){
         ans.emplace_back(FIN_ARCHIVO, temp, ini + 1);
         break;
      }else{
         throw error("Token desconocido", temp, ini + 1);
      }
   }

   return std::move(ans);
}

#endif
