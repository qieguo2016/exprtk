#ifndef EXPRTK_SRC_NODES_COMPOUND_HPP
#define EXPRTK_SRC_NODES_COMPOUND_HPP

#include "nodes_base.hpp"
#include "nodes_control.hpp"
#include "operators.hpp"

namespace exprtk
{
   namespace details
   {

      template <typename T>
      class vov_base_node : public expression_node<T>
      {
      public:

         virtual ~vov_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T& v0() const = 0;

         virtual const T& v1() const = 0;
      };

      template <typename T>
      class cov_base_node : public expression_node<T>
      {
      public:

         virtual ~cov_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual const T& v() const = 0;
      };

      template <typename T>
      class voc_base_node : public expression_node<T>
      {
      public:

         virtual ~voc_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual const T& v() const = 0;
      };

      template <typename T>
      class vob_base_node : public expression_node<T>
      {
      public:

         virtual ~vob_base_node()
         {}

         virtual const T& v() const = 0;
      };

      template <typename T>
      class bov_base_node : public expression_node<T>
      {
      public:

         virtual ~bov_base_node()
         {}

         virtual const T& v() const = 0;
      };

      template <typename T>
      class cob_base_node : public expression_node<T>
      {
      public:

         virtual ~cob_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual void set_c(const T) = 0;

         virtual expression_node<T>* move_branch(const std::size_t& index) = 0;
      };

      template <typename T>
      class boc_base_node : public expression_node<T>
      {
      public:

         virtual ~boc_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual void set_c(const T) = 0;

         virtual expression_node<T>* move_branch(const std::size_t& index) = 0;
      };

      template <typename T>
      class uv_base_node : public expression_node<T>
      {
      public:

         virtual ~uv_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T& v() const = 0;
      };

      template <typename T>
      class sos_base_node : public expression_node<T>
      {
      public:

         virtual ~sos_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }
      };

      template <typename T>
      class sosos_base_node : public expression_node<T>
      {
      public:

