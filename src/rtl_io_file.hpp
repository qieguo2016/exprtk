#ifndef EXPRTK_SRC_RTL_IO_FILE_HPP
#define EXPRTK_SRC_RTL_IO_FILE_HPP

#include "functions.hpp"
#include "common.hpp"
#include "symbol_table.hpp"

#ifndef exprtk_disable_rtl_io_file
#include <fstream>
namespace exprtk
{
   namespace rtl { namespace io { namespace file { namespace details
   {
      using ::exprtk::details::char_ptr;
      using ::exprtk::details::char_cptr;

      enum file_mode
      {
         e_error = 0,
         e_read  = 1,
         e_write = 2,
         e_rdwrt = 4
      };

      struct file_descriptor
      {
         file_descriptor(const std::string& fname, const std::string& access)
         : stream_ptr(0)
         , mode(get_file_mode(access))
         , file_name(fname)
         {}

         void*       stream_ptr;
         file_mode   mode;
         std::string file_name;

         bool open()
         {
            if (e_read == mode)
            {
               std::ifstream* stream = new std::ifstream(file_name.c_str(),std::ios::binary);

               if (!(*stream))
               {
                  file_name.clear();
                  delete stream;

                  return false;
               }

               stream_ptr = stream;

               return true;
            }
            else if (e_write == mode)
            {
               std::ofstream* stream = new std::ofstream(file_name.c_str(),std::ios::binary);

               if (!(*stream))
               {
                  file_name.clear();
                  delete stream;

                  return false;
               }

               stream_ptr = stream;

               return true;
            }
            else if (e_rdwrt == mode)
            {
               std::fstream* stream = new std::fstream(file_name.c_str(),std::ios::binary);

               if (!(*stream))
               {
                  file_name.clear();
                  delete stream;

                  return false;
               }

               stream_ptr = stream;

               return true;
            }

            return false;
         }

         template <typename Stream, typename Ptr>
         void close(Ptr& p)
         {
            Stream* stream = reinterpret_cast<Stream*>(p);
            stream->close();
            delete stream;
            p = reinterpret_cast<Ptr>(0);
         }

         bool close()
         {
            switch (mode)
            {
               case e_read  : close<std::ifstream>(stream_ptr);
                              break;

               case e_write : close<std::ofstream>(stream_ptr);
                              break;

               case e_rdwrt : close<std::fstream> (stream_ptr);
                              break;

               default      : return false;
            }

            return true;
         }

         template <typename View>
         bool write(const View& view, const std::size_t amount, const std::size_t offset = 0)
         {
            switch (mode)
            {
               case e_write : reinterpret_cast<std::ofstream*>(stream_ptr)->
                                 write(reinterpret_cast<char_cptr>(view.begin() + offset), amount * sizeof(typename View::value_t));
                              break;

               case e_rdwrt : reinterpret_cast<std::fstream*>(stream_ptr)->
                                 write(reinterpret_cast<char_cptr>(view.begin() + offset) , amount * sizeof(typename View::value_t));
                              break;

               default      : return false;
            }

            return true;
         }

         template <typename View>
         bool read(View& view, const std::size_t amount, const std::size_t offset = 0)
         {
            switch (mode)
            {
               case e_read  : reinterpret_cast<std::ifstream*>(stream_ptr)->
                                 read(reinterpret_cast<char_ptr>(view.begin() + offset), amount * sizeof(typename View::value_t));
                              break;

               case e_rdwrt : reinterpret_cast<std::fstream*>(stream_ptr)->
                                 read(reinterpret_cast<char_ptr>(view.begin() + offset) , amount * sizeof(typename View::value_t));
                              break;

               default      : return false;
            }

            return true;
         }

         bool getline(std::string& s)
         {
            switch (mode)
            {
               case e_read  : return (!!std::getline(*reinterpret_cast<std::ifstream*>(stream_ptr),s));
               case e_rdwrt : return (!!std::getline(*reinterpret_cast<std::fstream* >(stream_ptr),s));
               default      : return false;
            }
         }

