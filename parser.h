#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "parser_aux.h"
#include "parser_expresion.h"
#include "parser_sentencia.h"
#include <memory>
#include <vector>

struct declaracion_funcion {
   std::unique_ptr<expresion> retorno;
   const token_registrado& nombre;
   std::vector<std::unique_ptr<sentencia_declaracion>> parametros;
   std::vector<std::unique_ptr<sentencia>> cuerpo;
};

struct arbol_sintactico {
   std::vector<declaracion_funcion> funciones;
};

declaracion_funcion parser_funcion(const token_registrado*& p) {
   // por hacer (tarea 5)
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
