#ifndef EXPRTK_SRC_NODES_ASSIGN_HPP
#define EXPRTK_SRC_NODES_ASSIGN_HPP

#include "nodes_base.hpp"
#include "nodes_variable.hpp"
#include "nodes_string.hpp"

namespace exprtk
{
   namespace details
   {

      template <typename T>
      class assert_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef string_base_node<T>* str_base_ptr;
         typedef assert_check::assert_context assert_context_t;

         assert_node(expression_ptr   assert_condition_node,
                     expression_ptr   assert_message_node,
                     assert_check_ptr assert_check,
                     const assert_context_t& context)
         : assert_message_str_base_(0)
         , assert_check_(assert_check)
         , context_(context)
         {
            construct_branch_pair(assert_condition_node_, assert_condition_node);
            construct_branch_pair(assert_message_node_  , assert_message_node  );

            #ifndef exprtk_disable_string_capabilities
            if (
                  assert_message_node_.first &&
                  details::is_generally_string_node(assert_message_node_.first)
               )
            {
               assert_message_str_base_ = dynamic_cast<str_base_ptr>(assert_message_node_.first);
            }
            #endif

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            if (details::is_true(assert_condition_node_.first->value()))
            {
               return T(1);
            }

            #ifndef exprtk_disable_string_capabilities
            if (assert_message_node_.first)
            {
               assert_message_node_.first->value();
               assert(assert_message_str_base_);
               context_.message = assert_message_str_base_->str();
            }
            #endif

            assert_check_->handle_assert(context_);
            return T(0);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_assert;
         }

