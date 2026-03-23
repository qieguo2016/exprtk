#ifndef EXPRTK_SRC_EXPRESSION_HPP
#define EXPRTK_SRC_EXPRESSION_HPP

#include "symbol_table.hpp"
#include "nodes_base.hpp"
#include "nodes_compound.hpp"
#include "nodes_assign.hpp"
#include "lexer.hpp"
#include "arena_allocator.hpp"

namespace exprtk
{

   template <typename T>
   class function_compositor;

   template <typename T>
   class expression
   {
   private:

      typedef details::expression_node<T>*  expression_ptr;
      typedef details::vector_holder<T>*    vector_holder_ptr;
      typedef std::vector<symbol_table<T> > symtab_list_t;

      struct control_block
      {
         enum data_type
         {
            e_unknown  ,
            e_expr     ,
            e_vecholder,
            e_data     ,
            e_vecdata  ,
            e_string
         };

         static std::string to_str(data_type dt)
         {
            switch (dt)
            {
               case e_unknown   : return "e_unknown  ";
               case e_expr      : return "e_expr"     ;
               case e_vecholder : return "e_vecholder";
               case e_data      : return "e_data"     ;
               case e_vecdata   : return "e_vecdata"  ;
               case e_string    : return "e_string"   ;
            }

            return "";
         }

         struct data_pack
         {
            data_pack()
            : pointer(0)
            , type(e_unknown)
            , size(0)
            {}

            data_pack(void* ptr, const data_type dt, const std::size_t sz = 0)
            : pointer(ptr)
            , type(dt)
            , size(sz)
            {}

            void*       pointer;
            data_type   type;
            std::size_t size;
         };

         typedef std::vector<data_pack> local_data_list_t;
         typedef results_context<T>     results_context_t;
         typedef control_block*         cntrl_blck_ptr_t;

         control_block()
         : ref_count(0)
         , expr     (0)
         , results  (0)
         , arena    (0)
         , retinv_null(false)
         , return_invoked(&retinv_null)
         {}

         explicit control_block(expression_ptr e)
         : ref_count(1)
         , expr     (e)
         , results  (0)
         , arena    (0)
         , retinv_null(false)
         , return_invoked(&retinv_null)
         {}

        ~control_block()
         {
            if (expr && details::branch_deletable(expr))
            {
               destroy_node(expr);
            }

            if (!local_data_list.empty())
            {
               for (std::size_t i = 0; i < local_data_list.size(); ++i)
               {
                  switch (local_data_list[i].type)
                  {
                     case e_expr      :
                        {
                           expression_ptr eptr = reinterpret_cast<expression_ptr>(local_data_list[i].pointer);
                           if (eptr)
                           {
                              eptr->destroy_self();
                           }
                        }
                        break;

                     case e_vecholder : delete reinterpret_cast<vector_holder_ptr>(local_data_list[i].pointer);
                                        break;

                     case e_data      : delete reinterpret_cast<T*>(local_data_list[i].pointer);
                                        break;

                     case e_vecdata   : delete [] reinterpret_cast<T*>(local_data_list[i].pointer);
                                        break;

                     case e_string    : delete reinterpret_cast<std::string*>(local_data_list[i].pointer);
                                        break;

                     default          : break;
                  }
               }
            }

            if (results)
            {
               delete results;
            }

            // Arena is destroyed last; node destructors have already been called
            // by destroy_node() above, so it is safe to bulk-free the pages now.
            delete arena;
         }

         static inline cntrl_blck_ptr_t create(expression_ptr e)
         {
            return new control_block(e);
         }

         static inline void destroy(cntrl_blck_ptr_t& cntrl_blck)
         {
            if (cntrl_blck)
            {
               if (
                    (0 !=   cntrl_blck->ref_count) &&
                    (0 == --cntrl_blck->ref_count)
                  )
               {
                  delete cntrl_blck;
               }

               cntrl_blck = 0;
            }
         }

         std::size_t ref_count;
         expression_ptr expr;
         local_data_list_t local_data_list;
         results_context_t* results;
         details::arena_allocator* arena;
         bool  retinv_null;
         bool* return_invoked;

         friend class function_compositor<T>;
      };

   public:

      expression()
      : control_block_(0)
      {
         set_expression(new details::null_node<T>());
      }

