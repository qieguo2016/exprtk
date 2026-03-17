#ifndef EXPRTK_SRC_RTL_IO_HPP
#define EXPRTK_SRC_RTL_IO_HPP

#include "functions.hpp"
#include "symbol_table.hpp"

#ifndef exprtk_disable_rtl_io
namespace exprtk
{
   namespace rtl { namespace io { namespace details
   {
      template <typename T>
      inline void print_type(const std::string& fmt,
                             const T v,
                             exprtk::details::numeric::details::real_type_tag)
      {
         #if defined(__clang__)
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wformat-nonliteral"
         #elif defined(__GNUC__) || defined(__GNUG__)
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wformat-nonliteral"
         #elif defined(_MSC_VER)
         #endif

         printf(fmt.c_str(), v);

         #if defined(__clang__)
            #pragma clang diagnostic pop
         #elif defined(__GNUC__) || defined(__GNUG__)
            #pragma GCC diagnostic pop
         #elif defined(_MSC_VER)
         #endif
      }

      template <typename T>
      struct print_impl
      {
         typedef typename igeneric_function<T>::generic_type generic_type;
         typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;
         typedef typename generic_type::scalar_view scalar_t;
         typedef typename generic_type::vector_view vector_t;
         typedef typename generic_type::string_view string_t;
         typedef typename exprtk::details::numeric::details::number_type<T>::type num_type;

         static void process(const std::string& scalar_format, parameter_list_t parameters)
         {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
               generic_type& gt = parameters[i];

               switch (gt.type)
               {
                  case generic_type::e_scalar : print(scalar_format,scalar_t(gt));
                                                break;

                  case generic_type::e_vector : print(scalar_format,vector_t(gt));
                                                break;

                  case generic_type::e_string : print(string_t(gt));
                                                break;

                  default                     : continue;
               }
            }
         }

         static inline void print(const std::string& scalar_format, const scalar_t& s)
         {
            print_type(scalar_format,s(),num_type());
         }

         static inline void print(const std::string& scalar_format, const vector_t& v)
         {
            for (std::size_t i = 0; i < v.size(); ++i)
            {
               print_type(scalar_format,v[i],num_type());

               if ((i + 1) < v.size())
                  printf(" ");
            }
         }

         static inline void print(const string_t& s)
         {
            printf("%s",to_str(s).c_str());
         }
      };

   } // namespace exprtk::rtl::io::details

   template <typename T>
   struct print exprtk_final : public exprtk::igeneric_function<T>
   {
      typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;

      using exprtk::igeneric_function<T>::operator();

      explicit print(const std::string& scalar_format = "%10.5f")
      : scalar_format_(scalar_format)
      {
         exprtk::enable_zero_parameters(*this);
      }

      inline T operator() (parameter_list_t parameters) exprtk_override
      {
         details::print_impl<T>::process(scalar_format_,parameters);
         return T(0);
      }

      std::string scalar_format_;
   };

   template <typename T>
   struct println exprtk_final : public exprtk::igeneric_function<T>
   {
      typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;

      using exprtk::igeneric_function<T>::operator();

      explicit println(const std::string& scalar_format = "%10.5f")
      : scalar_format_(scalar_format)
      {
         exprtk::enable_zero_parameters(*this);
      }

      inline T operator() (parameter_list_t parameters) exprtk_override
      {
         details::print_impl<T>::process(scalar_format_,parameters);
         printf("\n");
         return T(0);
      }

      std::string scalar_format_;
   };

   template <typename T>
   struct package
   {
      print  <T> p;
      println<T> pl;

      bool register_package(exprtk::symbol_table<T>& symtab)
      {
         #define exprtk_register_function(FunctionName, FunctionType)             \
         if (!symtab.add_function(FunctionName,FunctionType))                     \
         {                                                                        \
            exprtk_debug((                                                        \
              "exprtk::rtl::io::register_package - Failed to add function: %s\n", \
              FunctionName));                                                     \
            return false;                                                         \
         }                                                                        \

         exprtk_register_function("print"  , p )
         exprtk_register_function("println", pl)
         #undef exprtk_register_function

         return true;
      }
   };

   } // namespace exprtk::rtl::io
   } // namespace exprtk::rtl
}    // namespace exprtk
#endif

#endif // EXPRTK_SRC_RTL_IO_HPP
