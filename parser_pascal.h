#ifndef PARSER_PASCAL_H
#define PARSER_PASCAL_H

#include "lexer.h"
#include "parser.h"
#include "parser_aux.h"
#include "parser_expresion.h"

class parser_pascal : public parser_base {
private:
   // funciones para las sentencias

   std::vector<std::unique_ptr<sentencia>> lista_stmt(const token_registrado*& p) const {
      std::vector<std::unique_ptr<sentencia>> res;
      while (p->tipo != FIN && p->tipo != FIN_EJE) {
         res.push_back(stmt(p));
      }
      return std::move(res);
   }

   std::vector<std::unique_ptr<sentencia>> one_stmt(const token_registrado*& p) const {
      std::vector<std::unique_ptr<sentencia>> res;
      res.push_back(stmt(p));
      return std::move(res);
   }

   void espera_fin_stmt(const token_registrado*& p, const std::vector<tipo_token>& v) const {
      int cont = 0;
      for(const auto& token_no_esperado : v){
         cont += (p->tipo != token_no_esperado);
      }

      if(cont == v.size()){
         espera(p, PUNTO_COMA);
      }
   }

   std::unique_ptr<sentencia> stmt(const token_registrado*& p) const {
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
         espera_fin_stmt(p, {FIN, SINO, FIN_EJE});
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
         auto cuerpo = cuerpo_stmt(p, {FIN, SINO, FIN_EJE});
         return std::make_unique<sentencia_while>(cv, std::move(ex), std::move(cuerpo));
      } else if(p->tipo == REPETIR){
         auto ex = expr(++p);
         espera(p, VECES);
         auto cuerpo = cuerpo_stmt(p, {FIN, SINO, FIN_EJE});
         return std::make_unique<sentencia_iterate>(cv, std::move(ex), std::move(cuerpo));
      } else if (p->tipo == IDENTIFICADOR) {
         auto nombre = espera(p, IDENTIFICADOR);
         auto parametro = (p->tipo == PARENTESIS_IZQ ? expr(p) : nullptr);
         espera_fin_stmt(p, {FIN, SINO, FIN_EJE});
         return std::make_unique<sentencia_llamada_usuario>(cv, *nombre, std::move(parametro));
      }  else if(p->tipo == INICIO){
         auto cuerpo = cuerpo_stmt(p, {FIN, FIN_EJE});
         return std::make_unique<sentencia_bloque>(cv, std::move(cuerpo));
      } else if(p->tipo == PUNTO_COMA){
         ++p;
         return std::make_unique<sentencia_vacia>(cv);
      } else {
         throw error("Sentencia esperada", p->vista);
      }
   }

   // funciones para el árbol sintáctico

   declaracion_funcion parser_funcion(const token_registrado*& p) const {
      espera(p, es_decl_funcion_pascal);
      auto nombre = espera(p, IDENTIFICADOR);
      auto parametro = (const token_registrado*)nullptr;
      if (p->tipo == PARENTESIS_IZQ) {
         parametro = espera(++p, IDENTIFICADOR);
         espera(p, PARENTESIS_DER);
      }
      espera_seq(p, {COMO, INICIO});
      auto cuerpo = lista_stmt(p);
      espera_seq(p, {FIN, PUNTO_COMA});
      return declaracion_funcion{*nombre, std::move(parametro), std::make_unique<std::vector<std::unique_ptr<sentencia>>>(std::move(cuerpo)) };
   }

   declaracion_funcion parser_prot(const token_registrado*& p) const {
      espera(p, DEFP_INS);
      auto nombre = espera(p, IDENTIFICADOR);
      auto parametro = (const token_registrado*)nullptr;
      if (p->tipo == PARENTESIS_IZQ) {
         parametro = espera(++p, IDENTIFICADOR);
         espera(p, PARENTESIS_DER);
      }
      espera(p, PUNTO_COMA);
      return declaracion_funcion{*nombre, parametro , nullptr};
   }

   std::vector<std::unique_ptr<sentencia>> parser_main(const token_registrado*& p) const {
      espera(p, INICIO_EJE);
      auto cuerpo = lista_stmt(p);
      espera(p, FIN_EJE);
      return std::move(cuerpo);
   }

public:
   void parser_arbol(const token_registrado*& p, arbol_sintactico& arbol) const {
      espera(p, INICIO_PROG);
      while (p->tipo != FIN_PROG) {
         if(p->tipo == DEFN_INS){
            arbol.funciones.push_back(parser_funcion(p));
         }else if(p->tipo == DEFP_INS){
            arbol.funciones.push_back(parser_prot(p));
         } else {
            arbol.mains.push_back(parser_main(p));
         }
      }
      ++p;
   }
};

#endif
