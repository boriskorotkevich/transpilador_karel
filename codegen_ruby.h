#ifndef CODEGEN_RUBY_H
#define CODEGEN_RUBY_H

#include "codegen.h"

struct codegen_ruby : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 1;

   codegen_ruby()
   : codegen_base(
      3,
   {
      {APAGATE, "apagate"},
      {SAL_INS, "sal-de-instruccion"},
      {RETURN, "return"},
      {GIRA_IZQ, "gira-izquierda"},
      {AVANZA, "avanza"},
      {COGE_ZUM, "coge-zumbador"},
      {DEJA_ZUM, "deja-zumbador"},
      {PRECEDE, "precede"},
      {SUCEDE, "sucede"},
      {ES_CERO, "si-es-cero"},
      {FRENTE_LIB, "frente-libre"},
      {FRENTE_BLOQ, "frente-bloqueado"},
      {IZQUIERDA_LIB, "izquierda-libre"},
      {IZQUIERDA_BLOQ, "izquierda-bloqueda"},
      {DERECHA_LIB, "derecha-libre"},
      {DERECHA_BLOQ, "derecha-bloqueada"},
      {JUNTO_ZUM, "junto-a-zumbador"},
      {NJUNTO_ZUM, "no-junto-a-zumbador"},
      {ALGUN_ZUM_BAG, "algun-zumbador-en-la-mochila"},
      {NINGUN_ZUM_BAG, "ningun-zumbador-en-la-mochila"},
      {ORIENTADO_NORTE, "orientado-al-norte"},
      {ORIENTADO_SUR, "orientado-al-sur"},
      {ORIENTADO_ESTE, "orientado-al-este"},
      {ORIENTADO_OESTE, "orientado-al-oeste"},
      {NORIENTADO_NORTE, "no-orientado-al-norte"},
      {NORIENTADO_SUR, "no-orientado-al-sur"},
      {NORIENTADO_ESTE, "no-orientado-al-este"},
      {NORIENTADO_OESTE, "no-orientado-al-oeste"},
      {NOT, "no"},
      {OR, "o"},
      {AND, "y"}
   }){
      return;
   }

   void genera(const expresion_termino* ex, std::ostream& os, const std::string_view& origen) const {
      os << mejora_id(ex->token, palabras, origen);
   }

   void genera(const expresion_binaria* ex, std::ostream& os, const std::string_view& origen) const {
      os << "( ";
      genera(ex->izq, os, origen);
      os << " " << palabras.find(ex->op.tipo)->second << " ";
      genera(ex->der, os, origen);
      os << " )";
   }

   void genera(const expresion_prefija* ex, std::ostream& os, const std::string_view& origen) const {
      os << palabras.find(ex->op.tipo)->second << "( ";
      genera(ex->ex, os, origen);
      os << " )";
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os, const std::string_view& origen) const {
      os << palabras.find(ex->funcion.tipo)->second << "(";
      genera(ex->parametro, os, origen);
      os << ")";
   }

   void genera(const sentencia_comando* s, std::ostream& os, const std::string_view& origen) const {
      auto itr = palabras.find(s->comando.tipo);
      os << printws(nivel_ind * tab) << palabras.find(s->comando.tipo)->second << "\n";
   }

   void genera(const sentencia_if* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind++ * tab) << "si ";
      genera(s->condicion, os, origen);
      os << "\n";
      genera(s->parte_si, os, origen);
      if(!s->parte_no.empty()){
         os << printws((nivel_ind - 1) * tab) << "sino\n";
         genera(s->parte_no, os, origen);
      }
      os << printws(--nivel_ind * tab) << "end\n";
   }

   void genera(const sentencia_while* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind++ * tab) << "mientras ";
      genera(s->condicion, os, origen);
      os << "\n";
      genera(s->cuerpo, os, origen);
      os << printws(--nivel_ind * tab) << "end\n";
   }

   void genera(const sentencia_iterate* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind++ * tab);
      genera(s->condicion, os, origen);
      os << " veces\n";
      genera(s->cuerpo, os, origen);
      os << printws(--nivel_ind * tab) << "end\n";
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind * tab) << mejora_id(s->funcion, palabras, origen) << "(";
      if(s->parametro != nullptr){
         genera(s->parametro, os, origen);
      }else{
         os << "0";
      }
      os << ")\n";
   }

   void genera(const sentencia_bloque* s, std::ostream& os, const std::string_view& origen) const {
      if(!s->cuerpo.empty()){
         genera(s->cuerpo, os, origen);
      }
   }

   void genera(const sentencia_vacia* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind * tab) << ";\n";
   }

   void genera(const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os, const std::string_view& origen) const {
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "def " << mejora_id(funcion.nombre, palabras, origen) << "(" << (funcion.parametro ? mejora_id(*(funcion.parametro), palabras, origen) : "n") << ")\n";
            genera(*funcion.cuerpo, os, origen);
            os << printws(--nivel_ind * tab) << "end\n\n";
         }
      }
      genera(*funcion.cuerpo, os, origen);
   }
};

#endif
