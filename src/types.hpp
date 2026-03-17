#ifndef EXPRTK_SRC_TYPES_HPP
#define EXPRTK_SRC_TYPES_HPP

#include "common.hpp"

namespace exprtk
{

   template <typename T>
   class vector_view
   {
   public:

      typedef T* data_ptr_t;

      vector_view(data_ptr_t data, const std::size_t& size)
      : base_size_(size)
      , size_(size)
      , data_(data)
      , data_ref_(0)
      {
         assert(size_ > 0);
      }

      vector_view(const vector_view<T>& vv)
      : base_size_(vv.base_size_)
      , size_(vv.size_)
      , data_(vv.data_)
      , data_ref_(0)
      {
         assert(size_ > 0);
      }

      inline void rebase(data_ptr_t data)
      {
         data_ = data;

         if (!data_ref_.empty())
         {
            for (std::size_t i = 0; i < data_ref_.size(); ++i)
            {
               (*data_ref_[i]) = data;
            }
         }
      }

      inline data_ptr_t data() const
      {
         return data_;
      }

      inline std::size_t base_size() const
      {
         return base_size_;
      }

      inline std::size_t size() const
      {
         return size_;
      }

      inline const T& operator[](const std::size_t index) const
      {
         assert(index < size_);
         return data_[index];
      }

      inline T& operator[](const std::size_t index)
      {
         assert(index < size_);
         return data_[index];
      }

      void set_ref(data_ptr_t* data_ref)
      {
         data_ref_.push_back(data_ref);
         exprtk_debug(("vector_view::set_ref() - data_ref: %p data_ref_.size(): %d\n",
                       reinterpret_cast<void*>(data_ref),
                       static_cast<int>(data_ref_.size())));
      }

      void set_size_ref(std::size_t* size_ref)
      {
         size_ref_.push_back(size_ref);
         exprtk_debug(("vector_view::set_size_ref() - size_ref: %p size_ref_.size(): %d\n",
                       reinterpret_cast<void*>(size_ref),
                       static_cast<int>(size_ref_.size())));
      }

      void remove_ref(data_ptr_t* data_ref)
      {
         data_ref_.erase(
            std::remove(data_ref_.begin(), data_ref_.end(), data_ref),
            data_ref_.end());
         exprtk_debug(("vector_view::remove_ref() - data_ref: %p data_ref_.size(): %d\n",
                       reinterpret_cast<void*>(data_ref),
                       static_cast<int>(data_ref_.size())));
      }

      void remove_size_ref(std::size_t* size_ref)
      {
         size_ref_.erase(
            std::remove(size_ref_.begin(), size_ref_.end(), size_ref),
            size_ref_.end());
         exprtk_debug(("vector_view::remove_size_ref() - size_ref: %p size_ref_.size(): %d\n",
                       reinterpret_cast<void*>(size_ref),
                       static_cast<int>(size_ref_.size())));
      }

      bool set_size(const std::size_t new_size)
      {
         if ((new_size > 0) && (new_size <= base_size_))
         {
            size_ = new_size;
            exprtk_debug(("vector_view::set_size() - data_: %p size: %lu\n",
                          reinterpret_cast<void*>(data_),
                          size_));

            if (!size_ref_.empty())
            {
               for (std::size_t i = 0; i < size_ref_.size(); ++i)
               {
                  (*size_ref_[i]) = new_size;
               }
            }

            return true;
         }

         exprtk_debug(("vector_view::set_size() - error invalid new_size: %lu  base_size: %lu\n",
                       new_size,
                       base_size_));
         return false;
      }

   private:

      const std::size_t base_size_;
      std::size_t size_;
      data_ptr_t  data_;
      std::vector<data_ptr_t*> data_ref_;
      std::vector<std::size_t*> size_ref_;
   };

   template <typename T>
   inline vector_view<T> make_vector_view(T* data,
                                          const std::size_t size, const std::size_t offset = 0)
   {
      return vector_view<T>(data + offset, size);
   }

   template <typename T>
   inline vector_view<T> make_vector_view(std::vector<T>& v,
                                          const std::size_t size, const std::size_t offset = 0)
   {
      return vector_view<T>(v.data() + offset, size);
   }

   template <typename T> class results_context;
   namespace  details { template <typename T> class vector_interface; }

   template <typename T>
   struct type_store
   {
      enum store_type
      {
         e_unknown,
         e_scalar ,
         e_vector ,
         e_string
      };

      type_store()
      : data(0)
      , size(0)
      , type(e_unknown)
      , ivec(0)
      {}

      union
      {
         void* data;
         T*    vec_data;
      };

      typedef details::vector_interface<T>* ivec_t;

      std::size_t size;
      store_type  type;
      ivec_t      ivec;

      class parameter_list
      {
      public:

         explicit parameter_list(std::vector<type_store>& pl)
         : parameter_list_(pl)
         {}

         inline bool empty() const
         {
            return parameter_list_.empty();
         }

         inline std::size_t size() const
         {
            return parameter_list_.size();
         }

         inline type_store& operator[](const std::size_t& index)
         {
            return parameter_list_[index];
         }

         inline const type_store& operator[](const std::size_t& index) const
         {
            return parameter_list_[index];
         }

         inline type_store& front()
         {
            return parameter_list_[0];
         }

         inline const type_store& front() const
         {
            return parameter_list_[0];
         }

         inline type_store& back()
         {
            return parameter_list_.back();
         }

         inline const type_store& back() const
         {
            return parameter_list_.back();
         }

         inline typename std::vector<type_store>::const_iterator begin() const
         {
            return parameter_list_.begin();
         }