         virtual ~sosos_base_node()
         {}

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }
      };

      template <typename T>
      class T0oT1oT2_base_node : public expression_node<T>
      {
      public:

         virtual ~T0oT1oT2_base_node()
         {}

         virtual std::string type_id() const = 0;
      };

      template <typename T>
      class T0oT1oT2oT3_base_node : public expression_node<T>
      {
      public:

         virtual ~T0oT1oT2oT3_base_node()
         {}

         virtual std::string type_id() const = 0;
      };

      template <typename T, typename Operation>
      class unary_variable_node exprtk_final : public uv_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         explicit unary_variable_node(const T& var)
         : v_(var)
         {}

         inline T value() const exprtk_override
         {
            return Operation::process(v_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline const T& v() const exprtk_override
         {
            return v_;
         }

      private:

         unary_variable_node(const unary_variable_node<T,Operation>&) exprtk_delete;
         unary_variable_node<T,Operation>& operator=(const unary_variable_node<T,Operation>&) exprtk_delete;

         const T& v_;
      };

      template <typename T>
      class uvouv_node exprtk_final : public expression_node<T>
      {
      public:

         // UOpr1(v0) Op UOpr2(v1)
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t    bfunc_t;
         typedef typename functor_t::ufunc_t    ufunc_t;
         typedef expression_node<T>*            expression_ptr;

         explicit uvouv_node(const T& var0,const T& var1,
                             ufunc_t uf0, ufunc_t uf1, bfunc_t bf)
         : v0_(var0)
         , v1_(var1)
         , u0_(uf0 )
         , u1_(uf1 )
         , f_ (bf  )
         {}

         inline T value() const exprtk_override
         {
            return f_(u0_(v0_),u1_(v1_));
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_uvouv;
         }

         inline const T& v0()
         {
            return v0_;
         }

         inline const T& v1()
         {
            return v1_;
         }

         inline ufunc_t u0()
         {
            return u0_;
         }

         inline ufunc_t u1()
         {
            return u1_;
         }

         inline ufunc_t f()
         {
            return f_;
         }

      private:

         uvouv_node(const uvouv_node<T>&) exprtk_delete;
         uvouv_node<T>& operator=(const uvouv_node<T>&) exprtk_delete;

         const T& v0_;
         const T& v1_;
         const ufunc_t u0_;
         const ufunc_t u1_;
         const bfunc_t f_;
      };

      template <typename T, typename Operation>
      class unary_branch_node exprtk_final : public expression_node<T>
      {
      public:

         typedef Operation                      operation_t;
         typedef expression_node<T>*            expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         explicit unary_branch_node(expression_ptr branch)
         {
            construct_branch_pair(branch_, branch);
         }

         inline T value() const exprtk_override
         {
            return Operation::process(branch_.first->value());
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         inline operator_type operation()
         {
            return Operation::operation();
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return branch_.first;
         }

         inline void release()
         {
            branch_.second = false;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         unary_branch_node(const unary_branch_node<T,Operation>&) exprtk_delete;
         unary_branch_node<T,Operation>& operator=(const unary_branch_node<T,Operation>&) exprtk_delete;

         branch_t branch_;
      };

      template <typename T> struct is_const                { enum {result = 0}; };
      template <typename T> struct is_const <const T>      { enum {result = 1}; };
      template <typename T> struct is_const_ref            { enum {result = 0}; };
      template <typename T> struct is_const_ref <const T&> { enum {result = 1}; };
      template <typename T> struct is_ref                  { enum {result = 0}; };
      template <typename T> struct is_ref<T&>              { enum {result = 1}; };
      template <typename T> struct is_ref<const T&>        { enum {result = 0}; };

      template <std::size_t State>
      struct param_to_str { static std::string result() { static const std::string r("v"); return r; } };

      template <>
      struct param_to_str<0> { static std::string result() { static const std::string r("c"); return r; } };

      #define exprtk_crtype(Type)                          \
      param_to_str<is_const_ref< Type >::result>::result() \

      template <typename T>
      struct T0oT1oT2process
      {
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t    bfunc_t;

         struct mode0
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const bfunc_t bf0, const bfunc_t bf1)
            {
               // (T0 o0 T1) o1 T2
               return bf1(bf0(t0,t1),t2);
            }

            template <typename T0, typename T1, typename T2>
            static inline std::string id()
            {
               static const std::string result = "(" + exprtk_crtype(T0) + "o"   +
                                                       exprtk_crtype(T1) + ")o(" +
                                                       exprtk_crtype(T2) + ")"   ;
               return result;
            }
         };

         struct mode1
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const bfunc_t bf0, const bfunc_t bf1)
            {
               // T0 o0 (T1 o1 T2)
               return bf0(t0,bf1(t1,t2));
            }

            template <typename T0, typename T1, typename T2>
            static inline std::string id()
            {
               static const std::string result = "(" + exprtk_crtype(T0) + ")o(" +
                                                       exprtk_crtype(T1) + "o"   +
                                                       exprtk_crtype(T2) + ")"   ;
               return result;
            }
         };
      };

      template <typename T>
      struct T0oT1oT20T3process
      {
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t    bfunc_t;

         struct mode0
         {
            static inline T process(const T& t0, const T& t1,
                                    const T& t2, const T& t3,
                                    const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (T0 o0 T1) o1 (T2 o2 T3)
               return bf1(bf0(t0,t1),bf2(t2,t3));
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "(" + exprtk_crtype(T0) + "o"  +
                                                       exprtk_crtype(T1) + ")o" +
                                                 "(" + exprtk_crtype(T2) + "o"  +
                                                       exprtk_crtype(T3) + ")"  ;
               return result;
            }
         };

         struct mode1
         {
            static inline T process(const T& t0, const T& t1,
                                    const T& t2, const T& t3,
                                    const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (T0 o0 (T1 o1 (T2 o2 T3))
               return bf0(t0,bf1(t1,bf2(t2,t3)));
            }
            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "(" + exprtk_crtype(T0) +  ")o((" +
                                                       exprtk_crtype(T1) +  ")o("  +
                                                       exprtk_crtype(T2) +  "o"    +
                                                       exprtk_crtype(T3) +  "))"   ;
               return result;
            }
         };

         struct mode2
         {
            static inline T process(const T& t0, const T& t1,
                                    const T& t2, const T& t3,
                                    const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (T0 o0 ((T1 o1 T2) o2 T3)
               return bf0(t0,bf2(bf1(t1,t2),t3));
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "(" + exprtk_crtype(T0) + ")o((" +
                                                       exprtk_crtype(T1) + "o"    +
                                                       exprtk_crtype(T2) + ")o("  +
                                                       exprtk_crtype(T3) + "))"   ;
               return result;
            }
         };

         struct mode3
         {
            static inline T process(const T& t0, const T& t1,
                                    const T& t2, const T& t3,
                                    const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (((T0 o0 T1) o1 T2) o2 T3)
               return bf2(bf1(bf0(t0,t1),t2),t3);
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "((" + exprtk_crtype(T0) + "o"    +
                                                        exprtk_crtype(T1) + ")o("  +
                                                        exprtk_crtype(T2) + "))o(" +
                                                        exprtk_crtype(T3) + ")";
               return result;
            }
         };

         struct mode4
         {
            static inline T process(const T& t0, const T& t1,
                                    const T& t2, const T& t3,
                                    const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // ((T0 o0 (T1 o1 T2)) o2 T3
               return bf2(bf0(t0,bf1(t1,t2)),t3);
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "((" + exprtk_crtype(T0) + ")o("  +
                                                        exprtk_crtype(T1) + "o"    +
                                                        exprtk_crtype(T2) + "))o(" +
                                                        exprtk_crtype(T3) + ")"    ;
               return result;
            }
         };
      };

      #undef exprtk_crtype

      template <typename T, typename T0, typename T1>
      struct nodetype_T0oT1 { static const typename expression_node<T>::node_type result; };
      template <typename T, typename T0, typename T1>
      const typename expression_node<T>::node_type nodetype_T0oT1<T,T0,T1>::result = expression_node<T>::e_none;

      #define synthesis_node_type_define(T0_, T1_, v_)                                                          \
      template <typename T, typename T0, typename T1>                                                           \
      struct nodetype_T0oT1<T,T0_,T1_> { static const typename expression_node<T>::node_type result; };         \
      template <typename T, typename T0, typename T1>                                                           \
      const typename expression_node<T>::node_type nodetype_T0oT1<T,T0_,T1_>::result = expression_node<T>:: v_; \

      synthesis_node_type_define(const T0&, const T1&,  e_vov)
      synthesis_node_type_define(const T0&, const T1 ,  e_voc)
      synthesis_node_type_define(const T0 , const T1&,  e_cov)
      synthesis_node_type_define(      T0&,       T1&, e_none)
      synthesis_node_type_define(const T0 , const T1 , e_none)
      synthesis_node_type_define(      T0&, const T1 , e_none)
      synthesis_node_type_define(const T0 ,       T1&, e_none)
      synthesis_node_type_define(const T0&,       T1&, e_none)
      synthesis_node_type_define(      T0&, const T1&, e_none)
      #undef synthesis_node_type_define

      template <typename T, typename T0, typename T1, typename T2>
      struct nodetype_T0oT1oT2 { static const typename expression_node<T>::node_type result; };
      template <typename T, typename T0, typename T1, typename T2>
      const typename expression_node<T>::node_type nodetype_T0oT1oT2<T,T0,T1,T2>::result = expression_node<T>::e_none;

      #define synthesis_node_type_define(T0_, T1_, T2_, v_)                                                            \
      template <typename T, typename T0, typename T1, typename T2>                                                     \
      struct nodetype_T0oT1oT2<T,T0_,T1_,T2_> { static const typename expression_node<T>::node_type result; };         \
      template <typename T, typename T0, typename T1, typename T2>                                                     \
      const typename expression_node<T>::node_type nodetype_T0oT1oT2<T,T0_,T1_,T2_>::result = expression_node<T>:: v_; \

      synthesis_node_type_define(const T0&, const T1&, const T2&, e_vovov)
      synthesis_node_type_define(const T0&, const T1&, const T2 , e_vovoc)
      synthesis_node_type_define(const T0&, const T1 , const T2&, e_vocov)
      synthesis_node_type_define(const T0 , const T1&, const T2&, e_covov)
      synthesis_node_type_define(const T0 , const T1&, const T2 , e_covoc)
      synthesis_node_type_define(const T0 , const T1 , const T2 , e_none )
      synthesis_node_type_define(const T0 , const T1 , const T2&, e_none )
      synthesis_node_type_define(const T0&, const T1 , const T2 , e_none )
      synthesis_node_type_define(      T0&,       T1&,       T2&, e_none )
      #undef synthesis_node_type_define

      template <typename T, typename T0, typename T1, typename T2, typename T3>
      struct nodetype_T0oT1oT2oT3 { static const typename expression_node<T>::node_type result; };
      template <typename T, typename T0, typename T1, typename T2, typename T3>
      const typename expression_node<T>::node_type nodetype_T0oT1oT2oT3<T,T0,T1,T2,T3>::result = expression_node<T>::e_none;

      #define synthesis_node_type_define(T0_, T1_, T2_, T3_, v_)                                                              \
      template <typename T, typename T0, typename T1, typename T2, typename T3>                                               \
      struct nodetype_T0oT1oT2oT3<T,T0_,T1_,T2_,T3_> { static const typename expression_node<T>::node_type result; };         \
      template <typename T, typename T0, typename T1, typename T2, typename T3>                                               \
      const typename expression_node<T>::node_type nodetype_T0oT1oT2oT3<T,T0_,T1_,T2_,T3_>::result = expression_node<T>:: v_; \

      synthesis_node_type_define(const T0&, const T1&, const T2&, const T3&, e_vovovov)
      synthesis_node_type_define(const T0&, const T1&, const T2&, const T3 , e_vovovoc)
      synthesis_node_type_define(const T0&, const T1&, const T2 , const T3&, e_vovocov)
      synthesis_node_type_define(const T0&, const T1 , const T2&, const T3&, e_vocovov)
      synthesis_node_type_define(const T0 , const T1&, const T2&, const T3&, e_covovov)
      synthesis_node_type_define(const T0 , const T1&, const T2 , const T3&, e_covocov)
      synthesis_node_type_define(const T0&, const T1 , const T2&, const T3 , e_vocovoc)
      synthesis_node_type_define(const T0 , const T1&, const T2&, const T3 , e_covovoc)
      synthesis_node_type_define(const T0&, const T1 , const T2 , const T3&, e_vococov)
      synthesis_node_type_define(const T0 , const T1 , const T2 , const T3 , e_none   )
      synthesis_node_type_define(const T0 , const T1 , const T2 , const T3&, e_none   )
      synthesis_node_type_define(const T0 , const T1 , const T2&, const T3 , e_none   )
      synthesis_node_type_define(const T0 , const T1&, const T2 , const T3 , e_none   )
      synthesis_node_type_define(const T0&, const T1 , const T2 , const T3 , e_none   )
      synthesis_node_type_define(const T0 , const T1 , const T2&, const T3&, e_none   )
      synthesis_node_type_define(const T0&, const T1&, const T2 , const T3 , e_none   )
      #undef synthesis_node_type_define

      template <typename T, typename T0, typename T1>
      class T0oT1 exprtk_final : public expression_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t    bfunc_t;
         typedef T value_type;
         typedef T0oT1<T,T0,T1> node_type;

         T0oT1(T0 p0, T1 p1, const bfunc_t p2)
         : t0_(p0)
         , t1_(p1)
         , f_ (p2)
         {}

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1<T,T0,T1>::result;
            return result;
         }

         inline operator_type operation() const exprtk_override
         {
            return e_default;
         }

         inline T value() const exprtk_override
         {
            return f_(t0_,t1_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline bfunc_t f() const
         {
            return f_;
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator,
                                                    T0 p0, T1 p1,
                                                    bfunc_t p2)
         {
            return allocator
                     .template allocate_type<node_type, T0, T1, bfunc_t&>
                        (p0, p1, p2);
         }

      private:

         T0oT1(const T0oT1<T,T0,T1>&) exprtk_delete;
         T0oT1<T,T0,T1>& operator=(const T0oT1<T,T0,T1>&) { return (*this); }

         T0 t0_;
         T1 t1_;
         const bfunc_t f_;
      };

      template <typename T, typename T0, typename T1, typename T2, typename ProcessMode>
      class T0oT1oT2 exprtk_final : public T0oT1oT2_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t    bfunc_t;
         typedef T value_type;
         typedef T0oT1oT2<T,T0,T1,T2,ProcessMode> node_type;
         typedef ProcessMode process_mode_t;

         T0oT1oT2(T0 p0, T1 p1, T2 p2, const bfunc_t p3, const bfunc_t p4)
         : t0_(p0)
         , t1_(p1)
         , t2_(p2)
         , f0_(p3)
         , f1_(p4)
         {}

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2<T,T0,T1,T2>::result;
            return result;
         }

         inline operator_type operation()
         {
            return e_default;
         }

         inline T value() const exprtk_override
         {
            return ProcessMode::process(t0_, t1_, t2_, f0_, f1_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         bfunc_t f0() const
         {
            return f0_;
         }

         bfunc_t f1() const
         {
            return f1_;
         }

         std::string type_id() const exprtk_override
         {
            return id();
         }

         static inline std::string id()
         {
            return process_mode_t::template id<T0,T1,T2>();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 p0, T1 p1, T2 p2, bfunc_t p3, bfunc_t p4)
         {
            return allocator
                      .template allocate_type<node_type, T0, T1, T2, bfunc_t, bfunc_t>
                         (p0, p1, p2, p3, p4);
         }

      private:

         T0oT1oT2(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;

         T0 t0_;
         T1 t1_;
         T2 t2_;
         const bfunc_t f0_;
         const bfunc_t f1_;
      };

      template <typename T, typename T0_, typename T1_, typename T2_, typename T3_, typename ProcessMode>
      class T0oT1oT2oT3 exprtk_final : public T0oT1oT2oT3_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t    bfunc_t;
         typedef T value_type;
         typedef T0_ T0;
         typedef T1_ T1;
         typedef T2_ T2;
         typedef T3_ T3;
         typedef T0oT1oT2oT3<T,T0,T1,T2,T3,ProcessMode> node_type;
         typedef ProcessMode process_mode_t;

         T0oT1oT2oT3(T0 p0, T1 p1, T2 p2, T3 p3, bfunc_t p4, bfunc_t p5, bfunc_t p6)
         : t0_(p0)
         , t1_(p1)
         , t2_(p2)
         , t3_(p3)
         , f0_(p4)
         , f1_(p5)
         , f2_(p6)
         {}

         inline T value() const exprtk_override
         {
            return ProcessMode::process(t0_, t1_, t2_, t3_, f0_, f1_, f2_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         inline T3 t3() const
         {
            return t3_;
         }

         inline bfunc_t f0() const
         {
            return f0_;
         }

         inline bfunc_t f1() const
         {
            return f1_;
         }

         inline bfunc_t f2() const
         {
            return f2_;
         }

         inline std::string type_id() const exprtk_override
         {
            return id();
         }

         static inline std::string id()
         {
            return process_mode_t::template id<T0, T1, T2, T3>();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator,
                                                    T0 p0, T1 p1, T2 p2, T3 p3,
                                                    bfunc_t p4, bfunc_t p5, bfunc_t p6)
         {
            return allocator
                      .template allocate_type<node_type, T0, T1, T2, T3, bfunc_t, bfunc_t>
                         (p0, p1, p2, p3, p4, p5, p6);
         }

      private:

         T0oT1oT2oT3(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;

         T0 t0_;
         T1 t1_;
         T2 t2_;
         T3 t3_;
         const bfunc_t f0_;
         const bfunc_t f1_;
         const bfunc_t f2_;
      };

      template <typename T, typename T0, typename T1, typename T2>
      class T0oT1oT2_sf3 exprtk_final : public T0oT1oT2_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::tfunc_t    tfunc_t;
         typedef T value_type;
         typedef T0oT1oT2_sf3<T,T0,T1,T2> node_type;

         T0oT1oT2_sf3(T0 p0, T1 p1, T2 p2, const tfunc_t p3)
         : t0_(p0)
         , t1_(p1)
         , t2_(p2)
         , f_ (p3)
         {}

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2<T,T0,T1,T2>::result;
            return result;
         }

         inline operator_type operation() const exprtk_override
         {
            return e_default;
         }

         inline T value() const exprtk_override
         {
            return f_(t0_, t1_, t2_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         tfunc_t f() const
         {
            return f_;
         }

         std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return "sf3";
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 p0, T1 p1, T2 p2, tfunc_t p3)
         {
            return allocator
                     .template allocate_type<node_type, T0, T1, T2, tfunc_t>
                        (p0, p1, p2, p3);
         }

      private:

         T0oT1oT2_sf3(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;

         T0 t0_;
         T1 t1_;
         T2 t2_;
         const tfunc_t f_;
      };

      template <typename T, typename T0, typename T1, typename T2>
      class sf3ext_type_node : public T0oT1oT2_base_node<T>
      {
      public:

         virtual ~sf3ext_type_node()
         {}

         virtual T0 t0() const = 0;

         virtual T1 t1() const = 0;

         virtual T2 t2() const = 0;
      };

      template <typename T, typename T0, typename T1, typename T2, typename SF3Operation>
      class T0oT1oT2_sf3ext exprtk_final : public sf3ext_type_node<T,T0,T1,T2>
      {
      public:

         typedef T value_type;
         typedef T0oT1oT2_sf3ext<T, T0, T1, T2, SF3Operation> node_type;

         T0oT1oT2_sf3ext(T0 p0, T1 p1, T2 p2)
         : t0_(p0)
         , t1_(p1)
         , t2_(p2)
         {}

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2<T,T0,T1,T2>::result;
            return result;
         }

         inline operator_type operation()
         {
            return e_default;
         }

         inline T value() const exprtk_override
         {
            return SF3Operation::process(t0_, t1_, t2_);
         }

         T0 t0() const exprtk_override
         {
            return t0_;
         }

         T1 t1() const exprtk_override
         {
            return t1_;
         }

         T2 t2() const exprtk_override
         {
            return t2_;
         }

         std::string type_id() const exprtk_override
         {
            return id();
         }

         static inline std::string id()
         {
            return SF3Operation::id();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 p0, T1 p1, T2 p2)
         {
            return allocator
                     .template allocate_type<node_type, T0, T1, T2>
                        (p0, p1, p2);
         }

      private:

         T0oT1oT2_sf3ext(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;

         T0 t0_;
         T1 t1_;
         T2 t2_;
      };

      template <typename T>
      inline bool is_sf3ext_node(const expression_node<T>* n)
      {
         switch (n->type())
         {
            case expression_node<T>::e_vovov : return true;
            case expression_node<T>::e_vovoc : return true;
            case expression_node<T>::e_vocov : return true;
            case expression_node<T>::e_covov : return true;
            case expression_node<T>::e_covoc : return true;
            default                          : return false;
         }
      }

      template <typename T, typename T0, typename T1, typename T2, typename T3>
      class T0oT1oT2oT3_sf4 exprtk_final : public T0oT1oT2_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::qfunc_t    qfunc_t;
         typedef T value_type;
         typedef T0oT1oT2oT3_sf4<T, T0, T1, T2, T3> node_type;

         T0oT1oT2oT3_sf4(T0 p0, T1 p1, T2 p2, T3 p3, const qfunc_t p4)
         : t0_(p0)
         , t1_(p1)
         , t2_(p2)
         , t3_(p3)
         , f_ (p4)
         {}

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2oT3<T,T0,T1,T2,T3>::result;
            return result;
         }

         inline operator_type operation() const exprtk_override
         {
            return e_default;
         }

         inline T value() const exprtk_override
         {
            return f_(t0_, t1_, t2_, t3_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         inline T3 t3() const
         {
            return t3_;
         }

         qfunc_t f() const
         {
            return f_;
         }

         std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return "sf4";
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 p0, T1 p1, T2 p2, T3 p3, qfunc_t p4)
         {
            return allocator
                     .template allocate_type<node_type, T0, T1, T2, T3, qfunc_t>
                        (p0, p1, p2, p3, p4);
         }

      private:

         T0oT1oT2oT3_sf4(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;

         T0 t0_;
         T1 t1_;
         T2 t2_;
         T3 t3_;
         const qfunc_t f_;
      };

      template <typename T, typename T0, typename T1, typename T2, typename T3, typename SF4Operation>
      class T0oT1oT2oT3_sf4ext exprtk_final : public T0oT1oT2oT3_base_node<T>
      {
      public:

         typedef T value_type;
         typedef T0oT1oT2oT3_sf4ext<T, T0, T1, T2, T3, SF4Operation> node_type;

         T0oT1oT2oT3_sf4ext(T0 p0, T1 p1, T2 p2, T3 p3)
         : t0_(p0)
         , t1_(p1)
         , t2_(p2)
         , t3_(p3)
         {}

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2oT3<T,T0,T1,T2,T3>::result;
            return result;
         }

         inline T value() const exprtk_override
         {
            return SF4Operation::process(t0_, t1_, t2_, t3_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         inline T3 t3() const
         {
            return t3_;
         }

         std::string type_id() const exprtk_override
         {
            return id();
         }

         static inline std::string id()
         {
            return SF4Operation::id();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 p0, T1 p1, T2 p2, T3 p3)
         {
            return allocator
                     .template allocate_type<node_type, T0, T1, T2, T3>
                        (p0, p1, p2, p3);
         }

      private:

         T0oT1oT2oT3_sf4ext(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;

         T0 t0_;
         T1 t1_;
         T2 t2_;
         T3 t3_;
      };

      template <typename T>
      inline bool is_sf4ext_node(const expression_node<T>* n)
      {
         switch (n->type())
         {
            case expression_node<T>::e_vovovov : return true;
            case expression_node<T>::e_vovovoc : return true;
            case expression_node<T>::e_vovocov : return true;
            case expression_node<T>::e_vocovov : return true;
            case expression_node<T>::e_covovov : return true;
            case expression_node<T>::e_covocov : return true;
            case expression_node<T>::e_vocovoc : return true;
            case expression_node<T>::e_covovoc : return true;
            case expression_node<T>::e_vococov : return true;
            default                            : return false;
         }
      }

      template <typename T, typename T0, typename T1>
      struct T0oT1_define
      {
         typedef details::T0oT1<T, T0, T1> type0;
      };

      template <typename T, typename T0, typename T1, typename T2>
      struct T0oT1oT2_define
      {
         typedef details::T0oT1oT2<T, T0, T1, T2, typename T0oT1oT2process<T>::mode0> type0;
         typedef details::T0oT1oT2<T, T0, T1, T2, typename T0oT1oT2process<T>::mode1> type1;
         typedef details::T0oT1oT2_sf3<T, T0, T1, T2> sf3_type;
         typedef details::sf3ext_type_node<T, T0, T1, T2> sf3_type_node;
      };

      template <typename T, typename T0, typename T1, typename T2, typename T3>
      struct T0oT1oT2oT3_define
      {
         typedef details::T0oT1oT2oT3<T, T0, T1, T2, T3, typename T0oT1oT20T3process<T>::mode0> type0;
         typedef details::T0oT1oT2oT3<T, T0, T1, T2, T3, typename T0oT1oT20T3process<T>::mode1> type1;
         typedef details::T0oT1oT2oT3<T, T0, T1, T2, T3, typename T0oT1oT20T3process<T>::mode2> type2;
         typedef details::T0oT1oT2oT3<T, T0, T1, T2, T3, typename T0oT1oT20T3process<T>::mode3> type3;
         typedef details::T0oT1oT2oT3<T, T0, T1, T2, T3, typename T0oT1oT20T3process<T>::mode4> type4;
         typedef details::T0oT1oT2oT3_sf4<T, T0, T1, T2, T3> sf4_type;
      };

      template <typename T, typename Operation>
      class vov_node exprtk_final : public vov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // variable op variable node
         explicit vov_node(const T& var0, const T& var1)
         : v0_(var0)
         , v1_(var1)
         {}

         inline T value() const exprtk_override
         {
            return Operation::process(v0_,v1_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline const T& v0() const exprtk_override
         {
            return v0_;
         }

         inline const T& v1() const exprtk_override
         {
            return v1_;
         }

      protected:

         const T& v0_;
         const T& v1_;

      private:

         vov_node(const vov_node<T,Operation>&) exprtk_delete;
         vov_node<T,Operation>& operator=(const vov_node<T,Operation>&) exprtk_delete;
      };

      template <typename T, typename Operation>
      class cov_node exprtk_final : public cov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // constant op variable node
         explicit cov_node(const T& const_var, const T& var)
         : c_(const_var)
         , v_(var)
         {}

         inline T value() const exprtk_override
         {
            return Operation::process(c_,v_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline const T c() const exprtk_override
         {
            return c_;
         }

         inline const T& v() const exprtk_override
         {
            return v_;
         }

      protected:

         const T  c_;
         const T& v_;

      private:

         cov_node(const cov_node<T,Operation>&) exprtk_delete;
         cov_node<T,Operation>& operator=(const cov_node<T,Operation>&) exprtk_delete;
      };

      template <typename T, typename Operation>
      class voc_node exprtk_final : public voc_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // variable op constant node
         explicit voc_node(const T& var, const T& const_var)
         : v_(var)
         , c_(const_var)
         {}

         inline T value() const exprtk_override
         {
            return Operation::process(v_,c_);
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline const T c() const exprtk_override
         {
            return c_;
         }

         inline const T& v() const exprtk_override
         {
            return v_;
         }

      protected:

         const T& v_;
         const T  c_;

      private:

         voc_node(const voc_node<T,Operation>&) exprtk_delete;
         voc_node<T,Operation>& operator=(const voc_node<T,Operation>&) exprtk_delete;
      };

      template <typename T, typename Operation>
      class vob_node exprtk_final : public vob_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // variable op binary node
         explicit vob_node(const T& var, const expression_ptr branch)
         : v_(var)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return Operation::process(v_,branch_.first->value());
         }

         inline const T& v() const exprtk_override
         {
            return v_;
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return branch_.first;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         vob_node(const vob_node<T,Operation>&) exprtk_delete;
         vob_node<T,Operation>& operator=(const vob_node<T,Operation>&) exprtk_delete;

         const T& v_;
         branch_t branch_;
      };

      template <typename T, typename Operation>
      class bov_node exprtk_final : public bov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // binary node op variable node
         explicit bov_node(const expression_ptr branch, const T& var)
         : v_(var)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return Operation::process(branch_.first->value(),v_);
         }

         inline const T& v() const exprtk_override
         {
            return v_;
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return branch_.first;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         bov_node(const bov_node<T,Operation>&) exprtk_delete;
         bov_node<T,Operation>& operator=(const bov_node<T,Operation>&) exprtk_delete;

         const T& v_;
         branch_t branch_;
      };

      template <typename T, typename Operation>
      class cob_node exprtk_final : public cob_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // constant op variable node
         explicit cob_node(const T const_var, const expression_ptr branch)
         : c_(const_var)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return Operation::process(c_,branch_.first->value());
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline const T c() const exprtk_override
         {
            return c_;
         }

         inline void set_c(const T new_c) exprtk_override
         {
            (*const_cast<T*>(&c_)) = new_c;
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return branch_.first;
         }

         inline expression_node<T>* move_branch(const std::size_t&) exprtk_override
         {
            branch_.second = false;
            return branch_.first;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         cob_node(const cob_node<T,Operation>&) exprtk_delete;
         cob_node<T,Operation>& operator=(const cob_node<T,Operation>&) exprtk_delete;

         const T  c_;
         branch_t branch_;
      };

      template <typename T, typename Operation>
      class boc_node exprtk_final : public boc_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // binary node op constant node
         explicit boc_node(const expression_ptr branch, const T const_var)
         : c_(const_var)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return Operation::process(branch_.first->value(),c_);
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline const T c() const exprtk_override
         {
            return c_;
         }

         inline void set_c(const T new_c) exprtk_override
         {
            (*const_cast<T*>(&c_)) = new_c;
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return branch_.first;
         }

         inline expression_node<T>* move_branch(const std::size_t&) exprtk_override
         {
            branch_.second = false;
            return branch_.first;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         boc_node(const boc_node<T,Operation>&) exprtk_delete;
         boc_node<T,Operation>& operator=(const boc_node<T,Operation>&) exprtk_delete;

         const T  c_;
         branch_t branch_;
      };

      #ifndef exprtk_disable_string_capabilities
      template <typename T, typename SType0, typename SType1, typename Operation>
      class sos_node exprtk_final : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // string op string node
         explicit sos_node(SType0 p0, SType1 p1)
         : s0_(p0)
         , s1_(p1)
         {}

         inline T value() const exprtk_override
         {
            return Operation::process(s0_,s1_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0 s0_;
         SType1 s1_;

      private:

         sos_node(const sos_node<T,SType0,SType1,Operation>&) exprtk_delete;
         sos_node<T,SType0,SType1,Operation>& operator=(const sos_node<T,SType0,SType1,Operation>&) exprtk_delete;
      };

      template <typename T, typename SType0, typename SType1, typename RangePack, typename Operation>
      class str_xrox_node exprtk_final : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;
         typedef str_xrox_node<T,SType0,SType1,RangePack,Operation> node_type;

         // string-range op string node
         explicit str_xrox_node(SType0 p0, SType1 p1, RangePack rp0)
         : s0_ (p0 )
         , s1_ (p1 )
         , rp0_(rp0)
         {}

        ~str_xrox_node() exprtk_override
         {
            rp0_.free();
         }

         inline T value() const exprtk_override
         {
            std::size_t r0 = 0;
            std::size_t r1 = 0;

            if (rp0_(r0, r1, s0_.size()))
               return Operation::process(s0_.substr(r0, (r1 - r0) + 1), s1_);
            else
               return T(0);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0    s0_;
         SType1    s1_;
         RangePack rp0_;

      private:

         str_xrox_node(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;
      };

      template <typename T, typename SType0, typename SType1, typename RangePack, typename Operation>
      class str_xoxr_node exprtk_final : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;
         typedef str_xoxr_node<T,SType0,SType1,RangePack,Operation> node_type;

         // string op string range node
         explicit str_xoxr_node(SType0 p0, SType1 p1, RangePack rp1)
         : s0_ (p0 )
         , s1_ (p1 )
         , rp1_(rp1)
         {}

        ~str_xoxr_node()
         {
            rp1_.free();
         }

         inline T value() const exprtk_override
         {
            std::size_t r0 = 0;
            std::size_t r1 = 0;

            if (rp1_(r0, r1, s1_.size()))
            {
               return Operation::process
                      (
                         s0_,
                         s1_.substr(r0, (r1 - r0) + 1)
                      );
            }
            else
               return T(0);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0    s0_;
         SType1    s1_;
         RangePack rp1_;

      private:

         str_xoxr_node(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;
      };

      template <typename T, typename SType0, typename SType1, typename RangePack, typename Operation>
      class str_xroxr_node exprtk_final : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;
         typedef str_xroxr_node<T,SType0,SType1,RangePack,Operation> node_type;

         // string-range op string-range node
         explicit str_xroxr_node(SType0 p0, SType1 p1, RangePack rp0, RangePack rp1)
         : s0_ (p0 )
         , s1_ (p1 )
         , rp0_(rp0)
         , rp1_(rp1)
         {}

        ~str_xroxr_node() exprtk_override
         {
            rp0_.free();
            rp1_.free();
         }

         inline T value() const exprtk_override
         {
            std::size_t r0_0 = 0;
            std::size_t r0_1 = 0;
            std::size_t r1_0 = 0;
            std::size_t r1_1 = 0;

            if (
                 rp0_(r0_0, r1_0, s0_.size()) &&
                 rp1_(r0_1, r1_1, s1_.size())
               )
            {
               return Operation::process
                      (
                         s0_.substr(r0_0, (r1_0 - r0_0) + 1),
                         s1_.substr(r0_1, (r1_1 - r0_1) + 1)
                      );
            }
            else
               return T(0);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0    s0_;
         SType1    s1_;
         RangePack rp0_;
         RangePack rp1_;

      private:

         str_xroxr_node(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;
      };

      template <typename T, typename Operation>
      class str_sogens_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node <T>* expression_ptr;
         typedef string_base_node<T>* str_base_ptr;
         typedef range_pack      <T>  range_t;
         typedef range_t*             range_ptr;
         typedef range_interface <T>  irange_t;
         typedef irange_t*            irange_ptr;

         using binary_node<T>::branch;

         str_sogens_node(const operator_type& opr,
                         expression_ptr branch0,
                         expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , str0_base_ptr_ (0)
         , str1_base_ptr_ (0)
         , str0_range_ptr_(0)
         , str1_range_ptr_(0)
         , initialised_   (false)
         {
            if (is_generally_string_node(branch(0)))
            {
               str0_base_ptr_ = dynamic_cast<str_base_ptr>(branch(0));

               if (0 == str0_base_ptr_)
                  return;

               irange_ptr range = dynamic_cast<irange_ptr>(branch(0));

               if (0 == range)
                  return;

               str0_range_ptr_ = &(range->range_ref());
            }

            if (is_generally_string_node(branch(1)))
            {
               str1_base_ptr_ = dynamic_cast<str_base_ptr>(branch(1));

               if (0 == str1_base_ptr_)
                  return;

               irange_ptr range = dynamic_cast<irange_ptr>(branch(1));

               if (0 == range)
                  return;

               str1_range_ptr_ = &(range->range_ref());
            }

            initialised_ =
               str0_base_ptr_  &&
               str1_base_ptr_  &&
               str0_range_ptr_ &&
               str1_range_ptr_;

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            branch(0)->value();
            branch(1)->value();

            std::size_t str0_r0 = 0;
            std::size_t str0_r1 = 0;

            std::size_t str1_r0 = 0;
            std::size_t str1_r1 = 0;

            const range_t& range0 = (*str0_range_ptr_);
            const range_t& range1 = (*str1_range_ptr_);

            if (
                 range0(str0_r0, str0_r1, str0_base_ptr_->size()) &&
                 range1(str1_r0, str1_r1, str1_base_ptr_->size())
               )
            {
               return Operation::process
                      (
                         str0_base_ptr_->str().substr(str0_r0,(str0_r1 - str0_r0)),
                         str1_base_ptr_->str().substr(str1_r0,(str1_r1 - str1_r0))
                      );
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline bool valid() const exprtk_override
         {
            return initialised_;
         }

      private:

         str_sogens_node(const str_sogens_node<T,Operation>&) exprtk_delete;
         str_sogens_node<T,Operation>& operator=(const str_sogens_node<T,Operation>&) exprtk_delete;

         str_base_ptr str0_base_ptr_;
         str_base_ptr str1_base_ptr_;
         range_ptr    str0_range_ptr_;
         range_ptr    str1_range_ptr_;
         bool         initialised_;
      };

      template <typename T, typename SType0, typename SType1, typename SType2, typename Operation>
      class sosos_node exprtk_final : public sosos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;
         typedef sosos_node<T, SType0, SType1, SType2, Operation> node_type;

         // string op string op string node
         explicit sosos_node(SType0 p0, SType1 p1, SType2 p2)
         : s0_(p0)
         , s1_(p1)
         , s2_(p2)
         {}

         inline T value() const exprtk_override
         {
            return Operation::process(s0_, s1_, s2_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return Operation::type();
         }

         inline operator_type operation() const exprtk_override
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

         inline std::string& s2()
         {
            return s2_;
         }

      protected:

         SType0 s0_;
         SType1 s1_;
         SType2 s2_;

      private:

         sosos_node(const node_type&) exprtk_delete;
         node_type& operator=(const node_type&) exprtk_delete;
      };
      #endif

      template <typename T, typename PowOp>
      class ipow_node exprtk_final: public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef PowOp operation_t;

         explicit ipow_node(const T& v)
         : v_(v)
         {}

         inline T value() const exprtk_override
         {
            return PowOp::result(v_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_ipow;
         }

      private:

         ipow_node(const ipow_node<T,PowOp>&) exprtk_delete;
         ipow_node<T,PowOp>& operator=(const ipow_node<T,PowOp>&) exprtk_delete;

         const T& v_;
      };

      template <typename T, typename PowOp>
      class bipow_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr, bool> branch_t;
         typedef PowOp operation_t;

         explicit bipow_node(expression_ptr branch)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return PowOp::result(branch_.first->value());
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_ipow;
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         bipow_node(const bipow_node<T,PowOp>&) exprtk_delete;
         bipow_node<T,PowOp>& operator=(const bipow_node<T,PowOp>&) exprtk_delete;

         branch_t branch_;
      };

      template <typename T, typename PowOp>
      class ipowinv_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef PowOp operation_t;

         explicit ipowinv_node(const T& v)
         : v_(v)
         {}

         inline T value() const exprtk_override
         {
            return (T(1) / PowOp::result(v_));
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_ipowinv;
         }

      private:

         ipowinv_node(const ipowinv_node<T,PowOp>&) exprtk_delete;
         ipowinv_node<T,PowOp>& operator=(const ipowinv_node<T,PowOp>&) exprtk_delete;

         const T& v_;
      };

      template <typename T, typename PowOp>
      class bipowinv_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr, bool> branch_t;
         typedef PowOp operation_t;

         explicit bipowinv_node(expression_ptr branch)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return (T(1) / PowOp::result(branch_.first->value()));
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_ipowinv;
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         bipowinv_node(const bipowinv_node<T,PowOp>&) exprtk_delete;
         bipowinv_node<T,PowOp>& operator=(const bipowinv_node<T,PowOp>&) exprtk_delete;

         branch_t branch_;
      };

      template <typename T>
      inline bool is_vov_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const vov_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_cov_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const cov_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_voc_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const voc_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_cob_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const cob_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_boc_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const boc_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_t0ot1ot2_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const T0oT1oT2_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_t0ot1ot2ot3_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const T0oT1oT2oT3_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_uv_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const uv_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_string_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_stringvar == node->type());
      }

      template <typename T>
      inline bool is_string_range_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_stringvarrng == node->type());
      }

      template <typename T>
      inline bool is_const_string_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_stringconst == node->type());
      }

      template <typename T>
      inline bool is_const_string_range_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_cstringvarrng == node->type());
      }

      template <typename T>
      inline bool is_string_assignment_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_strass == node->type());
      }

      template <typename T>
      inline bool is_string_concat_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_strconcat == node->type());
      }

      template <typename T>
      inline bool is_string_function_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_strfunction == node->type());
      }

      template <typename T>
      inline bool is_string_condition_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_strcondition == node->type());
      }

      template <typename T>
      inline bool is_string_ccondition_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_strccondition == node->type());
      }

      template <typename T>
      inline bool is_string_vararg_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_stringvararg == node->type());
      }

      template <typename T>
      inline bool is_genricstring_range_node(const expression_node<T>* node)
      {
         return node && (expression_node<T>::e_strgenrange == node->type());
      }

      template <typename T>
      inline bool is_generally_string_node(const expression_node<T>* node)
      {
         if (node)
         {
            switch (node->type())
            {
               case expression_node<T>::e_stringvar     :
               case expression_node<T>::e_stringconst   :
               case expression_node<T>::e_stringvarrng  :
               case expression_node<T>::e_cstringvarrng :
               case expression_node<T>::e_strgenrange   :
               case expression_node<T>::e_strass        :
               case expression_node<T>::e_strconcat     :
               case expression_node<T>::e_strfunction   :
               case expression_node<T>::e_strcondition  :
               case expression_node<T>::e_strccondition :
               case expression_node<T>::e_stringvararg  : return true;
               default                                  : return false;
            }
         }

         return false;
      }

      template <typename T>
      inline bool is_loop_node(const expression_node<T>* node)
      {
         if (node)
         {
            switch (node->type())
            {
               case expression_node<T>::e_for    :
               case expression_node<T>::e_repeat :
               case expression_node<T>::e_while  : return true;
               default                           : return false;
            }
         }

         return false;
      }

      template <typename T>
      inline bool is_block_node(const expression_node<T>* node)
      {
         if (node)
         {
            if (is_loop_node(node))
            {
               return true;
            }

            switch (node->type())
            {
               case expression_node<T>::e_conditional :
               case expression_node<T>::e_mswitch     :
               case expression_node<T>::e_switch      :
               case expression_node<T>::e_vararg      : return true;
               default                                : return false;
            }
         }

         return false;
      }

   } // namespace details
} // namespace exprtk

#endif // EXPRTK_SRC_NODES_COMPOUND_HPP