         inline bool valid() const exprtk_override
         {
            return (
                     assert_check_ &&
                     assert_condition_node_.first &&
                     assert_condition_node_.first->valid()
                   ) &&
                   (
                     (0 == assert_message_node_.first) ||
                     (
                       assert_message_node_.first &&
                       assert_message_str_base_   &&
                       assert_message_node_.first->valid() &&
                       details::is_generally_string_node(assert_message_node_.first)
                     )
                   );
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(assert_condition_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(assert_message_node_  , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (assert_condition_node_, assert_message_node_);
         }

      private:

         branch_t         assert_condition_node_;
         branch_t         assert_message_node_;
         str_base_ptr     assert_message_str_base_;
         assert_check_ptr assert_check_;
         mutable assert_context_t context_;
      };

      template <typename T, std::size_t N>
      inline T axn(const T a, const T x)
      {
         // a*x^n
         return a * exprtk::details::numeric::fast_exp<T,N>::result(x);
      }

      template <typename T, std::size_t N>
      inline T axnb(const T a, const T x, const T b)
      {
         // a*x^n+b
         return a * exprtk::details::numeric::fast_exp<T,N>::result(x) + b;
      }

      template <typename T>
      struct sf_base
      {
         typedef typename details::functor_t<T>::Type Type;
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::qfunc_t quaternary_functor_t;
         typedef typename functor_t::tfunc_t trinary_functor_t;
         typedef typename functor_t::bfunc_t binary_functor_t;
         typedef typename functor_t::ufunc_t unary_functor_t;
      };

      #define define_sfop3(NN, OP0, OP1)                 \
      template <typename T>                              \
      struct sf##NN##_op : public sf_base<T>             \
      {                                                  \
         typedef typename sf_base<T>::Type const Type;   \
         static inline T process(Type x, Type y, Type z) \
         {                                               \
            return (OP0);                                \
         }                                               \
         static inline std::string id()                  \
         {                                               \
            return (OP1);                                \
         }                                               \
      };                                                 \

      define_sfop3(00,(x + y) / z       ,"(t+t)/t")
      define_sfop3(01,(x + y) * z       ,"(t+t)*t")
      define_sfop3(02,(x + y) - z       ,"(t+t)-t")
      define_sfop3(03,(x + y) + z       ,"(t+t)+t")
      define_sfop3(04,(x - y) + z       ,"(t-t)+t")
      define_sfop3(05,(x - y) / z       ,"(t-t)/t")
      define_sfop3(06,(x - y) * z       ,"(t-t)*t")
      define_sfop3(07,(x * y) + z       ,"(t*t)+t")
      define_sfop3(08,(x * y) - z       ,"(t*t)-t")
      define_sfop3(09,(x * y) / z       ,"(t*t)/t")
      define_sfop3(10,(x * y) * z       ,"(t*t)*t")
      define_sfop3(11,(x / y) + z       ,"(t/t)+t")
      define_sfop3(12,(x / y) - z       ,"(t/t)-t")
      define_sfop3(13,(x / y) / z       ,"(t/t)/t")
      define_sfop3(14,(x / y) * z       ,"(t/t)*t")
      define_sfop3(15,x / (y + z)       ,"t/(t+t)")
      define_sfop3(16,x / (y - z)       ,"t/(t-t)")
      define_sfop3(17,x / (y * z)       ,"t/(t*t)")
      define_sfop3(18,x / (y / z)       ,"t/(t/t)")
      define_sfop3(19,x * (y + z)       ,"t*(t+t)")
      define_sfop3(20,x * (y - z)       ,"t*(t-t)")
      define_sfop3(21,x * (y * z)       ,"t*(t*t)")
      define_sfop3(22,x * (y / z)       ,"t*(t/t)")
      define_sfop3(23,x - (y + z)       ,"t-(t+t)")
      define_sfop3(24,x - (y - z)       ,"t-(t-t)")
      define_sfop3(25,x - (y / z)       ,"t-(t/t)")
      define_sfop3(26,x - (y * z)       ,"t-(t*t)")
      define_sfop3(27,x + (y * z)       ,"t+(t*t)")
      define_sfop3(28,x + (y / z)       ,"t+(t/t)")
      define_sfop3(29,x + (y + z)       ,"t+(t+t)")
      define_sfop3(30,x + (y - z)       ,"t+(t-t)")
      define_sfop3(31,(axnb<T,2>(x,y,z)),"       ")
      define_sfop3(32,(axnb<T,3>(x,y,z)),"       ")
      define_sfop3(33,(axnb<T,4>(x,y,z)),"       ")
      define_sfop3(34,(axnb<T,5>(x,y,z)),"       ")
      define_sfop3(35,(axnb<T,6>(x,y,z)),"       ")
      define_sfop3(36,(axnb<T,7>(x,y,z)),"       ")
      define_sfop3(37,(axnb<T,8>(x,y,z)),"       ")
      define_sfop3(38,(axnb<T,9>(x,y,z)),"       ")
      define_sfop3(39,x * numeric::log(y)   + z,"")
      define_sfop3(40,x * numeric::log(y)   - z,"")
      define_sfop3(41,x * numeric::log10(y) + z,"")
      define_sfop3(42,x * numeric::log10(y) - z,"")
      define_sfop3(43,x * numeric::sin(y) + z  ,"")
      define_sfop3(44,x * numeric::sin(y) - z  ,"")
      define_sfop3(45,x * numeric::cos(y) + z  ,"")
      define_sfop3(46,x * numeric::cos(y) - z  ,"")
      define_sfop3(47,details::is_true(x) ? y : z,"")

      #define define_sfop4(NN, OP0, OP1)                         \
      template <typename T>                                      \
      struct sf##NN##_op : public sf_base<T>                     \
      {                                                          \
         typedef typename sf_base<T>::Type const Type;           \
         static inline T process(Type x, Type y, Type z, Type w) \
         {                                                       \
            return (OP0);                                        \
         }                                                       \
         static inline std::string id()                          \
         {                                                       \
            return (OP1);                                        \
         }                                                       \
      };                                                         \

      define_sfop4(48,(x + ((y + z) / w)),"t+((t+t)/t)")
      define_sfop4(49,(x + ((y + z) * w)),"t+((t+t)*t)")
      define_sfop4(50,(x + ((y - z) / w)),"t+((t-t)/t)")
      define_sfop4(51,(x + ((y - z) * w)),"t+((t-t)*t)")
      define_sfop4(52,(x + ((y * z) / w)),"t+((t*t)/t)")
      define_sfop4(53,(x + ((y * z) * w)),"t+((t*t)*t)")
      define_sfop4(54,(x + ((y / z) + w)),"t+((t/t)+t)")
      define_sfop4(55,(x + ((y / z) / w)),"t+((t/t)/t)")
      define_sfop4(56,(x + ((y / z) * w)),"t+((t/t)*t)")
      define_sfop4(57,(x - ((y + z) / w)),"t-((t+t)/t)")
      define_sfop4(58,(x - ((y + z) * w)),"t-((t+t)*t)")
      define_sfop4(59,(x - ((y - z) / w)),"t-((t-t)/t)")
      define_sfop4(60,(x - ((y - z) * w)),"t-((t-t)*t)")
      define_sfop4(61,(x - ((y * z) / w)),"t-((t*t)/t)")
      define_sfop4(62,(x - ((y * z) * w)),"t-((t*t)*t)")
      define_sfop4(63,(x - ((y / z) / w)),"t-((t/t)/t)")
      define_sfop4(64,(x - ((y / z) * w)),"t-((t/t)*t)")
      define_sfop4(65,(((x + y) * z) - w),"((t+t)*t)-t")
      define_sfop4(66,(((x - y) * z) - w),"((t-t)*t)-t")
      define_sfop4(67,(((x * y) * z) - w),"((t*t)*t)-t")
      define_sfop4(68,(((x / y) * z) - w),"((t/t)*t)-t")
      define_sfop4(69,(((x + y) / z) - w),"((t+t)/t)-t")
      define_sfop4(70,(((x - y) / z) - w),"((t-t)/t)-t")
      define_sfop4(71,(((x * y) / z) - w),"((t*t)/t)-t")
      define_sfop4(72,(((x / y) / z) - w),"((t/t)/t)-t")
      define_sfop4(73,((x * y) + (z * w)),"(t*t)+(t*t)")
      define_sfop4(74,((x * y) - (z * w)),"(t*t)-(t*t)")
      define_sfop4(75,((x * y) + (z / w)),"(t*t)+(t/t)")
      define_sfop4(76,((x * y) - (z / w)),"(t*t)-(t/t)")
      define_sfop4(77,((x / y) + (z / w)),"(t/t)+(t/t)")
      define_sfop4(78,((x / y) - (z / w)),"(t/t)-(t/t)")
      define_sfop4(79,((x / y) - (z * w)),"(t/t)-(t*t)")
      define_sfop4(80,(x / (y + (z * w))),"t/(t+(t*t))")
      define_sfop4(81,(x / (y - (z * w))),"t/(t-(t*t))")
      define_sfop4(82,(x * (y + (z * w))),"t*(t+(t*t))")
      define_sfop4(83,(x * (y - (z * w))),"t*(t-(t*t))")

      define_sfop4(84,(axn<T,2>(x,y) + axn<T,2>(z,w)),"")
      define_sfop4(85,(axn<T,3>(x,y) + axn<T,3>(z,w)),"")
      define_sfop4(86,(axn<T,4>(x,y) + axn<T,4>(z,w)),"")
      define_sfop4(87,(axn<T,5>(x,y) + axn<T,5>(z,w)),"")
      define_sfop4(88,(axn<T,6>(x,y) + axn<T,6>(z,w)),"")
      define_sfop4(89,(axn<T,7>(x,y) + axn<T,7>(z,w)),"")
      define_sfop4(90,(axn<T,8>(x,y) + axn<T,8>(z,w)),"")
      define_sfop4(91,(axn<T,9>(x,y) + axn<T,9>(z,w)),"")
      define_sfop4(92,((details::is_true(x) && details::is_true(y)) ? z : w),"")
      define_sfop4(93,((details::is_true(x) || details::is_true(y)) ? z : w),"")
      define_sfop4(94,((x <  y) ? z : w),"")
      define_sfop4(95,((x <= y) ? z : w),"")
      define_sfop4(96,((x >  y) ? z : w),"")
      define_sfop4(97,((x >= y) ? z : w),"")
      define_sfop4(98,(details::is_true(numeric::equal(x,y)) ? z : w),"")
      define_sfop4(99,(x * numeric::sin(y) + z * numeric::cos(w)),"")

      define_sfop4(ext00,((x + y) - (z * w)),"(t+t)-(t*t)")
      define_sfop4(ext01,((x + y) - (z / w)),"(t+t)-(t/t)")
      define_sfop4(ext02,((x + y) + (z * w)),"(t+t)+(t*t)")
      define_sfop4(ext03,((x + y) + (z / w)),"(t+t)+(t/t)")
      define_sfop4(ext04,((x - y) + (z * w)),"(t-t)+(t*t)")
      define_sfop4(ext05,((x - y) + (z / w)),"(t-t)+(t/t)")
      define_sfop4(ext06,((x - y) - (z * w)),"(t-t)-(t*t)")
      define_sfop4(ext07,((x - y) - (z / w)),"(t-t)-(t/t)")
      define_sfop4(ext08,((x + y) - (z - w)),"(t+t)-(t-t)")
      define_sfop4(ext09,((x + y) + (z - w)),"(t+t)+(t-t)")
      define_sfop4(ext10,((x + y) + (z + w)),"(t+t)+(t+t)")
      define_sfop4(ext11,((x + y) * (z - w)),"(t+t)*(t-t)")
      define_sfop4(ext12,((x + y) / (z - w)),"(t+t)/(t-t)")
      define_sfop4(ext13,((x - y) - (z + w)),"(t-t)-(t+t)")
      define_sfop4(ext14,((x - y) + (z + w)),"(t-t)+(t+t)")
      define_sfop4(ext15,((x - y) * (z + w)),"(t-t)*(t+t)")
      define_sfop4(ext16,((x - y) / (z + w)),"(t-t)/(t+t)")
      define_sfop4(ext17,((x * y) - (z + w)),"(t*t)-(t+t)")
      define_sfop4(ext18,((x / y) - (z + w)),"(t/t)-(t+t)")
      define_sfop4(ext19,((x * y) + (z + w)),"(t*t)+(t+t)")
      define_sfop4(ext20,((x / y) + (z + w)),"(t/t)+(t+t)")
      define_sfop4(ext21,((x * y) + (z - w)),"(t*t)+(t-t)")
      define_sfop4(ext22,((x / y) + (z - w)),"(t/t)+(t-t)")
      define_sfop4(ext23,((x * y) - (z - w)),"(t*t)-(t-t)")
      define_sfop4(ext24,((x / y) - (z - w)),"(t/t)-(t-t)")
      define_sfop4(ext25,((x + y) * (z * w)),"(t+t)*(t*t)")
      define_sfop4(ext26,((x + y) * (z / w)),"(t+t)*(t/t)")
      define_sfop4(ext27,((x + y) / (z * w)),"(t+t)/(t*t)")
      define_sfop4(ext28,((x + y) / (z / w)),"(t+t)/(t/t)")
      define_sfop4(ext29,((x - y) / (z * w)),"(t-t)/(t*t)")
      define_sfop4(ext30,((x - y) / (z / w)),"(t-t)/(t/t)")
      define_sfop4(ext31,((x - y) * (z * w)),"(t-t)*(t*t)")
      define_sfop4(ext32,((x - y) * (z / w)),"(t-t)*(t/t)")
      define_sfop4(ext33,((x * y) * (z + w)),"(t*t)*(t+t)")
      define_sfop4(ext34,((x / y) * (z + w)),"(t/t)*(t+t)")
      define_sfop4(ext35,((x * y) / (z + w)),"(t*t)/(t+t)")
      define_sfop4(ext36,((x / y) / (z + w)),"(t/t)/(t+t)")
      define_sfop4(ext37,((x * y) / (z - w)),"(t*t)/(t-t)")
      define_sfop4(ext38,((x / y) / (z - w)),"(t/t)/(t-t)")
      define_sfop4(ext39,((x * y) * (z - w)),"(t*t)*(t-t)")
      define_sfop4(ext40,((x * y) / (z * w)),"(t*t)/(t*t)")
      define_sfop4(ext41,((x / y) * (z / w)),"(t/t)*(t/t)")
      define_sfop4(ext42,((x / y) * (z - w)),"(t/t)*(t-t)")
      define_sfop4(ext43,((x * y) * (z * w)),"(t*t)*(t*t)")
      define_sfop4(ext44,(x + (y * (z / w))),"t+(t*(t/t))")
      define_sfop4(ext45,(x - (y * (z / w))),"t-(t*(t/t))")
      define_sfop4(ext46,(x + (y / (z * w))),"t+(t/(t*t))")
      define_sfop4(ext47,(x - (y / (z * w))),"t-(t/(t*t))")
      define_sfop4(ext48,(((x - y) - z) * w),"((t-t)-t)*t")
      define_sfop4(ext49,(((x - y) - z) / w),"((t-t)-t)/t")
      define_sfop4(ext50,(((x - y) + z) * w),"((t-t)+t)*t")
      define_sfop4(ext51,(((x - y) + z) / w),"((t-t)+t)/t")
      define_sfop4(ext52,((x + (y - z)) * w),"(t+(t-t))*t")
      define_sfop4(ext53,((x + (y - z)) / w),"(t+(t-t))/t")
      define_sfop4(ext54,((x + y) / (z + w)),"(t+t)/(t+t)")
      define_sfop4(ext55,((x - y) / (z - w)),"(t-t)/(t-t)")
      define_sfop4(ext56,((x + y) * (z + w)),"(t+t)*(t+t)")
      define_sfop4(ext57,((x - y) * (z - w)),"(t-t)*(t-t)")
      define_sfop4(ext58,((x - y) + (z - w)),"(t-t)+(t-t)")
      define_sfop4(ext59,((x - y) - (z - w)),"(t-t)-(t-t)")
      define_sfop4(ext60,((x / y) + (z * w)),"(t/t)+(t*t)")
      define_sfop4(ext61,(((x * y) * z) / w),"((t*t)*t)/t")

      #undef define_sfop3
      #undef define_sfop4

      template <typename T, typename SpecialFunction>
      class sf3_node exprtk_final : public trinary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf3_node(const operator_type& opr,
                  expression_ptr branch0,
                  expression_ptr branch1,
                  expression_ptr branch2)
         : trinary_node<T>(opr, branch0, branch1, branch2)
         {}

         inline T value() const exprtk_override
         {
            const T x = trinary_node<T>::branch_[0].first->value();
            const T y = trinary_node<T>::branch_[1].first->value();
            const T z = trinary_node<T>::branch_[2].first->value();

            return SpecialFunction::process(x, y, z);
         }
      };

