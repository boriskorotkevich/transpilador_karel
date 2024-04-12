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

   void genera(const expresion_termino* ex, std::ostream& os, const std::string_view& origen) const {
      os << mejora_id(ex->token, palabras, origen);
   }

   void genera(const expresion_binaria* ex, std::ostream& os, const std::string_view& origen) const {
      os << "( ";
      genera(ex->izq, os, origen);
      auto itr = simbolos.find(ex->op.tipo);
      os << " " << itr->second << " ";
      genera(ex->der, os, origen);
      os << " )";
   }

   void genera(const expresion_prefija* ex, std::ostream& os, const std::string_view& origen) const {
      auto itr = simbolos.find(ex->op.tipo);
      os << itr->second << "( ";
      genera(ex->ex, os, origen);
      os << " )";
   }

   void genera(const expresion_llamada_nativa* ex, std::ostream& os, const std::string_view& origen) const {
      auto itr = palabras.find(ex->funcion.tipo);
      os << itr->second << "(";
      genera(ex->parametro, os, origen);
      os << ")";
   }

   void genera(const sentencia_comando* s, std::ostream& os, const std::string_view& origen) const {
      auto itr = palabras.find(s->comando.tipo);
      os << printws(nivel_ind * tab) << itr->second << "();\n";
   }

   void genera(const sentencia_if* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind++ * tab) << "if(";
      genera(s->condicion, os, origen);
      os << "){\n";
      genera(s->parte_si, os, origen);
      os << printws(--nivel_ind * tab) << "}";
      if(!s->parte_no.empty()){
         os << "else{\n";
         ++nivel_ind;
         genera(s->parte_no, os, origen);
         os << printws(--nivel_ind * tab) << "}";
      }
      os << "\n";
   }

   void genera(const sentencia_while* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind++ * tab) << "while(";
      genera(s->condicion, os, origen);
      os << "){\n";
      genera(s->cuerpo, os, origen);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const sentencia_iterate* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind++ * tab) << "iterate(";
      genera(s->condicion, os, origen);
      os << "){\n";
      genera(s->cuerpo, os, origen);
      os << printws(--nivel_ind * tab) << "}\n";
   }

   void genera(const sentencia_llamada_usuario* s, std::ostream& os, const std::string_view& origen) const {
      os << printws(nivel_ind * tab) << mejora_id(s->funcion, palabras, origen) << "(";
      if(s->parametro != nullptr){
         genera(s->parametro, os, origen);
      }
      os << ");\n";
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
      os << "class program {\n";
      for (const auto& funcion : arbol.funciones) {
         if(funcion.cuerpo != nullptr){
            os << printws(nivel_ind++ * tab) << "void " << mejora_id(funcion.nombre, palabras, origen) << "(" << (funcion.parametro ? mejora_id(*(funcion.parametro), palabras, origen) : "") << "){\n";
            genera(*funcion.cuerpo, os, origen);
            os << printws(--nivel_ind * tab) << "}\n\n";
         }
      }

      os << printws(nivel_ind++ * tab) << "program(){\n";
      genera(arbol.mains[0], os, origen);
      os << printws(--nivel_ind * tab) << "}\n";
      os << "}\n";
   }
};

#endif
