#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "semantico_aux.h"

#include <map>
#include <set>
#include <string_view>
#include <vector>

struct tabla_simbolos {
   bool sensible;
   std::map<std::string, const declaracion_funcion*, std::less<>> funciones;
   std::map<const declaracion_funcion*, std::set<const declaracion_funcion*>> grafica_dependencias;

   tabla_simbolos(bool s)
   : sensible(s) {
   }

   bool inserta(const std::string_view& s, const declaracion_funcion* d) {
      return inserta_simbolo(funciones, s, d, sensible);
   }

   void elimina(const std::string_view& s){
      elimina_simbolo(funciones, s, sensible);
   }

   const declaracion_funcion* busca(const std::string_view& s) const {
      return busca_simbolo(funciones, s, sensible);
   }
};

struct pila_simbolos {
   const tabla_simbolos& tabla;
   std::map<std::string, const token_registrado*, std::less<>> bloque;

   bool inserta(const std::string_view& s, const token_registrado* p) {
      return inserta_simbolo(bloque, s, p, tabla.sensible);
   }

   const declaracion_funcion* busca_funcion(const std::string_view& s) const {
      return tabla.busca(s);
   }

   const token_registrado* busca_variable(const std::string_view& s) const {
      return busca_simbolo(bloque, s, tabla.sensible);
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
   if(evalua(ex->izq, pila) == BOOL && evalua(ex->der, pila) == BOOL){
      if(ex->op.tipo == AND || ex->op.tipo == OR){
         return BOOL;
      }
   }else{
      throw error("Los operandos deben ser de tipo booleano", ex->vista);
   }
   throw error("Operador binario invalido", ex->vista);
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

void evalua(const sentencia_comando* s, auto& pila, auto& dependencias) {
   return;
}

void evalua(const sentencia_if* s, auto& pila, auto& dependencias) {
   if(evalua(s->condicion, pila) != BOOL){
      throw error("Tipo incorrecto en condicion", s->vista);
   }
   for(const auto& sentencia : s->parte_si){
      evalua(sentencia, pila, dependencias);
   }
   for(const auto& sentencia : s->parte_no){
      evalua(sentencia, pila, dependencias);
   }
}

void evalua(const sentencia_while* s, auto& pila, auto& dependencias) {
   if (evalua(s->condicion, pila) != BOOL) {
      throw error("Tipo incorrecto en condicion", s->vista);
   }
   for (const auto& actual : s->cuerpo) {
      evalua(actual, pila, dependencias);
   }
}

void evalua(const sentencia_iterate* s, auto& pila, auto& dependencias) {
   if (evalua(s->condicion, pila) != INT) {
      throw error("Tipo incorrecto en condicion", s->vista);
   }
   for (const auto& actual : s->cuerpo){
      evalua(actual, pila, dependencias);
   }
}

void evalua(const sentencia_llamada_usuario* s, auto& pila, auto& dependencias) {
   auto decl = pila.busca_funcion(s->funcion.vista);
   if (decl == nullptr) {
      throw error("Llamada a funcion no declarada", s->vista);
   }else if (decl->cuerpo == nullptr) {
      throw error("Llamada a funcion declarada pero no definida", s->vista);
   }else if ((decl->parametro == nullptr) != (s->parametro == nullptr)) {
      throw error("Numero de argumentos incorrecto", s->vista);
   }else if (decl->parametro != nullptr && evalua(s->parametro, pila) != INT) {
      throw error("Tipo de argumento incorrecto", s->vista);
   }
   dependencias.insert(decl);
}

void evalua(const sentencia_bloque* s, auto& pila, auto& dependencias) {
   for (const auto& actual : s->cuerpo) {
      evalua(actual, pila, dependencias);
   }
}

void evalua(const sentencia_vacia* s, auto& pila, auto& dependencias) {
   return;
}

tabla_simbolos semantico(const arbol_sintactico& arbol, const token_registrado& fin_archivo, bool sensible) {
   tabla_simbolos tabla(sensible);

   for (const auto& funcion : arbol.funciones) {
      if(!tabla.inserta(funcion.nombre.vista, &funcion)){
         auto decl_previa = tabla.busca(funcion.nombre.vista);
         if(decl_previa->cuerpo == nullptr && funcion.cuerpo == nullptr){
            throw error("Redeclaracion de funcion", funcion.nombre.vista);
         }else if(decl_previa->cuerpo == nullptr && funcion.cuerpo != nullptr){
            if((decl_previa->parametro == nullptr) == (funcion.parametro == nullptr)){
               tabla.elimina(funcion.nombre.vista);
               tabla.inserta(funcion.nombre.vista, &funcion);
            }else{
               throw error("El numero de parametros de la declaracion no coincide con el de su definicion", funcion.nombre.vista);
            }
         }else if(decl_previa->cuerpo != nullptr && funcion.cuerpo == nullptr){
            throw error("Una declaracion de funcion no debe aparecer despues de su definicion", funcion.nombre.vista);
         }else if(decl_previa->cuerpo != nullptr && funcion.cuerpo != nullptr){
            throw error("Redefinicion de funcion", funcion.nombre.vista);
         }
      }
   }

   for (const auto& funcion : arbol.funciones) {
      if(funcion.cuerpo != nullptr){
         pila_simbolos pila(tabla);
         if (funcion.parametro != nullptr) {
            pila.inserta(funcion.parametro->vista, funcion.parametro);
         }
         for (const auto& sentencia : *funcion.cuerpo) {
            evalua(sentencia, pila, tabla.grafica_dependencias[&funcion]);
         }
      }
   }

   if (arbol.mains.empty( )) {
      throw error("El programa no tiene funcion principal", fin_archivo.vista);
   } else if (arbol.mains.size( ) > 1) {
      throw error("El programa tiene mas de una funcion principal", fin_archivo.vista);
   } else {
      pila_simbolos pila(tabla);
      for (const auto& sentencia : arbol.mains[0]) {
         evalua(sentencia, pila, tabla.grafica_dependencias[nullptr]);
      }
   }

   return tabla;
}

#endif
