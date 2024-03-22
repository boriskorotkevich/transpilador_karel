#ifndef PARSER_JAVA_H
#define PARSER_JAVA_H

#include "lexer.h"
#include "parser.h"
#include "parser_aux.h"
#include "parser_expresion.h"

class parser_java : public parser_base {
private:
   // funciones para las sentencias

   std::vector<std::unique_ptr<sentencia>> lista_stmt(const token_registrado*& p) const {
      std::vector<std::unique_ptr<sentencia>> res;
      while (p->tipo != LLAVE_DER) {
         res.push_back(stmt(p));
      }
      return std::move(res);
   }

   std::vector<std::unique_ptr<sentencia>> one_stmt(const token_registrado*& p) const {
      std::vector<std::unique_ptr<sentencia>> res;
      res.push_back(stmt(p));
      return std::move(res);
   }

   std::unique_ptr<sentencia> stmt(const token_registrado*& p) const {
      control_vista cv(p);

      auto cuerpo_stmt = [&](const token_registrado*& p) -> std::vector<std::unique_ptr<sentencia>>{
         std::vector<std::unique_ptr<sentencia>> ans;
         if(p->tipo != LLAVE_IZQ){
            ans = one_stmt(p);
         }else{
            espera(p, LLAVE_IZQ);
            ans = lista_stmt(p);
            espera(p, LLAVE_DER);
         }

         return std::move(ans);
      };

      if (es_comando(p->tipo)) {
         auto comando = p;
         espera(++p, {PARENTESIS_IZQ, PARENTESIS_DER, PUNTO_COMA});
         return std::make_unique<sentencia_comando>(cv, *comando);
      } else if (p->tipo == IF && (p + 1)->tipo == PARENTESIS_IZQ) {
         auto ex = expr(++p);
         auto parte_si = cuerpo_stmt(p);
         auto parte_no = decltype(parte_si)( );
         if (p->tipo == ELSE) {
            parte_no = cuerpo_stmt(++p);
         }
         return std::make_unique<sentencia_if>(cv, std::move(ex), std::move(parte_si), std::move(parte_no));
      } else if (p->tipo == WHILE && (p + 1)->tipo == PARENTESIS_IZQ) {
         auto ex = expr(++p);
         auto cuerpo = cuerpo_stmt(p);
         return std::make_unique<sentencia_while>(cv, std::move(ex), std::move(cuerpo));
      } else if(p->tipo == ITERATE && (p + 1)->tipo == PARENTESIS_IZQ){
         auto ex = expr(++p);
         auto cuerpo = cuerpo_stmt(p);
         return std::make_unique<sentencia_iterate>(cv, std::move(ex), std::move(cuerpo));
      } else if (p->tipo == RETURN) {
         espera(++p, PUNTO_COMA);
         return std::make_unique<sentencia_return>(cv);
      } else if (p->tipo == IDENTIFICADOR) {
         auto nombre = p;
         espera(++p, PARENTESIS_IZQ);
         std::unique_ptr<expresion> parametro = (p->tipo == IDENTIFICADOR || p->tipo == LITERAL_ENTERA ? std::make_unique<expresion_termino>(control_vista(p), *p++) : nullptr);
         espera(p, {PARENTESIS_DER, PUNTO_COMA});
         return std::make_unique<sentencia_llamada_usuario>(cv, *nombre, std::move(parametro));
      } else {
         throw error("Sentencia esperada", p->vista);
      }
   }

   // funciones para el árbol sintáctico

   declaracion_funcion parser_funcion(const token_registrado*& p) const {
      espera(p, es_decl_funcion_java);
      auto nombre = espera(p, IDENTIFICADOR);
      espera(p, PARENTESIS_IZQ);
      std::unique_ptr<expresion> parametro = (p->tipo != IDENTIFICADOR ? nullptr : std::make_unique<expresion_termino>(control_vista(p), *p++));
      espera(p, {PARENTESIS_DER, LLAVE_IZQ});
      auto cuerpo = lista_stmt(p);
      espera(p, LLAVE_DER);
      return declaracion_funcion{*nombre, std::move(parametro), std::move(cuerpo) };
   }

   std::vector<std::unique_ptr<sentencia>> parser_main(const token_registrado*& p) const {
      espera(p, {PROGRAM, PARENTESIS_IZQ, PARENTESIS_DER, LLAVE_IZQ});
      auto cuerpo = lista_stmt(p);
      espera(p, LLAVE_DER);
      return std::move(cuerpo);
   }

public:
   void parser_arbol(const token_registrado*& p, arbol_sintactico& arbol) const {
      espera(p, {CLASS, PROGRAM, LLAVE_IZQ});
      while (p->tipo != LLAVE_DER) {
         if(p->tipo == PROGRAM){
            arbol.mains.push_back(parser_main(p));
         }else {
            arbol.funciones.push_back(parser_funcion(p));
         }
      }
      ++p;
   }
};

#endif
