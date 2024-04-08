#ifndef CODEGEN_RUBY_H
#define CODEGEN_RUBY_H

#include "codegen.h"

struct codegen_ruby : codegen_base {
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
      //...
   }
};

#endif
