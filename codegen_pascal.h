#ifndef CODEGEN_PASCAL_H
#define CODEGEN_PASCAL_H

#include "codegen.h"

struct codegen_pascal : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 1;

   codegen_pascal()
   : codegen_base(4){
      return;
   }

   void genera(const expresion_termino* ex, std::ostream& os, const configuracion_id& config) const {
      os << traduce(ex->token, config);
   }

   void genera(const expresion_binaria* ex, std::ostream& os, const configuracion_id& config) const {
      os << "( ";
      genera(ex->izq, os, config);
      os << " " << config.palabras.find(ex->op.tipo)->second << " ";
      genera(ex->der, os, config);
      os << " )";
   }

   void genera(const expresion_prefija* ex, std::ostream& os, const configuracion_id& config) const {
      os << config.palabras.find(ex->op.tipo)->second << "( ";
      genera(ex->ex, os, config);
      os << " )";
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os, const configuracion_id& config) const {
      os << config.palabras.find(ex->funcion.tipo)->second << "(";
      genera(ex->parametro, os, config);
      os << ")";
   }

   void genera(const sentencia_comando* s, std::ostream& os, const configuracion_id& config) const {
      auto itr = config.palabras.find(s->comando.tipo);
      os << printws(nivel_ind * tab) << itr->second << ";\n";
   }

   void genera(const sentencia_if* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "si ";
      genera(s->condicion, os, config);
      os << " entonces inicio\n";
      genera(s->parte_si, os, config);
      os << printws(--nivel_ind * tab) << "fin";
      if(!s->parte_no.empty()){
         os << " sino inicio\n";
         ++nivel_ind;
         genera(s->parte_no, os, config);
         os << printws(--nivel_ind * tab) << "fin";
      }
      os << ";\n";
   }

   void genera(const sentencia_while* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "mientras ";
      genera(s->condicion, os, config);
      os << " hacer inicio\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "fin;\n";
   }

   void genera(const sentencia_iterate* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind++ * tab) << "repetir ";
      genera(s->condicion, os, config);
      os << " veces inicio\n";
      genera(s->cuerpo, os, config);
      os << printws(--nivel_ind * tab) << "fin;\n";
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os, const configuracion_id& config) const {
      os << printws(nivel_ind * tab) << traduce(s->funcion, config);
      if(s->parametro != nullptr){
         os << "(";
         genera(s->parametro, os, config);
         os << ")";
      }
      os << ";\n";
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
      std::vector<const declaracion_funcion*> prototipar;
      std::set<const declaracion_funcion*> definidas;
      for (const auto& funcion : arbol.funciones) {
         auto iter = tabla.grafica_dependencias.find(&funcion);
         if (iter != tabla.grafica_dependencias.end( )) {
            definidas.insert(&funcion);
            for (auto llamada : iter->second) {
               if (!definidas.contains(llamada)) {
                  prototipar.push_back(llamada);
               }
            }
         }
      }
      std::sort(prototipar.begin( ), prototipar.end( ));
      prototipar.erase(std::unique(prototipar.begin( ), prototipar.end( )), prototipar.end( ));

      os << "iniciar-programa\n";
      for (auto decl : prototipar) {
         os << printws(nivel_ind * tab) << "define-prototipo-instruccion " << traduce(decl->nombre, config) << (decl->parametro ? "(" + traduce(*(decl->parametro), config) + ")": "") << ";\n";
      }

      os << "\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "define-nueva-instruccion " << traduce(funcion.nombre, config) << (funcion.parametro ? "(" + traduce(*(funcion.parametro), config) + ")": "") << " como inicio\n";
            genera(*funcion.cuerpo, os, config);
            os << printws(--nivel_ind * tab) << "fin;\n\n";
         }
      }

      os << printws(nivel_ind++ * tab) << "inicia-ejecucion\n";
      genera(arbol.mains[0], os, config);
      os << printws(--nivel_ind * tab) << "termina-ejecucion\n";
      os << "finalizar-programa\n";
   }

   void ajusta_id(std::string& s, bool primera) const {
      if (!primera) {
         s += '_';
      }
   }
};

#endif
