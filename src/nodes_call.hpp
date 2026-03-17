#ifndef EXPRTK_SRC_NODES_CALL_HPP
#define EXPRTK_SRC_NODES_CALL_HPP

#include "nodes_base.hpp"
#include "nodes_variable.hpp"

namespace exprtk
{
   namespace details
   {

      template <typename T, typename IFunction, std::size_t N>
      class function_N_node exprtk_final : public expression_node<T>
      {
      public:

         // Function of N parameters.
         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef IFunction ifunction;

         explicit function_N_node(ifunction* func)
         : function_((N == func->param_count) ? func : reinterpret_cast<ifunction*>(0))
         , parameter_count_(func->param_count)
         , initialised_(false)
         {}

         template <std::size_t NumBranches>
         bool init_branches(expression_ptr (&b)[NumBranches])
         {
            // Needed for incompetent and broken msvc compiler versions
            #ifdef _MSC_VER
             #pragma warning(push)
             #pragma warning(disable: 4127)
            #endif

            if (N != NumBranches)
            {
               return false;
            }

            for (std::size_t i = 0; i < NumBranches; ++i)
            {
               if (b[i] && b[i]->valid())
                  branch_[i] = std::make_pair(b[i],branch_deletable(b[i]));
               else
                  return false;
            }

            initialised_ = function_;
            assert(valid());
            return initialised_;

            #ifdef _MSC_VER
             #pragma warning(pop)
            #endif
         }

         inline bool operator <(const function_N_node<T,IFunction,N>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const exprtk_override
         {
            // Needed for incompetent and broken msvc compiler versions
            #ifdef _MSC_VER
             #pragma warning(push)
             #pragma warning(disable: 4127)
            #endif

            T v[N];
            evaluate_branches<T,N>::execute(v,branch_);
            return invoke<T,N>::execute(*function_,v);

            #ifdef _MSC_VER
             #pragma warning(pop)
            #endif
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_function;
         }

         inline bool valid() const exprtk_override
         {
            return initialised_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::template compute_node_depth<N>(branch_);
         }

         template <typename T_, std::size_t BranchCount>
         struct evaluate_branches
         {
            static inline void execute(T_ (&v)[BranchCount], const branch_t (&b)[BranchCount])
            {
               for (std::size_t i = 0; i < BranchCount; ++i)
               {
                  v[i] = b[i].first->value();
               }
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,6>
         {
            static inline void execute(T_ (&v)[6], const branch_t (&b)[6])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
               v[2] = b[2].first->value();
               v[3] = b[3].first->value();
               v[4] = b[4].first->value();
               v[5] = b[5].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,5>
         {
            static inline void execute(T_ (&v)[5], const branch_t (&b)[5])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
               v[2] = b[2].first->value();
               v[3] = b[3].first->value();
               v[4] = b[4].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,4>
         {
            static inline void execute(T_ (&v)[4], const branch_t (&b)[4])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
               v[2] = b[2].first->value();
               v[3] = b[3].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,3>
         {
            static inline void execute(T_ (&v)[3], const branch_t (&b)[3])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
               v[2] = b[2].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,2>
         {
            static inline void execute(T_ (&v)[2], const branch_t (&b)[2])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,1>
         {
            static inline void execute(T_ (&v)[1], const branch_t (&b)[1])
            {
               v[0] = b[0].first->value();
            }
         };

         template <typename T_, std::size_t ParamCount>
         struct invoke { static inline T execute(ifunction&, branch_t (&)[ParamCount]) { return std::numeric_limits<T_>::quiet_NaN(); } };

         template <typename T_>
         struct invoke<T_,20>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[20])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16],v[17],v[18],v[19]); }
         };

         template <typename T_>
         struct invoke<T_,19>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[19])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16],v[17],v[18]); }
         };

