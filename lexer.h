#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

enum tipo_token{
   // tokens relacionados con el inicio y fin del programa

   CLASS,            // java
   INICIO_PROG,      // pascal
   FIN_PROG,         // pascal

   // tokens relacionados con la función de arranque

   PROGRAM,          // java
   INICIO_EJE,       // pascal
   FIN_EJE,          // pascal

   // tokens relacionados con la definición de funciones

   VOID,             // java
   DEFINE,           // java
   DEFN_INS,         // pascal
   DEFP_INS,         // pascal
   COMO,             // pascal
   DEF,              // ruby

   // tokens relacionados con terminar una función

   RETURN,           // java
   SAL_INS,          // pascal y ruby

   // tokens relacionados con delimitadores de instrucción

   PUNTO_COMA,       // java
   SALTO_LINEA,      // ruby

   // tokens relacionados con delimitadores de bloque

   LLAVE_IZQ,        // java
   LLAVE_DER,        // java
   INICIO,           // pascal
   FIN,              // pascal y ruby

   // tokens relacionados con if y else

   IF,               // java
   ELSE,             // java
   SI,               // pascal y ruby
   SINO,             // pascal y ruby
   ENTONCES,         // pascal

   // tokens relacionados con while

   WHILE,            // java
   MIENTRAS,         // pascal y ruby
   HACER,            // pascal

   // tokens relacionados con iterate

   ITERATE,          // java
   REPETIR,          // pascal y ruby
   VECES,            // pascal y ruby

   // tokens relacionados con expresiones, comandos y fin de archivo (comunes para todos)

   APAGATE,
   AVANZA,
   GIRA_IZQ,
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
   IDENTIFICADOR,
   LITERAL_ENTERA,
   PARENTESIS_IZQ,
   PARENTESIS_DER,
   FIN_ARCHIVO
};

class lexer_base {
   std::map<std::string, tipo_token> palabras, simbolos;
   int max_tam_simbolo;

public:
   lexer_base(std::map<std::string, tipo_token>&& p, std::map<std::string, tipo_token>&& s)
   : palabras(std::move(p)), simbolos(std::move(s)), max_tam_simbolo(0) {
      for (const auto& [cad, token] : simbolos) {
         max_tam_simbolo = std::max(max_tam_simbolo, int(cad.size( )));
      }
   }

   virtual ~lexer_base( ) = default;
   virtual bool es_comentario_linea(const char*&) const = 0;
   virtual bool es_comentario_bloque(const char*&) const = 0;
   virtual void salta_espacios(const char*&) const = 0;

   bool es_literal_entera(const char*& p) const{
      if(isdigit(*p)){
         do{
            ++p;
         }while(std::isdigit(*p));
         return true;
      }
      return false;
   }

   bool es_identificador(const char*& p) const{
      if(std::isalpha(*p) || *p == '_'){
         do{
            ++p;
         }while(std::isalnum(*p) || *p == '_' || *p == '-');
         return true;
      }
      return false;
   }

   bool es_simbolo(const char*& p) const{
      const char* mejor = p;
      for (int i = 1; i <= max_tam_simbolo; ++i) {
         if(simbolos.contains(std::string(p, p + i))){
            mejor = p + i;
         }
      }
      if (p != mejor) {
         p = mejor;
         return true;
      }
      return false;
   }

   tipo_token token_id_palabra(const std::string& s) const{
      auto itr = palabras.find(s);
      return (itr != palabras.end() ? itr->second : IDENTIFICADOR);
   }

   tipo_token token_simbolo(const std::string& s) const{
      return simbolos.find(s)->second;
   }
};

struct token_registrado{
   tipo_token tipo;
   std::string_view vista;

   token_registrado() = default;

   token_registrado(tipo_token t, std::string_view v)
      : tipo(t), vista(v){
      }

   token_registrado(tipo_token t, const char* ini, const char* fin)
      : tipo(t), vista(ini, fin - ini){
      }
};

std::vector<token_registrado> lexer(const lexer_base& lb, const std::string& s){
   std::vector<token_registrado> ans;
   auto ini = &s[0], fin = ini + s.size();

   while(true){
      lb.salta_espacios(ini);
      auto temp = ini;

      if(lb.es_comentario_linea(ini) || lb.es_comentario_bloque(ini)){
         continue;
      }else if(lb.es_literal_entera(ini)){
         ans.emplace_back(LITERAL_ENTERA, temp, ini);
      }else if(lb.es_identificador(ini)){
         ans.emplace_back(lb.token_id_palabra(std::string(temp, ini)), temp, ini);
      }else if(lb.es_simbolo(ini)){
         ans.emplace_back(lb.token_simbolo(std::string(temp, ini)), temp, ini);
      }else if(ini == fin){
         ans.emplace_back(FIN_ARCHIVO, temp, ini + 1);
         break;
      }else{
         throw error("Token desconocido", temp, ini + 1);
      }
   }

   return ans;
}

#endif
