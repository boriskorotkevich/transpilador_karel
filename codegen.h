#ifndef CODEGEN_H
#define CODEGEN_H

#include "codegen_aux.h"
#include "parser.h"
#include "semantico.h"
#include <sstream>

struct codegen_base {
   void genera(const std::unique_ptr<expresion>& ex, auto&... params) const {
      ::genera(ex, *this, params...);
   }

   void genera(const std::unique_ptr<sentencia>& s, auto&... params) const {
      ::genera(s, *this, params...);
   }

   virtual void genera(const expresion_termino*, std::ostream&) const = 0;
   virtual void genera(const expresion_binaria*, std::ostream&) const = 0;
   virtual void genera(const expresion_prefija*, std::ostream&) const = 0;
   virtual void genera(const expresion_llamada_nativa*, std::ostream&) const = 0;

   virtual void genera(const sentencia_comando*, std::ostream&) const = 0;
   virtual void genera(const sentencia_if*, std::ostream&) const = 0;
   virtual void genera(const sentencia_while*, std::ostream&) const = 0;
   virtual void genera(const sentencia_iterate*, std::ostream&) const = 0;
   virtual void genera(const sentencia_llamada_usuario*, std::ostream&) const = 0;
   virtual void genera(const sentencia_bloque*, std::ostream&) const = 0;
   virtual void genera(const sentencia_vacia*, std::ostream&) const = 0;

   virtual void genera(const arbol_sintactico&, const tabla_simbolos&, std::ostream&) const = 0;
};

template<typename T>
void genera(const T* p, const codegen_base& cb, std::ostream& os) {
   cb.genera(p, os);
}

std::string codegen(const codegen_base& cb, const arbol_sintactico& arbol, const tabla_simbolos& tabla) {
   std::ostringstream os;
   cb.genera(arbol, tabla, os);
   return std::move(os).str( );
}

#endif
