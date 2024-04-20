#ifndef CODEGEN_JAVA_H
#define CODEGEN_JAVA_H

#include "codegen.h"

struct codegen_java : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 1;

   codegen_java()
   : codegen_base(3) {
      return;
   }

   void genera(const expresion_termino* ex, std::ostream& os, const configuracion_id& config) const {
      os << traduce(ex->token, config);
   }

   void genera(const expresion_binaria* ex, std::ostream& os, const configuracion_id& config) const {
      os << "( ";
      genera(ex->izq, os, config);
      os << " " << config.simbolos.find(ex->op.tipo)->second << " ";
      genera(ex->der, os, config);
      os << " )";
   }

   void genera(const expresion_prefija* ex, std::ostream& os, const configuracion_id& config) const {
      os << config.simbolos.find(ex->op.tipo)->second << "( ";
      genera(ex->ex, os, config);
      os << " )";
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os, const configuracion_id& config) const {
      os << config.palabras.find(ex->funcion.tipo)->second << "(";
      genera(ex->parametro, os, config);
      os << ")";
   }

   void genera(const sentencia_comando* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << config.palabras.find(s->comando.tipo)->second << "();\n";
   }

   void genera(const sentencia_if* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "if(";
      genera(s->condicion, os, config);
      os << "){\n";
      genera(s->parte_si, os, config);
      os << printws(--nivel_ind * tab) << "}";
      if(!s->parte_no.empty()){
         os << "else{\n";
         ++nivel_ind;
         genera(s->parte_no, os, config);
         os << printws(--nivel_ind * tab) << "}";
      }
      os << "\n";
   }

   void genera(const sentencia_while* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "while(";
      genera(s->condicion, os, config);
      os << "){\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const sentencia_iterate* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "iterate(";
      genera(s->condicion, os, config);
      os << "){\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << traduce(s->funcion, config) << "(";
      if(s->parametro != nullptr){
         genera(s->parametro, os, config);
      }
      os << ");\n";
   }

   void genera(const sentencia_bloque* s, std::ostream& os, const configuracion_id& config) const {
      if(!s->cuerpo.empty()){
         genera(s->cuerpo, os, config);
      }
   }

   void genera(const sentencia_vacia* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << ";\n";
   }

   void genera(const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os, const configuracion_id& config) const {
      os << "class program {\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "void " << traduce(funcion.nombre, config) << "(" << (funcion.parametro ? traduce(*(funcion.parametro), config) : "") << "){\n";
            genera(*funcion.cuerpo, os, config);
            os << printws(--nivel_ind * tab) << "}\n\n";
         }
      }

      os << printws(nivel_ind++ * tab) << "program(){\n";
      genera(arbol.mains[0], os, config);
      os << printws(--nivel_ind * tab) << "}\n";
      os << "}\n";
   }

   void ajusta_id(std::string& s, bool primera) const {
      if (primera) {
         std::replace(s.begin( ), s.end( ), '-', '_');
         if (s[0] == '_') {
            s.erase(s.begin( ));
         }
      } else {
         s += '_';
      }
   }
};

#endif
