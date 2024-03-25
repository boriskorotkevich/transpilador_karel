#ifndef PARSER_RUBY_H
#define PARSER_RUBY_H

#include "lexer.h"
#include "parser.h"
#include "parser_aux.h"
#include "parser_expresion.h"

class parser_ruby : public parser_base {
private:
   // funciones para las sentencias

   std::vector<std::unique_ptr<sentencia>> lista_stmt(const token_registrado*& p) const {
      std::vector<std::unique_ptr<sentencia>> res;
      while (skipws(p)->tipo != FIN && p->tipo != DEF && p->tipo != SINO && p->tipo != FIN_ARCHIVO) {
         res.push_back(stmt(p));
      }
      return res;
   }

   std::unique_ptr<sentencia> stmt(const token_registrado*& p) const {
      control_vista cv(p);

      if (es_comando(p->tipo)) {
         auto comando = p;
         espera(++p, SALTO_LINEA);
         return std::make_unique<sentencia_comando>(cv, *comando);
      } else if (p->tipo == SI) {
         auto ex = expr(++p);
         auto parte_si = lista_stmt(p); 
         auto parte_no = decltype(parte_si)( );
         if (p->tipo == SINO) {
            espera(++p, SALTO_LINEA);
            parte_no = lista_stmt(p);
         }
         espera_seq(skipws(p), {FIN, SALTO_LINEA});
         return std::make_unique<sentencia_if>(cv, std::move(ex), std::move(parte_si), std::move(parte_no));
      } else if (p->tipo == MIENTRAS) {
         auto ex = expr(++p);
         auto cuerpo = lista_stmt(p);
         espera_seq(p, {FIN, SALTO_LINEA});
         return std::make_unique<sentencia_while>(cv, std::move(ex), std::move(cuerpo));
      } else if((p->tipo == LITERAL_ENTERA || es_funcion_nativa(p->tipo) || p->tipo == IDENTIFICADOR) && !es_funcion_usuario(p)){
         auto ex = expr(p);
         espera(skipws(p), VECES);
         auto cuerpo = lista_stmt(p);
         espera_seq(p, {FIN, SALTO_LINEA});
         return std::make_unique<sentencia_iterate>(cv, std::move(ex), std::move(cuerpo));
      } else if (p->tipo == SAL_INS) {
         espera(++p, SALTO_LINEA);
         return std::make_unique<sentencia_return>(cv);
      } else if (p->tipo == IDENTIFICADOR) {
         auto nombre = p++;
         auto parametro = expr(p);
         return std::make_unique<sentencia_llamada_usuario>(cv, *nombre, std::move(parametro));
      } else {
         throw error("Sentencia esperada", p->vista);
      }
   }

   // funciones para el árbol sintáctico

   declaracion_funcion parser_funcion(const token_registrado*& p) const {
      espera(skipws(p), es_decl_funcion_ruby);
      auto nombre = espera(skipws(p), IDENTIFICADOR);
      espera(skipws(p), PARENTESIS_IZQ);
      auto parametro = expr(p);
      espera(skipws(p), PARENTESIS_DER);
      auto cuerpo = lista_stmt(p);
      espera_seq(p, {FIN, SALTO_LINEA});
      return declaracion_funcion{*nombre, std::move(parametro), std::move(cuerpo) };
   }

public:
   void parser_arbol(const token_registrado*& p, arbol_sintactico& arbol) const {
      while (skipws(p)->tipo != FIN_ARCHIVO) {
         if(p->tipo == DEF) {
            arbol.funciones.push_back(parser_funcion(p));
         }else{
            arbol.mains.push_back(lista_stmt(p));
         }
      }

//      if(arbol.mains.size() > 1){
//         for(int i = 1; i < arbol.mains.size(); ++i){
//            for(const auto& actual_stmt : arbol.mains[i]){
//               arbol.mains[0].emplace_back(actual_stmt);
//            }
//         }
//      }
   }
};

#endif