      expression(const expression<T>& e)
      : control_block_    (e.control_block_    )
      , symbol_table_list_(e.symbol_table_list_)
      {
         control_block_->ref_count++;
      }

      explicit expression(const symbol_table<T>& symbol_table)
      : control_block_(0)
      {
         set_expression(new details::null_node<T>());
         symbol_table_list_.push_back(symbol_table);
      }

      inline expression<T>& operator=(const expression<T>& e)
      {
         if (this != &e)
         {
            if (control_block_)
            {
               if (
                    (0 !=   control_block_->ref_count) &&
                    (0 == --control_block_->ref_count)
                  )
               {
                  delete control_block_;
               }

               control_block_ = 0;
            }

            control_block_ = e.control_block_;
            control_block_->ref_count++;
            symbol_table_list_ = e.symbol_table_list_;
         }

         return *this;
      }

      inline bool operator==(const expression<T>& e) const
      {
         return (this == &e);
      }

      inline bool operator!() const
      {
         return (
                  (0 == control_block_      ) ||
                  (0 == control_block_->expr)
                );
      }

      inline expression<T>& release()
      {
         exprtk::details::dump_ptr("expression::release", this);
         control_block::destroy(control_block_);

         return (*this);
      }

     ~expression()
      {
         control_block::destroy(control_block_);
      }

      inline T value() const
      {
         assert(control_block_      );
         assert(control_block_->expr);

         return control_block_->expr->value();
      }

      inline T operator() () const
      {
         return value();
      }

      inline operator T() const
      {
         return value();
      }

      inline operator bool() const
      {
         return details::is_true(value());
      }

      inline bool register_symbol_table(symbol_table<T>& st)
      {
         for (std::size_t i = 0; i < symbol_table_list_.size(); ++i)
         {
            if (st == symbol_table_list_[i])
            {
               return false;
            }
         }

         symbol_table_list_.push_back(st);
         return true;
      }

      inline const symbol_table<T>& get_symbol_table(const std::size_t& index = 0) const
      {
         return symbol_table_list_[index];
      }

      inline symbol_table<T>& get_symbol_table(const std::size_t& index = 0)
      {
         return symbol_table_list_[index];
      }

      std::size_t num_symbol_tables() const
      {
         return symbol_table_list_.size();
      }

      typedef results_context<T> results_context_t;

      inline const results_context_t& results() const
      {
         if (control_block_->results)
            return (*control_block_->results);
         else
         {
            static const results_context_t null_results;
            return null_results;
         }
      }

      inline bool return_invoked() const
      {
         assert(control_block_);

         return control_block_                 &&
                control_block_->return_invoked &&
                (*control_block_->return_invoked);
      }

   private:

      inline symtab_list_t get_symbol_table_list() const
      {
         return symbol_table_list_;
      }

      inline void set_expression(const expression_ptr expr)
      {
         if (expr)
         {
            if (control_block_)
            {
               if (0 == --control_block_->ref_count)
               {
                  delete control_block_;
               }
            }

            control_block_ = control_block::create(expr);
         }
      }

      inline void register_local_var(expression_ptr expr)
      {
         if (expr)
         {
            if (control_block_)
            {
               control_block_->
                  local_data_list.push_back(
                     typename expression<T>::control_block::
                        data_pack(reinterpret_cast<void*>(expr),
                                  control_block::e_expr));
            }
         }
      }

      inline void register_local_var(vector_holder_ptr vec_holder)
      {
         if (vec_holder)
         {
            if (control_block_)
            {
               control_block_->
                  local_data_list.push_back(
                     typename expression<T>::control_block::
                        data_pack(reinterpret_cast<void*>(vec_holder),
                                  control_block::e_vecholder));
            }
         }
      }

      inline void register_local_data(void* data, const std::size_t& size = 0, const std::size_t data_mode = 0)
      {
         if (data)
         {
            if (control_block_)
            {
               typename control_block::data_type dt = control_block::e_data;

               switch (data_mode)
               {
                  case 0 : dt = control_block::e_data;    break;
                  case 1 : dt = control_block::e_vecdata; break;
                  case 2 : dt = control_block::e_string;  break;
               }

               control_block_->
                  local_data_list.push_back(
                     typename expression<T>::control_block::
                        data_pack(reinterpret_cast<void*>(data), dt, size));
            }
         }
      }