         inline typename std::vector<type_store>::const_iterator end() const
         {
            return parameter_list_.end();
         }

         inline typename std::vector<type_store>::iterator begin()
         {
            return parameter_list_.begin();
         }

         inline typename std::vector<type_store>::iterator end()
         {
            return parameter_list_.end();
         }

      private:

         std::vector<type_store>& parameter_list_;

         friend class results_context<T>;
      };

      template <typename ViewType>
      struct type_view
      {
         typedef type_store<T> type_store_t;
         typedef ViewType      value_t;

         explicit type_view(type_store_t& ts)
         : ts_(ts)
         , data_(reinterpret_cast<value_t*>(ts_.data))
         {}

         explicit type_view(const type_store_t& ts)
         : ts_(const_cast<type_store_t&>(ts))
         , data_(reinterpret_cast<value_t*>(ts_.data))
         {}

         inline std::size_t size() const
         {
            return ts_.size;
         }

         inline value_t& operator[](const std::size_t& i)
         {
            return data_[i];
         }

         inline const value_t& operator[](const std::size_t& i) const
         {
            return data_[i];
         }

         inline const value_t* begin() const { return data_; }
         inline       value_t* begin()       { return data_; }

         inline const value_t* end() const
         {
            return static_cast<value_t*>(data_ + ts_.size);
         }

         inline value_t* end()
         {
            return static_cast<value_t*>(data_ + ts_.size);
         }

         type_store_t& ts_;
         value_t* data_;
      };

      typedef type_view<T>    vector_view;
      typedef type_view<char> string_view;

      struct scalar_view
      {
         typedef type_store<T> type_store_t;
         typedef T value_t;

         explicit scalar_view(type_store_t& ts)
         : v_(*reinterpret_cast<value_t*>(ts.data))
         {}

         explicit scalar_view(const type_store_t& ts)
         : v_(*reinterpret_cast<value_t*>(const_cast<type_store_t&>(ts).data))
         {}

         inline value_t& operator() ()
         {
            return v_;
         }

         inline const value_t& operator() () const
         {
            return v_;
         }

         inline operator value_t() const
         {
            return v_;
         }

         inline operator value_t()
         {
            return v_;
         }

         template <typename IntType>
         inline bool to_int(IntType& i) const
         {
            if (!exprtk::details::numeric::is_integer(v_))
               return false;

            i = static_cast<IntType>(v_);

            return true;
         }

         template <typename UIntType>
         inline bool to_uint(UIntType& u) const
         {
            if (v_ < T(0))
               return false;
            else if (!exprtk::details::numeric::is_integer(v_))
               return false;

            u = static_cast<UIntType>(v_);

            return true;
         }

         T& v_;
      };
   };

   template <typename StringView>
   inline std::string to_str(const StringView& view)
   {
      return std::string(view.begin(),view.size());
   }

   #ifndef exprtk_disable_return_statement
   namespace details
   {
      template <typename T> class return_node;
      template <typename T> class return_envelope_node;
   }
   #endif

   template <typename T>
   class results_context
   {
   public:

      typedef type_store<T> type_store_t;
      typedef typename type_store_t::scalar_view scalar_t;
      typedef typename type_store_t::vector_view vector_t;
      typedef typename type_store_t::string_view string_t;

      results_context()
      : results_available_(false)
      {}

      inline std::size_t count() const
      {
         if (results_available_)
            return parameter_list_.size();
         else
            return 0;
      }

      inline type_store_t& operator[](const std::size_t& index)
      {
         return parameter_list_[index];
      }

      inline const type_store_t& operator[](const std::size_t& index) const
      {
         return parameter_list_[index];
      }

      inline bool get_scalar(const std::size_t& index, T& out) const
      {
         if (
              (index < parameter_list_.size()) &&
              (parameter_list_[index].type == type_store_t::e_scalar)
            )
         {
            const scalar_t scalar(parameter_list_[index]);
            out = scalar();
            return true;
         }

         return false;
      }

      template <typename OutputIterator>
      inline bool get_vector(const std::size_t& index, OutputIterator out_itr) const
      {
         if (
              (index < parameter_list_.size()) &&
              (parameter_list_[index].type == type_store_t::e_vector)
            )
         {
            const vector_t vector(parameter_list_[index]);
            for (std::size_t i = 0; i < vector.size(); ++i)
            {
               *(out_itr++) = vector[i];
            }

            return true;
         }

         return false;
      }

      inline bool get_vector(const std::size_t& index, std::vector<T>& out) const
      {
         return get_vector(index,std::back_inserter(out));
      }

      inline bool get_string(const std::size_t& index, std::string& out) const
      {
         if (
              (index < parameter_list_.size()) &&
              (parameter_list_[index].type == type_store_t::e_string)
            )
         {
            const string_t str(parameter_list_[index]);
            out.assign(str.begin(),str.size());
            return true;
         }

         return false;
      }

   private:

      inline void clear()
      {
         results_available_ = false;
      }

      typedef std::vector<type_store_t> ts_list_t;
      typedef typename type_store_t::parameter_list parameter_list_t;

      inline void assign(const parameter_list_t& pl)
      {
         parameter_list_    = pl.parameter_list_;
         results_available_ = true;
      }

      bool results_available_;
      ts_list_t parameter_list_;

      #ifndef exprtk_disable_return_statement
      friend class details::return_node<T>;
      friend class details::return_envelope_node<T>;
      #endif
   };

} // namespace exprtk

#endif // EXPRTK_SRC_TYPES_HPP
