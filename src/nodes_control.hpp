#ifndef EXPRTK_SRC_NODES_CONTROL_HPP
#define EXPRTK_SRC_NODES_CONTROL_HPP

#include "nodes_base.hpp"

namespace exprtk
{
   namespace details
   {

      template <typename T>
      class null_node exprtk_final : public expression_node<T>
      {
      public:

         inline T value() const exprtk_override
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_null;
         }
      };

      template <typename T, std::size_t N>
      inline void construct_branch_pair(std::pair<expression_node<T>*,bool> (&branch)[N],
                                        expression_node<T>* b,
                                        const std::size_t& index)
      {
         if (b && (index < N))
         {
            branch[index] = std::make_pair(b,branch_deletable(b));
         }
      }

      template <typename T>
      inline void construct_branch_pair(std::pair<expression_node<T>*,bool>& branch, expression_node<T>* b)
      {
         if (b)
         {
            branch = std::make_pair(b,branch_deletable(b));
         }
      }

      template <std::size_t N, typename T>
      inline void init_branches(std::pair<expression_node<T>*,bool> (&branch)[N],
                                expression_node<T>* b0,
                                expression_node<T>* b1 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b2 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b3 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b4 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b5 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b6 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b7 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b8 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b9 = reinterpret_cast<expression_node<T>*>(0))
      {
         construct_branch_pair(branch, b0, 0);
         construct_branch_pair(branch, b1, 1);
         construct_branch_pair(branch, b2, 2);
         construct_branch_pair(branch, b3, 3);
         construct_branch_pair(branch, b4, 4);
         construct_branch_pair(branch, b5, 5);
         construct_branch_pair(branch, b6, 6);
         construct_branch_pair(branch, b7, 7);
         construct_branch_pair(branch, b8, 8);
         construct_branch_pair(branch, b9, 9);
      }

      template <typename T>
      class null_eq_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         explicit null_eq_node(expression_ptr branch, const bool equality = true)
         : equality_(equality)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            const T v = branch_.first->value();
            const bool result = details::numeric::is_nan(v);

