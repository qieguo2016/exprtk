#ifndef EXPRTK_SRC_LEXER_HPP
#define EXPRTK_SRC_LEXER_HPP

#include "common.hpp"

namespace exprtk
{

   namespace lexer
   {
      struct token
      {
         enum token_type
         {
            e_none        =   0, e_error       =   1, e_err_symbol  =   2,
            e_err_number  =   3, e_err_string  =   4, e_err_sfunc   =   5,
            e_eof         =   6, e_number      =   7, e_symbol      =   8,
            e_string      =   9, e_assign      =  10, e_addass      =  11,
            e_subass      =  12, e_mulass      =  13, e_divass      =  14,
            e_modass      =  15, e_shr         =  16, e_shl         =  17,
            e_lte         =  18, e_ne          =  19, e_gte         =  20,
            e_swap        =  21, e_lt          = '<', e_gt          = '>',
            e_eq          = '=', e_rbracket    = ')', e_lbracket    = '(',
            e_rsqrbracket = ']', e_lsqrbracket = '[', e_rcrlbracket = '}',
            e_lcrlbracket = '{', e_comma       = ',', e_add         = '+',
            e_sub         = '-', e_div         = '/', e_mul         = '*',
            e_mod         = '%', e_pow         = '^', e_colon       = ':',
            e_ternary     = '?'
         };

         token()
         : type(e_none)
         , value("")
         , position(std::numeric_limits<std::size_t>::max())
         {}

         void clear()
         {
            type     = e_none;
            value    = "";
            position = std::numeric_limits<std::size_t>::max();
         }

