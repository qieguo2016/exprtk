#ifndef EXPRTK_SRC_COMMON_HPP
#define EXPRTK_SRC_COMMON_HPP

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace exprtk
{
   #ifdef exprtk_enable_debugging
     #define exprtk_debug(params) printf params
   #else
     #define exprtk_debug(params) (void)0
   #endif

   #define exprtk_error_location             \
   "exprtk.hpp:" + details::to_str(__LINE__) \

   #if __cplusplus >= 201103L
      #define exprtk_override override
      #define exprtk_final    final
      #define exprtk_delete   = delete
   #else
      #define exprtk_override
      #define exprtk_final
      #define exprtk_delete
   #endif

   #if __cplusplus >= 201603L
      #define exprtk_fallthrough [[fallthrough]];
   #elif (__cplusplus >= 201103L) && (defined(__GNUC__) && !defined(__clang__))
      #define exprtk_fallthrough [[gnu::fallthrough]];
   #else
      #ifndef _MSC_VER
         #define exprtk_fallthrough __attribute__ ((fallthrough));
      #else
         #define exprtk_fallthrough
      #endif
   #endif

   namespace details
   {
      typedef char                   char_t;
      typedef char_t*                char_ptr;
      typedef char_t const*          char_cptr;
      typedef unsigned char          uchar_t;
      typedef uchar_t*               uchar_ptr;
      typedef uchar_t const*         uchar_cptr;
      typedef unsigned long long int _uint64_t;
      typedef long long int          _int64_t;

      inline bool is_whitespace(const char_t c)
      {
         return (' '  == c) || ('\n' == c) ||
                ('\r' == c) || ('\t' == c) ||
                ('\b' == c) || ('\v' == c) ||
                ('\f' == c) ;
      }

      inline bool is_operator_char(const char_t c)
      {
         return ('+' == c) || ('-' == c) ||
                ('*' == c) || ('/' == c) ||
                ('^' == c) || ('<' == c) ||
                ('>' == c) || ('=' == c) ||
                (',' == c) || ('!' == c) ||
                ('(' == c) || (')' == c) ||
                ('[' == c) || (']' == c) ||
                ('{' == c) || ('}' == c) ||
                ('%' == c) || (':' == c) ||
                ('?' == c) || ('&' == c) ||
                ('|' == c) || (';' == c) ;
      }

      inline bool is_letter(const char_t c)
      {
         return (('a' <= c) && (c <= 'z')) ||
                (('A' <= c) && (c <= 'Z')) ;
      }

      inline bool is_digit(const char_t c)
      {
         return ('0' <= c) && (c <= '9');
      }

      inline bool is_letter_or_digit(const char_t c)
      {
         return is_letter(c) || is_digit(c);
      }

      inline bool is_left_bracket(const char_t c)
      {
         return ('(' == c) || ('[' == c) || ('{' == c);
      }

      inline bool is_right_bracket(const char_t c)
      {
         return (')' == c) || (']' == c) || ('}' == c);
      }

      inline bool is_bracket(const char_t c)
      {
         return is_left_bracket(c) || is_right_bracket(c);
      }

      inline bool is_sign(const char_t c)
      {
         return ('+' == c) || ('-' == c);
      }

      inline bool is_invalid(const char_t c)
      {
         return !is_whitespace   (c) &&
                !is_operator_char(c) &&
                !is_letter       (c) &&
                !is_digit        (c) &&
                ('.'  != c)          &&
                ('_'  != c)          &&
                ('$'  != c)          &&
                ('~'  != c)          &&
                ('\'' != c);
      }

      inline bool is_valid_string_char(const char_t c)
      {
         return std::isprint(static_cast<uchar_t>(c)) ||
                is_whitespace(c);
      }

      #ifndef exprtk_disable_caseinsensitivity
      inline void case_normalise(std::string& s)
      {
         for (std::size_t i = 0; i < s.size(); ++i)
         {
            s[i] = static_cast<std::string::value_type>(std::tolower(s[i]));
         }
      }

      inline bool imatch(const char_t c1, const char_t c2)
      {
         return std::tolower(c1) == std::tolower(c2);
      }

      inline bool imatch(const std::string& s1, const std::string& s2)
      {
         if (s1.size() == s2.size())
         {
            for (std::size_t i = 0; i < s1.size(); ++i)
            {
               if (std::tolower(s1[i]) != std::tolower(s2[i]))
               {
                  return false;
               }
            }

            return true;
         }

         return false;
      }

      struct ilesscompare
      {
         inline bool operator() (const std::string& s1, const std::string& s2) const
         {
            const std::size_t length = std::min(s1.size(),s2.size());

            for (std::size_t i = 0; i < length; ++i)
            {
               const char_t c1 = static_cast<char_t>(std::tolower(s1[i]));
               const char_t c2 = static_cast<char_t>(std::tolower(s2[i]));

               if (c1 < c2)
                  return true;
               else if (c2 < c1)
                  return false;
            }

            return s1.size() < s2.size();
         }
      };

      #else
      inline void case_normalise(std::string&)
      {}

      inline bool imatch(const char_t c1, const char_t c2)
      {
         return c1 == c2;
      }

      inline bool imatch(const std::string& s1, const std::string& s2)
      {
         return s1 == s2;
      }

      struct ilesscompare
      {
         inline bool operator() (const std::string& s1, const std::string& s2) const
         {
            return s1 < s2;
         }
      };
      #endif

      inline bool is_valid_sf_symbol(const std::string& symbol)
      {
         // Special function: $f12 or $F34
         return (4 == symbol.size())  &&
                ('$' == symbol[0])    &&
                imatch('f',symbol[1]) &&
                is_digit(symbol[2])   &&
                is_digit(symbol[3]);
      }

      inline const char_t& front(const std::string& s)
      {
         return s[0];
      }

      inline const char_t& back(const std::string& s)
      {
         return s[s.size() - 1];
      }

      template <typename SignedType>
      inline std::string to_str_impl(SignedType i)
      {
         if (0 == i)
            return std::string("0");

         std::string result;

         const int sign = (i < 0) ? -1 : 1;

         for ( ; i; i /= 10)
         {
            result += '0' + static_cast<char_t>(sign * (i % 10));
         }

         if (sign < 0)
         {
            result += '-';
         }

         std::reverse(result.begin(), result.end());

         return result;
      }

      inline std::string to_str(int i)
      {
         return to_str_impl(i);
      }

      inline std::string to_str(std::size_t i)
      {
         return to_str_impl(static_cast<long long int>(i));
      }

      inline bool is_hex_digit(const uchar_t digit)
      {
         return (('0' <= digit) && (digit <= '9')) ||
                (('A' <= digit) && (digit <= 'F')) ||
                (('a' <= digit) && (digit <= 'f')) ;
      }

      inline uchar_t hex_to_bin(uchar_t h)
      {
         if (('0' <= h) && (h <= '9'))
            return (h - '0');
         else
            return static_cast<uchar_t>(std::toupper(h) - 'A' + 10);
      }

      template <typename Iterator>
      inline bool parse_hex(Iterator& itr, Iterator end,
                            char_t& result)
      {
         if (
              (end ==  (itr    ))               ||
              (end ==  (itr + 1))               ||
              (end ==  (itr + 2))               ||
              (end ==  (itr + 3))               ||
              ('0' != *(itr    ))               ||
              ('X' != std::toupper(*(itr + 1))) ||
              (!is_hex_digit(*(itr + 2)))       ||
              (!is_hex_digit(*(itr + 3)))
            )
         {
            return false;
         }

         result = static_cast<char_t>(
                     hex_to_bin(static_cast<uchar_t>(*(itr + 2))) << 4 |
                     hex_to_bin(static_cast<uchar_t>(*(itr + 3)))) ;

         return true;
      }

      inline bool cleanup_escapes(std::string& s)
      {
         typedef std::string::iterator str_itr_t;

         str_itr_t itr1 = s.begin();
         str_itr_t itr2 = s.begin();
         str_itr_t end  = s.end  ();

         std::size_t removal_count  = 0;

         while (end != itr1)
         {
            if ('\\' == (*itr1))
            {
               if (end == ++itr1)
               {
                  return false;
               }
               else if (parse_hex(itr1, end, *itr2))
               {
                  itr1 += 4;
                  itr2 += 1;
                  removal_count += 4;
               }
               else if ('a' == (*itr1)) { (*itr2++) = '\a'; ++itr1; ++removal_count; }
               else if ('b' == (*itr1)) { (*itr2++) = '\b'; ++itr1; ++removal_count; }
               else if ('f' == (*itr1)) { (*itr2++) = '\f'; ++itr1; ++removal_count; }
               else if ('n' == (*itr1)) { (*itr2++) = '\n'; ++itr1; ++removal_count; }
               else if ('r' == (*itr1)) { (*itr2++) = '\r'; ++itr1; ++removal_count; }
               else if ('t' == (*itr1)) { (*itr2++) = '\t'; ++itr1; ++removal_count; }
               else if ('v' == (*itr1)) { (*itr2++) = '\v'; ++itr1; ++removal_count; }
               else if ('0' == (*itr1)) { (*itr2++) = '\0'; ++itr1; ++removal_count; }
               else
               {
                  (*itr2++) = (*itr1++);
                  ++removal_count;
               }

               continue;
            }
            else
               (*itr2++) = (*itr1++);
         }

         if ((removal_count > s.size()) || (0 == removal_count))
            return false;

         s.resize(s.size() - removal_count);

         return true;
      }

      class build_string
      {
      public:

         explicit build_string(const std::size_t& initial_size = 64)
         {
            data_.reserve(initial_size);
         }

         inline build_string& operator << (const std::string& s)
         {
            data_ += s;
            return (*this);
         }

         inline build_string& operator << (char_cptr s)
         {
            data_ += std::string(s);
            return (*this);
         }

         inline operator std::string () const
         {
            return data_;
         }

         inline std::string as_string() const
         {
            return data_;
         }

      private:

         std::string data_;
      };

      static const std::string reserved_words[] =
      {
         "assert",  "break", "case", "continue", "const",  "default",
         "false", "for", "if", "else", "ilike", "in", "like",  "and",
         "nand",  "nor",  "not",  "null",  "or",  "repeat", "return",
         "shl",  "shr",  "swap",  "switch",  "true",  "until", "var",
         "while", "xnor", "xor", "&", "|"
      };

      static const std::size_t reserved_words_size = sizeof(reserved_words) / sizeof(std::string);

      static const std::string reserved_symbols[] =
      {
         "abs", "acos",  "acosh", "and",  "asin", "asinh",  "assert",
         "atan", "atanh",  "atan2", "avg",  "break", "case",  "ceil",
         "clamp", "continue", "const",  "cos", "cosh", "cot",  "csc",
         "default",  "deg2grad", "deg2rad",  "equal", "erf",  "erfc",
         "exp", "expm1", "false", "floor", "for", "frac", "grad2deg",
         "hypot", "iclamp", "if",  "else", "ilike", "in",  "inrange",
         "like",  "log",  "log10", "log2",  "logn",  "log1p", "mand",
         "max", "min",  "mod", "mor",  "mul", "ncdf",  "nand", "nor",
         "not",   "not_equal",   "null",   "or",   "pow",  "rad2deg",
         "repeat", "return", "root", "round", "roundn", "sec", "sgn",
         "shl", "shr", "sin", "sinc", "sinh", "sqrt", "sum",  "swap",
         "switch", "tan",  "tanh", "true",  "trunc", "until",  "var",
         "while", "xnor", "xor", "&", "|"
      };

      static const std::size_t reserved_symbols_size = sizeof(reserved_symbols) / sizeof(std::string);

      static const std::string base_function_list[] =
      {
         "abs", "acos",  "acosh", "asin",  "asinh", "atan",  "atanh",
         "atan2",  "avg",  "ceil",  "clamp",  "cos",  "cosh",  "cot",
         "csc",  "equal",  "erf",  "erfc",  "exp",  "expm1", "floor",
         "frac", "hypot", "iclamp",  "like", "log", "log10",  "log2",
         "logn", "log1p", "mand", "max", "min", "mod", "mor",  "mul",
         "ncdf",  "pow",  "root",  "round",  "roundn",  "sec", "sgn",
         "sin", "sinc", "sinh", "sqrt", "sum", "swap", "tan", "tanh",
         "trunc",  "not_equal",  "inrange",  "deg2grad",   "deg2rad",
         "rad2deg", "grad2deg"
      };

      static const std::size_t base_function_list_size = sizeof(base_function_list) / sizeof(std::string);

      static const std::string logic_ops_list[] =
      {
         "and", "nand", "nor", "not", "or",  "xnor", "xor", "&", "|"
      };

      static const std::size_t logic_ops_list_size = sizeof(logic_ops_list) / sizeof(std::string);

      static const std::string cntrl_struct_list[] =
      {
         "if", "switch", "for", "while", "repeat", "return"
      };

      static const std::size_t cntrl_struct_list_size = sizeof(cntrl_struct_list) / sizeof(std::string);

      static const std::string arithmetic_ops_list[] =
      {
         "+", "-", "*", "/", "%", "^"
      };

      static const std::size_t arithmetic_ops_list_size = sizeof(arithmetic_ops_list) / sizeof(std::string);

      static const std::string assignment_ops_list[] =
      {
         ":=", "+=", "-=",
         "*=", "/=", "%="
      };

      static const std::size_t assignment_ops_list_size = sizeof(assignment_ops_list) / sizeof(std::string);

      static const std::string inequality_ops_list[] =
      {
         "<",  "<=", "==",
         "=",  "!=", "<>",
         ">=",  ">"
      };

      static const std::size_t inequality_ops_list_size = sizeof(inequality_ops_list) / sizeof(std::string);

      inline bool is_reserved_word(const std::string& symbol)
      {
         for (std::size_t i = 0; i < reserved_words_size; ++i)
         {
            if (imatch(symbol, reserved_words[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_reserved_symbol(const std::string& symbol)
      {
         for (std::size_t i = 0; i < reserved_symbols_size; ++i)
         {
            if (imatch(symbol, reserved_symbols[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_base_function(const std::string& function_name)
      {
         for (std::size_t i = 0; i < base_function_list_size; ++i)
         {
            if (imatch(function_name, base_function_list[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_control_struct(const std::string& cntrl_strct)
      {
         for (std::size_t i = 0; i < cntrl_struct_list_size; ++i)
         {
            if (imatch(cntrl_strct, cntrl_struct_list[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_logic_opr(const std::string& lgc_opr)
      {
         for (std::size_t i = 0; i < logic_ops_list_size; ++i)
         {
            if (imatch(lgc_opr, logic_ops_list[i]))
            {
               return true;
            }
         }

         return false;
      }

      struct cs_match
      {
         static inline bool cmp(const char_t c0, const char_t c1)
         {
            return (c0 == c1);
         }
      };

      struct cis_match
      {
         static inline bool cmp(const char_t c0, const char_t c1)
         {
            return (std::tolower(c0) == std::tolower(c1));
         }
      };

      template <typename Iterator, typename Compare>
      inline bool match_impl(const Iterator pattern_begin,
                             const Iterator pattern_end  ,
                             const Iterator data_begin   ,
                             const Iterator data_end     ,
                             const typename std::iterator_traits<Iterator>::value_type& zero_or_more,
                             const typename std::iterator_traits<Iterator>::value_type& exactly_one )
      {
         typedef typename std::iterator_traits<Iterator>::value_type type;

         const Iterator null_itr(0);

         Iterator p_itr  = pattern_begin;
         Iterator d_itr  = data_begin;
         Iterator np_itr = null_itr;
         Iterator nd_itr = null_itr;

         for ( ; ; )
         {
            if (p_itr != pattern_end)
            {
               const type c = *(p_itr);

               if ((data_end != d_itr) && (Compare::cmp(c,*(d_itr)) || (exactly_one == c)))
               {
                  ++d_itr;
                  ++p_itr;
                  continue;
               }
               else if (zero_or_more == c)
               {
                  while ((pattern_end != p_itr) && (zero_or_more == *(p_itr)))
                  {
                     ++p_itr;
                  }

                  const type d = *(p_itr);

                  while ((data_end != d_itr) && !(Compare::cmp(d,*(d_itr)) || (exactly_one == d)))
                  {
                     ++d_itr;
                  }

                  // set backtrack iterators
                  np_itr = p_itr - 1;
                  nd_itr = d_itr + 1;

                  continue;
               }
            }
            else if (data_end == d_itr)
               break;

            if ((data_end == d_itr) || (null_itr == nd_itr))
                return false;

            p_itr = np_itr;
            d_itr = nd_itr;
         }

         return true;
      }

      inline bool wc_match(const std::string& wild_card,
                           const std::string& str)
      {
         return match_impl<char_cptr,cs_match>
                (
                   wild_card.data(),
                   wild_card.data() + wild_card.size(),
                   str.data(),
                   str.data() + str.size(),
                   '*', '?'
                );
      }

      inline bool wc_imatch(const std::string& wild_card,
                            const std::string& str)
      {
         return match_impl<char_cptr,cis_match>
                (
                   wild_card.data(),
                   wild_card.data() + wild_card.size(),
                   str.data(),
                   str.data() + str.size(),
                   '*', '?'
                );
      }

      inline bool sequence_match(const std::string& pattern,
                                 const std::string& str,
                                 std::size_t&       diff_index,
                                 char_t&            diff_value)
      {
         if (str.empty())
         {
            return ("Z" == pattern);
         }
         else if ('*' == pattern[0])
            return false;

         typedef std::string::const_iterator itr_t;

         itr_t p_itr = pattern.begin();
         itr_t s_itr = str    .begin();

         const itr_t p_end = pattern.end();
         const itr_t s_end = str    .end();

         while ((s_end != s_itr) && (p_end != p_itr))
         {
            if ('*' == (*p_itr))
            {
               const char_t target = static_cast<char_t>(std::toupper(*(p_itr - 1)));

               if ('*' == target)
               {
                  diff_index = static_cast<std::size_t>(std::distance(str.begin(),s_itr));
                  diff_value = static_cast<char_t>(std::toupper(*p_itr));

                  return false;
               }
               else
                  ++p_itr;

               while (s_itr != s_end)
               {
                  if (target != std::toupper(*s_itr))
                     break;
                  else
                     ++s_itr;
               }

               continue;
            }
            else if (
                      ('?' != *p_itr) &&
                      std::toupper(*p_itr) != std::toupper(*s_itr)
                    )
            {
               diff_index = static_cast<std::size_t>(std::distance(str.begin(),s_itr));
               diff_value = static_cast<char_t>(std::toupper(*p_itr));

               return false;
            }

            ++p_itr;
            ++s_itr;
         }

         return (
                  (s_end == s_itr) &&
                  (
                    (p_end ==  p_itr) ||
                    ('*'   == *p_itr)
                  )
                );
      }

      template<typename T>
      struct set_zero_value_impl
      {
         static inline void process(T* base_ptr, const std::size_t size)
         {
            const T zero = T(0);
            for (std::size_t i = 0; i < size; ++i)
            {
               base_ptr[i] = zero;
            }
         }
      };

      #define pod_set_zero_value(T)                                      \
      template <>                                                        \
      struct set_zero_value_impl<T>                                      \
      {                                                                  \
         static inline void process(T* base_ptr, const std::size_t size) \
         { std::memset(base_ptr, 0x00, size * sizeof(T)); }              \
      };                                                                 \

      pod_set_zero_value(float      )
      pod_set_zero_value(double     )
      pod_set_zero_value(long double)

      #ifdef pod_set_zero_value
      #undef pod_set_zero_value
      #endif

      template<typename T>
      inline void set_zero_value(T* data, const std::size_t size)
      {
         set_zero_value_impl<T>::process(data,size);
      }

      template<typename T>
      inline void set_zero_value(std::vector<T>& v)
      {
         set_zero_value(v.data(),v.size());
      }

      static const double pow10[] =
      {
         1.0,
         1.0E+001, 1.0E+002, 1.0E+003, 1.0E+004,
         1.0E+005, 1.0E+006, 1.0E+007, 1.0E+008,
         1.0E+009, 1.0E+010, 1.0E+011, 1.0E+012,
         1.0E+013, 1.0E+014, 1.0E+015, 1.0E+016
      };

      static const std::size_t pow10_size = sizeof(pow10) / sizeof(double);

      namespace numeric
      {
         namespace constant
         {
            static const double e       =  2.71828182845904523536028747135266249775724709369996;
            static const double pi      =  3.14159265358979323846264338327950288419716939937510;
            static const double pi_2    =  1.57079632679489661923132169163975144209858469968755;
            static const double pi_4    =  0.78539816339744830961566084581987572104929234984378;
            static const double pi_180  =  0.01745329251994329576923690768488612713442871888542;
            static const double _1_pi   =  0.31830988618379067153776752674502872406891929148091;
            static const double _2_pi   =  0.63661977236758134307553505349005744813783858296183;
            static const double _180_pi = 57.29577951308232087679815481410517033240547246656443;
            static const double log2    =  0.69314718055994530941723212145817656807550013436026;
            static const double sqrt2   =  1.41421356237309504880168872420969807856967187537695;
         }

         namespace details
         {
            struct unknown_type_tag { unknown_type_tag() {} };
            struct real_type_tag    { real_type_tag   () {} };
            struct int_type_tag     { int_type_tag    () {} };

            template <typename T>
            struct number_type
            {
               typedef unknown_type_tag type;
               number_type() {}
            };

            #define exprtk_register_real_type_tag(T)          \
            template <> struct number_type<T>                 \
            { typedef real_type_tag type; number_type() {} }; \

            #define exprtk_register_int_type_tag(T)           \
            template <> struct number_type<T>                 \
            { typedef int_type_tag type; number_type() {} };  \

            exprtk_register_real_type_tag(float      )
            exprtk_register_real_type_tag(double     )
            exprtk_register_real_type_tag(long double)

            exprtk_register_int_type_tag(short         )
            exprtk_register_int_type_tag(int           )
            exprtk_register_int_type_tag(_int64_t      )
            exprtk_register_int_type_tag(unsigned short)
            exprtk_register_int_type_tag(unsigned int  )
            exprtk_register_int_type_tag(_uint64_t     )

            #undef exprtk_register_real_type_tag
            #undef exprtk_register_int_type_tag

            template <typename T>
            struct epsilon_type {};

            #define exprtk_define_epsilon_type(Type, Epsilon)      \
            template <> struct epsilon_type<Type>                  \
            {                                                      \
               static inline Type value()                          \
               {                                                   \
                  const Type epsilon = static_cast<Type>(Epsilon); \
                  return epsilon;                                  \
               }                                                   \
            };                                                     \

            exprtk_define_epsilon_type(float      , 0.00000100000f)
            exprtk_define_epsilon_type(double     , 0.000000000100)
            exprtk_define_epsilon_type(long double, 0.000000000001)

            #undef exprtk_define_epsilon_type

            template <typename T>
            inline bool is_nan_impl(const T v, real_type_tag)
            {
               return std::not_equal_to<T>()(v,v);
            }

            template <typename T>
            inline int to_int32_impl(const T v, real_type_tag)
            {
               return static_cast<int>(v);
            }

            template <typename T>
            inline _int64_t to_int64_impl(const T v, real_type_tag)
            {
               return static_cast<_int64_t>(v);
            }

            template <typename T>
            inline _uint64_t to_uint64_impl(const T v, real_type_tag)
            {
               return static_cast<_uint64_t>(v);
            }

            template <typename T>
            inline bool is_true_impl(const T v)
            {
               return std::not_equal_to<T>()(T(0),v);
            }

            template <typename T>
            inline bool is_false_impl(const T v)
            {
               return std::equal_to<T>()(T(0),v);
            }

            template <typename T>
            inline T abs_impl(const T v, real_type_tag)
            {
               return ((v < T(0)) ? -v : v);
            }

            template <typename T>
            inline T min_impl(const T v0, const T v1, real_type_tag)
            {
               return std::min<T>(v0,v1);
            }

            template <typename T>
            inline T max_impl(const T v0, const T v1, real_type_tag)
            {
               return std::max<T>(v0,v1);
            }

            template <typename T>
            inline T equal_impl(const T v0, const T v1, real_type_tag)
            {
               const T epsilon = epsilon_type<T>::value();
               return (abs_impl(v0 - v1,real_type_tag()) <= (std::max(T(1),std::max(abs_impl(v0,real_type_tag()),abs_impl(v1,real_type_tag()))) * epsilon)) ? T(1) : T(0);
            }

            inline float equal_impl(const float v0, const float v1, real_type_tag)
            {
               const float epsilon = epsilon_type<float>::value();
               return (abs_impl(v0 - v1,real_type_tag()) <= (std::max(1.0f,std::max(abs_impl(v0,real_type_tag()),abs_impl(v1,real_type_tag()))) * epsilon)) ? 1.0f : 0.0f;
            }

            template <typename T>
            inline T equal_impl(const T v0, const T v1, int_type_tag)
            {
               return (v0 == v1) ? 1 : 0;
            }

            template <typename T>
            inline T nequal_impl(const T v0, const T v1, real_type_tag)
            {
               typedef real_type_tag rtg;
               const T epsilon = epsilon_type<T>::value();
               return (abs_impl(v0 - v1,rtg()) > (std::max(T(1),std::max(abs_impl(v0,rtg()),abs_impl(v1,rtg()))) * epsilon)) ? T(1) : T(0);
            }

            inline float nequal_impl(const float v0, const float v1, real_type_tag)
            {
               typedef real_type_tag rtg;
               const float epsilon = epsilon_type<float>::value();
               return (abs_impl(v0 - v1,rtg()) > (std::max(1.0f,std::max(abs_impl(v0,rtg()),abs_impl(v1,rtg()))) * epsilon)) ? 1.0f : 0.0f;
            }

            template <typename T>
            inline T nequal_impl(const T v0, const T v1, int_type_tag)
            {
               return (v0 != v1) ? 1 : 0;
            }

            template <typename T>
            inline T modulus_impl(const T v0, const T v1, real_type_tag)
            {
               return std::fmod(v0,v1);
            }

            template <typename T>
            inline T modulus_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 % v1;
            }

            template <typename T>
            inline T pow_impl(const T v0, const T v1, real_type_tag)
            {
               return std::pow(v0,v1);
            }

            template <typename T>
            inline T pow_impl(const T v0, const T v1, int_type_tag)
            {
               return std::pow(static_cast<double>(v0),static_cast<double>(v1));
            }

            template <typename T>
            inline T logn_impl(const T v0, const T v1, real_type_tag)
            {
               return std::log(v0) / std::log(v1);
            }

            template <typename T>
            inline T logn_impl(const T v0, const T v1, int_type_tag)
            {
               return static_cast<T>(logn_impl<double>(static_cast<double>(v0),static_cast<double>(v1),real_type_tag()));
            }

            template <typename T>
            inline T root_impl(const T v0, const T v1, real_type_tag)
            {
               if (v0 < T(0))
               {
                  return (v1 == trunc_impl(v1, real_type_tag())) &&
                         (modulus_impl(v1, T(2), real_type_tag()) != T(0)) ?
                         -std::pow(abs_impl(v0, real_type_tag()), T(1) / v1) :
                          std::numeric_limits<T>::quiet_NaN();
               }

               return std::pow(v0, T(1) / v1);
            }

            template <typename T>
            inline T root_impl(const T v0, const T v1, int_type_tag)
            {
               return root_impl<double>(static_cast<double>(v0),static_cast<double>(v1),real_type_tag());
            }

            template <typename T>
            inline T round_impl(const T v, real_type_tag)
            {
               return ((v < T(0)) ? std::ceil(v - T(0.5)) : std::floor(v + T(0.5)));
            }

            template <typename T>
            inline T roundn_impl(const T v0, const T v1, real_type_tag)
            {
               const int index = std::max<int>(0, std::min<int>(pow10_size - 1, static_cast<int>(std::floor(v1))));
               const T p10 = T(pow10[index]);

               if (v0 < T(0))
                  return T(std::ceil ((v0 * p10) - T(0.5)) / p10);
               else
                  return T(std::floor((v0 * p10) + T(0.5)) / p10);
            }

            template <typename T>
            inline T roundn_impl(const T v0, const T, int_type_tag)
            {
               return v0;
            }

            template <typename T>
            inline T hypot_impl(const T v0, const T v1, real_type_tag)
            {
               return std::sqrt((v0 * v0) + (v1 * v1));
            }

            template <typename T>
            inline T hypot_impl(const T v0, const T v1, int_type_tag)
            {
               return static_cast<T>(std::sqrt(static_cast<double>((v0 * v0) + (v1 * v1))));
            }

            template <typename T>
            inline T atan2_impl(const T v0, const T v1, real_type_tag)
            {
               return std::atan2(v0,v1);
            }

            template <typename T>
            inline T atan2_impl(const T, const T, int_type_tag)
            {
               return 0;
            }

            template <typename T>
            inline T shr_impl(const T v0, const T v1, real_type_tag)
            {
               return v0 * (T(1) / std::pow(T(2),static_cast<T>(static_cast<int>(v1))));
            }

            template <typename T>
            inline T shr_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 >> v1;
            }

            template <typename T>
            inline T shl_impl(const T v0, const T v1, real_type_tag)
            {
               return v0 * std::pow(T(2),static_cast<T>(static_cast<int>(v1)));
            }

            template <typename T>
            inline T shl_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 << v1;
            }

            template <typename T>
            inline T sgn_impl(const T v, real_type_tag)
            {
               if      (v > T(0)) return T(+1);
               else if (v < T(0)) return T(-1);
               else               return T( 0);
            }

            template <typename T>
            inline T sgn_impl(const T v, int_type_tag)
            {
               if      (v > T(0)) return T(+1);
               else if (v < T(0)) return T(-1);
               else               return T( 0);
            }

            template <typename T>
            inline T and_impl(const T v0, const T v1, real_type_tag)
            {
               return (is_true_impl(v0) && is_true_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T and_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 && v1;
            }

            template <typename T>
            inline T nand_impl(const T v0, const T v1, real_type_tag)
            {
               return (is_false_impl(v0) || is_false_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T nand_impl(const T v0, const T v1, int_type_tag)
            {
               return !(v0 && v1);
            }

            template <typename T>
            inline T or_impl(const T v0, const T v1, real_type_tag)
            {
               return (is_true_impl(v0) || is_true_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T or_impl(const T v0, const T v1, int_type_tag)
            {
               return (v0 || v1);
            }

            template <typename T>
            inline T nor_impl(const T v0, const T v1, real_type_tag)
            {
               return (is_false_impl(v0) && is_false_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T nor_impl(const T v0, const T v1, int_type_tag)
            {
               return !(v0 || v1);
            }

            template <typename T>
            inline T xor_impl(const T v0, const T v1, real_type_tag)
            {
               return (is_false_impl(v0) != is_false_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T xor_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 ^ v1;
            }

            template <typename T>
            inline T xnor_impl(const T v0, const T v1, real_type_tag)
            {
               const bool v0_true = is_true_impl(v0);
               const bool v1_true = is_true_impl(v1);

               if ((v0_true &&  v1_true) || (!v0_true && !v1_true))
                  return T(1);
               else
                  return T(0);
            }

            template <typename T>
            inline T xnor_impl(const T v0, const T v1, int_type_tag)
            {
               const bool v0_true = is_true_impl(v0);
               const bool v1_true = is_true_impl(v1);

               if ((v0_true &&  v1_true) || (!v0_true && !v1_true))
                  return T(1);
               else
                  return T(0);
            }

            #if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || !defined(_MSC_VER)
            #define exprtk_define_erf(TT, impl)                \
            inline TT erf_impl(const TT v) { return impl(v); } \

            exprtk_define_erf(float      , ::erff)
            exprtk_define_erf(double     , ::erf )
            exprtk_define_erf(long double, ::erfl)
            #undef exprtk_define_erf
            #endif

            template <typename T>
            inline T erf_impl(const T v, real_type_tag)
            {
               #if defined(_MSC_VER) && (_MSC_VER < 1900)
               // Credits: Abramowitz & Stegun Equations 7.1.25-28
               static const T c[] =
               {
                  T( 1.26551223), T(1.00002368),
                  T( 0.37409196), T(0.09678418),
                  T(-0.18628806), T(0.27886807),
                  T(-1.13520398), T(1.48851587),
                  T(-0.82215223), T(0.17087277)
               };

               const T t = T(1) / (T(1) + T(0.5) * abs_impl(v,real_type_tag()));

               const T result = T(1) - t * std::exp((-v * v) -
                                            c[0] + t * (c[1] + t *
                                           (c[2] + t * (c[3] + t *
                                           (c[4] + t * (c[5] + t *
                                           (c[6] + t * (c[7] + t *
                                           (c[8] + t * (c[9]))))))))));

               return (v >= T(0)) ? result : -result;
               #else
               return erf_impl(v);
               #endif
            }

            template <typename T>
            inline T erf_impl(const T v, int_type_tag)
            {
               return erf_impl(static_cast<double>(v),real_type_tag());
            }

            #if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || !defined(_MSC_VER)
            #define exprtk_define_erfc(TT, impl)                \
            inline TT erfc_impl(const TT v) { return impl(v); } \

            exprtk_define_erfc(float      ,::erfcf)
            exprtk_define_erfc(double     ,::erfc )
            exprtk_define_erfc(long double,::erfcl)
            #undef exprtk_define_erfc
            #endif

            template <typename T>
            inline T erfc_impl(const T v, real_type_tag)
            {
               #if defined(_MSC_VER) && (_MSC_VER < 1900)
               return T(1) - erf_impl(v,real_type_tag());
               #else
               return erfc_impl(v);
               #endif
            }

            template <typename T>
            inline T erfc_impl(const T v, int_type_tag)
            {
               return erfc_impl(static_cast<double>(v),real_type_tag());
            }

            template <typename T>
            inline T ncdf_impl(const T v, real_type_tag)
            {
               return T(0.5) * erfc_impl(-(v / T(numeric::constant::sqrt2)),real_type_tag());
            }

            template <typename T>
            inline T ncdf_impl(const T v, int_type_tag)
            {
               return ncdf_impl(static_cast<double>(v),real_type_tag());
            }

            template <typename T>
            inline T sinc_impl(const T v, real_type_tag)
            {
               if (std::abs(v) >= std::numeric_limits<T>::epsilon())
                   return(std::sin(v) / v);
               else
                  return T(1);
            }

            template <typename T>
            inline T sinc_impl(const T v, int_type_tag)
            {
               return sinc_impl(static_cast<double>(v),real_type_tag());
            }

            #if __cplusplus >= 201103L
            template <typename T>
            inline T acosh_impl(const T v, real_type_tag)
            {
               return std::acosh(v);
            }

            template <typename T>
            inline T asinh_impl(const T v, real_type_tag)
            {
               return std::asinh(v);
            }

            template <typename T>
            inline T atanh_impl(const T v, real_type_tag)
            {
               return std::atanh(v);
            }

            template <typename T>
            inline T trunc_impl(const T v, real_type_tag)
            {
               return std::trunc(v);
            }

            template <typename T>
            inline T expm1_impl(const T v, real_type_tag)
            {
               return std::expm1(v);
            }

            template <typename T>
            inline T expm1_impl(const T v, int_type_tag)
            {
               return std::expm1(v);
            }

            template <typename T>
            inline T log1p_impl(const T v, real_type_tag)
            {
               return std::log1p(v);
            }

            template <typename T>
            inline T log1p_impl(const T v, int_type_tag)
            {
               return std::log1p(v);
            }
            #else
            template <typename T>
            inline T acosh_impl(const T v, real_type_tag)
            {
               return std::log(v + std::sqrt((v * v) - T(1)));
            }

            template <typename T>
            inline T asinh_impl(const T v, real_type_tag)
            {
               return std::log(v + std::sqrt((v * v) + T(1)));
            }

            template <typename T>
            inline T atanh_impl(const T v, real_type_tag)
            {
               return (std::log(T(1) + v) - std::log(T(1) - v)) / T(2);
            }

            template <typename T>
            inline T trunc_impl(const T v, real_type_tag)
            {
               return T(static_cast<long long>(v));
            }

            template <typename T>
            inline T expm1_impl(const T v, real_type_tag)
            {
               if (abs_impl(v,real_type_tag()) < T(0.00001))
                  return v + (T(0.5) * v * v);
               else
                  return std::exp(v) - T(1);
            }

            template <typename T>
            inline T expm1_impl(const T v, int_type_tag)
            {
               return T(std::exp<double>(v)) - T(1);
            }

            template <typename T>
            inline T log1p_impl(const T v, real_type_tag)
            {
               if (v > T(-1))
               {
                  if (abs_impl(v,real_type_tag()) > T(0.0001))
                  {
                     return std::log(T(1) + v);
                  }
                  else
                     return (T(-0.5) * v + T(1)) * v;
               }

               return std::numeric_limits<T>::quiet_NaN();
            }

            template <typename T>
            inline T log1p_impl(const T v, int_type_tag)
            {
               if (v > T(-1))
               {
                  return std::log(T(1) + v);
               }

               return std::numeric_limits<T>::quiet_NaN();
            }
            #endif

            template <typename T> inline T  acos_impl(const T v, real_type_tag) { return std::acos (v); }
            template <typename T> inline T  asin_impl(const T v, real_type_tag) { return std::asin (v); }
            template <typename T> inline T  atan_impl(const T v, real_type_tag) { return std::atan (v); }
            template <typename T> inline T  ceil_impl(const T v, real_type_tag) { return std::ceil (v); }
            template <typename T> inline T   cos_impl(const T v, real_type_tag) { return std::cos  (v); }
            template <typename T> inline T  cosh_impl(const T v, real_type_tag) { return std::cosh (v); }
            template <typename T> inline T   exp_impl(const T v, real_type_tag) { return std::exp  (v); }
            template <typename T> inline T floor_impl(const T v, real_type_tag) { return std::floor(v); }
            template <typename T> inline T   log_impl(const T v, real_type_tag) { return std::log  (v); }
            template <typename T> inline T log10_impl(const T v, real_type_tag) { return std::log10(v); }
            template <typename T> inline T  log2_impl(const T v, real_type_tag) { return std::log(v)/T(numeric::constant::log2); }
            template <typename T> inline T   neg_impl(const T v, real_type_tag) { return -v;            }
            template <typename T> inline T   pos_impl(const T v, real_type_tag) { return +v;            }
            template <typename T> inline T   sin_impl(const T v, real_type_tag) { return std::sin  (v); }
            template <typename T> inline T  sinh_impl(const T v, real_type_tag) { return std::sinh (v); }
            template <typename T> inline T  sqrt_impl(const T v, real_type_tag) { return std::sqrt (v); }
            template <typename T> inline T   tan_impl(const T v, real_type_tag) { return std::tan  (v); }
            template <typename T> inline T  tanh_impl(const T v, real_type_tag) { return std::tanh (v); }
            template <typename T> inline T   cot_impl(const T v, real_type_tag) { return T(1) / std::tan(v); }
            template <typename T> inline T   sec_impl(const T v, real_type_tag) { return T(1) / std::cos(v); }
            template <typename T> inline T   csc_impl(const T v, real_type_tag) { return T(1) / std::sin(v); }
            template <typename T> inline T   r2d_impl(const T v, real_type_tag) { return (v * T(numeric::constant::_180_pi)); }
            template <typename T> inline T   d2r_impl(const T v, real_type_tag) { return (v * T(numeric::constant::pi_180));  }
            template <typename T> inline T   d2g_impl(const T v, real_type_tag) { return (v * T(10.0/9.0)); }
            template <typename T> inline T   g2d_impl(const T v, real_type_tag) { return (v * T(9.0/10.0)); }
            template <typename T> inline T  notl_impl(const T v, real_type_tag) { return (std::not_equal_to<T>()(T(0),v) ? T(0) : T(1)); }
            template <typename T> inline T  frac_impl(const T v, real_type_tag) { return (v - trunc_impl(v,real_type_tag())); }

            template <typename T> inline T   const_pi_impl(real_type_tag) { return T(numeric::constant::pi);            }
            template <typename T> inline T    const_e_impl(real_type_tag) { return T(numeric::constant::e);             }
            template <typename T> inline T const_qnan_impl(real_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }

            template <typename T> inline T   abs_impl(const T v, int_type_tag) { return ((v >= T(0)) ? v : -v); }
            template <typename T> inline T   exp_impl(const T v, int_type_tag) { return std::exp  (v); }
            template <typename T> inline T   log_impl(const T v, int_type_tag) { return std::log  (v); }
            template <typename T> inline T log10_impl(const T v, int_type_tag) { return std::log10(v); }
            template <typename T> inline T  log2_impl(const T v, int_type_tag) { return std::log(v)/T(numeric::constant::log2); }
            template <typename T> inline T   neg_impl(const T v, int_type_tag) { return -v;            }
            template <typename T> inline T   pos_impl(const T v, int_type_tag) { return +v;            }
            template <typename T> inline T  ceil_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T floor_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T round_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T  notl_impl(const T v, int_type_tag) { return !v;            }
            template <typename T> inline T  sqrt_impl(const T v, int_type_tag) { return std::sqrt (v); }
            template <typename T> inline T  frac_impl(const T  , int_type_tag) { return T(0);          }
            template <typename T> inline T trunc_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T  acos_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T acosh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  asin_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T asinh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  atan_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T atanh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   cos_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  cosh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   sin_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  sinh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   tan_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  tanh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   cot_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   sec_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   csc_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }

            template <typename T>
            inline bool is_integer_impl(const T& v, real_type_tag)
            {
               return std::equal_to<T>()(T(0),std::fmod(v,T(1)));
            }

            template <typename T>
            inline bool is_integer_impl(const T&, int_type_tag)
            {
               return true;
            }
         }

         template <typename Type>
         struct numeric_info { enum { length = 0, size = 32, bound_length = 0, min_exp = 0, max_exp = 0 }; };

         template <> struct numeric_info<int        > { enum { length = 10, size = 16, bound_length = 9 }; };
         template <> struct numeric_info<float      > { enum { min_exp =  -38, max_exp =  +38 }; };
         template <> struct numeric_info<double     > { enum { min_exp = -308, max_exp = +308 }; };
         template <> struct numeric_info<long double> { enum { min_exp = -308, max_exp = +308 }; };

         template <typename T>
         inline int to_int32(const T v)
         {
            const typename details::number_type<T>::type num_type;
            return to_int32_impl(v, num_type);
         }

         template <typename T>
         inline _int64_t to_int64(const T v)
         {
            const typename details::number_type<T>::type num_type;
            return to_int64_impl(v, num_type);
         }

         template <typename T>
         inline _uint64_t to_uint64(const T v)
         {
            const typename details::number_type<T>::type num_type;
            return to_uint64_impl(v, num_type);
         }

         template <typename T>
         inline bool is_nan(const T v)
         {
            const typename details::number_type<T>::type num_type;
            return is_nan_impl(v, num_type);
         }

         template <typename T>
         inline T min(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return min_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T max(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return max_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T equal(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return equal_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T nequal(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return nequal_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T modulus(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return modulus_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T pow(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return pow_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T logn(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return logn_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T root(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return root_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T roundn(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return roundn_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T hypot(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return hypot_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T atan2(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return atan2_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T shr(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return shr_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T shl(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return shl_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T and_opr(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return and_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T nand_opr(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return nand_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T or_opr(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return or_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T nor_opr(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return nor_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T xor_opr(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return xor_impl(v0, v1, num_type);
         }

         template <typename T>
         inline T xnor_opr(const T v0, const T v1)
         {
            const typename details::number_type<T>::type num_type;
            return xnor_impl(v0, v1, num_type);
         }

         template <typename T>
         inline bool is_integer(const T v)
         {
            const typename details::number_type<T>::type num_type;
            return is_integer_impl(v, num_type);
         }

         template <typename T, unsigned int N>
         struct fast_exp
         {
            static inline T result(T v)
            {
               unsigned int k = N;
               T l = T(1);

               while (k)
               {
                  if (1 == (k % 2))
                  {
                     l *= v;
                     --k;
                  }

                  v *= v;
                  k /= 2;
               }

               return l;
            }
         };

         template <typename T> struct fast_exp<T,10> { static inline T result(const T v) { T v_5 = fast_exp<T,5>::result(v); return v_5 * v_5; } };
         template <typename T> struct fast_exp<T, 9> { static inline T result(const T v) { return fast_exp<T,8>::result(v) * v; } };
         template <typename T> struct fast_exp<T, 8> { static inline T result(const T v) { T v_4 = fast_exp<T,4>::result(v); return v_4 * v_4; } };
         template <typename T> struct fast_exp<T, 7> { static inline T result(const T v) { return fast_exp<T,6>::result(v) * v; } };
         template <typename T> struct fast_exp<T, 6> { static inline T result(const T v) { T v_3 = fast_exp<T,3>::result(v); return v_3 * v_3; } };
         template <typename T> struct fast_exp<T, 5> { static inline T result(const T v) { return fast_exp<T,4>::result(v) * v; } };
         template <typename T> struct fast_exp<T, 4> { static inline T result(const T v) { T v_2 = v * v; return v_2 * v_2; } };
         template <typename T> struct fast_exp<T, 3> { static inline T result(const T v) { return v * v * v; } };
         template <typename T> struct fast_exp<T, 2> { static inline T result(const T v) { return v * v;     } };
         template <typename T> struct fast_exp<T, 1> { static inline T result(const T v) { return v;         } };
         template <typename T> struct fast_exp<T, 0> { static inline T result(const T  ) { return T(1);      } };

         #define exprtk_define_unary_function(FunctionName)        \
         template <typename T>                                     \
         inline T FunctionName (const T v)                         \
         {                                                         \
            const typename details::number_type<T>::type num_type; \
            return  FunctionName##_impl(v,num_type);               \
         }                                                         \

         exprtk_define_unary_function(abs  )
         exprtk_define_unary_function(acos )
         exprtk_define_unary_function(acosh)
         exprtk_define_unary_function(asin )
         exprtk_define_unary_function(asinh)
         exprtk_define_unary_function(atan )
         exprtk_define_unary_function(atanh)
         exprtk_define_unary_function(ceil )
         exprtk_define_unary_function(cos  )
         exprtk_define_unary_function(cosh )
         exprtk_define_unary_function(exp  )
         exprtk_define_unary_function(expm1)
         exprtk_define_unary_function(floor)
         exprtk_define_unary_function(log  )
         exprtk_define_unary_function(log10)
         exprtk_define_unary_function(log2 )
         exprtk_define_unary_function(log1p)
         exprtk_define_unary_function(neg  )
         exprtk_define_unary_function(pos  )
         exprtk_define_unary_function(round)
         exprtk_define_unary_function(sin  )
         exprtk_define_unary_function(sinc )
         exprtk_define_unary_function(sinh )
         exprtk_define_unary_function(sqrt )
         exprtk_define_unary_function(tan  )
         exprtk_define_unary_function(tanh )
         exprtk_define_unary_function(cot  )
         exprtk_define_unary_function(sec  )
         exprtk_define_unary_function(csc  )
         exprtk_define_unary_function(r2d  )
         exprtk_define_unary_function(d2r  )
         exprtk_define_unary_function(d2g  )
         exprtk_define_unary_function(g2d  )
         exprtk_define_unary_function(notl )
         exprtk_define_unary_function(sgn  )
         exprtk_define_unary_function(erf  )
         exprtk_define_unary_function(erfc )
         exprtk_define_unary_function(ncdf )
         exprtk_define_unary_function(frac )
         exprtk_define_unary_function(trunc)
         #undef exprtk_define_unary_function
      }

      template <typename T>
      inline T compute_pow10(T d, const int exponent)
      {
         static const double fract10[] =
         {
            0.0,
            1.0E+001, 1.0E+002, 1.0E+003, 1.0E+004, 1.0E+005, 1.0E+006, 1.0E+007, 1.0E+008, 1.0E+009, 1.0E+010,
            1.0E+011, 1.0E+012, 1.0E+013, 1.0E+014, 1.0E+015, 1.0E+016, 1.0E+017, 1.0E+018, 1.0E+019, 1.0E+020,
            1.0E+021, 1.0E+022, 1.0E+023, 1.0E+024, 1.0E+025, 1.0E+026, 1.0E+027, 1.0E+028, 1.0E+029, 1.0E+030,
            1.0E+031, 1.0E+032, 1.0E+033, 1.0E+034, 1.0E+035, 1.0E+036, 1.0E+037, 1.0E+038, 1.0E+039, 1.0E+040,
            1.0E+041, 1.0E+042, 1.0E+043, 1.0E+044, 1.0E+045, 1.0E+046, 1.0E+047, 1.0E+048, 1.0E+049, 1.0E+050,
            1.0E+051, 1.0E+052, 1.0E+053, 1.0E+054, 1.0E+055, 1.0E+056, 1.0E+057, 1.0E+058, 1.0E+059, 1.0E+060,
            1.0E+061, 1.0E+062, 1.0E+063, 1.0E+064, 1.0E+065, 1.0E+066, 1.0E+067, 1.0E+068, 1.0E+069, 1.0E+070,
            1.0E+071, 1.0E+072, 1.0E+073, 1.0E+074, 1.0E+075, 1.0E+076, 1.0E+077, 1.0E+078, 1.0E+079, 1.0E+080,
            1.0E+081, 1.0E+082, 1.0E+083, 1.0E+084, 1.0E+085, 1.0E+086, 1.0E+087, 1.0E+088, 1.0E+089, 1.0E+090,
            1.0E+091, 1.0E+092, 1.0E+093, 1.0E+094, 1.0E+095, 1.0E+096, 1.0E+097, 1.0E+098, 1.0E+099, 1.0E+100,
            1.0E+101, 1.0E+102, 1.0E+103, 1.0E+104, 1.0E+105, 1.0E+106, 1.0E+107, 1.0E+108, 1.0E+109, 1.0E+110,
            1.0E+111, 1.0E+112, 1.0E+113, 1.0E+114, 1.0E+115, 1.0E+116, 1.0E+117, 1.0E+118, 1.0E+119, 1.0E+120,
            1.0E+121, 1.0E+122, 1.0E+123, 1.0E+124, 1.0E+125, 1.0E+126, 1.0E+127, 1.0E+128, 1.0E+129, 1.0E+130,
            1.0E+131, 1.0E+132, 1.0E+133, 1.0E+134, 1.0E+135, 1.0E+136, 1.0E+137, 1.0E+138, 1.0E+139, 1.0E+140,
            1.0E+141, 1.0E+142, 1.0E+143, 1.0E+144, 1.0E+145, 1.0E+146, 1.0E+147, 1.0E+148, 1.0E+149, 1.0E+150,
            1.0E+151, 1.0E+152, 1.0E+153, 1.0E+154, 1.0E+155, 1.0E+156, 1.0E+157, 1.0E+158, 1.0E+159, 1.0E+160,
            1.0E+161, 1.0E+162, 1.0E+163, 1.0E+164, 1.0E+165, 1.0E+166, 1.0E+167, 1.0E+168, 1.0E+169, 1.0E+170,
            1.0E+171, 1.0E+172, 1.0E+173, 1.0E+174, 1.0E+175, 1.0E+176, 1.0E+177, 1.0E+178, 1.0E+179, 1.0E+180,
            1.0E+181, 1.0E+182, 1.0E+183, 1.0E+184, 1.0E+185, 1.0E+186, 1.0E+187, 1.0E+188, 1.0E+189, 1.0E+190,
            1.0E+191, 1.0E+192, 1.0E+193, 1.0E+194, 1.0E+195, 1.0E+196, 1.0E+197, 1.0E+198, 1.0E+199, 1.0E+200,
            1.0E+201, 1.0E+202, 1.0E+203, 1.0E+204, 1.0E+205, 1.0E+206, 1.0E+207, 1.0E+208, 1.0E+209, 1.0E+210,
            1.0E+211, 1.0E+212, 1.0E+213, 1.0E+214, 1.0E+215, 1.0E+216, 1.0E+217, 1.0E+218, 1.0E+219, 1.0E+220,
            1.0E+221, 1.0E+222, 1.0E+223, 1.0E+224, 1.0E+225, 1.0E+226, 1.0E+227, 1.0E+228, 1.0E+229, 1.0E+230,
            1.0E+231, 1.0E+232, 1.0E+233, 1.0E+234, 1.0E+235, 1.0E+236, 1.0E+237, 1.0E+238, 1.0E+239, 1.0E+240,
            1.0E+241, 1.0E+242, 1.0E+243, 1.0E+244, 1.0E+245, 1.0E+246, 1.0E+247, 1.0E+248, 1.0E+249, 1.0E+250,
            1.0E+251, 1.0E+252, 1.0E+253, 1.0E+254, 1.0E+255, 1.0E+256, 1.0E+257, 1.0E+258, 1.0E+259, 1.0E+260,
            1.0E+261, 1.0E+262, 1.0E+263, 1.0E+264, 1.0E+265, 1.0E+266, 1.0E+267, 1.0E+268, 1.0E+269, 1.0E+270,
            1.0E+271, 1.0E+272, 1.0E+273, 1.0E+274, 1.0E+275, 1.0E+276, 1.0E+277, 1.0E+278, 1.0E+279, 1.0E+280,
            1.0E+281, 1.0E+282, 1.0E+283, 1.0E+284, 1.0E+285, 1.0E+286, 1.0E+287, 1.0E+288, 1.0E+289, 1.0E+290,
            1.0E+291, 1.0E+292, 1.0E+293, 1.0E+294, 1.0E+295, 1.0E+296, 1.0E+297, 1.0E+298, 1.0E+299, 1.0E+300,
            1.0E+301, 1.0E+302, 1.0E+303, 1.0E+304, 1.0E+305, 1.0E+306, 1.0E+307, 1.0E+308
         };

         static const int fract10_size = static_cast<int>(sizeof(fract10) / sizeof(double));

         const int e = std::abs(exponent);

         if (exponent >= std::numeric_limits<T>::min_exponent10)
         {
            if (e < fract10_size)
            {
               if (exponent > 0)
                  return T(d * fract10[e]);
               else
                  return T(d / fract10[e]);
            }
            else
               return T(d * std::pow(10.0, 10.0 * exponent));
         }
         else
         {
                     d /= T(fract10[           -std::numeric_limits<T>::min_exponent10]);
            return T(d /    fract10[-exponent + std::numeric_limits<T>::min_exponent10]);
         }
      }

      template <typename Iterator, typename T>
      inline bool string_to_type_converter_impl_ref(Iterator& itr, const Iterator end, T& result)
      {
         if (itr == end)
            return false;

         const bool negative = ('-' == (*itr));

         if (negative || ('+' == (*itr)))
         {
            if (end == ++itr)
               return false;
         }

         static const uchar_t zero = static_cast<uchar_t>('0');

         while ((end != itr) && (zero == (*itr))) ++itr;

         bool return_result = true;
         unsigned int digit = 0;
         const std::size_t length = static_cast<std::size_t>(std::distance(itr,end));

         if (length <= 4)
         {
            switch (length)
            {
               #ifdef exprtk_use_lut

               #define exprtk_process_digit                          \
               if ((digit = details::digit_table[(int)*itr++]) < 10) \
                  result = result * 10 + (digit);                    \
               else                                                  \
               {                                                     \
                  return_result = false;                             \
                  break;                                             \
               }                                                     \
               exprtk_fallthrough                                    \

               #else

               #define exprtk_process_digit        \
               if ((digit = (*itr++ - zero)) < 10) \
                  result = result * T(10) + digit; \
               else                                \
               {                                   \
                  return_result = false;           \
                  break;                           \
               }                                   \
               exprtk_fallthrough                  \

               #endif

               case 4 : exprtk_process_digit
               case 3 : exprtk_process_digit
               case 2 : exprtk_process_digit
               case 1 : if ((digit = (*itr - zero)) >= 10)
                        {
                           digit = 0;
                           return_result = false;
                        }

               #undef exprtk_process_digit
            }
         }
         else
            return_result = false;

         if (length && return_result)
         {
            result = result * 10 + static_cast<T>(digit);
            ++itr;
         }

         result = negative ? -result : result;
         return return_result;
      }

      template <typename Iterator, typename T>
      static inline bool parse_nan(Iterator& itr, const Iterator end, T& t)
      {
         typedef typename std::iterator_traits<Iterator>::value_type type;

         static const std::size_t nan_length = 3;

         if (std::distance(itr,end) != static_cast<int>(nan_length))
            return false;

         if (static_cast<type>('n') == (*itr))
         {
            if (
                 (static_cast<type>('a') != *(itr + 1)) ||
                 (static_cast<type>('n') != *(itr + 2))
               )
            {
               return false;
            }
         }
         else if (
                   (static_cast<type>('A') != *(itr + 1)) ||
                   (static_cast<type>('N') != *(itr + 2))
                 )
         {
            return false;
         }

         t = std::numeric_limits<T>::quiet_NaN();

         return true;
      }

      template <typename Iterator, typename T>
      static inline bool parse_inf(Iterator& itr, const Iterator end, T& t, const bool negative)
      {
         static const char_t inf_uc[] = "INFINITY";
         static const char_t inf_lc[] = "infinity";
         static const std::size_t inf_length = 8;

         const std::size_t length = static_cast<std::size_t>(std::distance(itr,end));

         if ((3 != length) && (inf_length != length))
            return false;

         char_cptr inf_itr = ('i' == (*itr)) ? inf_lc : inf_uc;

         while (end != itr)
         {
            if (*inf_itr == static_cast<char_t>(*itr))
            {
               ++itr;
               ++inf_itr;
               continue;
            }
            else
               return false;
         }

         if (negative)
            t = -std::numeric_limits<T>::infinity();
         else
            t =  std::numeric_limits<T>::infinity();

         return true;
      }

      template <typename T>
      inline bool valid_exponent(const int exponent, numeric::details::real_type_tag)
      {
         using namespace details::numeric;
         return (numeric_info<T>::min_exp <= exponent) && (exponent <= numeric_info<T>::max_exp);
      }

      template <typename Iterator, typename T>
      inline bool string_to_real(Iterator& itr_external, const Iterator end, T& t, numeric::details::real_type_tag)
      {
         if (end == itr_external) return false;

         Iterator itr = itr_external;

         T d = T(0);

         const bool negative = ('-' == (*itr));

         if (negative || '+' == (*itr))
         {
            if (end == ++itr)
               return false;
         }

         bool instate = false;

         static const char_t zero = static_cast<uchar_t>('0');

         #define parse_digit_1(d)          \
         if ((digit = (*itr - zero)) < 10) \
            { d = d * T(10) + digit; }     \
         else                              \
            { break; }                     \
         if (end == ++itr) break;          \

         #define parse_digit_2(d)          \
         if ((digit = (*itr - zero)) < 10) \
            { d = d * T(10) + digit; }     \
         else                              \
            { break; }                     \
            ++itr;                         \

         if ('.' != (*itr))
         {
            const Iterator curr = itr;

            while ((end != itr) && (zero == (*itr))) ++itr;

            while (end != itr)
            {
               unsigned int digit;
               parse_digit_1(d)
               parse_digit_1(d)
               parse_digit_2(d)
            }

            if (curr != itr) instate = true;
         }

         int exponent = 0;

         if (end != itr)
         {
            if ('.' == (*itr))
            {
               const Iterator curr = ++itr;
               T tmp_d = T(0);

               while (end != itr)
               {
                  unsigned int digit;
                  parse_digit_1(tmp_d)
                  parse_digit_1(tmp_d)
                  parse_digit_2(tmp_d)
               }

               if (curr != itr)
               {
                  instate = true;

                  const int frac_exponent = static_cast<int>(-std::distance(curr, itr));

                  if (!valid_exponent<T>(frac_exponent, numeric::details::real_type_tag()))
                     return false;

                  d += compute_pow10(tmp_d, frac_exponent);
               }

               #undef parse_digit_1
               #undef parse_digit_2
            }

            if (end != itr)
            {
               typename std::iterator_traits<Iterator>::value_type c = (*itr);

               if (('e' == c) || ('E' == c))
               {
                  int exp = 0;

                  if (!details::string_to_type_converter_impl_ref(++itr, end, exp))
                  {
                     if (end == itr)
                        return false;
                     else
                        c = (*itr);
                  }

                  exponent += exp;
               }

               if (end != itr)
               {
                  if (('f' == c) || ('F' == c) || ('l' == c) || ('L' == c))
                     ++itr;
                  else if ('#' == c)
                  {
                     if (end == ++itr)
                        return false;

                     if (('I' <= (*itr)) && ((*itr) <= 'n'))
                     {
                        if (('i' == (*itr)) || ('I' == (*itr)))
                        {
                           return parse_inf(itr, end, t, negative);
                        }
                        else if (('n' == (*itr)) || ('N' == (*itr)))
                        {
                           return parse_nan(itr, end, t);
                        }
                        else
                           return false;
                     }
                     else
                        return false;
                  }
                  else if (('I' <= (*itr)) && ((*itr) <= 'n'))
                  {
                     if (('i' == (*itr)) || ('I' == (*itr)))
                     {
                        return parse_inf(itr, end, t, negative);
                     }
                     else if (('n' == (*itr)) || ('N' == (*itr)))
                     {
                        return parse_nan(itr, end, t);
                     }
                     else
                        return false;
                  }
                  else
                     return false;
               }
            }
         }

         if ((end != itr) || (!instate))
            return false;
         else if (!valid_exponent<T>(exponent, numeric::details::real_type_tag()))
            return false;
         else if (exponent)
            d = compute_pow10(d,exponent);

         t = static_cast<T>((negative) ? -d : d);
         return true;
      }

      template <typename T>
      inline bool string_to_real(const std::string& s, T& t)
      {
         const typename numeric::details::number_type<T>::type num_type;

         char_cptr begin = s.data();
         char_cptr end   = s.data() + s.size();

         return string_to_real(begin, end, t, num_type);
      }

      template <typename T>
      struct functor_t
      {
         /*
            Note: The following definitions for Type, may require tweaking
                  based on the compiler and target architecture. The benchmark
                  should provide enough information to make the right choice.
         */
         //typedef T Type;
         //typedef const T Type;
         typedef const T& Type;
         typedef       T& RefType;
         typedef T (*qfunc_t)(Type t0, Type t1, Type t2, Type t3);
         typedef T (*tfunc_t)(Type t0, Type t1, Type t2);
         typedef T (*bfunc_t)(Type t0, Type t1);
         typedef T (*ufunc_t)(Type t0);
      };

   } // namespace details

   struct loop_runtime_check
   {
      enum loop_types
      {
         e_invalid           = 0,
         e_for_loop          = 1,
         e_while_loop        = 2,
         e_repeat_until_loop = 4,
         e_all_loops         = 7
      };

      enum violation_type
      {
          e_unknown         = 0,
          e_iteration_count = 1,
          e_timeout         = 2
      };

      loop_types loop_set;

      loop_runtime_check()
      : loop_set(e_invalid)
      , max_loop_iterations(0)
      {}

      details::_uint64_t max_loop_iterations;

      struct violation_context
      {
         loop_types loop;
         violation_type violation;
         details::_uint64_t iteration_count;
      };

      virtual bool check()
      {
         return true;
      }

      virtual void handle_runtime_violation(const violation_context&)
      {
         throw std::runtime_error("ExprTk Loop runtime violation.");
      }

      virtual ~loop_runtime_check()
      {}
   };

   typedef loop_runtime_check* loop_runtime_check_ptr;

   struct vector_access_runtime_check
   {
      struct violation_context
      {
         void* base_ptr;
         void* end_ptr;
         void* access_ptr;
         std::size_t type_size;
      };

      virtual ~vector_access_runtime_check()
      {}

      virtual bool handle_runtime_violation(violation_context& /*context*/)
      {
         throw std::runtime_error("ExprTk runtime vector access violation.");
         #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
         return false;
         #endif
      }
   };

   typedef vector_access_runtime_check* vector_access_runtime_check_ptr;

   struct assert_check
   {
      struct assert_context
      {
         std::string condition;
         std::string message;
         std::string id;
         std::size_t offet;
      };

      virtual ~assert_check()
      {}

      virtual void handle_assert(const assert_context& /*context*/)
      {}
   };

   typedef assert_check* assert_check_ptr;

   struct compilation_check
   {
      struct compilation_context
      {
         std::string error_message;
      };

      virtual bool continue_compilation(compilation_context& /*context*/) = 0;

      virtual ~compilation_check()
      {}
   };

   typedef compilation_check* compilation_check_ptr;

} // namespace exprtk

#endif // EXPRTK_SRC_COMMON_HPP