            if (result)
               return equality_ ? T(1) : T(0);
            else
               return equality_ ? T(0) : T(1);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_nulleq;
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return branch_.first;
         }

         inline bool valid() const exprtk_override
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

         bool equality_;
         branch_t branch_;
      };

      template <typename T>
      class literal_node exprtk_final : public expression_node<T>
      {
      public:

         explicit literal_node(const T& v)
         : value_(v)
         {}

         inline T value() const exprtk_override
         {
            return value_;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_constant;
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return reinterpret_cast<expression_node<T>*>(0);
         }

      private:

         literal_node(const literal_node<T>&) exprtk_delete;
         literal_node<T>& operator=(const literal_node<T>&) exprtk_delete;

         const T value_;
      };

      template <typename T>
      struct range_pack;

      template <typename T>
      struct range_data_type;

      template <typename T>
      class range_interface
      {
      public:

         typedef range_pack<T> range_t;

         virtual ~range_interface()
         {}

         virtual range_t& range_ref() = 0;

         virtual const range_t& range_ref() const = 0;
      };

      #ifndef exprtk_disable_string_capabilities
      template <typename T>
      class string_base_node
      {
      public:

         typedef range_data_type<T> range_data_type_t;

         virtual ~string_base_node()
         {}

         virtual std::string str () const = 0;

         virtual char_cptr   base() const = 0;

         virtual std::size_t size() const = 0;
      };

      template <typename T>
      class string_literal_node exprtk_final
                                : public expression_node <T>
                                , public string_base_node<T>
                                , public range_interface <T>
      {
      public:

         typedef range_pack<T> range_t;

         explicit string_literal_node(const std::string& v)
         : value_(v)
         {
            rp_.n0_c = std::make_pair<bool,std::size_t>(true, 0);
            rp_.n1_c = std::make_pair<bool,std::size_t>(true, v.size());
            rp_.cache.first  = rp_.n0_c.second;
            rp_.cache.second = rp_.n1_c.second;
         }

         inline T value() const exprtk_override
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_stringconst;
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return reinterpret_cast<expression_node<T>*>(0);
         }

         std::string str() const exprtk_override
         {
            return value_;
         }

         char_cptr base() const exprtk_override
         {
            return value_.data();
         }

         std::size_t size() const exprtk_override
         {
            return value_.size();
         }

         range_t& range_ref() exprtk_override
         {
            return rp_;
         }

         const range_t& range_ref() const exprtk_override
         {
            return rp_;
         }

      private:

         string_literal_node(const string_literal_node<T>&) exprtk_delete;
         string_literal_node<T>& operator=(const string_literal_node<T>&) exprtk_delete;

         const std::string value_;
         range_t rp_;
      };
      #endif

      template <typename T>
      class unary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         unary_node(const operator_type& opr, expression_ptr branch)
         : operation_(opr)
         {
            construct_branch_pair(branch_,branch);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return numeric::process<T>
                     (operation_,branch_.first->value());
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_unary;
         }

         inline operator_type operation()
         {
            return operation_;
         }

         inline expression_node<T>* branch(const std::size_t&) const exprtk_override
         {
            return branch_.first;
         }

         inline bool valid() const exprtk_override
         {
            return branch_.first && branch_.first->valid();
         }

         inline void release()
         {
            branch_.second = false;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_final
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         operator_type operation_;
         branch_t branch_;
      };

      template <typename T>
      class binary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         binary_node(const operator_type& opr,
                     expression_ptr branch0,
                     expression_ptr branch1)
         : operation_(opr)
         {
            init_branches<2>(branch_, branch0, branch1);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return numeric::process<T>
                   (
                      operation_,
                      branch_[0].first->value(),
                      branch_[1].first->value()
                   );
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_binary;
         }

         inline operator_type operation()
         {
            return operation_;
         }

         inline expression_node<T>* branch(const std::size_t& index = 0) const exprtk_override
         {
            assert(index < 2);
            return branch_[index].first;
         }

         inline bool valid() const exprtk_override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_final
         {
            return expression_node<T>::ndb_t::template compute_node_depth<2>(branch_);
         }

      private:

         operator_type operation_;
         branch_t branch_[2];
      };

      template <typename T, typename Operation>
      class binary_ext_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         binary_ext_node(expression_ptr branch0, expression_ptr branch1)
         {
            init_branches<2>(branch_, branch0, branch1);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            return Operation::process(arg0,arg1);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_binary_ext;
         }

         inline operator_type operation()
         {
            return Operation::operation();
         }

         inline expression_node<T>* branch(const std::size_t& index = 0) const exprtk_override
         {
            assert(index < 2);
            return branch_[index].first;
         }

         inline bool valid() const exprtk_override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::template compute_node_depth<2>(branch_);
         }

      protected:

         branch_t branch_[2];
      };

      template <typename T>
      class trinary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         trinary_node(const operator_type& opr,
                      expression_ptr branch0,
                      expression_ptr branch1,
                      expression_ptr branch2)
         : operation_(opr)
         {
            init_branches<3>(branch_, branch0, branch1, branch2);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();

            switch (operation_)
            {
               case e_inrange : return (arg1 < arg0) ? T(0) : ((arg1 > arg2) ? T(0) : T(1));

               case e_clamp   : return (arg1 < arg0) ? arg0 : (arg1 > arg2 ? arg2 : arg1);

               case e_iclamp  : if ((arg1 <= arg0) || (arg1 >= arg2))
                                   return arg1;
                                else
                                   return ((T(2) * arg1  <= (arg2 + arg0)) ? arg0 : arg2);

               default        : exprtk_debug(("trinary_node::value() - Error: Invalid operation\n"));
                                return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_trinary;
         }

         inline bool valid() const exprtk_override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() &&
               branch_[2].first && branch_[2].first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override exprtk_final
         {
            return expression_node<T>::ndb_t::template compute_node_depth<3>(branch_);
         }

      protected:

         operator_type operation_;
         branch_t branch_[3];
      };

      template <typename T>
      class quaternary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         quaternary_node(const operator_type& opr,
                         expression_ptr branch0,
                         expression_ptr branch1,
                         expression_ptr branch2,
                         expression_ptr branch3)
         : operation_(opr)
         {
            init_branches<4>(branch_, branch0, branch1, branch2, branch3);
         }

         inline T value() const exprtk_override
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_quaternary;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override exprtk_final
         {
            return expression_node<T>::ndb_t::template compute_node_depth<4>(branch_);
         }

         inline bool valid() const exprtk_override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() &&
               branch_[2].first && branch_[2].first->valid() &&
               branch_[3].first && branch_[3].first->valid() ;
         }

      protected:

         operator_type operation_;
         branch_t branch_[4];
      };

      template <typename T>
      class conditional_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         conditional_node(expression_ptr condition,
                          expression_ptr consequent,
                          expression_ptr alternative)
         {
            construct_branch_pair(condition_  , condition  );
            construct_branch_pair(consequent_ , consequent );
            construct_branch_pair(alternative_, alternative);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            if (is_true(condition_))
               return consequent_.first->value();
            else
               return alternative_.first->value();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_conditional;
         }

         inline bool valid() const exprtk_override
         {
            return
               condition_  .first && condition_  .first->valid() &&
               consequent_ .first && consequent_ .first->valid() &&
               alternative_.first && alternative_.first->valid() ;
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

         branch_t condition_;
         branch_t consequent_;
         branch_t alternative_;
      };

      template <typename T>
      class cons_conditional_node exprtk_final : public expression_node<T>
      {
      public:

         // Consequent only conditional statement node
         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         cons_conditional_node(expression_ptr condition,
                               expression_ptr consequent)
         {
            construct_branch_pair(condition_ , condition );
            construct_branch_pair(consequent_, consequent);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            if (is_true(condition_))
               return consequent_.first->value();
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_conditional;
         }

         inline bool valid() const exprtk_override
         {
            return
               condition_ .first && condition_ .first->valid() &&
               consequent_.first && consequent_.first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(condition_  , node_delete_list);
            expression_node<T>::ndb_t::collect(consequent_ , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::
               compute_node_depth(condition_, consequent_);
         }

      private:

         branch_t condition_;
         branch_t consequent_;
      };

      #ifndef exprtk_disable_break_continue
      template <typename T>
      class break_exception
      {
      public:

         explicit break_exception(const T& v)
         : value(v)
         {}

         T value;
      };

      class continue_exception {};

      template <typename T>
      class break_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         explicit break_node(expression_ptr ret = expression_ptr(0))
         {
            construct_branch_pair(return_, ret);
         }

         inline T value() const exprtk_override
         {
            const T result = return_.first ?
                             return_.first->value() :
                             std::numeric_limits<T>::quiet_NaN();

            throw break_exception<T>(result);

            #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
            return std::numeric_limits<T>::quiet_NaN();
            #endif
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_break;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(return_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(return_);
         }

      private:

         branch_t return_;
      };

      template <typename T>
      class continue_node exprtk_final : public expression_node<T>
      {
      public:

         inline T value() const exprtk_override
         {
            throw continue_exception();
            #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
            return std::numeric_limits<T>::quiet_NaN();
            #endif
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_break;
         }
      };
      #endif

      struct loop_runtime_checker
      {
         loop_runtime_checker(loop_runtime_check_ptr loop_runtime_check,
                              loop_runtime_check::loop_types lp_typ = loop_runtime_check::e_invalid)
         : iteration_count_(0)
         , loop_runtime_check_(loop_runtime_check)
         , max_loop_iterations_(loop_runtime_check_->max_loop_iterations)
         , loop_type_(lp_typ)
         {
            assert(loop_runtime_check_);
         }

         inline void reset(const _uint64_t initial_value = 0) const
         {
            iteration_count_ = initial_value;
         }

         inline bool check() const
         {
            assert(loop_runtime_check_);

            if (
                 (++iteration_count_ <= max_loop_iterations_) &&
                 loop_runtime_check_->check()
               )
            {
               return true;
            }

            loop_runtime_check::violation_context ctxt;
            ctxt.loop      = loop_type_;
            ctxt.violation = loop_runtime_check::e_iteration_count;

            loop_runtime_check_->handle_runtime_violation(ctxt);

            return false;
         }

         bool valid() const
         {
            return 0 != loop_runtime_check_;
         }

         mutable _uint64_t iteration_count_;
         mutable loop_runtime_check_ptr loop_runtime_check_;
         const details::_uint64_t& max_loop_iterations_;
         loop_runtime_check::loop_types loop_type_;
      };

      template <typename T>
      class while_loop_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         while_loop_node(expression_ptr condition,
                         expression_ptr loop_body)
         {
            construct_branch_pair(condition_, condition);
            construct_branch_pair(loop_body_, loop_body);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            while (is_true(condition_))
            {
               result = loop_body_.first->value();
            }

            return result;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_while;
         }

         inline bool valid() const exprtk_override
         {
            return
               condition_.first && condition_.first->valid() &&
               loop_body_.first && loop_body_.first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(condition_ , node_delete_list);
            expression_node<T>::ndb_t::collect(loop_body_ , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(condition_, loop_body_);
         }

      protected:

         branch_t condition_;
         branch_t loop_body_;
      };

      template <typename T>
      class while_loop_rtc_node exprtk_final
                                : public while_loop_node<T>
                                , public loop_runtime_checker
      {
      public:

         typedef while_loop_node<T>  parent_t;
         typedef expression_node<T>* expression_ptr;

         while_loop_rtc_node(expression_ptr condition,
                             expression_ptr loop_body,
                             loop_runtime_check_ptr loop_rt_chk)
         : parent_t(condition, loop_body)
         , loop_runtime_checker(loop_rt_chk, loop_runtime_check::e_while_loop)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {

            T result = T(0);

            loop_runtime_checker::reset();

            while (is_true(parent_t::condition_) && loop_runtime_checker::check())
            {
               result = parent_t::loop_body_.first->value();
            }

            return result;
         }

         using parent_t::valid;

         bool valid() const exprtk_override exprtk_final
         {
            return parent_t::valid() &&
                   loop_runtime_checker::valid();
         }
      };

      template <typename T>
      class repeat_until_loop_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         repeat_until_loop_node(expression_ptr condition,
                                expression_ptr loop_body)
         {
            construct_branch_pair(condition_, condition);
            construct_branch_pair(loop_body_, loop_body);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            do
            {
               result = loop_body_.first->value();
            }
            while (is_false(condition_.first));

            return result;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_repeat;
         }

         inline bool valid() const exprtk_override
         {
            return
               condition_.first && condition_.first->valid() &&
               loop_body_.first && loop_body_.first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(condition_ , node_delete_list);
            expression_node<T>::ndb_t::collect(loop_body_ , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(condition_, loop_body_);
         }

      protected:

         branch_t condition_;
         branch_t loop_body_;
      };

      template <typename T>
      class repeat_until_loop_rtc_node exprtk_final
                                       : public repeat_until_loop_node<T>
                                       , public loop_runtime_checker
      {
      public:

         typedef repeat_until_loop_node<T> parent_t;
         typedef expression_node<T>*       expression_ptr;

         repeat_until_loop_rtc_node(expression_ptr condition,
                                    expression_ptr loop_body,
                                    loop_runtime_check_ptr loop_rt_chk)
         : parent_t(condition, loop_body)
         , loop_runtime_checker(loop_rt_chk, loop_runtime_check::e_repeat_until_loop)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            loop_runtime_checker::reset(1);

            do
            {
               result = parent_t::loop_body_.first->value();
            }
            while (is_false(parent_t::condition_.first) && loop_runtime_checker::check());

            return result;
         }

         using parent_t::valid;

         inline bool valid() const exprtk_override exprtk_final
         {
            return parent_t::valid() &&
                   loop_runtime_checker::valid();
         }
      };

      template <typename T>
      class for_loop_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         for_loop_node(expression_ptr initialiser,
                       expression_ptr condition,
                       expression_ptr incrementor,
                       expression_ptr loop_body)
         {
            construct_branch_pair(initialiser_, initialiser);
            construct_branch_pair(condition_  , condition  );
            construct_branch_pair(incrementor_, incrementor);
            construct_branch_pair(loop_body_  , loop_body  );
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            if (initialiser_.first)
               initialiser_.first->value();

            if (incrementor_.first)
            {
               while (is_true(condition_))
               {
                  result = loop_body_.first->value();
                  incrementor_.first->value();
               }
            }
            else
            {
               while (is_true(condition_))
               {
                  result = loop_body_.first->value();
               }
            }

            return result;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_for;
         }

         inline bool valid() const exprtk_override
         {
            return condition_.first && loop_body_.first;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_ , node_delete_list);
            expression_node<T>::ndb_t::collect(condition_   , node_delete_list);
            expression_node<T>::ndb_t::collect(incrementor_ , node_delete_list);
            expression_node<T>::ndb_t::collect(loop_body_   , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (initialiser_, condition_, incrementor_, loop_body_);
         }

      protected:

         branch_t initialiser_;
         branch_t condition_  ;
         branch_t incrementor_;
         branch_t loop_body_  ;
      };

      template <typename T>
      class for_loop_rtc_node exprtk_final
                              : public for_loop_node<T>
                              , public loop_runtime_checker
      {
      public:

         typedef for_loop_node<T>    parent_t;
         typedef expression_node<T>* expression_ptr;

         for_loop_rtc_node(expression_ptr initialiser,
                           expression_ptr condition,
                           expression_ptr incrementor,
                           expression_ptr loop_body,
                           loop_runtime_check_ptr loop_rt_chk)
         : parent_t(initialiser, condition, incrementor, loop_body)
         , loop_runtime_checker(loop_rt_chk, loop_runtime_check::e_for_loop)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            loop_runtime_checker::reset();

            if (parent_t::initialiser_.first)
               parent_t::initialiser_.first->value();

            if (parent_t::incrementor_.first)
            {
               while (is_true(parent_t::condition_) && loop_runtime_checker::check())
               {
                  result = parent_t::loop_body_.first->value();
                  parent_t::incrementor_.first->value();
               }
            }
            else
            {
               while (is_true(parent_t::condition_) && loop_runtime_checker::check())
               {
                  result = parent_t::loop_body_.first->value();
               }
            }

            return result;
         }

         using parent_t::valid;

         inline bool valid() const exprtk_override exprtk_final
         {
            return parent_t::valid() &&
                   loop_runtime_checker::valid();
         }
      };

      #ifndef exprtk_disable_break_continue
      template <typename T>
      class while_loop_bc_node : public while_loop_node<T>
      {
      public:

         typedef while_loop_node<T>  parent_t;
         typedef expression_node<T>* expression_ptr;

         while_loop_bc_node(expression_ptr condition,
                            expression_ptr loop_body)
         : parent_t(condition, loop_body)
         {
            assert(parent_t::valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            while (is_true(parent_t::condition_))
            {
               try
               {
                  result = parent_t::loop_body_.first->value();
               }
               catch(const break_exception<T>& e)
               {
                  return e.value;
               }
               catch(const continue_exception&)
               {}
            }

            return result;
         }
      };

      template <typename T>
      class while_loop_bc_rtc_node exprtk_final
                                   : public while_loop_bc_node<T>
                                   , public loop_runtime_checker
      {
      public:

         typedef while_loop_bc_node<T> parent_t;
         typedef expression_node<T>*   expression_ptr;

         while_loop_bc_rtc_node(expression_ptr condition,
                                expression_ptr loop_body,
                                loop_runtime_check_ptr loop_rt_chk)
         : parent_t(condition, loop_body)
         , loop_runtime_checker(loop_rt_chk, loop_runtime_check::e_while_loop)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            loop_runtime_checker::reset();

            while (is_true(parent_t::condition_) && loop_runtime_checker::check())
            {
               try
               {
                  result = parent_t::loop_body_.first->value();
               }
               catch(const break_exception<T>& e)
               {
                  return e.value;
               }
               catch(const continue_exception&)
               {}
            }

            return result;
         }

         using parent_t::valid;

         inline bool valid() const exprtk_override exprtk_final
         {
            return parent_t::valid() &&
                   loop_runtime_checker::valid();
         }
      };

      template <typename T>
      class repeat_until_loop_bc_node : public repeat_until_loop_node<T>
      {
      public:

         typedef repeat_until_loop_node<T> parent_t;
         typedef expression_node<T>*       expression_ptr;

         repeat_until_loop_bc_node(expression_ptr condition,
                                   expression_ptr loop_body)
         : parent_t(condition, loop_body)
         {
            assert(parent_t::valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            do
            {
               try
               {
                  result = parent_t::loop_body_.first->value();
               }
               catch(const break_exception<T>& e)
               {
                  return e.value;
               }
               catch(const continue_exception&)
               {}
            }
            while (is_false(parent_t::condition_.first));

            return result;
         }
      };

      template <typename T>
      class repeat_until_loop_bc_rtc_node exprtk_final
                                          : public repeat_until_loop_bc_node<T>
                                          , public loop_runtime_checker
      {
      public:

         typedef repeat_until_loop_bc_node<T> parent_t;
         typedef expression_node<T>*          expression_ptr;

         repeat_until_loop_bc_rtc_node(expression_ptr condition,
                                       expression_ptr loop_body,
                                       loop_runtime_check_ptr loop_rt_chk)
         : parent_t(condition, loop_body)
         , loop_runtime_checker(loop_rt_chk, loop_runtime_check::e_repeat_until_loop)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            loop_runtime_checker::reset();

            do
            {
               try
               {
                  result = parent_t::loop_body_.first->value();
               }
               catch(const break_exception<T>& e)
               {
                  return e.value;
               }
               catch(const continue_exception&)
               {}
            }
            while (is_false(parent_t::condition_.first) && loop_runtime_checker::check());

            return result;
         }

         using parent_t::valid;

         inline bool valid() const exprtk_override exprtk_final
         {
            return parent_t::valid() &&
                   loop_runtime_checker::valid();
         }
      };

      template <typename T>
      class for_loop_bc_node : public for_loop_node<T>
      {
      public:

         typedef for_loop_node<T>    parent_t;
         typedef expression_node<T>* expression_ptr;

         for_loop_bc_node(expression_ptr initialiser,
                          expression_ptr condition,
                          expression_ptr incrementor,
                          expression_ptr loop_body)
         : parent_t(initialiser, condition, incrementor, loop_body)
         {
            assert(parent_t::valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            if (parent_t::initialiser_.first)
               parent_t::initialiser_.first->value();

            if (parent_t::incrementor_.first)
            {
               while (is_true(parent_t::condition_))
               {
                  try
                  {
                     result = parent_t::loop_body_.first->value();
                  }
                  catch(const break_exception<T>& e)
                  {
                     return e.value;
                  }
                  catch(const continue_exception&)
                  {}

                  parent_t::incrementor_.first->value();
               }
            }
            else
            {
               while (is_true(parent_t::condition_))
               {
                  try
                  {
                     result = parent_t::loop_body_.first->value();
                  }
                  catch(const break_exception<T>& e)
                  {
                     return e.value;
                  }
                  catch(const continue_exception&)
                  {}
               }
            }

            return result;
         }
      };

      template <typename T>
      class for_loop_bc_rtc_node exprtk_final
                                 : public for_loop_bc_node<T>
                                 , public loop_runtime_checker
      {
      public:

         typedef for_loop_bc_node<T> parent_t;
         typedef expression_node<T>* expression_ptr;

         for_loop_bc_rtc_node(expression_ptr initialiser,
                              expression_ptr condition,
                              expression_ptr incrementor,
                              expression_ptr loop_body,
                              loop_runtime_check_ptr loop_rt_chk)
         : parent_t(initialiser, condition, incrementor, loop_body)
         , loop_runtime_checker(loop_rt_chk, loop_runtime_check::e_for_loop)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T result = T(0);

            loop_runtime_checker::reset();

            if (parent_t::initialiser_.first)
               parent_t::initialiser_.first->value();

            if (parent_t::incrementor_.first)
            {
               while (is_true(parent_t::condition_) && loop_runtime_checker::check())
               {
                  try
                  {
                     result = parent_t::loop_body_.first->value();
                  }
                  catch(const break_exception<T>& e)
                  {
                     return e.value;
                  }
                  catch(const continue_exception&)
                  {}

                  parent_t::incrementor_.first->value();
               }
            }
            else
            {
               while (is_true(parent_t::condition_) && loop_runtime_checker::check())
               {
                  try
                  {
                     result = parent_t::loop_body_.first->value();
                  }
                  catch(const break_exception<T>& e)
                  {
                     return e.value;
                  }
                  catch(const continue_exception&)
                  {}
               }
            }

            return result;
         }

         using parent_t::valid;

         inline bool valid() const exprtk_override exprtk_final
         {
            return parent_t::valid() &&
                   loop_runtime_checker::valid();
         }
      };
      #endif

      template <typename T>
      class switch_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         explicit switch_node(const Sequence<expression_ptr,Allocator>& arg_list)
         {
            if (1 != (arg_list.size() & 1))
               return;

            arg_list_.resize(arg_list.size());

            for (std::size_t i = 0; i < arg_list.size(); ++i)
            {
               if (arg_list[i] && arg_list[i]->valid())
               {
                  construct_branch_pair(arg_list_[i], arg_list[i]);
               }
               else
               {
                  arg_list_.clear();
                  return;
               }
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            const std::size_t upper_bound = (arg_list_.size() - 1);

            for (std::size_t i = 0; i < upper_bound; i += 2)
            {
               expression_ptr condition  = arg_list_[i    ].first;
               expression_ptr consequent = arg_list_[i + 1].first;

               if (is_true(condition))
               {
                  return consequent->value();
               }
            }

            return arg_list_[upper_bound].first->value();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override exprtk_final
         {
            return expression_node<T>::e_switch;
         }

         inline bool valid() const exprtk_override
         {
            return !arg_list_.empty();
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(arg_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override exprtk_final
         {
            return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
         }

      protected:

         std::vector<branch_t> arg_list_;
      };

      template <typename T, typename Switch_N>
      class switch_n_node exprtk_final : public switch_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         explicit switch_n_node(const Sequence<expression_ptr,Allocator>& arg_list)
         : switch_node<T>(arg_list)
         {}

         inline T value() const exprtk_override
         {
            return Switch_N::process(switch_node<T>::arg_list_);
         }
      };

      template <typename T>
      class multi_switch_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         explicit multi_switch_node(const Sequence<expression_ptr,Allocator>& arg_list)
         {
            if (0 != (arg_list.size() & 1))
               return;

            arg_list_.resize(arg_list.size());

            for (std::size_t i = 0; i < arg_list.size(); ++i)
            {
               if (arg_list[i] && arg_list[i]->valid())
               {
                  construct_branch_pair(arg_list_[i], arg_list[i]);
               }
               else
               {
                  arg_list_.clear();
                  return;
               }
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            const std::size_t upper_bound = (arg_list_.size() - 1);

            T result = T(0);

            for (std::size_t i = 0; i < upper_bound; i += 2)
            {
               expression_ptr condition  = arg_list_[i    ].first;
               expression_ptr consequent = arg_list_[i + 1].first;

               if (is_true(condition))
               {
                  result = consequent->value();
               }
            }

            return result;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_mswitch;
         }

         inline bool valid() const exprtk_override
         {
            return !arg_list_.empty() && (0 == (arg_list_.size() % 2));
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(arg_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override exprtk_final
         {
            return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
         }

      private:

         std::vector<branch_t> arg_list_;
      };

   } // namespace details
} // namespace exprtk

#endif // EXPRTK_SRC_NODES_CONTROL_HPP