         template <typename Iterator>
         inline token& set_operator(const token_type tt,
                                    const Iterator begin, const Iterator end,
                                    const Iterator base_begin = Iterator(0))
         {
            type = tt;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         template <typename Iterator>
         inline token& set_symbol(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_symbol;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         template <typename Iterator>
         inline token& set_numeric(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_number;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         template <typename Iterator>
         inline token& set_string(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_string;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         inline token& set_string(const std::string& s, const std::size_t p)
         {
            type     = e_string;
            value    = s;
            position = p;
            return (*this);
         }

         template <typename Iterator>
         inline token& set_error(const token_type et,
                                 const Iterator begin, const Iterator end,
                                 const Iterator base_begin = Iterator(0))
         {
            if (
                 (e_error      == et) ||
                 (e_err_symbol == et) ||
                 (e_err_number == et) ||
                 (e_err_string == et) ||
                 (e_err_sfunc  == et)
               )
            {
               type = et;
            }
            else
               type = e_error;

            value.assign(begin,end);

            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));

            return (*this);
         }

         static inline std::string to_str(const token_type t)
         {
            switch (t)
            {
               case e_none        : return "NONE";
               case e_error       : return "ERROR";
               case e_err_symbol  : return "ERROR_SYMBOL";
               case e_err_number  : return "ERROR_NUMBER";
               case e_err_string  : return "ERROR_STRING";
               case e_eof         : return "EOF";
               case e_number      : return "NUMBER";
               case e_symbol      : return "SYMBOL";
               case e_string      : return "STRING";
               case e_assign      : return ":=";
               case e_addass      : return "+=";
               case e_subass      : return "-=";
               case e_mulass      : return "*=";
               case e_divass      : return "/=";
               case e_modass      : return "%=";
               case e_shr         : return ">>";
               case e_shl         : return "<<";
               case e_lte         : return "<=";
               case e_ne          : return "!=";
               case e_gte         : return ">=";
               case e_lt          : return "<";
               case e_gt          : return ">";
               case e_eq          : return "=";
               case e_rbracket    : return ")";
               case e_lbracket    : return "(";
               case e_rsqrbracket : return "]";
               case e_lsqrbracket : return "[";
               case e_rcrlbracket : return "}";
               case e_lcrlbracket : return "{";
               case e_comma       : return ",";
               case e_add         : return "+";
               case e_sub         : return "-";
               case e_div         : return "/";
               case e_mul         : return "*";
               case e_mod         : return "%";
               case e_pow         : return "^";
               case e_colon       : return ":";
               case e_ternary     : return "?";
               case e_swap        : return "<=>";
               default            : return "UNKNOWN";
            }
         }

         static inline std::string seperator_to_str(const token_type t)
         {
            switch (t)
            {
               case e_comma : return ",";
               case e_colon : return ":";
               case e_eof   : return ";";
               default      : return "UNKNOWN";
            }

            #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
            return "UNKNOWN";
            #endif
         }

         inline bool is_error() const
         {
            return (e_error      == type) ||
                   (e_err_symbol == type) ||
                   (e_err_number == type) ||
                   (e_err_string == type) ||
                   (e_err_sfunc  == type) ;
         }

         token_type type;
         std::string value;
         std::size_t position;
      };

      class generator
      {
      public:

         typedef token token_t;
         typedef std::vector<token_t> token_list_t;
         typedef token_list_t::iterator token_list_itr_t;
         typedef details::char_t char_t;

         generator()
         : base_itr_(0)
         , s_itr_   (0)
         , s_end_   (0)
         {
            clear();
         }

         inline void clear()
         {
            base_itr_ = 0;
            s_itr_    = 0;
            s_end_    = 0;
            token_list_.clear();
            token_itr_ = token_list_.end();
            store_token_itr_ = token_list_.end();
         }

         inline bool process(const std::string& str)
         {
            base_itr_ = str.data();
            s_itr_    = str.data();
            s_end_    = str.data() + str.size();

            eof_token_.set_operator(token_t::e_eof, s_end_, s_end_, base_itr_);
            token_list_.clear();

            while (!is_end(s_itr_))
            {
               scan_token();

               if (!token_list_.empty() && token_list_.back().is_error())
                  return false;
            }

            return true;
         }

         inline bool empty() const
         {
            return token_list_.empty();
         }

         inline std::size_t size() const
         {
            return token_list_.size();
         }

         inline void begin()
         {
            token_itr_ = token_list_.begin();
            store_token_itr_ = token_list_.begin();
         }

         inline void store()
         {
            store_token_itr_ = token_itr_;
         }

         inline void restore()
         {
            token_itr_ = store_token_itr_;
         }

         inline token_t& next_token()
         {
            if (token_list_.end() != token_itr_)
            {
               return *token_itr_++;
            }
            else
               return eof_token_;
         }

         inline token_t& peek_next_token()
         {
            if (token_list_.end() != token_itr_)
            {
               return *token_itr_;
            }
            else
               return eof_token_;
         }

         inline token_t& operator[](const std::size_t& index)
         {
            if (index < token_list_.size())
            {
               return token_list_[index];
            }
            else
               return eof_token_;
         }

         inline token_t operator[](const std::size_t& index) const
         {
            if (index < token_list_.size())
            {
               return token_list_[index];
            }
            else
               return eof_token_;
         }

         inline bool finished() const
         {
            return (token_list_.end() == token_itr_);
         }

         inline void insert_front(token_t::token_type tk_type)
         {
            if (
                 !token_list_.empty() &&
                 (token_list_.end() != token_itr_)
               )
            {
               token_t t = *token_itr_;

               t.type     = tk_type;
               token_itr_ = token_list_.insert(token_itr_,t);
            }
         }

         inline std::string substr(const std::size_t& begin, const std::size_t& end) const
         {
            const details::char_cptr begin_itr = ((base_itr_ + begin) < s_end_) ? (base_itr_ + begin) : s_end_;
            const details::char_cptr end_itr   = ((base_itr_ + end  ) < s_end_) ? (base_itr_ + end  ) : s_end_;

            return std::string(begin_itr,end_itr);
         }

         inline std::string remaining() const
         {
            if (finished())
               return "";
            else if (token_list_.begin() != token_itr_)
               return std::string(base_itr_ + (token_itr_ - 1)->position, s_end_);
            else
               return std::string(base_itr_ + token_itr_->position, s_end_);
         }

      private:

         inline bool is_end(details::char_cptr itr) const
         {
            return (s_end_ == itr);
         }

         #ifndef exprtk_disable_comments
         inline bool is_comment_start(details::char_cptr itr) const
         {
            const char_t c0 = *(itr + 0);
            const char_t c1 = *(itr + 1);

            if ('#' == c0)
               return true;
            else if (!is_end(itr + 1))
            {
               if (('/' == c0) && ('/' == c1)) return true;
               if (('/' == c0) && ('*' == c1)) return true;
            }
            return false;
         }
         #else
         inline bool is_comment_start(details::char_cptr) const
         {
            return false;
         }
         #endif

         inline void skip_whitespace()
         {
            while (!is_end(s_itr_) && details::is_whitespace(*s_itr_))
            {
               ++s_itr_;
            }
         }

         inline void skip_comments()
         {
            #ifndef exprtk_disable_comments
            // The following comment styles are supported:
            // 1. // .... \n
            // 2. #  .... \n
            // 3. /* .... */
            struct test
            {
               static inline bool comment_start(const char_t c0, const char_t c1, int& mode, int& incr)
               {
                  mode = 0;
                  if      ('#' == c0)    { mode = 1; incr = 1; }
                  else if ('/' == c0)
                  {
                     if      ('/' == c1) { mode = 1; incr = 2; }
                     else if ('*' == c1) { mode = 2; incr = 2; }
                  }
                  return (0 != mode);
               }

               static inline bool comment_end(const char_t c0, const char_t c1, int& mode)
               {
                  if (
                       ((1 == mode) && ('\n' == c0)) ||
                       ((2 == mode) && ( '*' == c0) && ('/' == c1))
                     )
                  {
                     mode = 0;
                     return true;
                  }
                  else
                     return false;
               }
            };

            int mode      = 0;
            int increment = 0;

            if (is_end(s_itr_))
               return;
            else if (!test::comment_start(*s_itr_, *(s_itr_ + 1), mode, increment))
               return;

            details::char_cptr cmt_start = s_itr_;

            s_itr_ += increment;

            while (!is_end(s_itr_))
            {
               if (details::is_invalid(*s_itr_))
               {
                  token_t t;
                  t.set_error(token::e_error, s_itr_, s_itr_ + 1, base_itr_);
                  token_list_.push_back(t);
                  return;
               }

               if ((1 == mode) && test::comment_end(*s_itr_, 0, mode))
               {
                  ++s_itr_;
                  return;
               }

               if ((2 == mode))
               {
                  if (!is_end((s_itr_ + 1)) && test::comment_end(*s_itr_, *(s_itr_ + 1), mode))
                  {
                     s_itr_ += 2;
                     return;
                  }
               }

               ++s_itr_;
            }

            if (2 == mode)
            {
               token_t t;
               t.set_error(token::e_error, cmt_start, cmt_start + mode, base_itr_);
               token_list_.push_back(t);
            }
            #endif
         }

         inline bool next_is_digit(const details::char_cptr itr) const
         {
            return ((itr + 1) != s_end_) &&
                   details::is_digit(*(itr + 1));
         }

         inline void scan_token()
         {
            const char_t c = *s_itr_;

            if (details::is_whitespace(c))
            {
               skip_whitespace();
               return;
            }
            else if (is_comment_start(s_itr_))
            {
               skip_comments();
               return;
            }
            else if (details::is_operator_char(c))
            {
               scan_operator();
               return;
            }
            else if (details::is_letter(c))
            {
               scan_symbol();
               return;
            }
            else if (('.' == c) && !next_is_digit(s_itr_))
            {
               scan_operator();
               return;
            }
            else if (details::is_digit(c) || ('.' == c))
            {
               scan_number();
               return;
            }
            else if ('$' == c)
            {
               scan_special_function();
               return;
            }
            #ifndef exprtk_disable_string_capabilities
            else if ('\'' == c)
            {
               scan_string();
               return;
            }
            #endif
            else if ('~' == c)
            {
               token_t t;
               t.set_symbol(s_itr_, s_itr_ + 1, base_itr_);
               token_list_.push_back(t);
               ++s_itr_;
               return;
            }
            else
            {
               token_t t;
               t.set_error(token::e_error, s_itr_, s_itr_ + 2, base_itr_);
               token_list_.push_back(t);
               ++s_itr_;
            }
         }

         inline void scan_operator()
         {
            token_t t;

            const char_t c0 = s_itr_[0];

            if (!is_end(s_itr_ + 1))
            {
               const char_t c1 = s_itr_[1];

               if (!is_end(s_itr_ + 2))
               {
                  const char_t c2 = s_itr_[2];

                  if ((c0 == '<') && (c1 == '=') && (c2 == '>'))
                  {
                     t.set_operator(token_t::e_swap, s_itr_, s_itr_ + 3, base_itr_);
                     token_list_.push_back(t);
                     s_itr_ += 3;
                     return;
                  }
               }

               token_t::token_type ttype = token_t::e_none;

               if      ((c0 == '<') && (c1 == '=')) ttype = token_t::e_lte;
               else if ((c0 == '>') && (c1 == '=')) ttype = token_t::e_gte;
               else if ((c0 == '<') && (c1 == '>')) ttype = token_t::e_ne;
               else if ((c0 == '!') && (c1 == '=')) ttype = token_t::e_ne;
               else if ((c0 == '=') && (c1 == '=')) ttype = token_t::e_eq;
               else if ((c0 == ':') && (c1 == '=')) ttype = token_t::e_assign;
               else if ((c0 == '<') && (c1 == '<')) ttype = token_t::e_shl;
               else if ((c0 == '>') && (c1 == '>')) ttype = token_t::e_shr;
               else if ((c0 == '+') && (c1 == '=')) ttype = token_t::e_addass;
               else if ((c0 == '-') && (c1 == '=')) ttype = token_t::e_subass;
               else if ((c0 == '*') && (c1 == '=')) ttype = token_t::e_mulass;
               else if ((c0 == '/') && (c1 == '=')) ttype = token_t::e_divass;
               else if ((c0 == '%') && (c1 == '=')) ttype = token_t::e_modass;

               if (token_t::e_none != ttype)
               {
                  t.set_operator(ttype, s_itr_, s_itr_ + 2, base_itr_);
                  token_list_.push_back(t);
                  s_itr_ += 2;
                  return;
               }
            }

            if ('<' == c0)
               t.set_operator(token_t::e_lt , s_itr_, s_itr_ + 1, base_itr_);
            else if ('>' == c0)
               t.set_operator(token_t::e_gt , s_itr_, s_itr_ + 1, base_itr_);
            else if (';' == c0)
               t.set_operator(token_t::e_eof, s_itr_, s_itr_ + 1, base_itr_);
            else if ('&' == c0)
               t.set_symbol(s_itr_, s_itr_ + 1, base_itr_);
            else if ('|' == c0)
               t.set_symbol(s_itr_, s_itr_ + 1, base_itr_);
            else
               t.set_operator(token_t::token_type(c0), s_itr_, s_itr_ + 1, base_itr_);

            token_list_.push_back(t);
            ++s_itr_;
         }

         inline void scan_symbol()
         {
            details::char_cptr initial_itr = s_itr_;

            while (!is_end(s_itr_))
            {
               if (!details::is_letter_or_digit(*s_itr_) && ('_' != (*s_itr_)))
               {
                  if ('.' != (*s_itr_))
                     break;
                  /*
                     Permit symbols that contain a 'dot'
                     Allowed   : abc.xyz, a123.xyz, abc.123, abc_.xyz a123_.xyz abc._123
                     Disallowed: .abc, abc.<white-space>, abc.<eof>, abc.<operator +,-,*,/...>
                  */
                  if (
                       (s_itr_ != initial_itr)                     &&
                       !is_end(s_itr_ + 1)                         &&
                       !details::is_letter_or_digit(*(s_itr_ + 1)) &&
                       ('_' != (*(s_itr_ + 1)))
                     )
                     break;
               }

               ++s_itr_;
            }

            token_t t;
            t.set_symbol(initial_itr, s_itr_, base_itr_);
            token_list_.push_back(t);
         }

         inline void scan_number()
         {
            /*
               Attempt to match a valid numeric value in one of the following formats:
               (01) 123456
               (02) 123456.
               (03) 123.456
               (04) 123.456e3
               (05) 123.456E3
               (06) 123.456e+3
               (07) 123.456E+3
               (08) 123.456e-3
               (09) 123.456E-3
               (00) .1234
               (11) .1234e3
               (12) .1234E+3
               (13) .1234e+3
               (14) .1234E-3
               (15) .1234e-3
            */

            details::char_cptr initial_itr = s_itr_;
            bool dot_found                 = false;
            bool e_found                   = false;
            bool post_e_sign_found         = false;
            bool post_e_digit_found        = false;
            token_t t;

            while (!is_end(s_itr_))
            {
               if ('.' == (*s_itr_))
               {
                  if (dot_found)
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }

                  dot_found = true;
                  ++s_itr_;

                  continue;
               }
               else if ('e' == std::tolower(*s_itr_))
               {
                  const char_t& c = *(s_itr_ + 1);

                  if (is_end(s_itr_ + 1))
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }
                  else if (
                            ('+' != c) &&
                            ('-' != c) &&
                            !details::is_digit(c)
                          )
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }

                  e_found = true;
                  ++s_itr_;

                  continue;
               }
               else if (e_found && details::is_sign(*s_itr_) && !post_e_digit_found)
               {
                  if (post_e_sign_found)
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }

                  post_e_sign_found = true;
                  ++s_itr_;

                  continue;
               }
               else if (e_found && details::is_digit(*s_itr_))
               {
                  post_e_digit_found = true;
                  ++s_itr_;

                  continue;
               }
               else if (('.' != (*s_itr_)) && !details::is_digit(*s_itr_))
                  break;
               else
                  ++s_itr_;
            }

