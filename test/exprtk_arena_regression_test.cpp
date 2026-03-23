#include "common.hpp"

#define private public
#include "exprtk.hpp"
#undef private

#include <cstdio>
#include <string>
#include <vector>

namespace
{
   typedef double numeric_type;
   typedef exprtk::expression<numeric_type> expression_t;
   typedef exprtk::parser<numeric_type>     parser_t;
   typedef exprtk::symbol_table<numeric_type> symbol_table_t;

   bool test_failed_compile_clears_parser_arena()
   {
      parser_t parser;
      expression_t expression;

      if (parser.compile("", expression))
      {
         std::printf("test_failed_compile_clears_parser_arena() - empty expression unexpectedly compiled\n");
         return false;
      }

      if (0 != parser.node_allocator_.get_arena())
      {
         std::printf("test_failed_compile_clears_parser_arena() - parser retained arena after failed compile\n");
         return false;
      }

      return true;
   }

   bool test_vector_view_refs_are_released_on_expression_destroy()
   {
      std::vector<numeric_type> data(4, numeric_type(1));
      exprtk::vector_view<numeric_type> view = exprtk::make_vector_view(data, data.size());

      if (!view.data_ref_.empty())
      {
         std::printf("test_vector_view_refs_are_released_on_expression_destroy() - vector view started with refs\n");
         return false;
      }

      {
         symbol_table_t symbol_table;
         symbol_table.add_vector("x", view);

         expression_t expression;
         expression.register_symbol_table(symbol_table);

         parser_t parser;
         if (!parser.compile("x", expression))
         {
            std::printf("test_vector_view_refs_are_released_on_expression_destroy() - compile failed: %s\n",
                        parser.error().c_str());
            return false;
         }

         if (view.data_ref_.empty())
         {
            std::printf("test_vector_view_refs_are_released_on_expression_destroy() - compile did not register vector refs\n");
            return false;
         }
      }

      if (!view.data_ref_.empty())
      {
         std::printf("test_vector_view_refs_are_released_on_expression_destroy() - vector refs remained after expression destruction\n");
         return false;
      }

      return true;
   }
}

int main()
{
   bool result = true;

   result &= test_failed_compile_clears_parser_arena();
   result &= test_vector_view_refs_are_released_on_expression_destroy();

   return result ? 0 : 1;
}
