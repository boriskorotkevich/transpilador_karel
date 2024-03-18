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
   std::vector<declaracion_funcion> main;
   std::vector<declaracion_funcion> funciones;
};

declaracion_funcion parser_funcion(const token_registrado*& p) {
   espera(p, VOID);
   auto nombre = espera(p, IDENTIFICADOR); 
   espera(p, PARENTESIS_IZQ); 
   auto parametro = expr(p);
   espera(p, PARENTESIS_DER); 
   espera(p, LLAVE_IZQ); 
   auto cuerpo = lista_stmt(p); 
   espera(p, LLAVE_DER); 
   return declaracion_funcion{*nombre, std::move(parametro), std::move(cuerpo) }; 
}

arbol_sintactico parser(const std::vector<token_registrado>& tokens) {
   auto p = &tokens[0];
   arbol_sintactico arbol;
   while (p->tipo != FIN_ARCHIVO) {
      arbol.funciones.push_back(parser_funcion(p));
   }
   return arbol;
}

#endif