      inline const typename control_block::local_data_list_t& local_data_list()
      {
         if (control_block_)
         {
            return control_block_->local_data_list;
         }
         else
         {
            static typename control_block::local_data_list_t null_local_data_list;
            return null_local_data_list;
         }
      }

      inline void register_return_results(results_context_t* rc)
      {
         if (control_block_ && rc)
         {
            control_block_->results = rc;
         }
      }

      inline void set_retinvk(bool* retinvk_ptr)
      {
         if (control_block_)
         {
            control_block_->return_invoked = retinvk_ptr;
         }
      }

      inline void set_arena(details::arena_allocator* arena)
      {
         if (control_block_)
         {
            // Transfer ownership of arena to the control_block.
            // Any previously owned arena is discarded (should not happen in practice).
            delete control_block_->arena;
            control_block_->arena = arena;
         }
      }

      control_block* control_block_;
      symtab_list_t  symbol_table_list_;

      friend class parser<T>;
      friend class expression_helper<T>;
      friend class function_compositor<T>;
      template <typename TT>
      friend bool is_valid(const expression<TT>& expr);
   }; // class expression

   template <typename T>
   class expression_helper
   {
   public:

      enum node_types
      {
         e_literal,
         e_variable,
         e_string,
         e_unary,
         e_binary,
         e_function,
         e_vararg,
         e_null,
         e_assert,
         e_sf3ext,
         e_sf4ext
      };

      static inline bool is_literal(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_literal_node(expr.control_block_->expr);
      }

      static inline bool is_variable(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_variable_node(expr.control_block_->expr);
      }

      static inline bool is_string(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_generally_string_node(expr.control_block_->expr);
      }

      static inline bool is_unary(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_unary_node(expr.control_block_->expr);
      }

      static inline bool is_binary(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_binary_node(expr.control_block_->expr);
      }

      static inline bool is_function(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_function(expr.control_block_->expr);
      }

      static inline bool is_vararg(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_vararg_node(expr.control_block_->expr);
      }

      static inline bool is_null(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_null_node(expr.control_block_->expr);
      }

      static inline bool is_assert(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_assert_node(expr.control_block_->expr);
      }

      static inline bool is_sf3ext(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_sf3ext_node(expr.control_block_->expr);
      }

      static inline bool is_sf4ext(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_sf4ext_node(expr.control_block_->expr);
      }

      static inline bool is_type(const expression<T>& expr, const node_types node_type)
      {
         if (0 == expr.control_block_)
         {
            return false;
         }

         switch (node_type)
         {
            case e_literal  : return is_literal_node(expr);
            case e_variable : return is_variable    (expr);
            case e_string   : return is_string      (expr);
            case e_unary    : return is_unary       (expr);
            case e_binary   : return is_binary      (expr);
            case e_function : return is_function    (expr);
            case e_null     : return is_null        (expr);
            case e_assert   : return is_assert      (expr);
            case e_sf3ext   : return is_sf3ext      (expr);
            case e_sf4ext   : return is_sf4ext      (expr);
         };

         return false;
      }

      static inline bool match_type_sequence(const expression<T>& expr, const std::vector<node_types>& type_seq)
      {
         if ((0 == expr.control_block_) || !is_vararg(expr))
         {
            return false;
         }

         typedef details::vararg_node<T, exprtk::details::vararg_multi_op<T> > mo_vararg_t;

         mo_vararg_t* vnode = dynamic_cast<mo_vararg_t*>(expr.control_block_->expr);

         if (
              (0 == vnode) ||
              type_seq.empty() ||
              (vnode->size() < type_seq.size())
            )
         {
            return false;
         }

         for (std::size_t i = 0; i < type_seq.size(); ++i)
         {
            assert((*vnode)[i]);

            switch (type_seq[i])
            {
               case e_literal  : { if (details::is_literal_node         ((*vnode)[i])) continue; } break;
               case e_variable : { if (details::is_variable_node        ((*vnode)[i])) continue; } break;
               case e_string   : { if (details::is_generally_string_node((*vnode)[i])) continue; } break;
               case e_unary    : { if (details::is_unary_node           ((*vnode)[i])) continue; } break;
               case e_binary   : { if (details::is_binary_node          ((*vnode)[i])) continue; } break;
               case e_function : { if (details::is_function             ((*vnode)[i])) continue; } break;
               case e_null     : { if (details::is_null_node            ((*vnode)[i])) continue; } break;
               case e_assert   : { if (details::is_assert_node          ((*vnode)[i])) continue; } break;
               case e_sf3ext   : { if (details::is_sf3ext_node          ((*vnode)[i])) continue; } break;
               case e_sf4ext   : { if (details::is_sf4ext_node          ((*vnode)[i])) continue; } break;
               case e_vararg   : break;
            }

            return false;
         }

         return true;
      }
   };

