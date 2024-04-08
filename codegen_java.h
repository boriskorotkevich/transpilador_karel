#ifndef CODEGEN_JAVA_H
#define CODEGEN_JAVA_H

#include "codegen.h"

struct codegen_java : codegen_base {
   using codegen_base::genera;

   void genera(const expresion_termino* ex, std::ostream& os) const {
      //...
   }

   void genera(const expresion_binaria* ex, std::ostream& os) const {
      //...
   }

   void genera(const expresion_prefija* ex, std::ostream& os) const {
      //...
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os) const {
      //...
   }

   void genera(const sentencia_comando* s, std::ostream& os) const {
      //...
   }

   void genera(const sentencia_if* s, std::ostream& os) const {
      //...
   }

   void genera(const sentencia_while* s, std::ostream& os) const {
      //...
   }

   void genera(const sentencia_iterate* s, std::ostream& os) const {
      //...
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os) const {
      //...
   }

   void genera(const sentencia_bloque* s, std::ostream& os) const {
      //...
   }

   void genera(const sentencia_vacia* s, std::ostream& os) const {
      //...
   }

   void genera(const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os) const {
      os << "class program {\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << "void " << "COSAS(COSAS) {\n";
            for (const auto& sentencia : *funcion.cuerpo) {
               genera(sentencia, os);
            }
            os << "}\n";
         }
      }
   }
};

#endif
