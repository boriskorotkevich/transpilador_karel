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
   while (p->tipo != FIN && p->tipo != FIN_EJE) {
      res.push_back(stmt(p));
   }
   return std::move(res);
}

std::vector<std::unique_ptr<sentencia>> one_stmt(const token_registrado*& p){
   std::vector<std::unique_ptr<sentencia>> res;
   res.push_back(stmt(p));
   return std::move(res);
}

void espera_fin_stmt(const token_registrado*& p, const std::vector<tipo_token>& v){
   int cont = 0;
   for(const auto& token_no_esperado : v){
      cont += (p->tipo != token_no_esperado);
   }

   if(cont == v.size()){
      espera(p, PUNTO_COMA);
   }
}

std::unique_ptr<sentencia> stmt(const token_registrado*& p) {
   control_vista cv(p);

   auto cuerpo_stmt = [&](const token_registrado*& p, const std::vector<tipo_token>& v)
      -> std::vector<std::unique_ptr<sentencia>> {
         std::vector<std::unique_ptr<sentencia>> cuerpo;
         if(p->tipo == INICIO){
            cuerpo = lista_stmt(++p);
            espera(p, FIN);
            espera_fin_stmt(p, v);
         }else{
            cuerpo = one_stmt(p);
         }
      return std::move(cuerpo);
      };


   if (es_comando(p->tipo)) {
      auto comando = p++;
      espera_fin_stmt(p, {SINO, FIN, FIN_EJE});
      return std::make_unique<sentencia_comando>(cv, *comando);
   } else if (p->tipo == SI) {
      auto ex = expr(++p);
      espera(p, ENTONCES);
      auto parte_si = cuerpo_stmt(p, {FIN, SINO, FIN_EJE});
      auto parte_no = decltype(parte_si)( );
      if (p->tipo == SINO) {
         parte_no = cuerpo_stmt(++p, {FIN, FIN_EJE});
      }
      return std::make_unique<sentencia_if>(cv, std::move(ex), std::move(parte_si), std::move(parte_no));
   } else if (p->tipo == MIENTRAS) {
      auto ex = expr(++p);
      espera(p, HACER);
      auto cuerpo = cuerpo_stmt(p, {FIN, FIN_EJE});
      return std::make_unique<sentencia_while>(cv, std::move(ex), std::move(cuerpo));
   } else if(p->tipo == REPETIR && ((p + 1)->tipo == LITERAL_ENTERA || (p + 1)->tipo == IDENTIFICADOR)){
      auto ex = expr(++p);
      espera(p, VECES);
      auto cuerpo = cuerpo_stmt(p, {FIN, FIN_EJE});
      return std::make_unique<sentencia_iterate>(cv, std::move(ex), std::move(cuerpo));
   } else if (p->tipo == SAL_INS) {
      espera(p, SAL_INS);
      espera_fin_stmt(p, {FIN, SINO, FIN_EJE});
      return std::make_unique<sentencia_return>(cv);
   } else if (p->tipo == IDENTIFICADOR) {
      auto nombre = espera(p, IDENTIFICADOR);
      auto parametro = (p->tipo == PARENTESIS_IZQ ? expr(p) : nullptr);
      espera_fin_stmt(p, {FIN, SINO, FIN_EJE});
      return std::make_unique<sentencia_llamada_usuario>(cv, *nombre, std::move(parametro));
   } else {
      throw error("Sentencia esperada", p->vista);
   }
}

#endif
