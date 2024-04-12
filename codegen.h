#ifndef CODEGEN_H
#define CODEGEN_H

#include "codegen_aux.h"
#include "lexer.h"
#include "parser.h"
#include "semantico.h"

#include <map>
#include <sstream>

struct codegen_base {
   protected:
      int tab;
      std::map<tipo_token, std::string_view> palabras, simbolos;

   public:
      codegen_base(int t, std::map<tipo_token, std::string_view>&& p, std::map<tipo_token, std::string_view>&& s)
      : tab(t), palabras(std::move(p)), simbolos(std::move(s)){
      }

      codegen_base(int t, std::map<tipo_token, std::string_view>&& p)
      : tab(t), palabras(std::move(p)){
      }

      void genera(const std::vector<std::unique_ptr<sentencia>>& vs, std::ostream& os, const std::string_view& origen) const {
         for(const auto& sentencia : vs){
            genera(sentencia, os, origen);
         }
      }

      void genera(const std::unique_ptr<expresion>& ex, auto&... params) const {
         ::genera(ex, *this, params...);
      }

      void genera(const std::unique_ptr<sentencia>& s, auto&... params) const {
         ::genera(s, *this, params...);
      }

      virtual void genera(const expresion_termino*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const expresion_binaria*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const expresion_prefija*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const expresion_llamada_nativa*, std::ostream&, const std::string_view&) const = 0;

      virtual void genera(const sentencia_comando*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const sentencia_if*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const sentencia_while*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const sentencia_iterate*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const sentencia_llamada_usuario*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const sentencia_bloque*, std::ostream&, const std::string_view&) const = 0;
      virtual void genera(const sentencia_vacia*, std::ostream&, const std::string_view&) const = 0;

      virtual void genera(const arbol_sintactico&, const tabla_simbolos&, std::ostream&, const std::string_view&) const = 0;
};

template<typename T>
void genera(const T* p, const codegen_base& cb, std::ostream& os, const std::string_view& origen) {
   cb.genera(p, os, origen);
}

std::string codegen(const codegen_base& cb, const arbol_sintactico& arbol, const tabla_simbolos& tabla, const std::string_view& origen) {
   std::ostringstream os;
   cb.genera(arbol, tabla, os, origen);
   return std::move(os).str( );
}

#endif
