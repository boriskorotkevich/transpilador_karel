#ifndef CODEGEN_JAVA_H
#define CODEGEN_JAVA_H

#include "codegen.h"

struct codegen_java : codegen_base {
   using codegen_base::genera;
   mutable int nivel_ind = 1;

   codegen_java() 
   : codegen_base(
      3,
   {
      {RETURN, "return"},
      {SAL_INS, "return"},
      {APAGATE, "turnoff"},
      {GIRA_IZQ, "turnleft"},
      {AVANZA, "move"},
      {COGE_ZUM, "pickbeeper"},
      {DEJA_ZUM, "putbeeper"},
      {PRECEDE, "pred"},
      {SUCEDE, "succ"},
      {ES_CERO, "iszero"},
      {FRENTE_LIB, "frontIsClear"},
      {FRENTE_BLOQ, "frontIsBlocked"},
      {IZQUIERDA_LIB, "leftIsClear"},
      {IZQUIERDA_BLOQ, "leftIsBlocked"},
      {DERECHA_LIB, "rightIsClear"},
      {DERECHA_BLOQ, "rightIsBlocked"},
      {JUNTO_ZUM, "nextToABeeper"},
      {NJUNTO_ZUM, "notNextToABeeper"},
      {ALGUN_ZUM_BAG, "anyBeepersInBeeperBag"},
      {NINGUN_ZUM_BAG, "noBeepersInBeeperBag"},
      {ORIENTADO_NORTE, "facingNorth"},
      {ORIENTADO_SUR, "facingSouth"},
      {ORIENTADO_ESTE, "facingEast"},
      {ORIENTADO_OESTE, "facingWest"},
      {NORIENTADO_NORTE, "notFacingNorth"},
      {NORIENTADO_SUR, "notFacingSouth"},
      {NORIENTADO_ESTE, "notFacingEast"},
      {NORIENTADO_OESTE, "notFacingWest"}
   },

   /* simbolos */
   {
      {NOT, "!"},
      {OR, "||"},
      {AND, "&&"}
   }) {
      return;
   }

   void genera(const std::string& org, const expresion_termino* ex, std::ostream& os) const {
      os << mejora_id(org, ex->token, palabras);
   }

   void genera(const std::string& org, const expresion_binaria* ex, std::ostream& os) const {
      os << "( ";
      genera(org, ex->izq, os);
      auto itr = simbolos.find(ex->op.tipo);
      os << " " << itr->second << " ";
      genera(org, ex->der, os);
      os << " )";
   }

   void genera(const std::string& org, const expresion_prefija* ex, std::ostream& os) const {
      auto itr = simbolos.find(ex->op.tipo);
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
      os << printws(nivel_ind * tab) << itr->second << "();\n";
   }

   void genera(const std::string& org, const sentencia_if* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "if(";
      genera(org, s->condicion, os); 
      os << "){\n";
      genera(org, s->parte_si, os);
      os << printws(--nivel_ind * tab) << "}";
      if(!s->parte_no.empty()){
         os << "else{\n";
         ++nivel_ind;
         genera(org, s->parte_no, os);
         os << printws(--nivel_ind * tab) << "}\n";
         return;
      }
      os << "\n";
   }

   void genera(const std::string& org, const sentencia_while* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "while(";
      genera(org, s->condicion, os);
      os << "){\n";
      genera(org, s->cuerpo, os);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const std::string& org, const sentencia_iterate* s, std::ostream& os) const {
      os << printws(nivel_ind++ * tab) << "iterate(";
      genera(org, s->condicion, os);
      os << "){\n";
      genera(org, s->cuerpo, os);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const std::string& org, const sentencia_llamada_usuario* s, std::ostream& os) const {
      os << printws(nivel_ind * tab) << mejora_id(org, s->funcion, palabras) << "(";
      genera(org, s->parametro, os);
      os << ");\n";
   }

   void genera(const std::string& org, const sentencia_bloque* s, std::ostream& os) const {
      if(s->cuerpo.empty()){
         return;
      }
      genera(org, s->cuerpo, os);
   }

   void genera(const std::string& org, const sentencia_vacia* s, std::ostream& os) const {
      os << printws(nivel_ind * tab) << ";\n";
   }

   void genera(const std::string& org, const arbol_sintactico& arbol, const tabla_simbolos& tabla, std::ostream& os) const {
      os << "class program {\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "void " << mejora_id(org, funcion.nombre, palabras) << "(" << (funcion.parametro ? mejora_id(org, *(funcion.parametro), palabras) : "") << "){\n";
            for (const auto& sentencia : *funcion.cuerpo) {
               genera(org, sentencia, os);
            }
            os << printws(--nivel_ind * tab) << "}\n\n";
         }
      }

      os << printws(nivel_ind++ * tab) << "program(){\n";
      for (const auto& sentencia : arbol.mains[0]){
         genera(org, sentencia, os);
      }
      os << printws(--nivel_ind * tab) << "}\n";
      os << "}\n";
   }
};

#endif