   template <typename T>
   inline bool is_valid(const expression<T>& expr)
   {
      return expr.control_block_ && !expression_helper<T>::is_null(expr);
   }

   namespace parser_error
   {
      enum error_mode
      {
         e_unknown   = 0,
         e_syntax    = 1,
         e_token     = 2,
         e_numeric   = 4,
         e_symtab    = 5,
         e_lexer     = 6,
         e_synthesis = 7,
         e_helper    = 8,
         e_parser    = 9
      };

      struct type
      {
         type()
         : mode(parser_error::e_unknown)
         , line_no  (0)
         , column_no(0)
         {}

         lexer::token token;
         error_mode mode;
         std::string diagnostic;
         std::string src_location;
         std::string error_line;
         std::size_t line_no;
         std::size_t column_no;
      };

      inline type make_error(const error_mode mode,
                             const std::string& diagnostic   = "",
                             const std::string& src_location = "")
      {
         type t;
         t.mode         = mode;
         t.token.type   = lexer::token::e_error;
         t.diagnostic   = diagnostic;
         t.src_location = src_location;
         exprtk_debug(("%s\n", diagnostic .c_str()));
         return t;
      }

      inline type make_error(const error_mode mode,
                             const lexer::token& tk,
                             const std::string& diagnostic   = "",
                             const std::string& src_location = "")
      {
         type t;
         t.mode         = mode;
         t.token        = tk;
         t.diagnostic   = diagnostic;
         t.src_location = src_location;
         exprtk_debug(("%s\n", diagnostic .c_str()));
         return t;
      }

      inline std::string to_str(error_mode mode)
      {
         switch (mode)
         {
            case e_unknown : return std::string("Unknown Error");
            case e_syntax  : return std::string("Syntax Error" );
            case e_token   : return std::string("Token Error"  );
            case e_numeric : return std::string("Numeric Error");
            case e_symtab  : return std::string("Symbol Error" );
            case e_lexer   : return std::string("Lexer Error"  );
            case e_helper  : return std::string("Helper Error" );
            case e_parser  : return std::string("Parser Error" );
            default        : return std::string("Unknown Error");
         }
      }

      inline bool update_error(type& error, const std::string& expression)
      {
         if (
              expression.empty()                         ||
              (error.token.position > expression.size()) ||
              (std::numeric_limits<std::size_t>::max() == error.token.position)
            )
         {
            return false;
         }

         std::size_t error_line_start = 0;

         for (std::size_t i = error.token.position; i > 0; --i)
         {
            const details::char_t c = expression[i];

            if (('\n' == c) || ('\r' == c))
            {
               error_line_start = i + 1;
               break;
            }
         }

         std::size_t next_nl_position = std::min(expression.size(),
                                                 expression.find_first_of('\n',error.token.position + 1));

         error.column_no  = error.token.position - error_line_start;
         error.error_line = expression.substr(error_line_start,
                                              next_nl_position - error_line_start);

         error.line_no = 0;

         for (std::size_t i = 0; i < next_nl_position; ++i)
         {
            if ('\n' == expression[i])
               ++error.line_no;
         }

         return true;
      }

      inline void dump_error(const type& error)
      {
         printf("Position: %02d   Type: [%s]   Msg: %s\n",
                static_cast<int>(error.token.position),
                exprtk::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str());
      }
   }

   namespace details
   {
      template <typename Parser>
      inline void disable_type_checking(Parser& p)
      {
         p.state_.type_check_enabled = false;
      }
   }

} // namespace exprtk

#endif // EXPRTK_SRC_EXPRESSION_HPP