         bool eof() const
         {
            switch (mode)
            {
               case e_read  : return reinterpret_cast<std::ifstream*>(stream_ptr)->eof();
               case e_write : return reinterpret_cast<std::ofstream*>(stream_ptr)->eof();
               case e_rdwrt : return reinterpret_cast<std::fstream* >(stream_ptr)->eof();
               default      : return true;
            }
         }

         file_mode get_file_mode(const std::string& access) const
         {
            if (access.empty() || access.size() > 2)
               return e_error;

            std::size_t w_cnt = 0;
            std::size_t r_cnt = 0;

            for (std::size_t i = 0; i < access.size(); ++i)
            {
               switch (std::tolower(access[i]))
               {
                  case 'r' : r_cnt++; break;
                  case 'w' : w_cnt++; break;
                  default  : return e_error;
               }
            }

            if ((0 == r_cnt) && (0 == w_cnt))
               return e_error;
            else if ((r_cnt > 1) || (w_cnt > 1))
               return e_error;
            else if ((1 == r_cnt) && (1 == w_cnt))
               return e_rdwrt;
            else if (1 == r_cnt)
               return e_read;
            else
               return e_write;
         }
      };

      template <typename T>
      file_descriptor* make_handle(T v)
      {
         const std::size_t fd_size    = sizeof(details::file_descriptor*);
         details::file_descriptor* fd = reinterpret_cast<file_descriptor*>(0);

         std::memcpy(reinterpret_cast<char_ptr >(&fd),
                     reinterpret_cast<char_cptr>(&v ),
                     fd_size);
         return fd;
      }

      template <typename T>
      void perform_check()
      {
         #ifdef _MSC_VER
         #pragma warning(push)
         #pragma warning(disable: 4127)
         #endif
         if (sizeof(T) < sizeof(void*))
         {
            throw std::runtime_error("exprtk::rtl::io::file - Error - pointer size larger than holder.");
         }
         #ifdef _MSC_VER
         #pragma warning(pop)
         #endif
         assert(sizeof(T) <= sizeof(void*));
      }

   } // namespace exprtk::rtl::io::file::details

   template <typename T>
   class open exprtk_final : public exprtk::igeneric_function<T>
   {
   public:

      typedef typename exprtk::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;

      using igfun_t::operator();

      open()
      : exprtk::igeneric_function<T>("S|SS")
      { details::perform_check<T>(); }

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) exprtk_override
      {
         const std::string file_name = to_str(string_t(parameters[0]));

         if (file_name.empty())
         {
            return T(0);
         }

         if ((1 == ps_index) && (0 == string_t(parameters[1]).size()))
         {
            return T(0);
         }

         const std::string access =
            (0 == ps_index) ? "r" : to_str(string_t(parameters[1]));

         details::file_descriptor* fd = new details::file_descriptor(file_name,access);

         if (fd->open())
         {
            T t = T(0);

            const std::size_t fd_size = sizeof(details::file_descriptor*);

            std::memcpy(reinterpret_cast<char*>(&t ),
                        reinterpret_cast<char*>(&fd),
                        fd_size);
            return t;
         }
         else
         {
            delete fd;
            return T(0);
         }
      }
   };

   template <typename T>
   struct close exprtk_final : public exprtk::ifunction<T>
   {
      using exprtk::ifunction<T>::operator();

      close()
      : exprtk::ifunction<T>(1)
      { details::perform_check<T>(); }

      inline T operator() (const T& v) exprtk_override
      {
         details::file_descriptor* fd = details::make_handle(v);

         if (!fd->close())
            return T(0);

         delete fd;

         return T(1);
      }
   };

   template <typename T>
   class write exprtk_final : public exprtk::igeneric_function<T>
   {
   public:

      typedef typename exprtk::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      write()
      : igfun_t("TS|TST|TV|TVT")
      { details::perform_check<T>(); }

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) exprtk_override
      {
         details::file_descriptor* fd = details::make_handle(scalar_t(parameters[0])());

         switch (ps_index)
         {
            case 0  : {
                         const string_t buffer(parameters[1]);
                         const std::size_t amount = buffer.size();
                         return T(fd->write(buffer, amount) ? 1 : 0);
                      }

            case 1  : {
                         const string_t buffer(parameters[1]);
                         const std::size_t amount =
                                  std::min(buffer.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->write(buffer, amount) ? 1 : 0);
                      }

            case 2  : {
                         const vector_t vec(parameters[1]);
                         const std::size_t amount = vec.size();
                         return T(fd->write(vec, amount) ? 1 : 0);
                      }

            case 3  : {
                         const vector_t vec(parameters[1]);
                         const std::size_t amount =
                                  std::min(vec.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->write(vec, amount) ? 1 : 0);
                      }
         }

         return T(0);
      }
   };

   template <typename T>
   class read exprtk_final : public exprtk::igeneric_function<T>
   {
   public:

      typedef typename exprtk::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      read()
      : igfun_t("TS|TST|TV|TVT")
      { details::perform_check<T>(); }

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) exprtk_override
      {
         details::file_descriptor* fd = details::make_handle(scalar_t(parameters[0])());

         switch (ps_index)
         {
            case 0  : {
                         string_t buffer(parameters[1]);
                         const std::size_t amount = buffer.size();
                         return T(fd->read(buffer,amount) ? 1 : 0);
                      }

            case 1  : {
                         string_t buffer(parameters[1]);
                         const std::size_t amount =
                                  std::min(buffer.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->read(buffer,amount) ? 1 : 0);
                      }

            case 2  : {
                         vector_t vec(parameters[1]);
                         const std::size_t amount = vec.size();
                         return T(fd->read(vec,amount) ? 1 : 0);
                      }

            case 3  : {
                         vector_t vec(parameters[1]);
                         const std::size_t amount =
                                  std::min(vec.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->read(vec,amount) ? 1 : 0);
                      }
         }

         return T(0);
      }
   };

   template <typename T>
   class getline exprtk_final : public exprtk::igeneric_function<T>
   {
   public:

      typedef typename exprtk::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::scalar_view    scalar_t;

      using igfun_t::operator();

      getline()
      : igfun_t("T",igfun_t::e_rtrn_string)
      { details::perform_check<T>(); }

      inline T operator() (std::string& result, parameter_list_t parameters) exprtk_override
      {
         details::file_descriptor* fd = details::make_handle(scalar_t(parameters[0])());
         return T(fd->getline(result) ? 1 : 0);
      }
   };

   template <typename T>
   struct eof exprtk_final : public exprtk::ifunction<T>
   {
      using exprtk::ifunction<T>::operator();

      eof()
      : exprtk::ifunction<T>(1)
      { details::perform_check<T>(); }

      inline T operator() (const T& v) exprtk_override
      {
         details::file_descriptor* fd = details::make_handle(v);
         return (fd->eof() ? T(1) : T(0));
      }
   };

   template <typename T>
   struct package
   {
      open   <T> o;
      close  <T> c;
      write  <T> w;
      read   <T> r;
      getline<T> g;
      eof    <T> e;

      bool register_package(exprtk::symbol_table<T>& symtab)
      {
         #define exprtk_register_function(FunctionName, FunctionType)                   \
         if (!symtab.add_function(FunctionName,FunctionType))                           \
         {                                                                              \
            exprtk_debug((                                                              \
              "exprtk::rtl::io::file::register_package - Failed to add function: %s\n", \
              FunctionName));                                                           \
            return false;                                                               \
         }                                                                              \

         exprtk_register_function("open"    , o)
         exprtk_register_function("close"   , c)
         exprtk_register_function("write"   , w)
         exprtk_register_function("read"    , r)
         exprtk_register_function("getline" , g)
         exprtk_register_function("eof"     , e)
         #undef exprtk_register_function

         return true;
      }
   };

   } // namespace exprtk::rtl::io::file
   } // namespace exprtk::rtl::io
   } // namespace exprtk::rtl
}    // namespace exprtk
#endif

#endif // EXPRTK_SRC_RTL_IO_FILE_HPP