            t.set_numeric(initial_itr, s_itr_, base_itr_);
            token_list_.push_back(t);

            return;
         }

         inline void scan_special_function()
         {
            details::char_cptr initial_itr = s_itr_;
            token_t t;

            // $fdd(x,x,x) = at least 11 chars
            if (std::distance(s_itr_,s_end_) < 11)
            {
               t.set_error(
                  token::e_err_sfunc,
                  initial_itr, std::min(initial_itr + 11, s_end_),
                  base_itr_);
               token_list_.push_back(t);

               return;
            }

            if (
                 !(('$' == *s_itr_)                       &&
                   (details::imatch  ('f',*(s_itr_ + 1))) &&
                   (details::is_digit(*(s_itr_ + 2)))     &&
                   (details::is_digit(*(s_itr_ + 3))))
               )
            {
               t.set_error(
                  token::e_err_sfunc,
                  initial_itr, std::min(initial_itr + 4, s_end_),
                  base_itr_);
               token_list_.push_back(t);

               return;
            }

            s_itr_ += 4; // $fdd = 4chars

            t.set_symbol(initial_itr, s_itr_, base_itr_);
            token_list_.push_back(t);

            return;
         }

         #ifndef exprtk_disable_string_capabilities
         inline void scan_string()
         {
            details::char_cptr initial_itr = s_itr_ + 1;
            token_t t;

            if (std::distance(s_itr_,s_end_) < 2)
            {
               t.set_error(token::e_err_string, s_itr_, s_end_, base_itr_);
               token_list_.push_back(t);

               return;
            }

            ++s_itr_;

            bool escaped_found = false;
            bool escaped = false;

            while (!is_end(s_itr_))
            {
               if (!details::is_valid_string_char(*s_itr_))
               {
                  t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
                  token_list_.push_back(t);

                  return;
               }
               else if (!escaped && ('\\' == *s_itr_))
               {
                  escaped_found = true;
                  escaped = true;
                  ++s_itr_;

                  continue;
               }
               else if (!escaped)
               {
                  if ('\'' == *s_itr_)
                     break;
               }
               else if (escaped)
               {
                  if (
                       !is_end(s_itr_) && ('0' == *(s_itr_)) &&
                       ((s_itr_ + 4) <= s_end_)
                     )
                  {
                     const bool x_separator = ('X' == std::toupper(*(s_itr_ + 1)));

                     const bool both_digits = details::is_hex_digit(*(s_itr_ + 2)) &&
                                              details::is_hex_digit(*(s_itr_ + 3)) ;

                     if (!(x_separator && both_digits))
                     {
                        t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
                        token_list_.push_back(t);

                        return;
                     }
                     else
                        s_itr_ += 3;
                  }

                  escaped = false;
               }

               ++s_itr_;
            }

            if (is_end(s_itr_))
            {
               t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
               token_list_.push_back(t);

               return;
            }

            if (!escaped_found)
               t.set_string(initial_itr, s_itr_, base_itr_);
            else
            {
               std::string parsed_string(initial_itr,s_itr_);

               if (!details::cleanup_escapes(parsed_string))
               {
                  t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
                  token_list_.push_back(t);

                  return;
               }

               t.set_string(
                  parsed_string,
                  static_cast<std::size_t>(std::distance(base_itr_,initial_itr)));
            }

            token_list_.push_back(t);
            ++s_itr_;

            return;
         }
         #endif

