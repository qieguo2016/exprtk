#ifndef EXPRTK_SRC_INFO_HPP
#define EXPRTK_SRC_INFO_HPP

#include "common.hpp"

namespace exprtk
{
   namespace information
   {
      using ::exprtk::details::char_cptr;

      static char_cptr library = "Mathematical Expression Toolkit";
      static char_cptr version = "0.0.1";
      static char_cptr date    = "20260317";
      static char_cptr min_cpp = "199711L";

      static inline std::string data()
      {
         static const std::string info_str = std::string(library) +
                                             std::string(" v") + std::string(version) +
                                             std::string(" (") + date + std::string(")") +
                                             std::string(" (") + min_cpp + std::string(")");
         return info_str;
      }

   } // namespace information

   #ifdef exprtk_debug
   #undef exprtk_debug
   #endif

   #ifdef exprtk_error_location
   #undef exprtk_error_location
   #endif

   #ifdef exprtk_fallthrough
   #undef exprtk_fallthrough
   #endif

   #ifdef exprtk_override
   #undef exprtk_override
   #endif

   #ifdef exprtk_final
   #undef exprtk_final
   #endif

   #ifdef exprtk_delete
   #undef exprtk_delete
   #endif

} // namespace exprtk

#endif // EXPRTK_SRC_INFO_HPP
