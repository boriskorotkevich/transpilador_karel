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

   virtual ~sentencia() = default;
};

struct sentencia_comando : sentencia {
   const token_registrado& comando;
   sentencia_comando(const control_vista& cv, const token_registrado& t)
   : sentencia(cv), comando(t) {
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

struct sentencia_iterate : sentencia{
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> cuerpo;

   sentencia_iterate(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> v)
   : sentencia(cv), condicion(std::move(c)), cuerpo(std::move(v)) {
   }
};

struct sentencia_llamada_usuario : sentencia {
   const token_registrado& funcion;
   std::unique_ptr<expresion> parametro;

   sentencia_llamada_usuario(const control_vista& cv, const token_registrado& f, std::unique_ptr<expresion> p)
   : sentencia(cv), funcion(f), parametro(std::move(p)) {
   }
};

struct sentencia_return : sentencia {
   sentencia_return(const control_vista& cv)
   : sentencia(cv) {
   }
};

std::unique_ptr<sentencia> stmt(const token_registrado*&);

std::vector<std::unique_ptr<sentencia>> lista_stmt(const token_registrado*& p) {
   std::vector<std::unique_ptr<sentencia>> res;
   while (p->tipo != LLAVE_DER) {
      res.push_back(stmt(p));
   }
   return std::move(res);
}

std::vector<std::unique_ptr<sentencia>> one_stmt(const token_registrado*& p){
   std::vector<std::unique_ptr<sentencia>> res;
   res.push_back(stmt(p));
   return std::move(res);
}

std::unique_ptr<sentencia> stmt(const token_registrado*& p) {
   control_vista cv(p);

   auto cuerpo_stmt = [&](const token_registrado*& p) -> std::vector<std::unique_ptr<sentencia>>{
      std::vector<std::unique_ptr<sentencia>> ans;
      if(p->tipo != LLAVE_IZQ){
         ans = one_stmt(p);
      }else{
         espera(p, LLAVE_IZQ);
         ans = lista_stmt(p);
         espera(p, LLAVE_DER);
      }

      return std::move(ans);
   };

   if (es_comando(p->tipo)) {
      auto comando = p;
      espera(++p, {PARENTESIS_IZQ, PARENTESIS_DER, PUNTO_COMA});
      return std::make_unique<sentencia_comando>(cv, *comando);
   } else if (p->tipo == IF) {
      auto ex = expr(++p);
      auto parte_si = cuerpo_stmt(p); 
      auto parte_no = decltype(parte_si)( );
      if (p->tipo == ELSE) {
         parte_no = cuerpo_stmt(++p);
      }
      return std::make_unique<sentencia_if>(cv, std::move(ex), std::move(parte_si), std::move(parte_no));
   } else if (p->tipo == WHILE) {
      auto ex = expr(++p);
      auto cuerpo = cuerpo_stmt(p);
      return std::make_unique<sentencia_while>(cv, std::move(ex), std::move(cuerpo));
   } else if(p->tipo == ITERATE){
      auto ex = expr(++p);
      auto cuerpo = cuerpo_stmt(p);
      return std::make_unique<sentencia_iterate>(cv, std::move(ex), std::move(cuerpo));
   } else if (p->tipo == RETURN) {
      espera(++p, PUNTO_COMA);
      return std::make_unique<sentencia_return>(cv);
   } else if (p->tipo == IDENTIFICADOR) {
      auto nombre = p;
      espera(++p, PARENTESIS_IZQ);
      std::unique_ptr<expresion> parametro = (p->tipo == IDENTIFICADOR || p->tipo == LITERAL_ENTERA ? std::make_unique<expresion_termino>(control_vista(p), *p++) : nullptr);
      espera(p, {PARENTESIS_DER, PUNTO_COMA});
      return std::make_unique<sentencia_llamada_usuario>(cv, *nombre, std::move(parametro));
   } else {
      throw error("Sentencia esperada", p->vista);
   }
}

#endif