      template <typename T, typename SpecialFunction>
      class sf4_node exprtk_final : public quaternary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf4_node(const operator_type& opr,
                  expression_ptr branch0,
                  expression_ptr branch1,
                  expression_ptr branch2,
                  expression_ptr branch3)
         : quaternary_node<T>(opr, branch0, branch1, branch2, branch3)
         {}

         inline T value() const exprtk_override
         {
            const T x = quaternary_node<T>::branch_[0].first->value();
            const T y = quaternary_node<T>::branch_[1].first->value();
            const T z = quaternary_node<T>::branch_[2].first->value();
            const T w = quaternary_node<T>::branch_[3].first->value();

            return SpecialFunction::process(x, y, z, w);
         }
      };

      template <typename T, typename SpecialFunction>
      class sf3_var_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf3_var_node(const T& v0, const T& v1, const T& v2)
         : v0_(v0)
         , v1_(v1)
         , v2_(v2)
         {}

         inline T value() const exprtk_override
         {
            return SpecialFunction::process(v0_, v1_, v2_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_trinary;
         }

      private:

         sf3_var_node(const sf3_var_node<T,SpecialFunction>&) exprtk_delete;
         sf3_var_node<T,SpecialFunction>& operator=(const sf3_var_node<T,SpecialFunction>&) exprtk_delete;

         const T& v0_;
         const T& v1_;
         const T& v2_;
      };

      template <typename T, typename SpecialFunction>
      class sf4_var_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf4_var_node(const T& v0, const T& v1, const T& v2, const T& v3)
         : v0_(v0)
         , v1_(v1)
         , v2_(v2)
         , v3_(v3)
         {}

         inline T value() const exprtk_override
         {
            return SpecialFunction::process(v0_, v1_, v2_, v3_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_trinary;
         }

      private:

         sf4_var_node(const sf4_var_node<T,SpecialFunction>&) exprtk_delete;
         sf4_var_node<T,SpecialFunction>& operator=(const sf4_var_node<T,SpecialFunction>&) exprtk_delete;

         const T& v0_;
         const T& v1_;
         const T& v2_;
         const T& v3_;
      };

