#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "parser_aux.h"
#include "parser_expresion.h"
#include "parser_sentencia_pascal.h"

#include <memory>
#include <vector>

struct declaracion_funcion {
   const token_registrado& nombre;
   std::unique_ptr<expresion> parametro;
   std::vector<std::unique_ptr<sentencia>> cuerpo;
};

struct declaracion_prototipo{
   const token_registrado& nombre;
   std::unique_ptr<expresion> parametro;
};

struct arbol_sintactico {
   std::vector<std::vector<std::unique_ptr<sentencia>>> mains;
   std::vector<declaracion_prototipo> prototipos;
   std::vector<declaracion_funcion> funciones;
};

declaracion_funcion parser_funcion(const token_registrado*& p) {
   espera(p, es_decl_funcion_pascal);
   auto nombre = espera(p, IDENTIFICADOR);
   auto parametro = (p->tipo == PARENTESIS_IZQ ? expr(p) : nullptr);
   espera(p, {COMO, INICIO});
   auto cuerpo = lista_stmt(p);
   espera(p, {FIN, PUNTO_COMA});
   return declaracion_funcion{*nombre, std::move(parametro), std::move(cuerpo) };
}

std::vector<std::unique_ptr<sentencia>> parser_main(const token_registrado*& p){
   espera(p, INICIO_EJE);
   auto cuerpo = lista_stmt(p);
   espera(p, FIN_EJE);
   return std::move(cuerpo);
}

declaracion_prototipo parser_prot(const token_registrado*& p){
   espera(p, DEFP_INS);
   auto nombre = espera(p, IDENTIFICADOR);
   auto parametro = (p->tipo == PARENTESIS_IZQ ? expr(p) : nullptr);
   espera(p, PUNTO_COMA);
   return declaracion_prototipo{*nombre, std::move(parametro)};
}

arbol_sintactico parser_pascal(const std::vector<token_registrado>& tokens) {
   auto p = &tokens[0];
   arbol_sintactico arbol;
   espera(p, INICIO_PROG);
   while (p <= &tokens[tokens.size() - 1]) {
      if((p + 1)->tipo == FIN_ARCHIVO){
         espera(p, FIN_PROG);
         break;
      }else if(p->tipo == INICIO_EJE){
         arbol.mains.push_back(parser_main(p));
      }else if(p->tipo == DEFN_INS){
         arbol.funciones.push_back(parser_funcion(p));
      }else if(p->tipo == DEFP_INS){
         arbol.prototipos.push_back(parser_prot(p));
      }
   }
   return std::move(arbol);
}

#endif
