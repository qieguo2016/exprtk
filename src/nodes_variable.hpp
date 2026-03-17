#ifndef EXPRTK_SRC_NODES_VARIABLE_HPP
#define EXPRTK_SRC_NODES_VARIABLE_HPP

#include "nodes_base.hpp"
#include "nodes_control.hpp"

namespace exprtk
{
   namespace details
   {

      template <typename T>
      class ivariable
      {
      public:

         virtual ~ivariable()
         {}

         virtual T& ref() = 0;
         virtual const T& ref() const = 0;
      };

      template <typename T>
      class variable_node exprtk_final
                          : public expression_node<T>
                          , public ivariable      <T>
      {
      public:

         static T null_value;

         explicit variable_node()
         : value_(&null_value)
         {}

         explicit variable_node(T& v)
         : value_(&v)
         {}

         inline bool operator <(const variable_node<T>& v) const
         {
            return this < (&v);
         }

         inline T value() const exprtk_override
         {
            return (*value_);
         }

         inline T& ref() exprtk_override
         {
            return (*value_);
         }

         inline const T& ref() const exprtk_override
         {
            return (*value_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_variable;
         }

      private:

         T* value_;
      };

      template <typename T>
      T variable_node<T>::null_value = T(std::numeric_limits<T>::quiet_NaN());

      template <typename T>
      struct range_pack
      {
         typedef expression_node<T>*           expression_node_ptr;
         typedef std::pair<std::size_t,std::size_t> cached_range_t;

         range_pack()
         : n0_e (std::make_pair(false,expression_node_ptr(0)))
         , n1_e (std::make_pair(false,expression_node_ptr(0)))
         , n0_c (std::make_pair(false,0))
         , n1_c (std::make_pair(false,0))
         , cache(std::make_pair(0,0))
         {}

         void clear()
         {
            n0_e  = std::make_pair(false,expression_node_ptr(0));
            n1_e  = std::make_pair(false,expression_node_ptr(0));
            n0_c  = std::make_pair(false,0);
            n1_c  = std::make_pair(false,0);
            cache = std::make_pair(0,0);
         }

         void free()
         {
            if (n0_e.first && n0_e.second)
            {
               n0_e.first = false;

               if (
                    !is_variable_node(n0_e.second) &&
                    !is_string_node  (n0_e.second)
                  )
               {
                  destroy_node(n0_e.second);
               }
            }

            if (n1_e.first && n1_e.second)
            {
               n1_e.first = false;

               if (
                    !is_variable_node(n1_e.second) &&
                    !is_string_node  (n1_e.second)
                  )
               {
                  destroy_node(n1_e.second);
               }
            }
         }

         bool const_range() const
         {
           return ( n0_c.first &&  n1_c.first) &&
                  (!n0_e.first && !n1_e.first);
         }

         bool var_range() const
         {
           return ( n0_e.first &&  n1_e.first) &&
                  (!n0_c.first && !n1_c.first);
         }

         bool operator() (std::size_t& r0, std::size_t& r1,
                          const std::size_t& size = std::numeric_limits<std::size_t>::max()) const
         {
            if (n0_c.first)
               r0 = n0_c.second;
            else if (n0_e.first)
            {
               r0 = static_cast<std::size_t>(details::numeric::to_int64(n0_e.second->value()));
            }
            else
               return false;

            if (n1_c.first)
               r1 = n1_c.second;
            else if (n1_e.first)
            {
               r1 = static_cast<std::size_t>(details::numeric::to_int64(n1_e.second->value()));
            }
            else
               return false;

            if (
                 (std::numeric_limits<std::size_t>::max() != size) &&
                 (std::numeric_limits<std::size_t>::max() == r1  )
               )
            {
               r1 = size;
            }

            cache.first  = r0;
            cache.second = r1;

            #ifndef exprtk_enable_range_runtime_checks
            return (r0 <= r1);
            #else
            return range_runtime_check(r0, r1, size);
            #endif
         }

         inline std::size_t const_size() const
         {
            return (n1_c.second - n0_c.second);
         }

         inline std::size_t cache_size() const
         {
            return (cache.second - cache.first);
         }

         std::pair<bool,expression_node_ptr> n0_e;
         std::pair<bool,expression_node_ptr> n1_e;
         std::pair<bool,std::size_t        > n0_c;
         std::pair<bool,std::size_t        > n1_c;
         mutable cached_range_t             cache;

         #ifdef exprtk_enable_range_runtime_checks
         bool range_runtime_check(const std::size_t r0,
                                  const std::size_t r1,
                                  const std::size_t size) const
         {
            if (r0 > size)
            {
               throw std::runtime_error("range error: (r0 < 0) || (r0 > size)");
               #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
               return false;
               #endif
            }

            if (r1 > size)
            {
               throw std::runtime_error("range error: (r1 < 0) || (r1 > size)");
               #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
               return false;
               #endif
            }

            return (r0 <= r1);
         }
         #endif
      };

      template <typename T>
      class string_base_node;

      template <typename T>
      struct range_data_type
      {
         typedef range_pack<T> range_t;
         typedef string_base_node<T>* strbase_ptr_t;

         range_data_type()
         : range(0)
         , data (0)
         , size (0)
         , type_size(0)
         , str_node (0)
         {}

         range_t*      range;
         void*         data;
         std::size_t   size;
         std::size_t   type_size;
         strbase_ptr_t str_node;
      };

      template <typename T> class vector_node;

      template <typename T>
      class vector_interface
      {
      public:

         typedef vector_node<T>*   vector_node_ptr;
         typedef vec_data_store<T> vds_t;

         virtual ~vector_interface()
         {}

         virtual std::size_t size     () const = 0;

         virtual std::size_t base_size() const = 0;

         virtual vector_node_ptr vec  () const = 0;

         virtual vector_node_ptr vec  ()       = 0;

         virtual       vds_t& vds     ()       = 0;

         virtual const vds_t& vds     () const = 0;

         virtual bool side_effect     () const { return false; }
      };

      template <typename T>
      class vector_node exprtk_final
                        : public expression_node <T>
                        , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vec_data_store<T>   vds_t;

         explicit vector_node(vector_holder_t* vh)
         : vector_holder_(vh)
         , vds_((*vector_holder_).size(),(*vector_holder_)[0])
         {
            vector_holder_->set_ref(&vds_.ref());
         }

         vector_node(const vds_t& vds, vector_holder_t* vh)
         : vector_holder_(vh)
         , vds_(vds)
         {}

        ~vector_node() exprtk_override
         {
            assert(valid());
            vector_holder_->remove_ref(&vds_.ref());
         }

         inline T value() const exprtk_override
         {
            return vds().data()[0];
         }

         vector_node_ptr vec() const exprtk_override
         {
            return const_cast<vector_node_ptr>(this);
         }

         vector_node_ptr vec() exprtk_override
         {
            return this;
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vector;
         }

         inline bool valid() const exprtk_override
         {
            return vector_holder_;
         }

         std::size_t size() const exprtk_override
         {
            return vec_holder().size();
         }

         std::size_t base_size() const exprtk_override
         {
            return vec_holder().base_size();
         }

         vds_t& vds() exprtk_override
         {
            return vds_;
         }

         const vds_t& vds() const exprtk_override
         {
            return vds_;
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         inline vector_holder_t& vec_holder() const
         {
            return (*vector_holder_);
         }

      private:

         vector_holder_t* vector_holder_;
         vds_t                      vds_;
      };

      template <typename T>
      class vector_size_node exprtk_final
                        : public expression_node <T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_holder<T>    vector_holder_t;

         explicit vector_size_node(vector_holder_t* vh)
         : vector_holder_(vh)
         {}

        ~vector_size_node() exprtk_override
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            assert(vector_holder_);
            return static_cast<T>(vector_holder_->size());
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecsize;
         }

