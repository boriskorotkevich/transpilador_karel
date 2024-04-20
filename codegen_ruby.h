#ifndef CODEGEN_RUBY_H
#define CODEGEN_RUBY_H

#include "codegen.h"

struct codegen_ruby : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 0;

   codegen_ruby()
   : codegen_base(3){
      return;
   }

   void genera(const expresion_termino* ex, std::ostream& os, const configuracion_id& config) const {
      os << traduce(ex->token, config);
   }

   void genera(const expresion_binaria* ex, std::ostream& os, const configuracion_id& config) const {
      os << "(";
      genera(ex->izq, os, config);
      os << " " << config.palabras.find(ex->op.tipo)->second << " ";
      genera(ex->der, os, config);
      os << ")";
   }

   void genera(const expresion_prefija* ex, std::ostream& os, const configuracion_id& config) const {
      os << config.palabras.find(ex->op.tipo)->second << "(";
      genera(ex->ex, os, config);
      os << ")";
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os, const configuracion_id& config) const {
      os << config.palabras.find(ex->funcion.tipo)->second << "(";
      genera(ex->parametro, os, config);
      os << ")";
   }

   void genera(const sentencia_comando* s, std::ostream& os, const configuracion_id& config) const {
      auto itr = config.palabras.find(s->comando.tipo);
      os << printws(nivel_ind * tab) << config.palabras.find(s->comando.tipo)->second << "\n";
   }

   void genera(const sentencia_if* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "si ";
      genera(s->condicion, os, config);
      os << "\n";
      genera(s->parte_si, os, config);
      if(!s->parte_no.empty()){
         os << printws((nivel_ind - 1) * tab) << "sino\n";
         genera(s->parte_no, os, config);
      }
      os << printws(--nivel_ind * tab) << "fin\n";
   }

   void genera(const sentencia_while* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "mientras ";
      genera(s->condicion, os, config);
      os << "\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "fin\n";
   }

   void genera(const sentencia_iterate* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab);
      genera(s->condicion, os, config);
      os << " veces\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "fin\n";
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << traduce(s->funcion, config) << "(";
      if(s->parametro != nullptr){
         genera(s->parametro, os, config);
      }else{
         os << "0";
      }
      os << ")\n";
   }

   void genera(const sentencia_bloque* s, std::ostream& os, const configuracion_id& config) const {
      if(!s->cuerpo.empty()){
         genera(s->cuerpo, os, config);
      }
   }

   void genera(const sentencia_vacia* s, std::ostream& os, const configuracion_id& config) const {
      return;
   }

   void genera(const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os, const configuracion_id& config) const {
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "def " << traduce(funcion.nombre, config) << "(" << (funcion.parametro ? traduce(*(funcion.parametro), config) : "n") << ")\n";
            genera(*funcion.cuerpo, os, config);
            os << printws(--nivel_ind * tab) << "fin\n\n";
         }
      }
      genera(arbol.mains[0], os, config);
   }

   void ajusta_id(std::string& s, bool primera) const {
      if (primera) {
         std::replace(s.begin( ), s.end( ), '_', '-');
         if (s[0] == '-') {
            s.erase(s.begin( ));
         }
      } else {
         s += '-';
      }
   }
};

#endif