      private:

         token_list_t       token_list_;
         token_list_itr_t   token_itr_;
         token_list_itr_t   store_token_itr_;
         token_t            eof_token_;
         details::char_cptr base_itr_;
         details::char_cptr s_itr_;
         details::char_cptr s_end_;

         friend class token_scanner;
         friend class token_modifier;
         friend class token_inserter;
         friend class token_joiner;
      }; // class generator

      class helper_interface
      {
      public:

         virtual void init()                     {              }
         virtual void reset()                    {              }
         virtual bool result()                   { return true; }
         virtual std::size_t process(generator&) { return 0;    }
         virtual ~helper_interface()             {              }
      };

      class token_scanner : public helper_interface
      {
      public:

         virtual ~token_scanner() exprtk_override
         {}

         explicit token_scanner(const std::size_t& stride)
         : stride_(stride)
         {
            if (stride > 4)
            {
               throw std::invalid_argument("token_scanner() - Invalid stride value");
            }
         }

         inline std::size_t process(generator& g) exprtk_override
         {
            if (g.token_list_.size() >= stride_)
            {
               for (std::size_t i = 0; i < (g.token_list_.size() - stride_ + 1); ++i)
               {
                  token t;

                  switch (stride_)
                  {
                     case 1 :
                              {
                                 const token& t0 = g.token_list_[i];

                                 if (!operator()(t0))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     case 2 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];

                                 if (!operator()(t0, t1))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     case 3 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];
                                 const token& t2 = g.token_list_[i + 2];

                                 if (!operator()(t0, t1, t2))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     case 4 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];
                                 const token& t2 = g.token_list_[i + 2];
                                 const token& t3 = g.token_list_[i + 3];

                                 if (!operator()(t0, t1, t2, t3))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     default: continue;
                  }
               }
            }

            return 0;
         }

         virtual bool operator() (const token&)
         {
            return false;
         }

         virtual bool operator() (const token&, const token&)
         {
            return false;
         }

         virtual bool operator() (const token&, const token&, const token&)
         {
            return false;
         }

         virtual bool operator() (const token&, const token&, const token&, const token&)
         {
            return false;
         }

      private:

         const std::size_t stride_;
      }; // class token_scanner

      class token_modifier : public helper_interface
      {
      public:

         inline std::size_t process(generator& g) exprtk_override
         {
            std::size_t changes = 0;

            for (std::size_t i = 0; i < g.token_list_.size(); ++i)
            {
               if (modify(g.token_list_[i])) changes++;
            }

            return changes;
         }

         virtual bool modify(token& t) = 0;
      };

      class token_inserter : public helper_interface
      {
      public:

         explicit token_inserter(const std::size_t& stride)
         : stride_(stride)
         {
            if (stride > 5)
            {
               throw std::invalid_argument("token_inserter() - Invalid stride value");
            }
         }

         inline std::size_t process(generator& g) exprtk_override
         {
            if (g.token_list_.empty())
               return 0;
            else if (g.token_list_.size() < stride_)
               return 0;

            std::size_t changes = 0;

            typedef std::pair<std::size_t, token> insert_t;
            std::vector<insert_t> insert_list;
            insert_list.reserve(10000);

            for (std::size_t i = 0; i < (g.token_list_.size() - stride_ + 1); ++i)
            {
               int insert_index = -1;
               token t;

               switch (stride_)
               {
                  case 1 : insert_index = insert(g.token_list_[i],t);
                           break;

                  case 2 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], t);
                           break;

                  case 3 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], g.token_list_[i + 2], t);
                           break;

                  case 4 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], g.token_list_[i + 2], g.token_list_[i + 3], t);
                           break;

                  case 5 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], g.token_list_[i + 2], g.token_list_[i + 3], g.token_list_[i + 4], t);
                           break;
               }

               if ((insert_index >= 0) && (insert_index <= (static_cast<int>(stride_) + 1)))
               {
                  insert_list.push_back(insert_t(i, t));
                  changes++;
               }
            }

            if (!insert_list.empty())
            {
               generator::token_list_t token_list;

               std::size_t insert_index = 0;

               for (std::size_t i = 0; i < g.token_list_.size(); ++i)
               {
                  token_list.push_back(g.token_list_[i]);

                  if (
                       (insert_index < insert_list.size()) &&
                       (insert_list[insert_index].first == i)
                     )
                  {
                     token_list.push_back(insert_list[insert_index].second);
                     insert_index++;
                  }
               }

               std::swap(g.token_list_,token_list);
            }

            return changes;
         }

         #define token_inserter_empty_body \
         {                                 \
            return -1;                     \
         }                                 \

         inline virtual int insert(const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, const token&, const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, const token&, const token&, const token&, token&)
         token_inserter_empty_body

         #undef token_inserter_empty_body

      private:

         const std::size_t stride_;
      };

      class token_joiner : public helper_interface
      {
      public:

         explicit token_joiner(const std::size_t& stride)
         : stride_(stride)
         {}

         inline std::size_t process(generator& g) exprtk_override
         {
            if (g.token_list_.empty())
               return 0;

            switch (stride_)
            {
               case 2  : return process_stride_2(g);
               case 3  : return process_stride_3(g);
               default : return 0;
            }
         }

         virtual bool join(const token&, const token&, token&)               { return false; }
         virtual bool join(const token&, const token&, const token&, token&) { return false; }

      private:

         inline std::size_t process_stride_2(generator& g)
         {
            if (g.token_list_.size() < 2)
               return 0;

            std::size_t changes = 0;

            generator::token_list_t token_list;
            token_list.reserve(10000);

            for (int i = 0; i < static_cast<int>(g.token_list_.size() - 1); ++i)
            {
               token t;

               for ( ; ; )
               {
                  if (!join(g[i], g[i + 1], t))
                  {
                     token_list.push_back(g[i]);
                     break;
                  }

                  token_list.push_back(t);

                  ++changes;

                  i += 2;

                  if (static_cast<std::size_t>(i) >= (g.token_list_.size() - 1))
                     break;
               }
            }

            token_list.push_back(g.token_list_.back());

            assert(token_list.size() <= g.token_list_.size());

            std::swap(token_list, g.token_list_);

            return changes;
         }

         inline std::size_t process_stride_3(generator& g)
         {
            if (g.token_list_.size() < 3)
               return 0;

            std::size_t changes = 0;

            generator::token_list_t token_list;
            token_list.reserve(10000);

            for (int i = 0; i < static_cast<int>(g.token_list_.size() - 2); ++i)
            {
               token t;

               for ( ; ; )
               {
                  if (!join(g[i], g[i + 1], g[i + 2], t))
                  {
                     token_list.push_back(g[i]);
                     break;
                  }

                  token_list.push_back(t);

                  ++changes;

                  i += 3;

                  if (static_cast<std::size_t>(i) >= (g.token_list_.size() - 2))
                     break;
               }
            }

            token_list.push_back(*(g.token_list_.begin() + g.token_list_.size() - 2));
            token_list.push_back(*(g.token_list_.begin() + g.token_list_.size() - 1));

            assert(token_list.size() <= g.token_list_.size());

            std::swap(token_list, g.token_list_);

            return changes;
         }

         const std::size_t stride_;
      };

      namespace helper
      {

         inline void dump(const lexer::generator& generator)
         {
            for (std::size_t i = 0; i < generator.size(); ++i)
            {
               const lexer::token& t = generator[i];
               printf("Token[%02d] @ %03d  %6s  -->  '%s'\n",
                      static_cast<int>(i),
                      static_cast<int>(t.position),
                      t.to_str(t.type).c_str(),
                      t.value.c_str());
            }
         }

         class commutative_inserter : public lexer::token_inserter
         {
         public:

            using lexer::token_inserter::insert;

            commutative_inserter()
            : lexer::token_inserter(2)
            {}

            inline void ignore_symbol(const std::string& symbol)
            {
               ignore_set_.insert(symbol);
            }

            inline int insert(const lexer::token& t0, const lexer::token& t1, lexer::token& new_token) exprtk_override
            {
               bool match         = false;
               new_token.type     = lexer::token::e_mul;
               new_token.value    = "*";
               new_token.position = t1.position;

               if (t0.type == lexer::token::e_symbol)
               {
                  if (ignore_set_.end() != ignore_set_.find(t0.value))
                  {
                     return -1;
                  }
                  else if (!t0.value.empty() && ('$' == t0.value[0]))
                  {
                     return -1;
                  }
               }

               if (t1.type == lexer::token::e_symbol)
               {
                  if (ignore_set_.end() != ignore_set_.find(t1.value))
                  {
                     return -1;
                  }
               }
               if      ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_symbol     )) match = true;
               else if ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_lbracket   )) match = true;
               else if ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_lcrlbracket)) match = true;
               else if ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_lsqrbracket)) match = true;
               else if ((t0.type == lexer::token::e_symbol     ) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rbracket   ) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rcrlbracket) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rsqrbracket) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rbracket   ) && (t1.type == lexer::token::e_symbol     )) match = true;
               else if ((t0.type == lexer::token::e_rcrlbracket) && (t1.type == lexer::token::e_symbol     )) match = true;
               else if ((t0.type == lexer::token::e_rsqrbracket) && (t1.type == lexer::token::e_symbol     )) match = true;
               else if ((t0.type == lexer::token::e_symbol     ) && (t1.type == lexer::token::e_symbol     )) match = true;

               return (match) ? 1 : -1;
            }

         private:

            std::set<std::string,details::ilesscompare> ignore_set_;
         };

         class operator_joiner exprtk_final : public token_joiner
         {
         public:

            explicit operator_joiner(const std::size_t& stride)
            : token_joiner(stride)
            {}

            inline bool join(const lexer::token& t0, const lexer::token& t1, lexer::token& t) exprtk_override
            {
               // ': =' --> ':='
               if ((t0.type == lexer::token::e_colon) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_assign;
                  t.value    = ":=";
                  t.position = t0.position;

                  return true;
               }
               // '+ =' --> '+='
               else if ((t0.type == lexer::token::e_add) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_addass;
                  t.value    = "+=";
                  t.position = t0.position;

                  return true;
               }
               // '- =' --> '-='
               else if ((t0.type == lexer::token::e_sub) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_subass;
                  t.value    = "-=";
                  t.position = t0.position;

                  return true;
               }
               // '* =' --> '*='
               else if ((t0.type == lexer::token::e_mul) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_mulass;
                  t.value    = "*=";
                  t.position = t0.position;

                  return true;
               }
               // '/ =' --> '/='
               else if ((t0.type == lexer::token::e_div) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_divass;
                  t.value    = "/=";
                  t.position = t0.position;

                  return true;
               }
               // '% =' --> '%='
               else if ((t0.type == lexer::token::e_mod) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_modass;
                  t.value    = "%=";
                  t.position = t0.position;

                  return true;
               }
               // '> =' --> '>='
               else if ((t0.type == lexer::token::e_gt) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_gte;
                  t.value    = ">=";
                  t.position = t0.position;

                  return true;
               }
               // '< =' --> '<='
               else if ((t0.type == lexer::token::e_lt) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_lte;
                  t.value    = "<=";
                  t.position = t0.position;

                  return true;
               }
               // '= =' --> '=='
               else if ((t0.type == lexer::token::e_eq) && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_eq;
                  t.value    = "==";
                  t.position = t0.position;

                  return true;
               }
               // '! =' --> '!='
               else if ((static_cast<details::char_t>(t0.type) == '!') && (t1.type == lexer::token::e_eq))
               {
                  t.type     = lexer::token::e_ne;
                  t.value    = "!=";
                  t.position = t0.position;

                  return true;
               }
               // '< >' --> '<>'
               else if ((t0.type == lexer::token::e_lt) && (t1.type == lexer::token::e_gt))
               {
                  t.type     = lexer::token::e_ne;
                  t.value    = "<>";
                  t.position = t0.position;

                  return true;
               }
               // '<= >' --> '<=>'
               else if ((t0.type == lexer::token::e_lte) && (t1.type == lexer::token::e_gt))
               {
                  t.type     = lexer::token::e_swap;
                  t.value    = "<=>";
                  t.position = t0.position;

                  return true;
               }
               // '+ -' --> '-'
               else if ((t0.type == lexer::token::e_add) && (t1.type == lexer::token::e_sub))
               {
                  t.type     = lexer::token::e_sub;
                  t.value    = "-";
                  t.position = t0.position;

                  return true;
               }
               // '- +' --> '-'
               else if ((t0.type == lexer::token::e_sub) && (t1.type == lexer::token::e_add))
               {
                  t.type     = lexer::token::e_sub;
                  t.value    = "-";
                  t.position = t0.position;

                  return true;
               }
               // '- -' --> '+'
               else if ((t0.type == lexer::token::e_sub) && (t1.type == lexer::token::e_sub))
               {
                  /*
                     Note: May need to reconsider this when wanting to implement
                     pre/postfix decrement operator
                  */
                  t.type     = lexer::token::e_add;
                  t.value    = "+";
                  t.position = t0.position;

                  return true;
               }
               else
                  return false;
            }

            inline bool join(const lexer::token& t0,
                             const lexer::token& t1,
                             const lexer::token& t2,
                             lexer::token& t) exprtk_override
            {
               // '[ * ]' --> '[*]'
               if (
                    (t0.type == lexer::token::e_lsqrbracket) &&
                    (t1.type == lexer::token::e_mul        ) &&
                    (t2.type == lexer::token::e_rsqrbracket)
                  )
               {
                  t.type     = lexer::token::e_symbol;
                  t.value    = "[*]";
                  t.position = t0.position;

                  return true;
               }
               else
                  return false;
            }
         };

         class bracket_checker exprtk_final : public lexer::token_scanner
         {
         public:

            using lexer::token_scanner::operator();

            bracket_checker()
            : token_scanner(1)
            , state_(true)
            {}

            bool result() exprtk_override
            {
               if (!stack_.empty())
               {
                  lexer::token t;
                  t.value      = stack_.top().first;
                  t.position   = stack_.top().second;
                  error_token_ = t;
                  state_       = false;

                  return false;
               }
               else
                  return state_;
            }

            lexer::token error_token() const
            {
               return error_token_;
            }

            void reset() exprtk_override
            {
               // Why? because msvc doesn't support swap properly.
               stack_ = std::stack<std::pair<char,std::size_t> >();
               state_ = true;
               error_token_.clear();
            }

            bool operator() (const lexer::token& t) exprtk_override
            {
               if (
                    !t.value.empty()                       &&
                    (lexer::token::e_string != t.type)     &&
                    (lexer::token::e_symbol != t.type)     &&
                    exprtk::details::is_bracket(t.value[0])
                  )
               {
                  details::char_t c = t.value[0];

                  if      (t.type == lexer::token::e_lbracket   ) stack_.push(std::make_pair(')',t.position));
                  else if (t.type == lexer::token::e_lcrlbracket) stack_.push(std::make_pair('}',t.position));
                  else if (t.type == lexer::token::e_lsqrbracket) stack_.push(std::make_pair(']',t.position));
                  else if (exprtk::details::is_right_bracket(c))
                  {
                     if (stack_.empty())
                     {
                        state_       = false;
                        error_token_ = t;

                        return false;
                     }
                     else if (c != stack_.top().first)
                     {
                        state_       = false;
                        error_token_ = t;

                        return false;
                     }
                     else
                        stack_.pop();
                  }
               }

               return true;
            }

         private:

            bool state_;
            std::stack<std::pair<char,std::size_t> > stack_;
            lexer::token error_token_;
         };

         template <typename T>
         class numeric_checker exprtk_final : public lexer::token_scanner
         {
         public:

            using lexer::token_scanner::operator();

            numeric_checker()
            : token_scanner (1)
            , current_index_(0)
            {}

            bool result() exprtk_override
            {
               return error_list_.empty();
            }

            void reset() exprtk_override
            {
               error_list_.clear();
               current_index_ = 0;
            }

            bool operator() (const lexer::token& t) exprtk_override
            {
               if (token::e_number == t.type)
               {
                  T v;

                  if (!exprtk::details::string_to_real(t.value,v))
                  {
                     error_list_.push_back(current_index_);
                  }
               }

               ++current_index_;

               return true;
            }

            std::size_t error_count() const
            {
               return error_list_.size();
            }

            std::size_t error_index(const std::size_t& i) const
            {
               if (i < error_list_.size())
                  return error_list_[i];
               else
                  return std::numeric_limits<std::size_t>::max();
            }

            void clear_errors()
            {
               error_list_.clear();
            }

         private:

            std::size_t current_index_;
            std::vector<std::size_t> error_list_;
         };

         class symbol_replacer exprtk_final : public lexer::token_modifier
         {
         private:

            typedef std::map<std::string,std::pair<std::string,token::token_type>,details::ilesscompare> replace_map_t;

         public:

            bool remove(const std::string& target_symbol)
            {
               const replace_map_t::iterator itr = replace_map_.find(target_symbol);

               if (replace_map_.end() == itr)
                  return false;

               replace_map_.erase(itr);

               return true;
            }

            bool add_replace(const std::string& target_symbol,
                             const std::string& replace_symbol,
                             const lexer::token::token_type token_type = lexer::token::e_symbol)
            {
               const replace_map_t::iterator itr = replace_map_.find(target_symbol);

               if (replace_map_.end() != itr)
               {
                  return false;
               }

               replace_map_[target_symbol] = std::make_pair(replace_symbol,token_type);

               return true;
            }

            void clear()
            {
               replace_map_.clear();
            }

         private:

            bool modify(lexer::token& t) exprtk_override
            {
               if (lexer::token::e_symbol == t.type)
               {
                  if (replace_map_.empty())
                     return false;

                  const replace_map_t::iterator itr = replace_map_.find(t.value);

                  if (replace_map_.end() != itr)
                  {
                     t.value = itr->second.first;
                     t.type  = itr->second.second;

                     return true;
                  }
               }

               return false;
            }

            replace_map_t replace_map_;
         };

         class sequence_validator exprtk_final : public lexer::token_scanner
         {
         private:

            typedef std::pair<lexer::token::token_type,lexer::token::token_type> token_pair_t;
            typedef std::set<token_pair_t> set_t;

         public:

            using lexer::token_scanner::operator();

            sequence_validator()
            : lexer::token_scanner(2)
            {
               add_invalid(lexer::token::e_number, lexer::token::e_number);
               add_invalid(lexer::token::e_string, lexer::token::e_string);
               add_invalid(lexer::token::e_number, lexer::token::e_string);
               add_invalid(lexer::token::e_string, lexer::token::e_number);

               add_invalid_set1(lexer::token::e_assign );
               add_invalid_set1(lexer::token::e_shr    );
               add_invalid_set1(lexer::token::e_shl    );
               add_invalid_set1(lexer::token::e_lte    );
               add_invalid_set1(lexer::token::e_ne     );
               add_invalid_set1(lexer::token::e_gte    );
               add_invalid_set1(lexer::token::e_lt     );
               add_invalid_set1(lexer::token::e_gt     );
               add_invalid_set1(lexer::token::e_eq     );
               add_invalid_set1(lexer::token::e_comma  );
               add_invalid_set1(lexer::token::e_add    );
               add_invalid_set1(lexer::token::e_sub    );
               add_invalid_set1(lexer::token::e_div    );
               add_invalid_set1(lexer::token::e_mul    );
               add_invalid_set1(lexer::token::e_mod    );
               add_invalid_set1(lexer::token::e_pow    );
               add_invalid_set1(lexer::token::e_colon  );
               add_invalid_set1(lexer::token::e_ternary);
            }

            bool result() exprtk_override
            {
               return error_list_.empty();
            }

            bool operator() (const lexer::token& t0, const lexer::token& t1) exprtk_override
            {
               const set_t::value_type p = std::make_pair(t0.type,t1.type);

               if (invalid_bracket_check(t0.type,t1.type))
               {
                  error_list_.push_back(std::make_pair(t0,t1));
               }
               else if (invalid_comb_.find(p) != invalid_comb_.end())
               {
                  error_list_.push_back(std::make_pair(t0,t1));
               }

               return true;
            }

            std::size_t error_count() const
            {
               return error_list_.size();
            }

            std::pair<lexer::token,lexer::token> error(const std::size_t index)
            {
               if (index < error_list_.size())
               {
                  return error_list_[index];
               }
               else
               {
                  static const lexer::token error_token;
                  return std::make_pair(error_token,error_token);
               }
            }

            void clear_errors()
            {
               error_list_.clear();
            }

         private:

            void add_invalid(const lexer::token::token_type base, const lexer::token::token_type t)
            {
               invalid_comb_.insert(std::make_pair(base,t));
            }

            void add_invalid_set1(const lexer::token::token_type t)
            {
               add_invalid(t, lexer::token::e_assign);
               add_invalid(t, lexer::token::e_shr   );
               add_invalid(t, lexer::token::e_shl   );
               add_invalid(t, lexer::token::e_lte   );
               add_invalid(t, lexer::token::e_ne    );
               add_invalid(t, lexer::token::e_gte   );
               add_invalid(t, lexer::token::e_lt    );
               add_invalid(t, lexer::token::e_gt    );
               add_invalid(t, lexer::token::e_eq    );
               add_invalid(t, lexer::token::e_comma );
               add_invalid(t, lexer::token::e_div   );
               add_invalid(t, lexer::token::e_mul   );
               add_invalid(t, lexer::token::e_mod   );
               add_invalid(t, lexer::token::e_pow   );
               add_invalid(t, lexer::token::e_colon );
            }

            bool invalid_bracket_check(const lexer::token::token_type base, const lexer::token::token_type t)
            {
               if (details::is_right_bracket(static_cast<details::char_t>(base)))
               {
                  switch (t)
                  {
                     case lexer::token::e_assign : return (']' != base);
                     case lexer::token::e_string : return (')' != base);
                     default                     : return false;
                  }
               }
               else if (details::is_left_bracket(static_cast<details::char_t>(base)))
               {
                  if (details::is_right_bracket(static_cast<details::char_t>(t)))
                     return false;
                  else if (details::is_left_bracket(static_cast<details::char_t>(t)))
                     return false;
                  else
                  {
                     switch (t)
                     {
                        case lexer::token::e_number  : return false;
                        case lexer::token::e_symbol  : return false;
                        case lexer::token::e_string  : return false;
                        case lexer::token::e_add     : return false;
                        case lexer::token::e_sub     : return false;
                        case lexer::token::e_colon   : return false;
                        case lexer::token::e_ternary : return false;
                        default                      : return true ;
                     }
                  }
               }
               else if (details::is_right_bracket(static_cast<details::char_t>(t)))
               {
                  switch (base)
                  {
                     case lexer::token::e_number  : return false;
                     case lexer::token::e_symbol  : return false;
                     case lexer::token::e_string  : return false;
                     case lexer::token::e_eof     : return false;
                     case lexer::token::e_colon   : return false;
                     case lexer::token::e_ternary : return false;
                     default                      : return true ;
                  }
               }
               else if (details::is_left_bracket(static_cast<details::char_t>(t)))
               {
                  switch (base)
                  {
                     case lexer::token::e_rbracket    : return true;
                     case lexer::token::e_rsqrbracket : return true;
                     case lexer::token::e_rcrlbracket : return true;
                     default                          : return false;
                  }
               }

               return false;
            }

            set_t invalid_comb_;
            std::vector<std::pair<lexer::token,lexer::token> > error_list_;
         };

         class sequence_validator_3tokens exprtk_final : public lexer::token_scanner
         {
         private:

            typedef lexer::token::token_type token_t;
            typedef std::pair<token_t,std::pair<token_t,token_t> > token_triplet_t;
            typedef std::set<token_triplet_t> set_t;

         public:

            using lexer::token_scanner::operator();

            sequence_validator_3tokens()
            : lexer::token_scanner(3)
            {
               add_invalid(lexer::token::e_number , lexer::token::e_number , lexer::token::e_number);
               add_invalid(lexer::token::e_string , lexer::token::e_string , lexer::token::e_string);
               add_invalid(lexer::token::e_comma  , lexer::token::e_comma  , lexer::token::e_comma );

               add_invalid(lexer::token::e_add    , lexer::token::e_add    , lexer::token::e_add   );
               add_invalid(lexer::token::e_sub    , lexer::token::e_sub    , lexer::token::e_sub   );
               add_invalid(lexer::token::e_div    , lexer::token::e_div    , lexer::token::e_div   );
               add_invalid(lexer::token::e_mul    , lexer::token::e_mul    , lexer::token::e_mul   );
               add_invalid(lexer::token::e_mod    , lexer::token::e_mod    , lexer::token::e_mod   );
               add_invalid(lexer::token::e_pow    , lexer::token::e_pow    , lexer::token::e_pow   );

               add_invalid(lexer::token::e_add    , lexer::token::e_sub    , lexer::token::e_add   );
               add_invalid(lexer::token::e_sub    , lexer::token::e_add    , lexer::token::e_sub   );
               add_invalid(lexer::token::e_div    , lexer::token::e_mul    , lexer::token::e_div   );
               add_invalid(lexer::token::e_mul    , lexer::token::e_div    , lexer::token::e_mul   );
               add_invalid(lexer::token::e_mod    , lexer::token::e_pow    , lexer::token::e_mod   );
               add_invalid(lexer::token::e_pow    , lexer::token::e_mod    , lexer::token::e_pow   );
            }

            bool result() exprtk_override
            {
               return error_list_.empty();
            }

            bool operator() (const lexer::token& t0, const lexer::token& t1, const lexer::token& t2) exprtk_override
            {
               const set_t::value_type p = std::make_pair(t0.type,std::make_pair(t1.type,t2.type));

               if (invalid_comb_.find(p) != invalid_comb_.end())
               {
                  error_list_.push_back(std::make_pair(t0,t1));
               }

               return true;
            }

            std::size_t error_count() const
            {
               return error_list_.size();
            }

            std::pair<lexer::token,lexer::token> error(const std::size_t index)
            {
               if (index < error_list_.size())
               {
                  return error_list_[index];
               }
               else
               {
                  static const lexer::token error_token;
                  return std::make_pair(error_token,error_token);
               }
            }

            void clear_errors()
            {
               error_list_.clear();
            }

         private:

            void add_invalid(const token_t t0, const token_t t1, const token_t t2)
            {
               invalid_comb_.insert(std::make_pair(t0,std::make_pair(t1,t2)));
            }

            set_t invalid_comb_;
            std::vector<std::pair<lexer::token,lexer::token> > error_list_;
         };

         struct helper_assembly
         {
            inline bool register_scanner(lexer::token_scanner* scanner)
            {
               if (token_scanner_list.end() != std::find(token_scanner_list.begin(),
                                                         token_scanner_list.end  (),
                                                         scanner))
               {
                  return false;
               }

               token_scanner_list.push_back(scanner);

               return true;
            }

            inline bool register_modifier(lexer::token_modifier* modifier)
            {
               if (token_modifier_list.end() != std::find(token_modifier_list.begin(),
                                                          token_modifier_list.end  (),
                                                          modifier))
               {
                  return false;
               }

               token_modifier_list.push_back(modifier);

               return true;
            }

            inline bool register_joiner(lexer::token_joiner* joiner)
            {
               if (token_joiner_list.end() != std::find(token_joiner_list.begin(),
                                                        token_joiner_list.end  (),
                                                        joiner))
               {
                  return false;
               }

               token_joiner_list.push_back(joiner);

               return true;
            }

            inline bool register_inserter(lexer::token_inserter* inserter)
            {
               if (token_inserter_list.end() != std::find(token_inserter_list.begin(),
                                                          token_inserter_list.end  (),
                                                          inserter))
               {
                  return false;
               }

               token_inserter_list.push_back(inserter);

               return true;
            }

            inline bool run_modifiers(lexer::generator& g)
            {
               error_token_modifier = reinterpret_cast<lexer::token_modifier*>(0);

               for (std::size_t i = 0; i < token_modifier_list.size(); ++i)
               {
                  lexer::token_modifier& modifier = (*token_modifier_list[i]);

                  modifier.reset();
                  modifier.process(g);

                  if (!modifier.result())
                  {
                     error_token_modifier = token_modifier_list[i];

                     return false;
                  }
               }

               return true;
            }

            inline bool run_joiners(lexer::generator& g)
            {
               error_token_joiner = reinterpret_cast<lexer::token_joiner*>(0);

               for (std::size_t i = 0; i < token_joiner_list.size(); ++i)
               {
                  lexer::token_joiner& joiner = (*token_joiner_list[i]);

                  joiner.reset();
                  joiner.process(g);

                  if (!joiner.result())
                  {
                     error_token_joiner = token_joiner_list[i];

                     return false;
                  }
               }

               return true;
            }

            inline bool run_inserters(lexer::generator& g)
            {
               error_token_inserter = reinterpret_cast<lexer::token_inserter*>(0);

               for (std::size_t i = 0; i < token_inserter_list.size(); ++i)
               {
                  lexer::token_inserter& inserter = (*token_inserter_list[i]);

                  inserter.reset();
                  inserter.process(g);

                  if (!inserter.result())
                  {
                     error_token_inserter = token_inserter_list[i];

                     return false;
                  }
               }

               return true;
            }

            inline bool run_scanners(lexer::generator& g)
            {
               error_token_scanner = reinterpret_cast<lexer::token_scanner*>(0);

               for (std::size_t i = 0; i < token_scanner_list.size(); ++i)
               {
                  lexer::token_scanner& scanner = (*token_scanner_list[i]);

                  scanner.reset();
                  scanner.process(g);

                  if (!scanner.result())
                  {
                     error_token_scanner = token_scanner_list[i];

                     return false;
                  }
               }

               return true;
            }

            std::vector<lexer::token_scanner*>  token_scanner_list;
            std::vector<lexer::token_modifier*> token_modifier_list;
            std::vector<lexer::token_joiner*>   token_joiner_list;
            std::vector<lexer::token_inserter*> token_inserter_list;

            lexer::token_scanner*  error_token_scanner;
            lexer::token_modifier* error_token_modifier;
            lexer::token_joiner*   error_token_joiner;
            lexer::token_inserter* error_token_inserter;
         };
      }

      class parser_helper
      {
      public:

         typedef token     token_t;
         typedef generator generator_t;

         inline bool init(const std::string& str)
         {
            if (!lexer_.process(str))
            {
               return false;
            }

            lexer_.begin();

            next_token();

            return true;
         }

         inline generator_t& lexer()
         {
            return lexer_;
         }

         inline const generator_t& lexer() const
         {
            return lexer_;
         }

         inline void store_token()
         {
            lexer_.store();
            store_current_token_ = current_token_;
         }

         inline void restore_token()
         {
            lexer_.restore();
            current_token_ = store_current_token_;
         }

         inline void next_token()
         {
            current_token_ = lexer_.next_token();
         }

         inline const token_t& current_token() const
         {
            return current_token_;
         }

         inline const token_t& peek_next_token()
         {
            return lexer_.peek_next_token();
         }

         enum token_advance_mode
         {
            e_hold    = 0,
            e_advance = 1
         };

         inline void advance_token(const token_advance_mode mode)
         {
            if (e_advance == mode)
            {
               next_token();
            }
         }

         inline bool token_is(const token_t::token_type& ttype, const token_advance_mode mode = e_advance)
         {
            if (current_token().type != ttype)
            {
               return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is(const token_t::token_type& ttype,
                              const std::string& value,
                              const token_advance_mode mode = e_advance)
         {
            if (
                 (current_token().type != ttype) ||
                 !exprtk::details::imatch(value,current_token().value)
               )
            {
               return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is(const std::string& value,
                              const token_advance_mode mode = e_advance)
         {
            if (!exprtk::details::imatch(value,current_token().value))
            {
               return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is_arithmetic_opr(const token_advance_mode mode = e_advance)
         {
            switch (current_token().type)
            {
               case token_t::e_add :
               case token_t::e_sub :
               case token_t::e_div :
               case token_t::e_mul :
               case token_t::e_mod :
               case token_t::e_pow : break;
               default             : return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is_ineq_opr(const token_advance_mode mode = e_advance)
         {
            switch (current_token().type)
            {
               case token_t::e_eq  :
               case token_t::e_lte :
               case token_t::e_ne  :
               case token_t::e_gte :
               case token_t::e_lt  :
               case token_t::e_gt  : break;
               default             : return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is_left_bracket(const token_advance_mode mode = e_advance)
         {
            switch (current_token().type)
            {
               case token_t::e_lbracket    :
               case token_t::e_lcrlbracket :
               case token_t::e_lsqrbracket : break;
               default                     : return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is_right_bracket(const token_advance_mode mode = e_advance)
         {
            switch (current_token().type)
            {
               case token_t::e_rbracket    :
               case token_t::e_rcrlbracket :
               case token_t::e_rsqrbracket : break;
               default                     : return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is_bracket(const token_advance_mode mode = e_advance)
         {
            switch (current_token().type)
            {
               case token_t::e_rbracket    :
               case token_t::e_rcrlbracket :
               case token_t::e_rsqrbracket :
               case token_t::e_lbracket    :
               case token_t::e_lcrlbracket :
               case token_t::e_lsqrbracket : break;
               default                     : return false;
            }

            advance_token(mode);

            return true;
         }

         inline bool token_is_loop(const token_advance_mode mode = e_advance)
         {
            return token_is("for"   , mode) ||
                   token_is("while" , mode) ||
                   token_is("repeat", mode) ;
         }

         inline bool peek_token_is(const token_t::token_type& ttype)
         {
            return (lexer_.peek_next_token().type == ttype);
         }

         inline bool peek_token_is(const std::string& s)
         {
            return (exprtk::details::imatch(lexer_.peek_next_token().value,s));
         }

      private:

         generator_t lexer_;
         token_t     current_token_;
         token_t     store_current_token_;
      };
   }

} // namespace exprtk

#endif // EXPRTK_SRC_LEXER_HPP