         template <typename T_>
         struct invoke<T_,18>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[18])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17]); }
         };

         template <typename T_>
         struct invoke<T_,17>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[17])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16]); }
         };

         template <typename T_>
         struct invoke<T_,16>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[16])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15]); }
         };

         template <typename T_>
         struct invoke<T_,15>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[15])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14]); }
         };

         template <typename T_>
         struct invoke<T_,14>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[14])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13]); }
         };

         template <typename T_>
         struct invoke<T_,13>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[13])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12]); }
         };

         template <typename T_>
         struct invoke<T_,12>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[12])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11]); }
         };

         template <typename T_>
         struct invoke<T_,11>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[11])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10]); }
         };

         template <typename T_>
         struct invoke<T_,10>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[10])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9]); }
         };

         template <typename T_>
         struct invoke<T_,9>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[9])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]); }
         };

         template <typename T_>
         struct invoke<T_,8>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[8])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]); }
         };

         template <typename T_>
         struct invoke<T_,7>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[7])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6]); }
         };

         template <typename T_>
         struct invoke<T_,6>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[6])
            { return f(v[0], v[1], v[2], v[3], v[4], v[5]); }
         };

         template <typename T_>
         struct invoke<T_,5>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[5])
            { return f(v[0], v[1], v[2], v[3], v[4]); }
         };

         template <typename T_>
         struct invoke<T_,4>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[4])
            { return f(v[0], v[1], v[2], v[3]); }
         };

         template <typename T_>
         struct invoke<T_,3>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[3])
            { return f(v[0], v[1], v[2]); }
         };

         template <typename T_>
         struct invoke<T_,2>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[2])
            { return f(v[0], v[1]); }
         };

         template <typename T_>
         struct invoke<T_,1>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[1])
            { return f(v[0]); }
         };

      private:

         ifunction*  function_;
         std::size_t parameter_count_;
         branch_t    branch_[N];
         bool        initialised_;
      };

      template <typename T, typename IFunction>
      class function_N_node<T,IFunction,0> exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef IFunction ifunction;

         explicit function_N_node(ifunction* func)
         : function_((0 == func->param_count) ? func : reinterpret_cast<ifunction*>(0))
         {
            assert(valid());
         }

         inline bool operator <(const function_N_node<T,IFunction,0>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const exprtk_override
         {
            return (*function_)();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_function;
         }

         inline bool valid() const exprtk_override
         {
            return function_;
         }

      private:

         ifunction* function_;
      };

      template <typename T, typename VarArgFunction>
      class vararg_function_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vararg_function_node(VarArgFunction*  func,
                              const std::vector<expression_ptr>& arg_list)
         : function_(func)
         , arg_list_(arg_list)
         {
            value_list_.resize(arg_list.size(),std::numeric_limits<T>::quiet_NaN());
            assert(valid());
         }

         inline bool operator <(const vararg_function_node<T,VarArgFunction>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const exprtk_override
         {
            populate_value_list();
            return (*function_)(value_list_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vafunction;
         }

         inline bool valid() const exprtk_override
         {
            return function_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               if (arg_list_[i] && !details::is_variable_node(arg_list_[i]))
               {
                  node_delete_list.push_back(&arg_list_[i]);
               }
            }
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
         }

      private:

         inline void populate_value_list() const
         {
            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               value_list_[i] = arg_list_[i]->value();
            }
         }

         VarArgFunction* function_;
         std::vector<expression_ptr> arg_list_;
         mutable std::vector<T> value_list_;
      };

      template <typename T, typename GenericFunction>
      class generic_function_node : public expression_node<T>
      {
      public:

         typedef type_store<T>       type_store_t;
         typedef expression_node<T>* expression_ptr;
         typedef variable_node<T>    variable_node_t;
         typedef vector_node<T>      vector_node_t;
         typedef variable_node_t*    variable_node_ptr_t;
         typedef vector_node_t*      vector_node_ptr_t;
         typedef range_interface<T>  range_interface_t;
         typedef range_data_type<T>  range_data_type_t;
         typedef typename range_interface<T>::range_t range_t;

         typedef std::pair<expression_ptr,bool> branch_t;
         typedef vector_holder<T>* vh_t;
         typedef vector_view<T>*   vecview_t;

         typedef std::vector<T>                 tmp_vs_t;
         typedef std::vector<type_store_t>      typestore_list_t;
         typedef std::vector<range_data_type_t> range_list_t;

         explicit generic_function_node(const std::vector<expression_ptr>& arg_list,
                                        GenericFunction* func = reinterpret_cast<GenericFunction*>(0))
         : function_(func)
         , arg_list_(arg_list)
         {}

         virtual ~generic_function_node()
         {
            for (std::size_t i = 0; i < vv_list_.size(); ++i)
            {
               vecview_t& vv = vv_list_[i];
               if (vv)
               {
                  if (typestore_list_[i].vec_data)
                  {
                     vv->remove_ref(&typestore_list_[i].vec_data);
                  }

                  vv->remove_size_ref(&typestore_list_[i].size);
                  typestore_list_[i].vec_data = 0;
               }
            }
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override exprtk_final
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

         virtual bool init_branches()
         {
            expr_as_vec1_store_.resize(arg_list_.size(), T(0)               );
            typestore_list_    .resize(arg_list_.size(), type_store_t()     );
            range_list_        .resize(arg_list_.size(), range_data_type_t());
            branch_            .resize(arg_list_.size(), branch_t(reinterpret_cast<expression_ptr>(0),false));
            vv_list_           .resize(arg_list_.size(), vecview_t(0));

            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               type_store_t& ts = typestore_list_[i];

               if (0 == arg_list_[i])
                  return false;
               else if (is_ivector_node(arg_list_[i]))
               {
                  vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

                  if (0 == (vi = dynamic_cast<vector_interface<T>*>(arg_list_[i])))
                     return false;

                  ts.size = vi->size();
                  ts.data = vi->vds().data();
                  ts.type = type_store_t::e_vector;
                  ts.ivec = vi;

                  if (
                       vi->vec()->vec_holder().rebaseable() &&
                       vi->vec()->vec_holder().rebaseable_instance()
                     )
                  {
                     vv_list_[i] = vi->vec()->vec_holder().rebaseable_instance();
                     vv_list_[i]->set_size_ref(&ts.size);

                     if (!amalgamated_vecop(arg_list_[i]))
                     {
                        vv_list_[i]->set_ref(&ts.vec_data);
                     }
                  }
               }
               #ifndef exprtk_disable_string_capabilities
               else if (is_generally_string_node(arg_list_[i]))
               {
                  string_base_node<T>* sbn = reinterpret_cast<string_base_node<T>*>(0);

                  if (0 == (sbn = dynamic_cast<string_base_node<T>*>(arg_list_[i])))
                     return false;

                  ts.size = sbn->size();
                  ts.data = reinterpret_cast<void*>(const_cast<char_ptr>(sbn->base()));
                  ts.type = type_store_t::e_string;

                  range_list_[i].data      = ts.data;
                  range_list_[i].size      = ts.size;
                  range_list_[i].type_size = sizeof(char);
                  range_list_[i].str_node  = sbn;

                  range_interface_t* ri = reinterpret_cast<range_interface_t*>(0);

                  if (0 == (ri = dynamic_cast<range_interface_t*>(arg_list_[i])))
                     return false;

                  const range_t& rp = ri->range_ref();

                  if (
                       rp.const_range() &&
                       is_const_string_range_node(arg_list_[i])
                     )
                  {
                     ts.size = rp.const_size();
                     ts.data = static_cast<char_ptr>(ts.data) + rp.n0_c.second;
                     range_list_[i].range = reinterpret_cast<range_t*>(0);
                  }
                  else
                  {
                     range_list_[i].range = &(ri->range_ref());
                     range_param_list_.push_back(i);
                  }
               }
               #endif
               else if (is_variable_node(arg_list_[i]))
               {
                  variable_node_ptr_t var = variable_node_ptr_t(0);

                  if (0 == (var = dynamic_cast<variable_node_ptr_t>(arg_list_[i])))
                     return false;

                  ts.size = 1;
                  ts.data = &var->ref();
                  ts.type = type_store_t::e_scalar;
               }
               else
               {
                  ts.size = 1;
                  ts.data = reinterpret_cast<void*>(&expr_as_vec1_store_[i]);
                  ts.type = type_store_t::e_scalar;
               }

               branch_[i] = std::make_pair(arg_list_[i],branch_deletable(arg_list_[i]));
            }

            return true;
         }

         inline bool operator <(const generic_function_node<T,GenericFunction>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const exprtk_override
         {
            if (populate_value_list())
            {
               typedef typename GenericFunction::parameter_list_t parameter_list_t;

               return (*function_)(parameter_list_t(typestore_list_));
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_genfunction;
         }

         inline bool valid() const exprtk_override
         {
            return function_;
         }

      protected:

         inline virtual bool populate_value_list() const
         {
            assert(branch_.size() == typestore_list_.size());

            for (std::size_t i = 0; i < branch_.size(); ++i)
            {
               expr_as_vec1_store_[i] = branch_[i].first->value();
            }

            if (!range_param_list_.empty())
            {
               assert(range_param_list_.size() <= branch_.size());

               for (std::size_t i = 0; i < range_param_list_.size(); ++i)
               {
                  const std::size_t  index = range_param_list_[i];
                  range_data_type_t& rdt   = range_list_[index];

                  const range_t& rp = (*rdt.range);
                  std::size_t r0    = 0;
                  std::size_t r1    = 0;

                  const std::size_t data_size =
                  #ifndef exprtk_disable_string_capabilities
                     rdt.str_node ? rdt.str_node->size() : rdt.size;
                  #else
                     rdt.size;
                  #endif

                  if (!rp(r0, r1, data_size))
                  {
                     return false;
                  }

                  type_store_t& ts = typestore_list_[index];

                  ts.size = rp.cache_size();
                  #ifndef exprtk_disable_string_capabilities
                  if (ts.type == type_store_t::e_string)
                     ts.data = const_cast<char_ptr>(rdt.str_node->base()) + rp.cache.first;
                  else
                  #endif
                     ts.data = static_cast<char_ptr>(rdt.data) + (rp.cache.first * rdt.type_size);
               }
            }

            return true;
         }

         GenericFunction* function_;
         mutable typestore_list_t typestore_list_;

      private:

         std::vector<expression_ptr> arg_list_;
         std::vector<branch_t>       branch_;
         std::vector<vecview_t>      vv_list_;
         mutable tmp_vs_t            expr_as_vec1_store_;
         mutable range_list_t        range_list_;
         std::vector<std::size_t>    range_param_list_;
      };

      #ifndef exprtk_disable_string_capabilities
      template <typename T, typename StringFunction>
      class string_function_node : public generic_function_node<T,StringFunction>
                                 , public string_base_node<T>
                                 , public range_interface <T>
      {
      public:

         typedef generic_function_node<T,StringFunction> gen_function_t;
         typedef typename range_interface<T>::range_t range_t;

         string_function_node(StringFunction* func,
                              const std::vector<typename gen_function_t::expression_ptr>& arg_list)
         : gen_function_t(arg_list,func)
         {
            range_.n0_c = std::make_pair<bool,std::size_t>(true,0);
            range_.n1_c = std::make_pair<bool,std::size_t>(true,0);
            range_.cache.first  = range_.n0_c.second;
            range_.cache.second = range_.n1_c.second;
            assert(valid());
         }

         inline bool operator <(const string_function_node<T,StringFunction>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const exprtk_override
         {
            if (gen_function_t::populate_value_list())
            {
               typedef typename StringFunction::parameter_list_t parameter_list_t;

               const T result =
                  (*gen_function_t::function_)
                  (
                     ret_string_,
                     parameter_list_t(gen_function_t::typestore_list_)
                  );

               range_.n1_c.second  = ret_string_.size();
               range_.cache.second = range_.n1_c.second;

               return result;
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_strfunction;
         }

         inline bool valid() const exprtk_override
         {
            return gen_function_t::function_;
         }

         std::string str() const exprtk_override
         {
            return ret_string_;
         }

         char_cptr base() const exprtk_override
         {
           return &ret_string_[0];
         }

         std::size_t size() const exprtk_override
         {
            return ret_string_.size();
         }

         range_t& range_ref() exprtk_override
         {
            return range_;
         }

         const range_t& range_ref() const exprtk_override
         {
            return range_;
         }

      protected:

         mutable range_t     range_;
         mutable std::string ret_string_;
      };
      #endif

      template <typename T, typename GenericFunction>
      class multimode_genfunction_node : public generic_function_node<T,GenericFunction>
      {
      public:

         typedef generic_function_node<T,GenericFunction> gen_function_t;
         typedef typename gen_function_t::range_t         range_t;

         multimode_genfunction_node(GenericFunction* func,
                                    const std::size_t& param_seq_index,
                                    const std::vector<typename gen_function_t::expression_ptr>& arg_list)
         : gen_function_t(arg_list,func)
         , param_seq_index_(param_seq_index)
         {}

         inline T value() const exprtk_override
         {
            assert(gen_function_t::valid());

            if (gen_function_t::populate_value_list())
            {
               typedef typename GenericFunction::parameter_list_t parameter_list_t;

               return
                  (*gen_function_t::function_)
                  (
                     param_seq_index_,
                     parameter_list_t(gen_function_t::typestore_list_)
                  );
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override exprtk_final
         {
            return expression_node<T>::e_genfunction;
         }

      private:

         std::size_t param_seq_index_;
      };

      #ifndef exprtk_disable_string_capabilities
      template <typename T, typename StringFunction>
      class multimode_strfunction_node exprtk_final : public string_function_node<T,StringFunction>
      {
      public:

         typedef string_function_node<T,StringFunction> str_function_t;
         typedef typename str_function_t::range_t range_t;

         multimode_strfunction_node(StringFunction* func,
                                    const std::size_t& param_seq_index,
                                    const std::vector<typename str_function_t::expression_ptr>& arg_list)
         : str_function_t(func,arg_list)
         , param_seq_index_(param_seq_index)
         {}

         inline T value() const exprtk_override
         {
            if (str_function_t::populate_value_list())
            {
               typedef typename StringFunction::parameter_list_t parameter_list_t;

               const T result =
                  (*str_function_t::function_)
                  (
                     param_seq_index_,
                     str_function_t::ret_string_,
                     parameter_list_t(str_function_t::typestore_list_)
                  );

               str_function_t::range_.n1_c.second  = str_function_t::ret_string_.size();
               str_function_t::range_.cache.second = str_function_t::range_.n1_c.second;

               return result;
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_strfunction;
         }

      private:

         const std::size_t param_seq_index_;
      };
      #endif

      class return_exception {};

      template <typename T>
      class null_igenfunc
      {
      public:

         virtual ~null_igenfunc()
         {}

         typedef type_store<T> generic_type;
         typedef typename generic_type::parameter_list parameter_list_t;

         inline virtual T operator() (parameter_list_t)
         {
            return std::numeric_limits<T>::quiet_NaN();
         }
      };

      #ifndef exprtk_disable_return_statement
      template <typename T>
      class return_node exprtk_final : public generic_function_node<T,null_igenfunc<T> >
      {
      public:

         typedef results_context<T>   results_context_t;
         typedef null_igenfunc<T>     igeneric_function_t;
         typedef igeneric_function_t* igeneric_function_ptr;
         typedef generic_function_node<T,igeneric_function_t> gen_function_t;

         return_node(const std::vector<typename gen_function_t::expression_ptr>& arg_list,
                     results_context_t& rc)
         : gen_function_t  (arg_list)
         , results_context_(&rc)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            if (gen_function_t::populate_value_list())
            {
               prepare_typestore_list();

               typedef typename type_store<T>::parameter_list parameter_list_t;

               results_context_->
                  assign(parameter_list_t(gen_function_t::typestore_list_));

               throw return_exception();
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_return;
         }

         inline bool valid() const exprtk_override
         {
            return results_context_;
         }

      private:

         void prepare_typestore_list() const
         {
            for (std::size_t i = 0; i < gen_function_t::typestore_list_.size(); ++i)
            {
               typename gen_function_t::type_store_t& ts = gen_function_t::typestore_list_[i];

               if (ts.ivec)
               {
                  ts.size = ts.ivec->size();
               }
            }
         }

         results_context_t* results_context_;
      };

      template <typename T>
      class return_envelope_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef results_context<T>  results_context_t;
         typedef std::pair<expression_ptr,bool> branch_t;

         return_envelope_node(expression_ptr body, results_context_t& rc)
         : results_context_(&rc  )
         , return_invoked_ (false)
         {
            construct_branch_pair(body_, body);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            try
            {
               return_invoked_ = false;
               results_context_->clear();

               return body_.first->value();
            }
            catch(const return_exception&)
            {
               return_invoked_ = true;

               return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_retenv;
         }

         inline bool valid() const exprtk_override
         {
            return results_context_ && body_.first;
         }

         inline bool* retinvk_ptr()
         {
            return &return_invoked_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(body_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(body_);
         }

      private:

         results_context_t* results_context_;
         mutable bool        return_invoked_;
         branch_t                      body_;
      };
      #endif

   } // namespace details
} // namespace exprtk

#endif // EXPRTK_SRC_NODES_CALL_HPP