         inline bool valid() const exprtk_override
         {
            return vector_holder_ && vector_holder_->size();
         }

         inline vector_holder_t* vec_holder()
         {
            return vector_holder_;
         }

      private:

         vector_holder_t* vector_holder_;
      };

      template <typename T>
      class vector_elem_node exprtk_final
                             : public expression_node<T>
                             , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_elem_node(expression_ptr vec_node,
                          expression_ptr index,
                          vector_holder_ptr vec_holder)
         : vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return *access_vector();
         }

         inline T& ref() exprtk_override
         {
            return *access_vector();
         }

         inline const T& ref() const exprtk_override
         {
            return *access_vector();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecelem;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_      , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();
            return (vector_base_ + details::numeric::to_uint64(index_.first->value()));
         }

         vector_holder_ptr vector_holder_;
         T* vector_base_;
         branch_t vector_node_;
         branch_t index_;
      };

      template <typename T>
      class vector_celem_node exprtk_final
                              : public expression_node<T>
                              , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_celem_node(expression_ptr vec_node,
                           const std::size_t index,
                           vector_holder_ptr vec_holder)
         : index_(index)
         , vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         {
            construct_branch_pair(vector_node_, vec_node);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return *access_vector();
         }

         inline T& ref() exprtk_override
         {
            return *access_vector();
         }

         inline const T& ref() const exprtk_override
         {
            return *access_vector();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_veccelem;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();
            return (vector_base_ + index_);
         }

         const std::size_t index_;
         vector_holder_ptr vector_holder_;
         T* vector_base_;
         branch_t vector_node_;
      };

      template <typename T>
      class vector_elem_rtc_node exprtk_final
                                 : public expression_node<T>
                                 , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_elem_rtc_node(expression_ptr vec_node,
                              expression_ptr index,
                              vector_holder_ptr vec_holder,
                              vector_access_runtime_check_ptr vec_rt_chk)
         : vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         , vec_rt_chk_(vec_rt_chk)
         , max_vector_index_(vector_holder_->size() - 1)
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return *access_vector();
         }

         inline T& ref() exprtk_override
         {
            return *access_vector();
         }

         inline const T& ref() const exprtk_override
         {
            return *access_vector();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecelemrtc;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_,       node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            const _uint64_t index = details::numeric::to_uint64(index_.first->value());
            vector_node_.first->value();

            if (index <= max_vector_index_)
            {
               return (vector_holder_->data() + index);
            }

            assert(vec_rt_chk_);

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_base_);
            context.end_ptr    = reinterpret_cast<void*>(vector_base_ + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_base_ + index);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
               reinterpret_cast<T*>(context.access_ptr) :
               vector_base_ ;
         }

         vector_holder_ptr vector_holder_;
         T*                vector_base_;
         branch_t          vector_node_;
         branch_t          index_;
         vector_access_runtime_check_ptr vec_rt_chk_;
         const std::size_t max_vector_index_;
      };

      template <typename T>
      class vector_celem_rtc_node exprtk_final
                                 : public expression_node<T>
                                 , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_celem_rtc_node(expression_ptr vec_node,
                               const std::size_t index,
                               vector_holder_ptr vec_holder,
                               vector_access_runtime_check_ptr vec_rt_chk)
         : index_(index)
         , max_vector_index_(vec_holder->size() - 1)
         , vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         , vec_rt_chk_(vec_rt_chk)
         {
            construct_branch_pair(vector_node_, vec_node);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return *access_vector();
         }

         inline T& ref() exprtk_override
         {
            return *access_vector();
         }

         inline const T& ref() const exprtk_override
         {
            return *access_vector();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_veccelemrtc;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();

            if (index_ <= max_vector_index_)
            {
               return (vector_holder_->data() + index_);
            }

            assert(vec_rt_chk_);

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_base_);
            context.end_ptr    = reinterpret_cast<void*>(vector_base_ + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_base_ + index_);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
               reinterpret_cast<T*>(context.access_ptr) :
               vector_base_ ;
         }

         const std::size_t index_;
         const std::size_t max_vector_index_;
         vector_holder_ptr vector_holder_;
         T*                vector_base_;
         branch_t          vector_node_;
         vector_access_runtime_check_ptr vec_rt_chk_;
      };

      template <typename T>
      class rebasevector_elem_node exprtk_final
                                   : public expression_node<T>
                                   , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef vec_data_store<T>              vds_t;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_elem_node(expression_ptr vec_node,
                                expression_ptr index,
                                vector_holder_ptr vec_holder)
         : vector_holder_(vec_holder)
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return *access_vector();
         }

         inline T& ref() exprtk_override
         {
            return *access_vector();
         }

         inline const T& ref() const exprtk_override
         {
            return *access_vector();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_rbvecelem;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_,       node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();
            return (vector_holder_->data() + details::numeric::to_uint64(index_.first->value()));
         }

         vector_holder_ptr vector_holder_;
         branch_t          vector_node_;
         branch_t          index_;
      };

      template <typename T>
      class rebasevector_celem_node exprtk_final
                                    : public expression_node<T>
                                    , public ivariable      <T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_holder_t*    vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_celem_node(expression_ptr vec_node,
                                 const std::size_t index,
                                 vector_holder_ptr vec_holder)
         : index_(index)
         , vector_holder_(vec_holder)
         {
            construct_branch_pair(vector_node_, vec_node);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            vector_node_.first->value();
            return ref();
         }

         inline T& ref() exprtk_override
         {
            return *(vector_holder_->data() + index_);
         }

         inline const T& ref() const exprtk_override
         {
            return *(vector_holder_->data() + index_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_rbveccelem;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
         }

      private:

         const std::size_t index_;
         vector_holder_ptr vector_holder_;
         branch_t          vector_node_;
      };

      template <typename T>
      class rebasevector_elem_rtc_node exprtk_final
                                       : public expression_node<T>
                                       , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_elem_rtc_node(expression_ptr vec_node,
                                    expression_ptr index,
                                    vector_holder_ptr vec_holder,
                                    vector_access_runtime_check_ptr vec_rt_chk)
         : vector_holder_(vec_holder)
         , vec_rt_chk_(vec_rt_chk)
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return *access_vector();
         }

         inline T& ref() exprtk_override
         {
            return *access_vector();
         }

         inline const T& ref() const exprtk_override
         {
            return *access_vector();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_rbvecelemrtc;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_      , node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();
            const _uint64_t index = details::numeric::to_uint64(index_.first->value());

            if (index <= (vector_holder_->size() - 1))
            {
               return (vector_holder_->data() + index);
            }

            assert(vec_rt_chk_);

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_holder_->data());
            context.end_ptr    = reinterpret_cast<void*>(vector_holder_->data() + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_holder_->data() + index);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
                   reinterpret_cast<T*>(context.access_ptr) :
                   vector_holder_->data() ;
         }

         vector_holder_ptr vector_holder_;
         branch_t          vector_node_;
         branch_t          index_;
         vector_access_runtime_check_ptr vec_rt_chk_;
      };

      template <typename T>
      class rebasevector_celem_rtc_node exprtk_final
                                    : public expression_node<T>
                                    , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_celem_rtc_node(expression_ptr vec_node,
                                     const std::size_t index,
                                     vector_holder_ptr vec_holder,
                                     vector_access_runtime_check_ptr vec_rt_chk)
         : index_(index)
         , vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         , vec_rt_chk_(vec_rt_chk)
         {
            construct_branch_pair(vector_node_, vec_node);
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            return *access_vector();
         }

         inline T& ref() exprtk_override
         {
            return *access_vector();
         }

         inline const T& ref() const exprtk_override
         {
            return *access_vector();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_rbveccelemrtc;
         }

         inline bool valid() const exprtk_override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();

            if (index_ <= vector_holder_->size() - 1)
            {
               return (vector_holder_->data() + index_);
            }

            assert(vec_rt_chk_);

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_base_);
            context.end_ptr    = reinterpret_cast<void*>(vector_base_ + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_base_ + index_);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
               reinterpret_cast<T*>(context.access_ptr) :
               vector_base_ ;
         }

         const std::size_t index_;
         vector_holder_ptr vector_holder_;
         T*                vector_base_;
         branch_t          vector_node_;
         vector_access_runtime_check_ptr vec_rt_chk_;
      };

      template <typename T>
      class vector_initialisation_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_initialisation_node(T* vector_base,
                                    const std::size_t& size,
                                    const std::vector<expression_ptr>& initialiser_list,
                                    const bool single_value_initialse)
         : vector_base_(vector_base)
         , initialiser_list_(initialiser_list)
         , size_(size)
         , single_value_initialse_(single_value_initialse)
         , zero_value_initialse_(false)
         , const_nonzero_literal_value_initialse_(false)
         , single_initialiser_value_(T(0))
         {
            if (single_value_initialse_)
            {
               if (initialiser_list_.empty())
                  zero_value_initialse_ = true;
               else if (
                         (initialiser_list_.size() == 1) &&
                         details::is_constant_node(initialiser_list_[0]) &&
                         (T(0) == initialiser_list_[0]->value())
                       )
               {
                  zero_value_initialse_ = true;
               }
               else
               {
                  assert(initialiser_list_.size() == 1);

                  if (details::is_constant_node(initialiser_list_[0]))
                  {
                     const_nonzero_literal_value_initialse_ = true;
                     single_initialiser_value_ = initialiser_list_[0]->value();
                     assert(T(0) != single_initialiser_value_);
                  }
               }
            }
         }

         inline T value() const exprtk_override
         {
            if (single_value_initialse_)
            {
               if (zero_value_initialse_)
               {
                  details::set_zero_value(vector_base_, size_);
               }
               else if (const_nonzero_literal_value_initialse_)
               {
                  for (std::size_t i = 0; i < size_; ++i)
                  {
                     *(vector_base_ + i) = single_initialiser_value_;
                  }
               }
               else
               {
                  for (std::size_t i = 0; i < size_; ++i)
                  {
                     *(vector_base_ + i) = initialiser_list_[0]->value();
                  }
               }
            }
            else
            {
               const std::size_t initialiser_list_size = initialiser_list_.size();

               for (std::size_t i = 0; i < initialiser_list_size; ++i)
               {
                  *(vector_base_ + i) = initialiser_list_[i]->value();
               }

               if (initialiser_list_size < size_)
               {
                  details::set_zero_value(
                     vector_base_ + initialiser_list_size,
                     (size_ - initialiser_list_size));
               }
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_initialisation_node(const vector_initialisation_node<T>&) exprtk_delete;
         vector_initialisation_node<T>& operator=(const vector_initialisation_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         std::vector<expression_ptr> initialiser_list_;
         const std::size_t size_;
         const bool single_value_initialse_;
         bool zero_value_initialse_;
         bool const_nonzero_literal_value_initialse_;
         T single_initialiser_value_;
      };

      template <typename T>
      class vector_init_zero_value_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_zero_value_node(T* vector_base,
                                     const std::size_t& size,
                                     const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {}

         inline T value() const exprtk_override
         {
            details::set_zero_value(vector_base_, size_);
            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_init_zero_value_node(const vector_init_zero_value_node<T>&) exprtk_delete;
         vector_init_zero_value_node<T>& operator=(const vector_init_zero_value_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
      };

      template <typename T>
      class vector_init_single_constvalue_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_single_constvalue_node(T* vector_base,
                                            const std::size_t& size,
                                            const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {
            single_initialiser_value_ = initialiser_list_[0]->value();
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            for (std::size_t i = 0; i < size_; ++i)
            {
               *(vector_base_ + i) = single_initialiser_value_;
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 1) &&
                   (details::is_constant_node(initialiser_list_[0])) &&
                   (single_initialiser_value_ != T(0));
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_init_single_constvalue_node(const vector_init_single_constvalue_node<T>&) exprtk_delete;
         vector_init_single_constvalue_node<T>& operator=(const vector_init_single_constvalue_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
         T single_initialiser_value_;
      };

      template <typename T>
      class vector_init_single_value_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_single_value_node(T* vector_base,
                                       const std::size_t& size,
                                       const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            expression_node<T>& node = *initialiser_list_[0];

            for (std::size_t i = 0; i < size_; ++i)
            {
               *(vector_base_ + i) = node.value();
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 1) &&
                   !details::is_constant_node(initialiser_list_[0]);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_init_single_value_node(const vector_init_single_value_node<T>&) exprtk_delete;
         vector_init_single_value_node<T>& operator=(const vector_init_single_value_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
      };

      template <typename T>
      class vector_init_iota_constconst_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_constconst_node(T* vector_base,
                                          const std::size_t& size,
                                          const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {
            base_value_      = initialiser_list_[0]->value();
            increment_value_ = initialiser_list_[1]->value();

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T value = base_value_;

            for (std::size_t i = 0; i < size_; ++i, value += increment_value_)
            {
               *(vector_base_ + i) = value;
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 2) &&
                   (details::is_constant_node(initialiser_list_[0])) &&
                   (details::is_constant_node(initialiser_list_[1])) ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_init_iota_constconst_node(const vector_init_iota_constconst_node<T>&) exprtk_delete;
         vector_init_iota_constconst_node<T>& operator=(const vector_init_iota_constconst_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
         T base_value_;
         T increment_value_;
      };

      template <typename T>
      class vector_init_iota_constnconst_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_constnconst_node(T* vector_base,
                                           const std::size_t& size,
                                           const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {
            assert(valid());
            base_value_ = initialiser_list_[0]->value();
         }

         inline T value() const exprtk_override
         {
            T value = base_value_;
            expression_node<T>& increment = *initialiser_list_[1];

            for (std::size_t i = 0; i < size_; ++i, value += increment.value())
            {
               *(vector_base_ + i) = value;
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_ &&
                  (initialiser_list_.size() == 2) &&
                  ( details::is_constant_node(initialiser_list_[0])) &&
                  (!details::is_constant_node(initialiser_list_[1]));
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_init_iota_constnconst_node(const vector_init_iota_constnconst_node<T>&) exprtk_delete;
         vector_init_iota_constnconst_node<T>& operator=(const vector_init_iota_constnconst_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
         T base_value_;
      };

      template <typename T>
      class vector_init_iota_nconstconst_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_nconstconst_node(T* vector_base,
                                           const std::size_t& size,
                                           const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T value = initialiser_list_[0]->value();
            const T increment = initialiser_list_[1]->value();

            for (std::size_t i = 0; i < size_; ++i, value += increment)
            {
               *(vector_base_ + i) = value;
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 2) &&
                   (!details::is_constant_node(initialiser_list_[0])) &&
                   (details::is_constant_node(initialiser_list_[1]));
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_init_iota_nconstconst_node(const vector_init_iota_nconstconst_node<T>&) exprtk_delete;
         vector_init_iota_nconstconst_node<T>& operator=(const vector_init_iota_nconstconst_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
      };

      template <typename T>
      class vector_init_iota_nconstnconst_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_nconstnconst_node(T* vector_base,
                                            const std::size_t& size,
                                            const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {
            assert(valid());
         }

         inline T value() const exprtk_override
         {
            T value = initialiser_list_[0]->value();
            expression_node<T>& increment = *initialiser_list_[1];

            for (std::size_t i = 0; i < size_; ++i, value += increment.value())
            {
               *(vector_base_ + i) = value;
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const exprtk_override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 2) &&
                   (!details::is_constant_node(initialiser_list_[0])) &&
                   (!details::is_constant_node(initialiser_list_[1]));
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) exprtk_override
         {
            expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
         }

         std::size_t node_depth() const exprtk_override
         {
            return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
         }

      private:

         vector_init_iota_nconstnconst_node(const vector_init_iota_nconstnconst_node<T>&) exprtk_delete;
         vector_init_iota_nconstnconst_node<T>& operator=(const vector_init_iota_nconstnconst_node<T>&) exprtk_delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
      };

      template <typename T>
      class swap_node exprtk_final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef variable_node<T>*   variable_node_ptr;

         swap_node(variable_node_ptr var0, variable_node_ptr var1)
         : var0_(var0)
         , var1_(var1)
         {}

         inline T value() const exprtk_override
         {
            std::swap(var0_->ref(),var1_->ref());
            return var1_->ref();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_swap;
         }

      private:

         variable_node_ptr var0_;
         variable_node_ptr var1_;
      };

      template <typename T>
      class swap_generic_node exprtk_final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef ivariable<T>*       ivariable_ptr;

         swap_generic_node(expression_ptr var0, expression_ptr var1)
         : binary_node<T>(details::e_swap, var0, var1)
         , var0_(dynamic_cast<ivariable_ptr>(var0))
         , var1_(dynamic_cast<ivariable_ptr>(var1))
         {}

         inline T value() const exprtk_override
         {
            std::swap(var0_->ref(),var1_->ref());
            return var1_->ref();
         }

         inline typename expression_node<T>::node_type type() const exprtk_override
         {
            return expression_node<T>::e_swap;
         }

      private:

         ivariable_ptr var0_;
         ivariable_ptr var1_;
      };

      template <typename T>
      class swap_vecvec_node exprtk_final
                             : public binary_node     <T>
                             , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node    <T>* vector_node_ptr;
         typedef vec_data_store <T>  vds_t;

         using binary_node<T>::branch;

         swap_vecvec_node(expression_ptr branch0,
                          expression_ptr branch1)
         : binary_node<T>(details::e_swap, branch0, branch1)
         , vec0_node_ptr_(0)
         , vec1_node_ptr_(0)
         , initialised_  (false)
         {
            if (is_ivector_node(branch(0)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(0))))
               {
                  vec0_node_ptr_ = vi->vec();
                  vds()          = vi->vds();
               }
            }

            if (is_ivector_node(branch(1)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(1))))
               {
                  vec1_node_ptr_ = vi->vec();
               }
            }

            if (vec0_node_ptr_ && vec1_node_ptr_)
            {
               initialised_ = size() <= base_size();
            }

            assert(valid());
         }

         inline T value() const exprtk_override
         {
            binary_node<T>::branch(0)->value();
            binary_node<T>::branch(1)->value();

            T* vec0 = vec0_node_ptr_->vds().data();
            T* vec1 = vec1_node_ptr_->vds().data();

            assert(size() <= base_size());
            const std::size_t n = size();

            for (std::size_t i = 0; i < n; ++i)
            {
               std::swap(vec0[i],vec1[i]);
            }

            return vec1_node_ptr_->value();
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
            return expression_node<T>::e_vecvecswap;
         }

         inline bool valid() const exprtk_override
         {
            return initialised_ && binary_node<T>::valid();
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
         vds_t           vds_;
      };

   } // namespace details
} // namespace exprtk

#endif // EXPRTK_SRC_NODES_VARIABLE_HPP
