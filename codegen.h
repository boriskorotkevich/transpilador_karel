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
      codegen_base(int t, std::map<tipo_token, std::string_view>&& d, std::map<tipo_token, std::string_view>&& o) 
      : tab(t), palabras(std::move(d)), simbolos(std::move(o)){
      }

      codegen_base(int t, std::map<tipo_token, std::string_view>&& d)
      : tab(t), palabras(std::move(d)){
      }

      void genera(const std::string& org, const std::vector<std::unique_ptr<sentencia>>& vs, std::ostream& os) const {
         for(const auto& sentencia : vs){
            genera(org, sentencia, os);
         }
      }

      void genera(const std::string& org, const std::unique_ptr<expresion>& ex, auto&... params) const {
         ::genera(org, ex, *this, params...);
      }

      void genera(const std::string& org, const std::unique_ptr<sentencia>& s, auto&... params) const {
         ::genera(org, s, *this, params...);
      }

      virtual void genera(const std::string&, const expresion_termino*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const expresion_binaria*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const expresion_prefija*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const expresion_llamada_nativa*, std::ostream&) const = 0;

      virtual void genera(const std::string&, const sentencia_comando*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const sentencia_if*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const sentencia_while*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const sentencia_iterate*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const sentencia_llamada_usuario*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const sentencia_bloque*, std::ostream&) const = 0;
      virtual void genera(const std::string&, const sentencia_vacia*, std::ostream&) const = 0;

      virtual void genera(const std::string&, const arbol_sintactico&, const tabla_simbolos&, std::ostream&) const = 0;
};

template<typename T>
void genera(const std::string& org, const T* p, const codegen_base& cb, std::ostream& os) {
   cb.genera(org, p, os);
}

std::string codegen(const std::string& org, const codegen_base& cb, const arbol_sintactico& arbol, const tabla_simbolos& tabla) {
   std::ostringstream os;
   cb.genera(org, arbol, tabla, os);
   return std::move(os).str( );
}

#endif
