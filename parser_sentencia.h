#ifndef PARSER_SENTENCIA_H
#define PARSER_SENTENCIA_H

#include "parser_aux.h"
#include "parser_expresion.h"
#include <memory>
#include <string>
#include <vector>

struct sentencia {
   std::string_view vista;

   sentencia(const control_vista& cv)
   : vista(cv) {
   }

   virtual ~sentencia( ) = 0;
};

struct sentencia_expresion : sentencia {
   std::unique_ptr<expresion> ex;
   sentencia_expresion(const control_vista& cv, std::unique_ptr<expresion> e)
   : sentencia(cv), ex(std::move(e)) {
   }
};

struct sentencia_if : sentencia {
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> parte_si;
   std::vector<std::unique_ptr<sentencia>> parte_no;

   sentencia_if(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> s, std::vector<std::unique_ptr<sentencia>> n)
   : sentencia(cv), condicion(std::move(c)), parte_si(std::move(s)), parte_no(std::move(n)) {
   }
};

struct sentencia_while : sentencia {
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> cuerpo;

   sentencia_while(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> v)
   : sentencia(cv), condicion(std::move(c)), cuerpo(std::move(v)) {
   }
};

// falta el iterate

struct sentencia_iterate : sentencia{ // sentencia iterate
   std::unique_ptr<expresion> condicion;
   std::vector<tsd::unique_ptr<sentencia>> cuerpo;

   sentencia_while(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> v)
   : sentencia(cv), condicion(std::move(c)), cuerpo(std::move(v)) {
   }
}

struct sentencia_return : sentencia {
   sentencia_exit(const control_vista& cv)
   : sentencia(cv) {
   }
};

std::unique_ptr<sentencia> stmt(const token_registrado*&);

std::vector<std::unique_ptr<sentencia>> lista_stmt(const token_registrado*& p) {
   std::vector<std::unique_ptr<sentencia>> res;
   while (p->tipo != LLAVE_DER) {
      res.push_back(stmt(p));
   }
   return res;
}

std::unique_ptr<sentencia> stmt(const token_registrado*& p) {
   control_vista cv(p);
   if (p->tipo == IF) {
      auto ex = expr(++p);
      espera(p, LLAVE_IZQ);
      auto parte_si = lista_stmt(p), parte_no = decltype(parte_si)( );
      espera(p, LLAVE_DER);
      if (p->tipo == ELSE) {
         espera(p, LLAVE_IZQ);
         parte_no = lista_stmt(p);
         espera(p, LLAVE_DER);
      }
      return std::make_unique<sentencia_if>(cv, std::move(ex), std::move(parte_si), std::move(parte_no));
   } else if(p->tipo == ITERATE){
      // ...
   } else if (p->tipo == WHILE) {
      auto ex = expr(++p);
      espera(p, LLAVE_IZQ);
      auto cuerpo = lista_stmt(p);
      espera(p, LLAVE_DER);
      return std::make_unique<sentencia_while>(cv, std::move(ex), std::move(cuerpo));
   } else if (p->tipo == RETURN) {
      espera(++p, PUNTO_COMA);
      return std::make_unique<sentencia_return>(cv);
   } else {
      auto ex = expr(p);
      if (p->tipo == IDENTIFICADOR) {
         auto sen = stmt_declaracion(p, cv, std::move(ex), true);
         espera(p, PUNTO_COMA);
         return sen;
      } else {
         espera(p, PUNTO_COMA);
         return std::make_unique<sentencia_expresion>(cv, std::move(ex));
      }
   }
}

#endif
