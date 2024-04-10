#ifndef CODEGEN_PASCAL_H
#define CODEGEN_PASCAL_H

#include "codegen.h"

struct codegen_pascal : codegen_base {
   using codegen_base::genera;

   mutable int nivel_ind = 1;

   codegen_pascal()
   : codegen_base(
      4, 
   {
      {SAL_INS, "sal-de-instruccion"},
      {RETURN, "sal-de-instruccion"},
      {COMO, "como"},
      {APAGATE, "apagate"},
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
      {AND, "y"},
   }) {
      return;
   }

   void genera(const std::string& org, const expresion_termino* ex, std::ostream& os) const {
      auto itr = palabras.find(ex->token.tipo);
      os << mejora_id(org, ex->token, palabras);
   }

   void genera(const std::string& org, const expresion_binaria* ex, std::ostream& os) const {
      os << "( ";
      genera(org, ex->izq, os);
      auto itr = palabras.find(ex->op.tipo);
      os << " " << itr->second << " ";
      genera(org, ex->der, os);
      os << " )";
   }

   void genera(const std::string& org, const expresion_prefija* ex, std::ostream& os) const {
      auto itr = palabras.find(ex->op.tipo);
      os << itr->second << "( ";
      genera(org, ex->ex, os);
      os << " )";
   }

   void genera(const std::string& org, const expresion_llamada_nativa* ex, std::ostream& os) const {
      auto itr = palabras.find(ex->funcion.tipo);
      os << itr->second << "(";
      genera(org, ex->parametro, os);
      os << ")";
   }

   void genera(const std::string& org, const sentencia_comando* s, std::ostream& os) const {
      auto itr = palabras.find(s->comando.tipo);
      os << printws(nivel_ind * tab) << itr->second << ";\n";
   }

   void genera(const std::string& org, const sentencia_if* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "si ";
      genera(org, s->condicion, os); 
      os << " entonces inicio\n";
      genera(org, s->parte_si, os);
      os << printws(--nivel_ind * tab) << "fin";
      if(!s->parte_no.empty()){
         os << " sino inicio\n";
         ++nivel_ind;
         genera(org, s->parte_no, os);
         os << printws(--nivel_ind * tab) << "fin;\n";
         return;
      }
      os << ";\n";
   }

   void genera(const std::string& org, const sentencia_while* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "mientras ";
      genera(org, s->condicion, os);
      os << " hacer inicio\n";
      genera(org, s->cuerpo, os);
      os << printws(--nivel_ind * tab) << "fin;\n";
   }

   void genera(const std::string& org, const sentencia_iterate* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "repetir ";
      genera(org, s->condicion, os);
      os << " veces inicio\n";
      genera(org, s->cuerpo, os);
      os << printws(--nivel_ind * tab) << "fin;\n";
   }

   void genera(const std::string& org, const sentencia_llamada_usuario* s, std::ostream& os) const {
      os << printws(nivel_ind * tab) << mejora_id(org, s->funcion, palabras); 
      if(s->parametro != nullptr){
         os << "(";
         genera(org, s->parametro, os);
         os << ")";
      }
      os << ";\n";
   }

   void genera(const std::string& org, const sentencia_bloque* s, std::ostream& os) const {
      if(!s->cuerpo.empty()){
         genera(org, s->cuerpo, os);
      }
   }

   void genera(const std::string& org, const sentencia_vacia* s, std::ostream& os) const {
      os << printws(nivel_ind * tab) << ";\n";
   }

   void genera(const std::string& org, const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os) const {
      os << "iniciar-programa\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "define-nueva-instruccion " << mejora_id(org, funcion.nombre, palabras) << (funcion.parametro ? "(" + mejora_id(org, *(funcion.parametro), palabras) + ")": "") << " como inicio\n";
            for (const auto& sentencia : *funcion.cuerpo) {
               genera(org, sentencia, os);
            }
            os << printws(--nivel_ind * tab) << "fin;\n\n";
         }
      }

      os << printws(nivel_ind++ * tab) << "inicia-ejecucion\n";
      for (const auto& sentencia : arbol.mains[0]){
         genera(org, sentencia, os);
      }
      os << printws(--nivel_ind * tab) << "termina-ejecucion\n";
      os << "finalizar-programa\n";
   }
};

#endif