      template <typename T, typename VarArgFunction>
      class vararg_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         explicit vararg_node(const Sequence<expression_ptr,Allocator>& arg_list)
         : initialised_(false)
         {
            arg_list_.resize(arg_list.size());

            for (std::size_t i = 0; i < arg_list.size(); ++i)
            {
               if (arg_list[i] && arg_list[i]->valid())
               {
                  construct_branch_pair(arg_list_[i],arg_list[i]);
               }
               else
               {
                  arg_list_.clear();
                  return;
               }
            }

            initialised_ = (arg_list_.size() == arg_list.size());
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return VarArgFunction::process(arg_list_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vararg;
         }

         inline bool valid() const exprtk_override
         {
            return initialised_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(arg_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
         }

         std::size_t size() const
         {
            return arg_list_.size();
         }

         expression_ptr operator[](const std::size_t& index) const
         {
            return arg_list_[index].first;
         }

      private:

         std::vector<branch_t> arg_list_;
         bool initialised_;
      };

      template <typename T, typename VarArgFunction>
      class vararg_varnode exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         explicit vararg_varnode(const Sequence<expression_ptr,Allocator>& arg_list)
         : initialised_(false)
         {
            arg_list_.resize(arg_list.size());

            for (std::size_t i = 0; i < arg_list.size(); ++i)
            {
               if (arg_list[i] && arg_list[i]->valid() && is_variable_node(arg_list[i]))
               {
                  variable_node<T>* var_node_ptr = static_cast<variable_node<T>*>(arg_list[i]);
                  arg_list_[i] = (&var_node_ptr->ref());
               }
               else
               {
                  arg_list_.clear();
                  return;
               }
            }

            initialised_ = (arg_list.size() == arg_list_.size());
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return VarArgFunction::process(arg_list_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vararg;
         }

         inline bool valid() const exprtk_override
         {
            return initialised_;
         }

      private:

         std::vector<const T*> arg_list_;
         bool initialised_;
      };

      template <typename T, typename VecFunction>
      class vectorize_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         explicit vectorize_node(const expression_ptr v)
         : ivec_ptr_(0)
         {
            construct_branch_pair(v_, v);

            if (is_ivector_node(v_.first))
            {
               ivec_ptr_ = dynamic_cast<vector_interface<T>*>(v_.first);
            }
         }

         inline T value() const exprtk_override
         {
            v_.first->value();
            return VecFunction::process(ivec_ptr_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecfunc;
         }

         inline bool valid() const exprtk_override
         {
            return ivec_ptr_ && v_.first && v_.first->valid();
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(v_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(v_);
         }

      private:

         vector_interface<T>* ivec_ptr_;
         branch_t                    v_;
      };

      template <typename T>
      class assignment_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_node(const operator_type& opr,
                         expression_ptr branch0,
                         expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , var_node_ptr_(0)
         {
            if (is_variable_node(branch(0)))
            {
               var_node_ptr_ = static_cast<variable_node<T>*>(branch(0));
            }
         }

         inline T value() const exprtk_override
         {
            T& result = var_node_ptr_->ref();
               result = branch(1)->value();

            return result;
         }

         inline bool valid() const exprtk_override
         {
            return var_node_ptr_ && binary_node<T>::valid();
         }

      private:

         variable_node<T>* var_node_ptr_;
      };

      template <typename T>
      class assignment_vec_elem_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_vec_elem_node(const operator_type& opr,
                                  expression_ptr branch0,
                                  expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec_node_ptr_(0)
         {
            if (is_vector_elem_node(branch(0)))
            {
               vec_node_ptr_ = static_cast<vector_elem_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& result = vec_node_ptr_->ref();
               result = branch(1)->value();

            return result;
         }

         inline bool valid() const exprtk_override
         {
            return vec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         vector_elem_node<T>* vec_node_ptr_;
      };

      template <typename T>
      class assignment_vec_elem_rtc_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_vec_elem_rtc_node(const operator_type& opr,
                                      expression_ptr branch0,
                                      expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec_node_ptr_(0)
         {
            if (is_vector_elem_rtc_node(branch(0)))
            {
               vec_node_ptr_ = static_cast<vector_elem_rtc_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& result = vec_node_ptr_->ref();
               result = branch(1)->value();

            return result;
         }

         inline bool valid() const exprtk_override
         {
            return vec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         vector_elem_rtc_node<T>* vec_node_ptr_;
      };

      template <typename T>
      class assignment_rebasevec_elem_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using expression_node<T>::branch;

         assignment_rebasevec_elem_node(const operator_type& opr,
                                        expression_ptr branch0,
                                        expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , rbvec_node_ptr_(0)
         {
            if (is_rebasevector_elem_node(branch(0)))
            {
               rbvec_node_ptr_ = static_cast<rebasevector_elem_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& result = rbvec_node_ptr_->ref();
               result = branch(1)->value();

            return result;
         }

         inline bool valid() const exprtk_override
         {
            return rbvec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         rebasevector_elem_node<T>* rbvec_node_ptr_;
      };

      template <typename T>
      class assignment_rebasevec_elem_rtc_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using expression_node<T>::branch;

         assignment_rebasevec_elem_rtc_node(const operator_type& opr,
                                            expression_ptr branch0,
                                            expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , rbvec_node_ptr_(0)
         {
            if (is_rebasevector_elem_rtc_node(branch(0)))
            {
               rbvec_node_ptr_ = static_cast<rebasevector_elem_rtc_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& result = rbvec_node_ptr_->ref();
               result = branch(1)->value();

            return result;
         }

         inline bool valid() const exprtk_override
         {
            return rbvec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         rebasevector_elem_rtc_node<T>* rbvec_node_ptr_;
      };

      template <typename T>
      class assignment_rebasevec_celem_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_rebasevec_celem_node(const operator_type& opr,
                                         expression_ptr branch0,
                                         expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , rbvec_node_ptr_(0)
         {
            if (is_rebasevector_celem_node(branch(0)))
            {
               rbvec_node_ptr_ = static_cast<rebasevector_celem_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& result = rbvec_node_ptr_->ref();
               result = branch(1)->value();

            return result;
         }

         inline bool valid() const exprtk_override
         {
            return rbvec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         rebasevector_celem_node<T>* rbvec_node_ptr_;
      };

      template <typename T>
      class assignment_vec_node exprtk_final
                                : public binary_node     <T>
                                , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vec_data_store<T>   vds_t;

         using binary_node<T>::branch;

         assignment_vec_node(const operator_type& opr,
                             expression_ptr branch0,
                             expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec_node_ptr_(0)
         {
            if (is_vector_node(branch(0)))
            {
               vec_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
               vds()         = vec_node_ptr_->vds();
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            const T v = branch(1)->value();

            T* vec = vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec + lud.upper_bound;

            while (vec < upper_bound)
            {
               #define exprtk_loop(N) \
               vec[N] = v;            \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec += lud.batch_size;
            }

            switch (lud.remainder)
            {
               #define case_stmt(N) \
               case N : *vec++ = v; \
               exprtk_fallthrough   \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15) case_stmt(14)
               case_stmt(13) case_stmt(12)
               case_stmt(11) case_stmt(10)
               case_stmt( 9) case_stmt( 8)
               case_stmt( 7) case_stmt( 6)
               case_stmt( 5) case_stmt( 4)
               #endif
               case_stmt( 3) case_stmt( 2)
               case 1 : *vec++ = v;
            }

            #undef exprtk_loop
            #undef case_stmt

            return vec_node_ptr_->value();
         }

         vector_node_ptr vec() const exprtk_override
         {
            return vec_node_ptr_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return vec_node_ptr_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecvalass;
         }

         inline bool valid() const exprtk_override
         {
            return
               vec_node_ptr_ &&
               (vds().size() <= vec_node_ptr_->vec_holder().base_size()) &&
               binary_node<T>::valid();
         }

         std::size_t size() const exprtk_override
         {
            return vec_node_ptr_->vec_holder().size();
         }

         std::size_t base_size() const exprtk_override
         {
            return vec_node_ptr_->vec_holder().base_size();
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

      private:

         vector_node<T>* vec_node_ptr_;
         vds_t           vds_;
      };

      template <typename T>
      class assignment_vecvec_node exprtk_final
                                   : public binary_node     <T>
                                   , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vec_data_store<T>   vds_t;

         using binary_node<T>::branch;

         assignment_vecvec_node(const operator_type& opr,
                                expression_ptr branch0,
                                expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec0_node_ptr_(0)
         , vec1_node_ptr_(0)
         , initialised_(false)
         , src_is_ivec_(false)
         {
            if (is_vector_node(branch(0)))
            {
               vec0_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
               vds()          = vec0_node_ptr_->vds();
            }

            if (is_vector_node(branch(1)))
            {
               vec1_node_ptr_ = static_cast<vector_node<T>*>(branch(1));
               vds_t::match_sizes(vds(),vec1_node_ptr_->vds());
            }
            else if (is_ivector_node(branch(1)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(1))))
               {
                  vec1_node_ptr_ = vi->vec();

                  if (!vi->side_effect())
                  {
                     vi->vds()    = vds();
                     src_is_ivec_ = true;
                  }
                  else
                     vds_t::match_sizes(vds(),vi->vds());
               }
            }

            initialised_ =
               vec0_node_ptr_               &&
               vec1_node_ptr_               &&
               (size() <= base_size())      &&
               (vds_.size() <= base_size()) &&
               binary_node<T>::valid();

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            branch(1)->value();

            if (src_is_ivec_)
               return vec0_node_ptr_->value();

            T* vec0 = vec0_node_ptr_->vds().data();
            T* vec1 = vec1_node_ptr_->vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec0 + lud.upper_bound;

            while (vec0 < upper_bound)
            {
               #define exprtk_loop(N) \
               vec0[N] = vec1[N];     \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec0 += lud.batch_size;
               vec1 += lud.batch_size;
            }

            switch (lud.remainder)
            {
               #define case_stmt(N,fall_through) \
               case N : *vec0++ = *vec1++;       \
               fall_through                      \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15, exprtk_fallthrough) case_stmt(14, exprtk_fallthrough)
               case_stmt(13, exprtk_fallthrough) case_stmt(12, exprtk_fallthrough)
               case_stmt(11, exprtk_fallthrough) case_stmt(10, exprtk_fallthrough)
               case_stmt( 9, exprtk_fallthrough) case_stmt( 8, exprtk_fallthrough)
               case_stmt( 7, exprtk_fallthrough) case_stmt( 6, exprtk_fallthrough)
               case_stmt( 5, exprtk_fallthrough) case_stmt( 4, exprtk_fallthrough)
               #endif
               case_stmt( 3, exprtk_fallthrough) case_stmt( 2, exprtk_fallthrough)
               case_stmt( 1, (void)0;)
            }

            #undef exprtk_loop
            #undef case_stmt

            return vec0_node_ptr_->value();
         }

         vector_node_ptr vec() exprtk_override
         {
            return vec0_node_ptr_;
         }

         vector_node_ptr vec() const exprtk_override
         {
            return vec0_node_ptr_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecvecass;
         }

         inline bool valid() const exprtk_override
         {
            return initialised_;
         }

         std::size_t size() const exprtk_override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().size(),
               vec1_node_ptr_->vec_holder().size());
         }

         std::size_t base_size() const exprtk_override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().base_size(),
               vec1_node_ptr_->vec_holder().base_size());
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

      private:

         vector_node<T>* vec0_node_ptr_;
         vector_node<T>* vec1_node_ptr_;
         bool            initialised_;
         bool            src_is_ivec_;
         vds_t           vds_;
      };

      template <typename T, typename Operation>
      class assignment_op_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_op_node(const operator_type& opr,
                            expression_ptr branch0,
                            expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , var_node_ptr_(0)
         {
            if (is_variable_node(branch(0)))
            {
               var_node_ptr_ = static_cast<variable_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = var_node_ptr_->ref();
            v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return var_node_ptr_ && binary_node<T>::valid();
         }

      private:

         variable_node<T>* var_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_vec_elem_op_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_vec_elem_op_node(const operator_type& opr,
                                     expression_ptr branch0,
                                     expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec_node_ptr_(0)
         {
            if (is_vector_elem_node(branch(0)))
            {
               vec_node_ptr_ = static_cast<vector_elem_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = vec_node_ptr_->ref();
               v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return vec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         vector_elem_node<T>* vec_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_vec_elem_op_rtc_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_vec_elem_op_rtc_node(const operator_type& opr,
                                         expression_ptr branch0,
                                         expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec_node_ptr_(0)
         {
            if (is_vector_elem_rtc_node(branch(0)))
            {
               vec_node_ptr_ = static_cast<vector_elem_rtc_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = vec_node_ptr_->ref();
               v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return vec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         vector_elem_rtc_node<T>* vec_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_vec_celem_op_rtc_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_vec_celem_op_rtc_node(const operator_type& opr,
                                          expression_ptr branch0,
                                          expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec_node_ptr_(0)
         {
            if (is_vector_celem_rtc_node(branch(0)))
            {
               vec_node_ptr_ = static_cast<vector_celem_rtc_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = vec_node_ptr_->ref();
               v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return vec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         vector_celem_rtc_node<T>* vec_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_rebasevec_elem_op_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_rebasevec_elem_op_node(const operator_type& opr,
                                           expression_ptr branch0,
                                           expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , rbvec_node_ptr_(0)
         {
            if (is_rebasevector_elem_node(branch(0)))
            {
               rbvec_node_ptr_ = static_cast<rebasevector_elem_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = rbvec_node_ptr_->ref();
               v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return rbvec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         rebasevector_elem_node<T>* rbvec_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_rebasevec_celem_op_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_rebasevec_celem_op_node(const operator_type& opr,
                                            expression_ptr branch0,
                                            expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , rbvec_node_ptr_(0)
         {
            if (is_rebasevector_celem_node(branch(0)))
            {
               rbvec_node_ptr_ = static_cast<rebasevector_celem_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = rbvec_node_ptr_->ref();
               v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return rbvec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         rebasevector_celem_node<T>* rbvec_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_rebasevec_elem_op_rtc_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_rebasevec_elem_op_rtc_node(const operator_type& opr,
                                               expression_ptr branch0,
                                               expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , rbvec_node_ptr_(0)
         {
            if (is_rebasevector_elem_rtc_node(branch(0)))
            {
               rbvec_node_ptr_ = static_cast<rebasevector_elem_rtc_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = rbvec_node_ptr_->ref();
               v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return rbvec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         rebasevector_elem_rtc_node<T>* rbvec_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_rebasevec_celem_op_rtc_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         assignment_rebasevec_celem_op_rtc_node(const operator_type& opr,
                                                expression_ptr branch0,
                                                expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , rbvec_node_ptr_(0)
         {
            if (is_rebasevector_celem_rtc_node(branch(0)))
            {
               rbvec_node_ptr_ = static_cast<rebasevector_celem_rtc_node<T>*>(branch(0));
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T& v = rbvec_node_ptr_->ref();
               v = Operation::process(v,branch(1)->value());

            return v;
         }

         inline bool valid() const exprtk_override
         {
            return rbvec_node_ptr_ && binary_node<T>::valid();
         }

      private:

         rebasevector_celem_rtc_node<T>* rbvec_node_ptr_;
      };

      template <typename T, typename Operation>
      class assignment_vec_op_node exprtk_final
                                   : public binary_node     <T>
                                   , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vec_data_store<T>   vds_t;

         using binary_node<T>::branch;

         assignment_vec_op_node(const operator_type& opr,
                                expression_ptr branch0,
                                expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec_node_ptr_(0)
         {
            if (is_vector_node(branch(0)))
            {
               vec_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
               vds()         = vec_node_ptr_->vds();
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            const T v = branch(1)->value();

            T* vec = vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec + lud.upper_bound;

            while (vec < upper_bound)
            {
               #define exprtk_loop(N)       \
               Operation::assign(vec[N],v); \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec += lud.batch_size;
            }

            switch (lud.remainder)
            {
               #define case_stmt(N,fall_through)     \
               case N : Operation::assign(*vec++,v); \
               fall_through                          \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15, exprtk_fallthrough) case_stmt(14, exprtk_fallthrough)
               case_stmt(13, exprtk_fallthrough) case_stmt(12, exprtk_fallthrough)
               case_stmt(11, exprtk_fallthrough) case_stmt(10, exprtk_fallthrough)
               case_stmt( 9, exprtk_fallthrough) case_stmt( 8, exprtk_fallthrough)
               case_stmt( 7, exprtk_fallthrough) case_stmt( 6, exprtk_fallthrough)
               case_stmt( 5, exprtk_fallthrough) case_stmt( 4, exprtk_fallthrough)
               #endif
               case_stmt( 3, exprtk_fallthrough) case_stmt( 2, exprtk_fallthrough)
               case_stmt( 1, (void)0;)
            }

            #undef exprtk_loop
            #undef case_stmt

            return vec_node_ptr_->value();
         }

         vector_node_ptr vec() const exprtk_override
         {
            return vec_node_ptr_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return vec_node_ptr_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecopvalass;
         }

         inline bool valid() const exprtk_override
         {
            return
               vec_node_ptr_           &&
               (size() <= base_size()) &&
               binary_node<T>::valid() ;
         }

         std::size_t size() const exprtk_override
         {
            return vec_node_ptr_->vec_holder().size();
         }

         std::size_t base_size() const exprtk_override
         {
            return vec_node_ptr_->vec_holder().base_size();
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

         bool side_effect() const exprtk_override
         {
            return true;
         }

      private:

         vector_node<T>* vec_node_ptr_;
         vds_t           vds_;
      };

      template <typename T, typename Operation>
      class assignment_vecvec_op_node exprtk_final
                                      : public binary_node     <T>
                                      , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vec_data_store<T>   vds_t;

         using binary_node<T>::branch;

         assignment_vecvec_op_node(const operator_type& opr,
                                   expression_ptr branch0,
                                   expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec0_node_ptr_(0)
         , vec1_node_ptr_(0)
         , initialised_(false)
         {
            if (is_vector_node(branch(0)))
            {
               vec0_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
               vds()          = vec0_node_ptr_->vds();
            }

            if (is_vector_node(branch(1)))
            {
               vec1_node_ptr_ = static_cast<vector_node<T>*>(branch(1));
               vec1_node_ptr_->vds() = vds();
            }
            else if (is_ivector_node(branch(1)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(1))))
               {
                  vec1_node_ptr_ = vi->vec();
                  vec1_node_ptr_->vds() = vi->vds();
               }
               else
                  vds_t::match_sizes(vds(),vec1_node_ptr_->vds());
            }

            initialised_ =
               vec0_node_ptr_          &&
               vec1_node_ptr_          &&
               (size() <= base_size()) &&
               binary_node<T>::valid();

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            branch(0)->value();
            branch(1)->value();

                  T* vec0 = vec0_node_ptr_->vds().data();
            const T* vec1 = vec1_node_ptr_->vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec0 + lud.upper_bound;

            while (vec0 < upper_bound)
            {
               #define exprtk_loop(N)                          \
               vec0[N] = Operation::process(vec0[N], vec1[N]); \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec0 += lud.batch_size;
               vec1 += lud.batch_size;
            }

            int i = 0;

            switch (lud.remainder)
            {
               #define case_stmt(N,fall_through)                                 \
               case N : { vec0[i] = Operation::process(vec0[i], vec1[i]); ++i; } \
               fall_through                                                      \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15, exprtk_fallthrough) case_stmt(14, exprtk_fallthrough)
               case_stmt(13, exprtk_fallthrough) case_stmt(12, exprtk_fallthrough)
               case_stmt(11, exprtk_fallthrough) case_stmt(10, exprtk_fallthrough)
               case_stmt( 9, exprtk_fallthrough) case_stmt( 8, exprtk_fallthrough)
               case_stmt( 7, exprtk_fallthrough) case_stmt( 6, exprtk_fallthrough)
               case_stmt( 5, exprtk_fallthrough) case_stmt( 4, exprtk_fallthrough)
               #endif
               case_stmt( 3, exprtk_fallthrough) case_stmt( 2, exprtk_fallthrough)
               case_stmt( 1, (void)0;)
            }

            #undef exprtk_loop
            #undef case_stmt

            return vec0_node_ptr_->value();
         }

         vector_node_ptr vec() const exprtk_override
         {
            return vec0_node_ptr_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return vec0_node_ptr_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecopvecass;
         }

         inline bool valid() const exprtk_override
         {
            return initialised_;
         }

         std::size_t size() const exprtk_override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().size(),
               vec1_node_ptr_->vec_holder().size());
         }

         std::size_t base_size() const exprtk_override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().base_size(),
               vec1_node_ptr_->vec_holder().base_size());
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

         bool side_effect() const exprtk_override
         {
            return true;
         }

      private:

         vector_node<T>* vec0_node_ptr_;
         vector_node<T>* vec1_node_ptr_;
         bool            initialised_;
         vds_t           vds_;
      };

      template <typename T>
      struct memory_context_t
      {
         typedef vector_node<T>*  vector_node_ptr;
         typedef vector_holder<T> vector_holder_t;
         typedef vector_holder_t* vector_holder_ptr;

         memory_context_t()
         : temp_(0)
         , temp_vec_node_(0)
         {}

         void clear()
         {
            delete temp_vec_node_;
            delete temp_;
         }

         vector_holder_ptr temp_;
         vector_node_ptr   temp_vec_node_;
      };

      template <typename T>
      inline memory_context_t<T> make_memory_context(vector_holder<T>& vec_holder,
                                                     vec_data_store<T>& vds)
      {
         memory_context_t<T> result_ctxt;

         result_ctxt.temp_  = (vec_holder.rebaseable()) ?
                              new vector_holder<T>(vec_holder,vds) :
                              new vector_holder<T>(vds) ;

         result_ctxt.temp_vec_node_ = new vector_node<T>(vds,result_ctxt.temp_);

         return result_ctxt;
      }

      template <typename T>
      inline memory_context_t<T> make_memory_context(vector_holder<T>& vec_holder0,
                                                     vector_holder<T>& vec_holder1,
                                                     vec_data_store<T>& vds)
      {
         memory_context_t<T> result_ctxt;

         if (!vec_holder0.rebaseable() && !vec_holder1.rebaseable())
            result_ctxt.temp_ = new vector_holder<T>(vds);
         else if (vec_holder0.rebaseable() && !vec_holder1.rebaseable())
            result_ctxt.temp_ = new vector_holder<T>(vec_holder0,vds);
         else if (!vec_holder0.rebaseable() && vec_holder1.rebaseable())
            result_ctxt.temp_ = new vector_holder<T>(vec_holder1,vds);
         else
         {
            result_ctxt.temp_ = (vec_holder0.base_size() >= vec_holder1.base_size()) ?
                                new vector_holder<T>(vec_holder0, vds) :
                                new vector_holder<T>(vec_holder1, vds) ;
         }

         result_ctxt.temp_vec_node_ = new vector_node<T>(vds,result_ctxt.temp_);

         return result_ctxt;
      }

      template <typename T, typename Operation>
      class vec_binop_vecvec_node exprtk_final
                                  : public binary_node     <T>
                                  , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_holder_t*    vector_holder_ptr;
         typedef vec_data_store<T>   vds_t;
         typedef memory_context_t<T> memory_context;

         using binary_node<T>::branch;

         vec_binop_vecvec_node(const operator_type& opr,
                               expression_ptr branch0,
                               expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec0_node_ptr_(0)
         , vec1_node_ptr_(0)
         , initialised_(false)
         {
            bool v0_is_ivec = false;
            bool v1_is_ivec = false;

            if (is_vector_node(branch(0)))
            {
               vec0_node_ptr_ = static_cast<vector_node_ptr>(branch(0));
            }
            else if (is_ivector_node(branch(0)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(0))))
               {
                  vec0_node_ptr_ = vi->vec();
                  v0_is_ivec     = true;
               }
            }

            if (is_vector_node(branch(1)))
            {
               vec1_node_ptr_ = static_cast<vector_node_ptr>(branch(1));
            }
            else if (is_ivector_node(branch(1)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(1))))
               {
                  vec1_node_ptr_ = vi->vec();
                  v1_is_ivec     = true;
               }
            }

            if (vec0_node_ptr_ && vec1_node_ptr_)
            {
               vector_holder<T>& vec0 = vec0_node_ptr_->vec_holder();
               vector_holder<T>& vec1 = vec1_node_ptr_->vec_holder();

               if (v0_is_ivec && (vec0.base_size() <= vec1.base_size()))
               {
                  vds_ = vds_t(vec0_node_ptr_->vds());
               }
               else if (v1_is_ivec && (vec1.base_size() <= vec0.base_size()))
               {
                  vds_ = vds_t(vec1_node_ptr_->vds());
               }
               else
               {
                  vds_ = vds_t(std::min(vec0.base_size(),vec1.base_size()));
               }

               memory_context_ = make_memory_context(vec0, vec1, vds());

               initialised_ =
                  (size() <= base_size()) &&
                  binary_node<T>::valid();
            }

            assert(valid());
         }

        ~vec_binop_vecvec_node()
         {
            memory_context_.clear();
         }

         inline T value() const exprtk_override
         {
            branch(0)->value();
            branch(1)->value();

            const T* vec0 = vec0_node_ptr_->vds().data();
            const T* vec1 = vec1_node_ptr_->vds().data();
                  T* vec2 = vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec2 + lud.upper_bound;

            while (vec2 < upper_bound)
            {
               #define exprtk_loop(N)                          \
               vec2[N] = Operation::process(vec0[N], vec1[N]); \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec0 += lud.batch_size;
               vec1 += lud.batch_size;
               vec2 += lud.batch_size;
            }

            int i = 0;

            switch (lud.remainder)
            {
               #define case_stmt(N)                                              \
               case N : { vec2[i] = Operation::process(vec0[i], vec1[i]); ++i; } \
               exprtk_fallthrough                                                \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15) case_stmt(14)
               case_stmt(13) case_stmt(12)
               case_stmt(11) case_stmt(10)
               case_stmt( 9) case_stmt( 8)
               case_stmt( 7) case_stmt( 6)
               case_stmt( 5) case_stmt( 4)
               #endif
               case_stmt( 3) case_stmt( 2)
               case_stmt( 1)
               default: break;
            }

            #undef exprtk_loop
            #undef case_stmt

            return (vds().data())[0];
         }

         vector_node_ptr vec() const exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecvecarith;
         }

         inline bool valid() const exprtk_override
         {
            return initialised_;
         }

         std::size_t size() const exprtk_override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().size(),
               vec1_node_ptr_->vec_holder().size());
         }

         std::size_t base_size() const exprtk_override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().base_size(),
               vec1_node_ptr_->vec_holder().base_size());
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

      private:

         vector_node_ptr vec0_node_ptr_;
         vector_node_ptr vec1_node_ptr_;
         bool            initialised_;
         vds_t           vds_;
         memory_context  memory_context_;
      };

      template <typename T, typename Operation>
      class vec_binop_vecval_node exprtk_final
                                  : public binary_node     <T>
                                  , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_holder_t*    vector_holder_ptr;
         typedef vec_data_store<T>   vds_t;
         typedef memory_context_t<T> memory_context;

         using binary_node<T>::branch;

         vec_binop_vecval_node(const operator_type& opr,
                               expression_ptr branch0,
                               expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec0_node_ptr_(0)
         {
            bool v0_is_ivec = false;

            if (is_vector_node(branch(0)))
            {
               vec0_node_ptr_ = static_cast<vector_node_ptr>(branch(0));
            }
            else if (is_ivector_node(branch(0)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(0))))
               {
                  vec0_node_ptr_ = vi->vec();
                  v0_is_ivec     = true;
               }
            }

            if (vec0_node_ptr_)
            {
               if (v0_is_ivec)
                  vds() = vec0_node_ptr_->vds();
               else
                  vds() = vds_t(vec0_node_ptr_->base_size());

               memory_context_ = make_memory_context(vec0_node_ptr_->vec_holder(), vds());
            }

            assert(valid());
         }

        ~vec_binop_vecval_node()
         {
            memory_context_.clear();
         }

         inline T value() const exprtk_override
         {
                        branch(0)->value();
            const T v = branch(1)->value();

            const T* vec0 = vec0_node_ptr_->vds().data();
                  T* vec1 = vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec0 + lud.upper_bound;

            while (vec0 < upper_bound)
            {
               #define exprtk_loop(N)                    \
               vec1[N] = Operation::process(vec0[N], v); \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec0 += lud.batch_size;
               vec1 += lud.batch_size;
            }

            int i = 0;

            switch (lud.remainder)
            {
               #define case_stmt(N,fall_through)                           \
               case N : { vec1[i] = Operation::process(vec0[i], v); ++i; } \
               fall_through                                                \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15, exprtk_fallthrough) case_stmt(14, exprtk_fallthrough)
               case_stmt(13, exprtk_fallthrough) case_stmt(12, exprtk_fallthrough)
               case_stmt(11, exprtk_fallthrough) case_stmt(10, exprtk_fallthrough)
               case_stmt( 9, exprtk_fallthrough) case_stmt( 8, exprtk_fallthrough)
               case_stmt( 7, exprtk_fallthrough) case_stmt( 6, exprtk_fallthrough)
               case_stmt( 5, exprtk_fallthrough) case_stmt( 4, exprtk_fallthrough)
               #endif
               case_stmt( 3, exprtk_fallthrough) case_stmt( 2, exprtk_fallthrough)
               case_stmt( 1, (void)0;)
            }

            #undef exprtk_loop
            #undef case_stmt

            return (vds().data())[0];
         }

         vector_node_ptr vec() const exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecvalarith;
         }

         inline bool valid() const exprtk_override
         {
            return
               vec0_node_ptr_          &&
               (size() <= base_size()) &&
               binary_node<T>::valid();
         }

         std::size_t size() const exprtk_override
         {
            return vec0_node_ptr_->size();
         }

         std::size_t base_size() const exprtk_override
         {
            return vec0_node_ptr_->vec_holder().base_size();
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

      private:

         vector_node_ptr   vec0_node_ptr_;
         vds_t             vds_;
         memory_context    memory_context_;
      };

      template <typename T, typename Operation>
      class vec_binop_valvec_node exprtk_final
                                  : public binary_node     <T>
                                  , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_holder_t*    vector_holder_ptr;
         typedef vec_data_store<T>   vds_t;
         typedef memory_context_t<T> memory_context;

         using binary_node<T>::branch;

         vec_binop_valvec_node(const operator_type& opr,
                               expression_ptr branch0,
                               expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , vec1_node_ptr_(0)
         {
            bool v1_is_ivec = false;

            if (is_vector_node(branch(1)))
            {
               vec1_node_ptr_ = static_cast<vector_node_ptr>(branch(1));
            }
            else if (is_ivector_node(branch(1)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(1))))
               {
                  vec1_node_ptr_ = vi->vec();
                  v1_is_ivec     = true;
               }
            }

            if (vec1_node_ptr_)
            {
               if (v1_is_ivec)
                  vds() = vec1_node_ptr_->vds();
               else
                  vds() = vds_t(vec1_node_ptr_->base_size());

               memory_context_ = make_memory_context(vec1_node_ptr_->vec_holder(), vds());
            }

            assert(valid());
         }

        ~vec_binop_valvec_node()
         {
            memory_context_.clear();
         }

         inline T value() const exprtk_override
         {
            const T v = branch(0)->value();
                        branch(1)->value();

                  T* vec0 = vds().data();
            const T* vec1 = vec1_node_ptr_->vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec0 + lud.upper_bound;

            while (vec0 < upper_bound)
            {
               #define exprtk_loop(N)                    \
               vec0[N] = Operation::process(v, vec1[N]); \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec0 += lud.batch_size;
               vec1 += lud.batch_size;
            }

            int i = 0;

            switch (lud.remainder)
            {
               #define case_stmt(N,fall_through)                           \
               case N : { vec0[i] = Operation::process(v, vec1[i]); ++i; } \
               fall_through                                                \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15, exprtk_fallthrough) case_stmt(14, exprtk_fallthrough)
               case_stmt(13, exprtk_fallthrough) case_stmt(12, exprtk_fallthrough)
               case_stmt(11, exprtk_fallthrough) case_stmt(10, exprtk_fallthrough)
               case_stmt( 9, exprtk_fallthrough) case_stmt( 8, exprtk_fallthrough)
               case_stmt( 7, exprtk_fallthrough) case_stmt( 6, exprtk_fallthrough)
               case_stmt( 5, exprtk_fallthrough) case_stmt( 4, exprtk_fallthrough)
               #endif
               case_stmt( 3, exprtk_fallthrough) case_stmt( 2, exprtk_fallthrough)
               case_stmt( 1, (void)0;)
            }

            #undef exprtk_loop
            #undef case_stmt

            return (vds().data())[0];
         }

         vector_node_ptr vec() const exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecvalarith;
         }

         inline bool valid() const exprtk_override
         {
            return
               vec1_node_ptr_               &&
               (size() <= base_size())      &&
               (vds_.size() <= base_size()) &&
               binary_node<T>::valid();
         }

         std::size_t size() const exprtk_override
         {
            return vec1_node_ptr_->vec_holder().size();
         }

         std::size_t base_size() const exprtk_override
         {
            return vec1_node_ptr_->vec_holder().base_size();
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

      private:

         vector_node_ptr   vec1_node_ptr_;
         vds_t             vds_;
         memory_context    memory_context_;
      };

      template <typename T, typename Operation>
      class unary_vector_node exprtk_final
                              : public unary_node      <T>
                              , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_holder_t*    vector_holder_ptr;
         typedef vec_data_store<T>   vds_t;
         typedef memory_context_t<T> memory_context;

         using expression_node<T>::branch;

         unary_vector_node(const operator_type& opr, expression_ptr branch0)
         : unary_node<T>(opr, branch0)
         , vec0_node_ptr_(0)
         {
            bool vec0_is_ivec = false;

            if (is_vector_node(branch(0)))
            {
               vec0_node_ptr_ = static_cast<vector_node_ptr>(branch(0));
            }
            else if (is_ivector_node(branch(0)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(0))))
               {
                  vec0_node_ptr_ = vi->vec();
                  vec0_is_ivec   = true;
               }
            }

            if (vec0_node_ptr_)
            {
               if (vec0_is_ivec)
                  vds_ = vec0_node_ptr_->vds();
               else
                  vds_ = vds_t(vec0_node_ptr_->base_size());

               memory_context_ = make_memory_context(vec0_node_ptr_->vec_holder(), vds());
            }

            assert(valid());
         }

        ~unary_vector_node()
         {
            memory_context_.clear();
         }

         inline T value() const exprtk_override
         {
            branch()->value();

            const T* vec0 = vec0_node_ptr_->vds().data();
                  T* vec1 = vds().data();

            loop_unroll::details lud(size());
            const T* upper_bound = vec0 + lud.upper_bound;

            while (vec0 < upper_bound)
            {
               #define exprtk_loop(N)                 \
               vec1[N] = Operation::process(vec0[N]); \

               exprtk_loop( 0) exprtk_loop( 1)
               exprtk_loop( 2) exprtk_loop( 3)
               #ifndef exprtk_disable_superscalar_unroll
               exprtk_loop( 4) exprtk_loop( 5)
               exprtk_loop( 6) exprtk_loop( 7)
               exprtk_loop( 8) exprtk_loop( 9)
               exprtk_loop(10) exprtk_loop(11)
               exprtk_loop(12) exprtk_loop(13)
               exprtk_loop(14) exprtk_loop(15)
               #endif

               vec0 += lud.batch_size;
               vec1 += lud.batch_size;
            }

            int i = 0;

            switch (lud.remainder)
            {
               #define case_stmt(N)                                     \
               case N : { vec1[i] = Operation::process(vec0[i]); ++i; } \
               exprtk_fallthrough                                       \

               #ifndef exprtk_disable_superscalar_unroll
               case_stmt(15) case_stmt(14)
               case_stmt(13) case_stmt(12)
               case_stmt(11) case_stmt(10)
               case_stmt( 9) case_stmt( 8)
               case_stmt( 7) case_stmt( 6)
               case_stmt( 5) case_stmt( 4)
               #endif
               case_stmt( 3) case_stmt( 2)
               case_stmt( 1)
               default: break;
            }

            #undef exprtk_loop
            #undef case_stmt

            return (vds().data())[0];
         }

         vector_node_ptr vec() const exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecunaryop;
         }

         inline bool valid() const exprtk_override
         {
            return vec0_node_ptr_ && unary_node<T>::valid();
         }

         std::size_t size() const exprtk_override
         {
            return vec0_node_ptr_->vec_holder().size();
         }

         std::size_t base_size() const exprtk_override
         {
            return vec0_node_ptr_->vec_holder().base_size();
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

      private:

         vector_node_ptr vec0_node_ptr_;
         vds_t           vds_;
         memory_context  memory_context_;
      };

      template <typename T>
      class conditional_vector_node exprtk_final
                                    : public expression_node <T>
                                    , public vector_interface<T>
      {
      public:

         typedef expression_node <T>* expression_ptr;
         typedef vector_interface<T>* vec_interface_ptr;
         typedef vector_node     <T>* vector_node_ptr;
         typedef vector_holder   <T>  vector_holder_t;
         typedef vector_holder_t*     vector_holder_ptr;
         typedef vec_data_store  <T>  vds_t;
         typedef memory_context_t<T>  memory_context;
         typedef std::pair<expression_ptr,bool> branch_t;

         conditional_vector_node(expression_ptr condition,
                                 expression_ptr consequent,
                                 expression_ptr alternative)
         : consequent_node_ptr_ (0)
         , alternative_node_ptr_(0)
         , temp_vec_node_       (0)
         , temp_                (0)
         , result_vec_size_     (0)
         , initialised_         (false)
         {
            construct_branch_pair(condition_  , condition  );
            construct_branch_pair(consequent_ , consequent );
            construct_branch_pair(alternative_, alternative);

            if (details::is_ivector_node(consequent_.first))
            {
               vec_interface_ptr ivec_ptr = dynamic_cast<vec_interface_ptr>(consequent_.first);

               if (0 != ivec_ptr)
               {
                  consequent_node_ptr_ = ivec_ptr->vec();
               }
            }

            if (details::is_ivector_node(alternative_.first))
            {
               vec_interface_ptr ivec_ptr = dynamic_cast<vec_interface_ptr>(alternative_.first);

               if (0 != ivec_ptr)
               {
                  alternative_node_ptr_ = ivec_ptr->vec();
               }
            }

            if (consequent_node_ptr_ && alternative_node_ptr_)
            {
               const std::size_t vec_size =
                  std::max(consequent_node_ptr_ ->vec_holder().base_size(),
                           alternative_node_ptr_->vec_holder().base_size());

               vds_            = vds_t(vec_size);
               memory_context_ = make_memory_context(
                  consequent_node_ptr_ ->vec_holder(),
                  alternative_node_ptr_->vec_holder(),
                  vds());

               initialised_ = (vec_size > 0);
            }

            assert(initialised_);
         }

        ~conditional_vector_node()
         {
            memory_context_.clear();
         }

         inline T value() const exprtk_override
         {
            T result = T(0);
            T* source_vector = 0;
            T* result_vector = vds().data();

            if (is_true(condition_))
            {
               result           = consequent_.first->value();
               source_vector    = consequent_node_ptr_->vds().data();
               result_vec_size_ = consequent_node_ptr_->size();
            }
            else
            {
               result           = alternative_.first->value();
               source_vector    = alternative_node_ptr_->vds().data();
               result_vec_size_ = alternative_node_ptr_->size();
            }

            for (std::size_t i = 0; i < result_vec_size_; ++i)
            {
               result_vector[i] = source_vector[i];
            }

            return result;
         }

         vector_node_ptr vec() const exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         vector_node_ptr vec() exprtk_override
         {
            return memory_context_.temp_vec_node_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecondition;
         }

         inline bool valid() const exprtk_override
         {
            return
               initialised_                                      &&
               condition_  .first && condition_  .first->valid() &&
               consequent_ .first && consequent_ .first->valid() &&
               alternative_.first && alternative_.first->valid() &&
               size() <= base_size();
         }

         std::size_t size() const exprtk_override
         {
            return result_vec_size_;
         }

         std::size_t base_size() const exprtk_override
         {
            return std::min(
               consequent_node_ptr_ ->vec_holder().base_size(),
               alternative_node_ptr_->vec_holder().base_size());
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(condition_   , node_delete_list);
            expression_node<T>::ndb_t::collect(consequent_  , node_delete_list);
            expression_node<T>::ndb_t::collect(alternative_ , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (condition_, consequent_, alternative_);
         }

      private:

         branch_t            condition_;
         branch_t            consequent_;
         branch_t            alternative_;
         vector_node_ptr     consequent_node_ptr_;
         vector_node_ptr     alternative_node_ptr_;
         vector_node_ptr     temp_vec_node_;
         vector_holder_ptr   temp_;
         vds_t               vds_;
         mutable std::size_t result_vec_size_;
         bool                initialised_;
         memory_context      memory_context_;
      };

      template <typename T>
      class scand_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         scand_node(const operator_type& opr,
                    expression_ptr branch0,
                    expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         {
            assert(binary_node<T>::valid());
         }

         inline T value() const exprtk_override
         {
            return (
                     std::not_equal_to<T>()
                        (T(0),branch(0)->value()) &&
                     std::not_equal_to<T>()
                        (T(0),branch(1)->value())
                   ) ? T(1) : T(0);
         }
      };

      template <typename T>
      class scor_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         using binary_node<T>::branch;

         scor_node(const operator_type& opr,
                   expression_ptr branch0,
                   expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         {
            assert(binary_node<T>::valid());
         }

         inline T value() const exprtk_override
         {
            return (
                     std::not_equal_to<T>()
                        (T(0),branch(0)->value()) ||
                     std::not_equal_to<T>()
                        (T(0),branch(1)->value())
                   ) ? T(1) : T(0);
         }
      };

   } // namespace details
} // namespace exprtk

#endif // EXPRTK_SRC_NODES_ASSIGN_HPP
