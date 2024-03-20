#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "parser_aux.h"
#include "parser_expresion.h"
#include "parser_sentencia.h"

#include <memory>
#include <vector>

struct declaracion_funcion {
   const token_registrado& nombre;
   std::unique_ptr<expresion> parametro;
   std::vector<std::unique_ptr<sentencia>> cuerpo;
};

struct arbol_sintactico {
   std::vector<std::vector<std::unique_ptr<sentencia>>> mains;
   std::vector<declaracion_funcion> funciones;
};

declaracion_funcion parser_funcion(const token_registrado*& p) {
   espera(p, es_decl_funcion);
   auto nombre = espera(p, IDENTIFICADOR);
   espera(p, PARENTESIS_IZQ);
   std::unique_ptr<expresion> parametro = (p->tipo != IDENTIFICADOR ? nullptr : std::make_unique<expresion_termino>(control_vista(p), *p++));
   espera(p, {PARENTESIS_DER, LLAVE_IZQ});
   auto cuerpo = lista_stmt(p);
   espera(p, LLAVE_DER);
   return declaracion_funcion{*nombre, std::move(parametro), std::move(cuerpo) };
}

std::vector<std::unique_ptr<sentencia>> parser_main(const token_registrado*& p){
   espera(p, {PROGRAM, PARENTESIS_IZQ, PARENTESIS_DER, LLAVE_IZQ});
   auto cuerpo = lista_stmt(p);
   espera(p, LLAVE_DER);
   return std::move(cuerpo);
}

arbol_sintactico parser(const std::vector<token_registrado>& tokens) {
   auto p = &tokens[0];
   arbol_sintactico arbol;
   espera(p, {CLASS, PROGRAM, LLAVE_IZQ});
   while (p <= &tokens[tokens.size() - 1]) {
      if((p + 1)->tipo == FIN_ARCHIVO){
         espera(p, LLAVE_DER);
         break;
      }else if(p->tipo == PROGRAM){
         arbol.mains.push_back(parser_main(p));
      }else{
         arbol.funciones.push_back(parser_funcion(p));
      }
   }
   return arbol;
}

#endif
