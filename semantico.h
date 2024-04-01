#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "semantico_aux.h"
#include <set>
#include <map>
#include <string_view>
#include <vector>

struct tabla_simbolos {
   std::map<std::string_view, const declaracion_funcion*> funciones;
};

struct pila_simbolos {
   const tabla_simbolos& tabla;
   std::map<std::string_view, const token_registrado*> bloque;

   const declaracion_funcion* busca_funcion(const std::string_view& s) const {
      auto iter = tabla.funciones.find(s);
      return (iter != tabla.funciones.end( ) ? iter->second : nullptr);
   }

   const token_registrado* busca_variable(const std::string_view& s) const {
      if (auto iter = bloque.find(s); iter != bloque.end( )) {
         return iter->second;
      }
      return nullptr;
   }
};

tipo_evaluado evalua(const expresion_termino* ex, auto& pila) {
   if (ex->token.tipo == IDENTIFICADOR) {
      if (pila.busca_variable(ex->token.vista) != nullptr) {
         return INT;
      }
      throw error("El identificador no denota una variable", ex->vista);
   } else if (ex->token.tipo == LITERAL_ENTERA) {
      return INT;
   } else if (es_booleano(ex->token.tipo)) {
      return BOOL;
   } else {
      throw error("Termino inesperado en expresion", ex->vista);
   }
}

tipo_evaluado evalua(const expresion_binaria* ex, auto& pila) {
   //...
}

tipo_evaluado evalua(const expresion_prefija* ex, auto& pila) {
   if (ex->op.tipo == NOT && evalua(ex->ex, pila) == BOOL) {
      return BOOL;
   }
   throw error("Operador prefijo invalido", ex->vista);
}

tipo_evaluado evalua(const expresion_llamada_nativa* ex, auto& pila) {
   auto temp = evalua(ex->parametro, pila);
   if ((ex->funcion.tipo == PRECEDE || ex->funcion.tipo == SUCEDE) && temp == INT) {
      return INT;
   } else if (ex->funcion.tipo == ES_CERO && temp == INT) {
      return BOOL;
   }
   throw error("Llamada invalida a funcion incorporada", ex->vista);
}

void evalua(const sentencia_comando* s, auto& pila) {
   return;
}

void evalua(const sentencia_if* s, auto& pila) {
   //...
}

void evalua(const sentencia_while* s, auto& pila) {
   if (evalua(s->condicion, pila) != BOOL) {
      throw error("Tipo incorrecto en condicion", s->vista);
   }
   for (const auto& actual : s->cuerpo) {
      evalua(actual, pila);
   }
}

void evalua(const sentencia_iterate* s, auto& pila) {
   //...
}

void evalua(const sentencia_llamada_usuario* s, auto& pila) {
   if (pila.busca_variable(s->funcion.vista) != nullptr) {
      throw error("El identificador no denota una funcion", s->vista);
   }

   auto decl = pila.busca_funcion(s->funcion.vista);
   if (decl == nullptr) {
      throw error("Llamada a funcion no declarada", s->vista);
   } else if ((decl->parametro == nullptr) != (s->parametro == nullptr)) {
      throw error("Numero de argumentos incorrecto", s->vista);
   } else if (decl->parametro != nullptr && evalua(s->parametro, pila) != INT) {
      throw error("Tipo de argumento incorrecto", s->vista);
   }
}

void evalua(const sentencia_bloque* s, auto& pila) {
   for (const auto& actual : s->cuerpo) {
      evalua(actual, pila);
   }
}

void evalua(const sentencia_vacia* s, auto& pila) {
   return;
}

tabla_simbolos semantico(const arbol_sintactico& arbol, const token_registrado& fin_archivo) {
   tabla_simbolos tabla;
   //... hacer algo con los prototipos
   for (const auto& funcion : arbol.funciones) {
      if (!tabla.funciones.emplace(funcion.nombre.vista, &funcion).second) {
         throw error("Nombre de funcion repetida", funcion.nombre.vista);
      }
   }
   for (const auto& funcion : arbol.funciones) {
      auto& decl = tabla.funciones.find(funcion.nombre.vista)->second;
      pila_simbolos pila(tabla);
      if (decl->parametro != nullptr) {
         pila.bloque.emplace(decl->parametro->vista, decl->parametro);
      }
      for (const auto& sentencia : decl->cuerpo) {
         evalua(sentencia, pila);
      }
   }

   if (arbol.mains.empty( )) {
      throw error("El programa no tiene funcion principal", fin_archivo.vista);
   } else if (arbol.mains.size( ) > 1) {
      throw error("El programa mas de un funcion principal", fin_archivo.vista);
   } else {
      pila_simbolos pila(tabla);
      for (const auto& sentencia : arbol.mains[0]) {
         evalua(sentencia, pila);
      }
   }

   return tabla;
}

#endif
