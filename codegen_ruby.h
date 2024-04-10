#ifndef CODEGEN_RUBY_H
#define CODEGEN_RUBY_H

#include "codegen.h"

struct codegen_ruby : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 0;

   codegen_ruby()
   : codegen_base(
      3,
   {
      {APAGATE, "apagate"},
      {SAL_INS, "sal-de-instruccion"},
      {RETURN, "sal-de-instruccion"},
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

   void genera(const std::string& org, const expresion_termino* ex, std::ostream& os) const {
      os << mejora_id(org, ex->token, palabras);
   }

   void genera(const std::string& org, const expresion_binaria* ex, std::ostream& os) const {
      os << "( ";
      genera(org, ex->izq, os);
      os << " " << palabras.find(ex->op.tipo)->second << " ";
      genera(org, ex->der, os);
      os << " )";
   }

   void genera(const std::string& org, const expresion_prefija* ex, std::ostream& os) const {
      os << palabras.find(ex->op.tipo)->second << "( ";
      genera(org, ex->ex, os);
      os << " )";
   }

   void genera(const std::string& org, const expresion_llamada_nativa* ex, std::ostream& os) const {
      os << palabras.find(ex->funcion.tipo)->second << "(";
      genera(org, ex->parametro, os);
      os << ")";
   }

   void genera(const std::string& org, const sentencia_comando* s, std::ostream& os) const {
      auto itr = palabras.find(s->comando.tipo);
      os << printws(nivel_ind * tab) << palabras.find(s->comando.tipo)->second << "\n";
   }

   void genera(const std::string& org, const sentencia_if* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "si ";
      genera(org, s->condicion, os); 
      os << "\n";
      genera(org, s->parte_si, os);
      if(!s->parte_no.empty()){
         os << printws(--nivel_ind * tab) << "sino\n";
         ++nivel_ind;
         genera(org, s->parte_no, os);
      }
      os << printws(--nivel_ind * tab) << "fin\n";
   }

   void genera(const std::string& org, const sentencia_while* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "mientras ";
      genera(org, s->condicion, os);
      os << "\n";
      genera(org, s->cuerpo, os);
      os << printws(--nivel_ind * tab) << "fin\n";
   }

   void genera(const std::string& org, const sentencia_iterate* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab);
      genera(org, s->condicion, os);
      os << " veces\n";
      genera(org, s->cuerpo, os);
      os << printws(--nivel_ind * tab) << "fin\n";
   }

   void genera(const std::string& org, const sentencia_llamada_usuario* s, std::ostream& os) const {
      os << printws(nivel_ind * tab) << mejora_id(org, s->funcion, palabras) << "(";
      if(s->parametro != nullptr){
         genera(org, s->parametro, os);
      }else{
         os << "0";
      }
      os << ")\n";
   }

   void genera(const std::string& org, const sentencia_bloque* s, std::ostream& os) const {
      return;
   }

   void genera(const std::string& org, const sentencia_vacia* s, std::ostream& os) const {
      return;
   }

   void genera(const std::string& org, const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os) const {
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "def " << mejora_id(org, funcion.nombre, palabras) << "(" << (funcion.parametro ? mejora_id(org, *(funcion.parametro), palabras) : "N") << ")\n";
            for (const auto& sentencia : *funcion.cuerpo) {
               genera(org, sentencia, os);
            }
            os << printws(--nivel_ind * tab) << "fin\n\n";
         }
      }

      for (const auto& sentencia : arbol.mains[0]){
         genera(org, sentencia, os);
      }
   }
};

#endif
