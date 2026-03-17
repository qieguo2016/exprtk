#ifndef EXPRTK_SRC_EXTRAS_HPP
#define EXPRTK_SRC_EXTRAS_HPP

#include "parser.hpp"

namespace exprtk
{

   namespace details
   {
      template <typename T>
      struct collector_helper
      {
         typedef exprtk::symbol_table<T> symbol_table_t;
         typedef exprtk::expression<T>   expression_t;
         typedef exprtk::parser<T>       parser_t;
         typedef typename parser_t::dependent_entity_collector::symbol_t symbol_t;
         typedef typename parser_t::unknown_symbol_resolver usr_t;

         struct resolve_as_vector : public usr_t
         {
            typedef exprtk::parser<T> parser_t;

            using usr_t::process;

            resolve_as_vector()
            : usr_t(usr_t::e_usrmode_extended)
            {}

            virtual bool process(const std::string& unknown_symbol,
                                 symbol_table_t& symbol_table,
                                 std::string&) exprtk_override
            {
               static T v[1];
               symbol_table.add_vector(unknown_symbol,v);
               return true;
            }
         };

         static inline bool collection_pass(const std::string& expression_string,
                                            std::set<std::string>& symbol_set,
                                            const bool collect_variables,
                                            const bool collect_functions,
                                            const bool vector_pass,
                                            symbol_table_t& ext_symbol_table)
         {
            symbol_table_t symbol_table;
            expression_t   expression;
            parser_t       parser;

            resolve_as_vector vect_resolver;

            expression.register_symbol_table(symbol_table    );
            expression.register_symbol_table(ext_symbol_table);

            if (vector_pass)
               parser.enable_unknown_symbol_resolver(&vect_resolver);
            else
               parser.enable_unknown_symbol_resolver();

            if (collect_variables)
               parser.dec().collect_variables() = true;

            if (collect_functions)
               parser.dec().collect_functions() = true;

            bool pass_result = false;

            details::disable_type_checking(parser);

            if (parser.compile(expression_string, expression))
            {
               pass_result = true;

               std::deque<symbol_t> symb_list;
               parser.dec().symbols(symb_list);

               for (std::size_t i = 0; i < symb_list.size(); ++i)
               {
                  symbol_set.insert(symb_list[i].first);
               }
            }

            return pass_result;
         }
      };
   }

   template <typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_variables(const std::string& expression,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef double T;
      typedef details::collector_helper<T> collect_t;

      collect_t::symbol_table_t null_symbol_table;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, true, false, false, null_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, true, false,  true, null_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename T,
             typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_variables(const std::string& expression,
                                 exprtk::symbol_table<T>& extrnl_symbol_table,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef details::collector_helper<T> collect_t;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, true, false, false, extrnl_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, true, false,  true, extrnl_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_functions(const std::string& expression,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef double T;
      typedef details::collector_helper<T> collect_t;

      collect_t::symbol_table_t null_symbol_table;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, false, true, false, null_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, false, true,  true, null_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename T,
             typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_functions(const std::string& expression,
                                 exprtk::symbol_table<T>& extrnl_symbol_table,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef details::collector_helper<T> collect_t;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, false, true, false, extrnl_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, false, true,  true, extrnl_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename T>
   inline T integrate(const expression<T>& e,
                      T& x,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      if (r0 > r1)
         return T(0);

      const T h = (r1 - r0) / (T(2) * number_of_intervals);
      T total_area = T(0);

      for (std::size_t i = 0; i < number_of_intervals; ++i)
      {
         x = r0 + T(2) * i * h;
         const T y0 = e.value(); x += h;
         const T y1 = e.value(); x += h;
         const T y2 = e.value(); x += h;
         total_area += h * (y0 + T(4) * y1 + y2) / T(3);
      }

      return total_area;
   }

   template <typename T>
   inline T integrate(const expression<T>& e,
                      const std::string& variable_name,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = integrate(e, x, r0, r1, number_of_intervals);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T derivative(const expression<T>& e,
                       T& x,
                       const T& h = T(0.00000001))
   {
      const T x_init = x;
      const T _2h    = T(2) * h;

      x = x_init + _2h;
      const T y0 = e.value();
      x = x_init + h;
      const T y1 = e.value();
      x = x_init - h;
      const T y2 = e.value();
      x = x_init - _2h;
      const T y3 = e.value();
      x = x_init;

      return (-y0 + T(8) * (y1 - y2) + y3) / (T(12) * h);
   }

   template <typename T>
   inline T second_derivative(const expression<T>& e,
                              T& x,
                              const T& h = T(0.00001))
   {
      const T x_init = x;
      const T _2h    = T(2) * h;

      const T y = e.value();
      x = x_init + _2h;
      const T y0 = e.value();
      x = x_init + h;
      const T y1 = e.value();
      x = x_init - h;
      const T y2 = e.value();
      x = x_init - _2h;
      const T y3 = e.value();
      x = x_init;

      return (-y0 + T(16) * (y1 + y2) - T(30) * y - y3) / (T(12) * h * h);
   }

   template <typename T>
   inline T third_derivative(const expression<T>& e,
                             T& x,
                             const T& h = T(0.0001))
   {
      const T x_init = x;
      const T _2h    = T(2) * h;

      x = x_init + _2h;
      const T y0 = e.value();
      x = x_init + h;
      const T y1 = e.value();
      x = x_init - h;
      const T y2 = e.value();
      x = x_init - _2h;
      const T y3 = e.value();
      x = x_init;

      return (y0 + T(2) * (y2 - y1) - y3) / (T(2) * h * h * h);
   }

   template <typename T>
   inline T derivative(const expression<T>& e,
                       const std::string& variable_name,
                       const T& h = T(0.00000001))
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = derivative(e, x, h);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T second_derivative(const expression<T>& e,
                              const std::string& variable_name,
                              const T& h = T(0.00001))
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = second_derivative(e, x, h);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T third_derivative(const expression<T>& e,
                             const std::string& variable_name,
                             const T& h = T(0.0001))
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = third_derivative(e, x, h);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   /*
      Note: The following 'compute' routines are simple helpers,
      for quickly setting up the required pieces of code in order
      to evaluate an expression. By virtue of how they operate
      there will be an overhead with regards to their setup and
      teardown and hence should not be used in time critical
      sections of code.
      Furthermore they only assume a small sub set of variables,
      no string variables or user defined functions.
   */
   template <typename T>
   inline bool compute(const std::string& expression_string, T& result)
   {
      // No variables
      symbol_table<T> symbol_table;
      symbol_table.add_constants();

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T& x,
                       T& result)
   {
      // Only 'x'
      static const std::string x_var("x");

      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_constant(x_var,x);

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T&x, const T& y,
                       T& result)
   {
      // Only 'x' and 'y'
      static const std::string x_var("x");
      static const std::string y_var("y");

      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_constant(x_var,x);
      symbol_table.add_constant(y_var,y);

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T& x, const T& y, const T& z,
                       T& result)
   {
      // Only 'x', 'y' or 'z'
      static const std::string x_var("x");
      static const std::string y_var("y");
      static const std::string z_var("z");

      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_constant(x_var,x);
      symbol_table.add_constant(y_var,y);
      symbol_table.add_constant(z_var,z);

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T, std::size_t N>
   class polynomial : public ifunction<T>
   {
   private:

      template <typename Type, std::size_t NumberOfCoefficients>
      struct poly_impl { };

      template <typename Type>
      struct poly_impl <Type,12>
      {
         static inline T evaluate(const Type x,
                                  const Type c12, const Type c11, const Type c10, const Type c9, const Type c8,
                                  const Type  c7, const Type  c6, const Type  c5, const Type c4, const Type c3,
                                  const Type  c2, const Type  c1, const Type  c0)
         {
            // p(x) = c_12x^12 + c_11x^11 + c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((((((c12 * x + c11) * x + c10) * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,11>
      {
         static inline T evaluate(const Type x,
                                  const Type c11, const Type c10, const Type c9, const Type c8, const Type c7,
                                  const Type c6,  const Type  c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1,  const Type  c0)
         {
            // p(x) = c_11x^11 + c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((((((c11 * x + c10) * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,10>
      {
         static inline T evaluate(const Type x,
                                  const Type c10, const Type c9, const Type c8, const Type c7, const Type c6,
                                  const Type c5,  const Type c4, const Type c3, const Type c2, const Type c1,
                                  const Type c0)
         {
            // p(x) = c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((((c10 * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,9>
      {
         static inline T evaluate(const Type x,
                                  const Type c9, const Type c8, const Type c7, const Type c6, const Type c5,
                                  const Type c4, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((((c9 * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,8>
      {
         static inline T evaluate(const Type x,
                                  const Type c8, const Type c7, const Type c6, const Type c5, const Type c4,
                                  const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((c8 * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,7>
      {
         static inline T evaluate(const Type x,
                                  const Type c7, const Type c6, const Type c5, const Type c4, const Type c3,
                                  const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((c7 * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,6>
      {
         static inline T evaluate(const Type x,
                                  const Type c6, const Type c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1, const Type c0)
         {
            // p(x) = c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((c6 * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,5>
      {
         static inline T evaluate(const Type x,
                                  const Type c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1, const Type c0)
         {
            // p(x) = c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((c5 * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,4>
      {
         static inline T evaluate(const Type x, const Type c4, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((c4 * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,3>
      {
         static inline T evaluate(const Type x, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((c3 * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,2>
      {
         static inline T evaluate(const Type x, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_2x^2 + c_1x^1 + c_0x^0
            return ((c2 * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,1>
      {
         static inline T evaluate(const Type x, const Type c1, const Type c0)
         {
            // p(x) = c_1x^1 + c_0x^0
            return (c1 * x + c0);
         }
      };

   public:

      using ifunction<T>::operator();

      polynomial()
      : ifunction<T>((N+2 <= 20) ? (N + 2) : std::numeric_limits<std::size_t>::max())
      {
         disable_has_side_effects(*this);
      }

      virtual ~polynomial() exprtk_override
      {}

      #define poly_rtrn(NN) \
      return (NN != N) ? std::numeric_limits<T>::quiet_NaN() :

      inline virtual T operator() (const T& x, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(1) (poly_impl<T,1>::evaluate(x, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c2, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(2) (poly_impl<T,2>::evaluate(x, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c3, const T& c2, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(3) (poly_impl<T,3>::evaluate(x, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c4, const T& c3, const T& c2, const T& c1,
                                   const T& c0) exprtk_override
      {
         poly_rtrn(4) (poly_impl<T,4>::evaluate(x, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c5, const T& c4, const T& c3, const T& c2,
                                   const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(5) (poly_impl<T,5>::evaluate(x, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c6, const T& c5, const T& c4, const T& c3,
                                   const T& c2, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(6) (poly_impl<T,6>::evaluate(x, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c7, const T& c6, const T& c5, const T& c4,
                                   const T& c3, const T& c2, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(7) (poly_impl<T,7>::evaluate(x, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c8, const T& c7, const T& c6, const T& c5,
                                   const T& c4, const T& c3, const T& c2, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(8) (poly_impl<T,8>::evaluate(x, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c9, const T& c8, const T& c7, const T& c6,
                                   const T& c5, const T& c4, const T& c3, const T& c2, const T& c1,
                                   const T& c0) exprtk_override
      {
         poly_rtrn(9) (poly_impl<T,9>::evaluate(x, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c10, const T& c9, const T& c8, const T& c7,
                                   const T& c6, const T& c5, const T& c4, const T& c3, const T& c2,
                                   const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(10) (poly_impl<T,10>::evaluate(x, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c11, const T& c10, const T& c9, const T& c8,
                                   const T& c7, const T& c6, const T& c5, const T& c4, const T& c3,
                                   const T& c2, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(11) (poly_impl<T,11>::evaluate(x, c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c12, const T& c11, const T& c10, const T& c9,
                                   const T& c8, const T& c7, const T& c6, const T& c5, const T& c4,
                                   const T& c3, const T& c2, const T& c1, const T& c0) exprtk_override
      {
         poly_rtrn(12) (poly_impl<T,12>::evaluate(x, c12, c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      #undef poly_rtrn

      inline virtual T operator() () exprtk_override
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator() (const T&) exprtk_override
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator() (const T&, const T&) exprtk_override
      {
         return std::numeric_limits<T>::quiet_NaN();
      }
   };

   template <typename T>
   class function_compositor
   {
   public:

      typedef exprtk::expression<T>             expression_t;
      typedef exprtk::symbol_table<T>           symbol_table_t;
      typedef exprtk::parser<T>                 parser_t;
      typedef typename parser_t::settings_store settings_t;

      struct function
      {
         function()
         {}

         explicit function(const std::string& n)
         : name_(n)
         {}

         function(const std::string& name,
                  const std::string& expression)
         : name_(name)
         , expression_(expression)
         {}

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1,
                  const std::string& v2)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
            v_.push_back(v2);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1,
                  const std::string& v2, const std::string& v3)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
            v_.push_back(v2); v_.push_back(v3);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1,
                  const std::string& v2, const std::string& v3,
                  const std::string& v4)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
            v_.push_back(v2); v_.push_back(v3);
            v_.push_back(v4);
         }

         inline function& name(const std::string& n)
         {
            name_ = n;
            return (*this);
         }

         inline function& expression(const std::string& e)
         {
            expression_ = e;
            return (*this);
         }

         inline function& var(const std::string& v)
         {
            v_.push_back(v);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1,
                               const std::string& v2)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            v_.push_back(v2);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1,
                               const std::string& v2,
                               const std::string& v3)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            v_.push_back(v2);
            v_.push_back(v3);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1,
                               const std::string& v2,
                               const std::string& v3,
                               const std::string& v4)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            v_.push_back(v2);
            v_.push_back(v3);
            v_.push_back(v4);
            return (*this);
         }

         std::string name_;
         std::string expression_;
         std::deque<std::string> v_;
      };

   private:

      struct base_func : public exprtk::ifunction<T>
      {
         typedef const T&                  type;
         typedef exprtk::ifunction<T>      function_t;
         typedef std::vector<T*>           varref_t;
         typedef std::vector<T>            var_t;
         typedef std::vector<std::string>  str_t;
         typedef std::pair<T*,std::size_t> lvarref_t;
         typedef std::vector<lvarref_t>    lvr_vec_t;
         typedef std::vector<std::string*> lstr_vec_t;

         using exprtk::ifunction<T>::operator();

         explicit base_func(const std::size_t& pc = 0)
         : exprtk::ifunction<T>(pc)
         , local_var_stack_size(0)
         , stack_depth(0)
         {
            v.resize(pc);
         }

         virtual ~base_func()
         {}

         #define exprtk_assign(Index) \
         (*v[Index]) = v##Index;      \

         inline void update(const T& v0)
         {
            exprtk_assign(0)
         }

         inline void update(const T& v0, const T& v1)
         {
            exprtk_assign(0) exprtk_assign(1)
         }

         inline void update(const T& v0, const T& v1, const T& v2)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2)
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2) exprtk_assign(3)
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2) exprtk_assign(3)
            exprtk_assign(4)
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4, const T& v5)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2) exprtk_assign(3)
            exprtk_assign(4) exprtk_assign(5)
         }

         #ifdef exprtk_assign
         #undef exprtk_assign
         #endif

         inline function_t& setup(expression_t& expr)
         {
            expression = expr;

            typedef typename expression_t::control_block  ctrlblk_t;
            typedef typename ctrlblk_t::local_data_list_t ldl_t;
            typedef typename ctrlblk_t::data_type         data_t;
            typedef typename ldl_t::value_type            ldl_value_type;

            const ldl_t ldl = expr.local_data_list();

            std::vector<std::pair<std::size_t,data_t> > index_list;

            for (std::size_t i = 0; i < ldl.size(); ++i)
            {
               exprtk_debug(("base_func::setup() - element[%02d] type: %s size: %d\n",
                             static_cast<int>(i),
                             expression_t::control_block::to_str(ldl[i].type).c_str(),
                             static_cast<int>(ldl[i].size)));

               switch (ldl[i].type)
               {
                  case ctrlblk_t::e_unknown   : continue;
                  case ctrlblk_t::e_expr      : continue;
                  case ctrlblk_t::e_vecholder : continue;
                  default                     : break;
               }

               if (ldl[i].size)
               {
                  index_list.push_back(std::make_pair(i,ldl[i].type));
               }
            }

            std::size_t input_param_count = 0;

            for (std::size_t i = 0; i < index_list.size(); ++i)
            {
               const std::size_t index         = index_list[i].first;
               const ldl_value_type& local_var = ldl[index];

               assert(local_var.pointer);

               if (i < (index_list.size() - v.size()))
               {
                  if (local_var.type == ctrlblk_t::e_string)
                  {
                     local_str_vars.push_back(
                        reinterpret_cast<std::string*>(local_var.pointer));
                  }
                  else if (
                            (local_var.type == ctrlblk_t::e_data   ) ||
                            (local_var.type == ctrlblk_t::e_vecdata)
                          )
                  {
                     local_vars.push_back(std::make_pair(
                        reinterpret_cast<T*>(local_var.pointer),
                        local_var.size));

                     local_var_stack_size += local_var.size;
                  }
               }
               else
               {
                  v[input_param_count++] = reinterpret_cast<T*>(local_var.pointer);
               }
            }

            clear_stack();

            return (*this);
         }

         inline void pre()
         {
            if (stack_depth++)
            {
               if (!v.empty())
               {
                  var_t var_stack(v.size(),T(0));
                  copy(v,var_stack);
                  input_params_stack.push_back(var_stack);
               }

               if (!local_vars.empty())
               {
                  var_t local_vec_frame(local_var_stack_size,T(0));
                  copy(local_vars,local_vec_frame);
                  local_var_stack.push_back(local_vec_frame);
               }

               if (!local_str_vars.empty())
               {
                  str_t local_str_frame(local_str_vars.size());
                  copy(local_str_vars,local_str_frame);
                  local_str_stack.push_back(local_str_frame);
               }
            }
         }

         inline void post()
         {
            if (--stack_depth)
            {
               if (!v.empty())
               {
                  copy(input_params_stack.back(), v);
                  input_params_stack.pop_back();
               }

               if (!local_vars.empty())
               {
                  copy(local_var_stack.back(), local_vars);
                  local_var_stack.pop_back();
               }

               if (!local_str_vars.empty())
               {
                  copy(local_str_stack.back(), local_str_vars);
                  local_str_stack.pop_back();
               }
            }
         }

         void copy(const varref_t& src_v, var_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               dest_v[i] = (*src_v[i]);
            }
         }

         void copy(const lstr_vec_t& src_v, str_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               dest_v[i] = (*src_v[i]);
            }
         }

         void copy(const var_t& src_v, varref_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               (*dest_v[i]) = src_v[i];
            }
         }

         void copy(const lvr_vec_t& src_v, var_t& dest_v)
         {
            typename var_t::iterator itr = dest_v.begin();
            typedef  typename std::iterator_traits<typename var_t::iterator>::difference_type diff_t;

            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               lvarref_t vr = src_v[i];

               if (1 == vr.second)
                  *itr++ = (*vr.first);
               else
               {
                  std::copy(vr.first, vr.first + vr.second, itr);
                  itr += static_cast<diff_t>(vr.second);
               }
            }
         }

         void copy(const var_t& src_v, lvr_vec_t& dest_v)
         {
            typename var_t::const_iterator itr = src_v.begin();
            typedef  typename std::iterator_traits<typename var_t::iterator>::difference_type diff_t;

            for (std::size_t i = 0; i < dest_v.size(); ++i)
            {
               lvarref_t& vr = dest_v[i];

               assert(vr.first != 0);
               assert(vr.second > 0);

               if (1 == vr.second)
                  (*vr.first) = *itr++;
               else
               {
                  std::copy(itr, itr + static_cast<diff_t>(vr.second), vr.first);
                  itr += static_cast<diff_t>(vr.second);
               }
            }
         }

         void copy(const str_t& src_str, lstr_vec_t& dest_str)
         {
            assert(src_str.size() == dest_str.size());

            for (std::size_t i = 0; i < dest_str.size(); ++i)
            {
               *dest_str[i] = src_str[i];
            }
         }

         inline void clear_stack()
         {
            for (std::size_t i = 0; i < v.size(); ++i)
            {
               (*v[i]) = 0;
            }
         }

         inline virtual T value(expression_t& e)
         {
            return e.value();
         }

         expression_t      expression;
         varref_t          v;
         lvr_vec_t         local_vars;
         lstr_vec_t        local_str_vars;
         std::size_t       local_var_stack_size;
         std::size_t       stack_depth;
         std::deque<var_t> input_params_stack;
         std::deque<var_t> local_var_stack;
         std::deque<str_t> local_str_stack;
      };

      typedef std::map<std::string,base_func*> funcparam_t;

      typedef const T& type;

      template <typename BaseFuncType>
      struct scoped_bft
      {
         explicit scoped_bft(BaseFuncType& bft)
         : bft_(bft)
         {
            bft_.pre ();
         }

        ~scoped_bft()
         {
            bft_.post();
         }

         BaseFuncType& bft_;

      private:

         scoped_bft(const scoped_bft&) exprtk_delete;
         scoped_bft& operator=(const scoped_bft&) exprtk_delete;
      };

      struct func_0param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_0param() : base_func(0) {}

         inline T operator() () exprtk_override
         {
            scoped_bft<func_0param> sb(*this);
            return this->value(base_func::expression);
         }
      };

      struct func_1param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_1param() : base_func(1) {}

         inline T operator() (type v0) exprtk_override
         {
            scoped_bft<func_1param> sb(*this);
            base_func::update(v0);
            return this->value(base_func::expression);
         }
      };

      struct func_2param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_2param() : base_func(2) {}

         inline T operator() (type v0, type v1) exprtk_override
         {
            scoped_bft<func_2param> sb(*this);
            base_func::update(v0, v1);
            return this->value(base_func::expression);
         }
      };

      struct func_3param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_3param() : base_func(3) {}

         inline T operator() (type v0, type v1, type v2) exprtk_override
         {
            scoped_bft<func_3param> sb(*this);
            base_func::update(v0, v1, v2);
            return this->value(base_func::expression);
         }
      };

      struct func_4param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_4param() : base_func(4) {}

         inline T operator() (type v0, type v1, type v2, type v3) exprtk_override
         {
            scoped_bft<func_4param> sb(*this);
            base_func::update(v0, v1, v2, v3);
            return this->value(base_func::expression);
         }
      };

      struct func_5param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_5param() : base_func(5) {}

         inline T operator() (type v0, type v1, type v2, type v3, type v4) exprtk_override
         {
            scoped_bft<func_5param> sb(*this);
            base_func::update(v0, v1, v2, v3, v4);
            return this->value(base_func::expression);
         }
      };

      struct func_6param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_6param() : base_func(6) {}

         inline T operator() (type v0, type v1, type v2, type v3, type v4, type v5) exprtk_override
         {
            scoped_bft<func_6param> sb(*this);
            base_func::update(v0, v1, v2, v3, v4, v5);
            return this->value(base_func::expression);
         }
      };

      static T return_value(expression_t& e)
      {
         typedef exprtk::results_context<T> results_context_t;
         typedef typename results_context_t::type_store_t type_t;
         typedef typename type_t::scalar_view scalar_t;

         const T result = e.value();

         if (e.return_invoked())
         {
            // Due to the post compilation checks, it can be safely
            // assumed that there will be at least one parameter
            // and that the first parameter will always be scalar.
            return scalar_t(e.results()[0])();
         }

         return result;
      }

      #define def_fp_retval(N)                                            \
      struct func_##N##param_retval exprtk_final : public func_##N##param \
      {                                                                   \
         inline T value(expression_t& e) exprtk_override                  \
         {                                                                \
            return return_value(e);                                       \
         }                                                                \
      };                                                                  \

      def_fp_retval(0)
      def_fp_retval(1)
      def_fp_retval(2)
      def_fp_retval(3)
      def_fp_retval(4)
      def_fp_retval(5)
      def_fp_retval(6)

      #undef def_fp_retval

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline bool add(const std::string& name,
                      const std::string& expression,
                      const Sequence<std::string,Allocator>& var_list,
                      const bool override = false)
      {
         const typename std::map<std::string,expression_t>::iterator itr = expr_map_.find(name);

         if (expr_map_.end() != itr)
         {
            if (!override)
            {
               exprtk_debug(("Compositor error(add): function '%s' already defined\n",
                             name.c_str()));

               return false;
            }

            remove(name, var_list.size());
         }

         if (compile_expression(name, expression, var_list))
         {
            const std::size_t n = var_list.size();

            fp_map_[n][name]->setup(expr_map_[name]);

            return true;
         }
         else
         {
            exprtk_debug(("Compositor error(add): Failed to compile function '%s'\n",
                          name.c_str()));

            return false;
         }
      }

   public:

      function_compositor()
      : parser_(settings_t::default_compile_all_opts +
                settings_t::e_disable_zero_return)
      , fp_map_(7)
      , load_variables_(false)
      , load_vectors_(false)
      {}

      explicit function_compositor(const symbol_table_t& st)
      : symbol_table_(st)
      , parser_(settings_t::default_compile_all_opts +
                settings_t::e_disable_zero_return)
      , fp_map_(7)
      , load_variables_(false)
      , load_vectors_(false)
      {}

     ~function_compositor()
      {
         clear();
      }

      inline symbol_table_t& symbol_table()
      {
         return symbol_table_;
      }

      inline const symbol_table_t& symbol_table() const
      {
         return symbol_table_;
      }

      inline void add_auxiliary_symtab(symbol_table_t& symtab)
      {
         auxiliary_symtab_list_.push_back(&symtab);
      }

      void load_variables(const bool load = true)
      {
         load_variables_ = load;
      }

      void load_vectors(const bool load = true)
      {
         load_vectors_ = load;
      }

      inline void register_loop_runtime_check(loop_runtime_check& lrtchk)
      {
         parser_.register_loop_runtime_check(lrtchk);
      }

      inline void register_vector_access_runtime_check(vector_access_runtime_check& vartchk)
      {
         parser_.register_vector_access_runtime_check(vartchk);
      }

      inline void register_compilation_timeout_check(compilation_check& compchk)
      {
         parser_.register_compilation_timeout_check(compchk);
      }

      inline void clear_loop_runtime_check()
      {
         parser_.clear_loop_runtime_check();
      }

      inline void clear_vector_access_runtime_check()
      {
         parser_.clear_vector_access_runtime_check();
      }

      inline void clear_compilation_timeout_check()
      {
         parser_.clear_compilation_timeout_check();
      }

      void clear()
      {
         symbol_table_.clear();
         expr_map_    .clear();

         for (std::size_t i = 0; i < fp_map_.size(); ++i)
         {
            typename funcparam_t::iterator itr = fp_map_[i].begin();
            typename funcparam_t::iterator end = fp_map_[i].end  ();

            while (itr != end)
            {
               delete itr->second;
               ++itr;
            }

            fp_map_[i].clear();
         }

         clear_loop_runtime_check         ();
         clear_vector_access_runtime_check();
         clear_compilation_timeout_check  ();
      }

      inline bool add(const function& f, const bool override = false)
      {
         return add(f.name_, f.expression_, f.v_,override);
      }

      inline std::string error() const
      {
         if (!error_list_.empty())
         {
            return error_list_[0].diagnostic;
         }
         else
            return std::string("No Error");
      }

      inline std::size_t error_count() const
      {
         return error_list_.size();
      }

      inline parser_error::type get_error(const std::size_t& index) const
      {
         if (index < error_list_.size())
         {
            return error_list_[index];
         }

         throw std::invalid_argument("compositor::get_error() - Invalid error index specified");
      }

   private:

      template <typename Allocator,
                template <typename, typename> class Sequence>
      bool compile_expression(const std::string& name,
                              const std::string& expression,
                              const Sequence<std::string,Allocator>& input_var_list,
                              bool  return_present = false)
      {
         expression_t compiled_expression;
         symbol_table_t local_symbol_table;

         local_symbol_table.load_from(symbol_table_);
         local_symbol_table.add_constants();

         if (load_variables_)
         {
            local_symbol_table.load_variables_from(symbol_table_);
         }

         if (load_vectors_)
         {
            local_symbol_table.load_vectors_from(symbol_table_);
         }

         error_list_.clear();

         if (!valid(name,input_var_list.size()))
         {
            parser_error::type error =
               parser_error::make_error(
                  parser_error::e_parser,
                  lexer::token(),
                  "ERR283 - Function '" + name + "' is an invalid overload",
                  exprtk_error_location);

            error_list_.push_back(error);
            return false;
         }

         if (!forward(name,
                      input_var_list.size(),
                      local_symbol_table,
                      return_present))
            return false;

         compiled_expression.register_symbol_table(local_symbol_table);

         for (std::size_t i = 0; i < auxiliary_symtab_list_.size(); ++i)
         {
            compiled_expression.register_symbol_table((*auxiliary_symtab_list_[i]));
         }

         std::string mod_expression;

         for (std::size_t i = 0; i < input_var_list.size(); ++i)
         {
            mod_expression += " var " + input_var_list[i] + "{};\n";
         }

         if (
              ('{' == details::front(expression)) &&
              ('}' == details::back (expression))
            )
            mod_expression += "~" + expression + ";";
         else
            mod_expression += "~{" + expression + "};";

         if (!parser_.compile(mod_expression,compiled_expression))
         {
            exprtk_debug(("Compositor Error: %s\n", parser_.error().c_str()));
            exprtk_debug(("Compositor modified expression: \n%s\n", mod_expression.c_str()));

            remove(name,input_var_list.size());

            for (std::size_t err_index = 0; err_index < parser_.error_count(); ++err_index)
            {
               error_list_.push_back(parser_.get_error(err_index));
            }

            return false;
         }

         if (!return_present && parser_.dec().return_present())
         {
            remove(name,input_var_list.size());
            return compile_expression(name, expression, input_var_list, true);
         }

         // Make sure every return point has a scalar as its first parameter
         if (parser_.dec().return_present())
         {
            typedef std::vector<std::string> str_list_t;

            str_list_t ret_param_list = parser_.dec().return_param_type_list();

            for (std::size_t i = 0; i < ret_param_list.size(); ++i)
            {
               const std::string& params = ret_param_list[i];

               if (params.empty() || ('T' != params[0]))
               {
                  exprtk_debug(("Compositor Error: Return statement in function '%s' is invalid\n",
                                name.c_str()));

                  remove(name,input_var_list.size());

                  return false;
               }
            }
         }

         expr_map_[name] = compiled_expression;

         exprtk::ifunction<T>& ifunc = (*(fp_map_[input_var_list.size()])[name]);

         if (symbol_table_.add_function(name,ifunc))
            return true;
         else
         {
            exprtk_debug(("Compositor Error: Failed to add function '%s' to symbol table\n",
                          name.c_str()));
            return false;
         }
      }

      inline bool symbol_used(const std::string& symbol) const
      {
         return (
                  symbol_table_.is_variable       (symbol) ||
                  symbol_table_.is_stringvar      (symbol) ||
                  symbol_table_.is_function       (symbol) ||
                  symbol_table_.is_vector         (symbol) ||
                  symbol_table_.is_vararg_function(symbol)
                );
      }

      inline bool valid(const std::string& name,
                        const std::size_t& arg_count) const
      {
         if (arg_count > 6)
            return false;
         else if (symbol_used(name))
            return false;
         else if (fp_map_[arg_count].end() != fp_map_[arg_count].find(name))
            return false;
         else
            return true;
      }

      inline bool forward(const std::string& name,
                          const std::size_t& arg_count,
                          symbol_table_t& sym_table,
                          const bool ret_present = false)
      {
         switch (arg_count)
         {
            #define case_stmt(N)                                     \
            case N : (fp_map_[arg_count])[name] =                    \
                     (!ret_present) ? static_cast<base_func*>        \
                                      (new func_##N##param) :        \
                                      static_cast<base_func*>        \
                                      (new func_##N##param_retval) ; \
                     break;                                          \

            case_stmt(0) case_stmt(1) case_stmt(2)
            case_stmt(3) case_stmt(4) case_stmt(5)
            case_stmt(6)
            #undef case_stmt
         }

         exprtk::ifunction<T>& ifunc = (*(fp_map_[arg_count])[name]);

         return sym_table.add_function(name,ifunc);
      }

      inline void remove(const std::string& name, const std::size_t& arg_count)
      {
         if (arg_count > 6)
            return;

         const typename std::map<std::string,expression_t>::iterator em_itr = expr_map_.find(name);

         if (expr_map_.end() != em_itr)
         {
            expr_map_.erase(em_itr);
         }

         const typename funcparam_t::iterator fp_itr = fp_map_[arg_count].find(name);

         if (fp_map_[arg_count].end() != fp_itr)
         {
            delete fp_itr->second;
            fp_map_[arg_count].erase(fp_itr);
         }

         symbol_table_.remove_function(name);
      }

   private:

      symbol_table_t symbol_table_;
      parser_t parser_;
      std::map<std::string,expression_t> expr_map_;
      std::vector<funcparam_t> fp_map_;
      std::vector<symbol_table_t*> auxiliary_symtab_list_;
      std::deque<parser_error::type> error_list_;
      bool load_variables_;
      bool load_vectors_;
   }; // class function_compositor

} // namespace exprtk

#endif // EXPRTK_SRC_EXTRAS_HPP
