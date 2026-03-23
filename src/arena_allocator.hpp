#ifndef EXPRTK_SRC_ARENA_ALLOCATOR_HPP
#define EXPRTK_SRC_ARENA_ALLOCATOR_HPP

#include <cstddef>
#include <cstdlib>
#include <vector>
#include <cassert>

namespace exprtk
{
   namespace details
   {

      // Arena allocator for AST nodes.
      //
      // Allocates nodes in contiguous 64KB pages using placement-new, keeping
      // nodes from the same expression close together in memory and dramatically
      // improving L1/L2 cache hit rates during value() tree-walk evaluation.
      //
      // Nodes larger than the page size fall back to heap allocation (tracked
      // separately and freed individually).
      //
      // Lifetime: owned by expression::control_block; destroyed after all node
      // destructors have been called by node_collection_destructor::delete_nodes.
      class arena_allocator
      {
      public:

         static const std::size_t page_size = 65536; // 64 KB pages

         arena_allocator()
         : current_page_(0)
         , current_offset_(0)
         {}

         ~arena_allocator()
         {
            for (std::size_t i = 0; i < pages_.size(); ++i)
            {
               ::free(pages_[i]);
            }

            for (std::size_t i = 0; i < oversized_.size(); ++i)
            {
               ::free(oversized_[i]);
            }
         }

         // Allocate size bytes aligned to align bytes.
         // Returns a pointer to uninitialized memory, or NULL on failure.
         void* allocate(const std::size_t size, const std::size_t align = sizeof(void*))
         {
            if (size > page_size)
            {
               // Oversized allocation: fall back to malloc, track separately.
               void* ptr = ::malloc(size);
               if (ptr)
               {
                  oversized_.push_back(ptr);
               }
               return ptr;
            }

            if (!try_allocate_from_current(size, align))
            {
               if (!add_page())
               {
                  return 0;
               }
            }

            return allocate_from_current(size, align);
         }

         // Check whether a pointer falls within any arena-managed page.
         // Used to determine if an expression_node's memory is arena-owned.
         bool is_arena_ptr(const void* ptr) const
         {
            const char* p = static_cast<const char*>(ptr);

            for (std::size_t i = 0; i < pages_.size(); ++i)
            {
               const char* page_start = static_cast<const char*>(pages_[i]);
               if (p >= page_start && p < page_start + page_size)
               {
                  return true;
               }
            }

            for (std::size_t i = 0; i < oversized_.size(); ++i)
            {
               if (ptr == oversized_[i])
               {
                  return true;
               }
            }

            return false;
         }

      private:

         arena_allocator(const arena_allocator&);
         arena_allocator& operator=(const arena_allocator&);

         bool add_page()
         {
            void* page = ::malloc(page_size);
            if (!page) return false;
            pages_.push_back(page);
            current_page_   = page;
            current_offset_ = 0;
            return true;
         }

         bool try_allocate_from_current(const std::size_t size, const std::size_t align) const
         {
            if (!current_page_) return false;

            const std::size_t aligned_offset = align_offset(current_offset_, align);
            return (aligned_offset + size) <= page_size;
         }

         void* allocate_from_current(const std::size_t size, const std::size_t align)
         {
            const std::size_t aligned_offset = align_offset(current_offset_, align);
            void* ptr = static_cast<char*>(current_page_) + aligned_offset;
            current_offset_ = aligned_offset + size;
            return ptr;
         }

         static std::size_t align_offset(const std::size_t offset, const std::size_t align)
         {
            return (offset + align - 1) & ~(align - 1);
         }

         std::vector<void*> pages_;
         std::vector<void*> oversized_;
         void*       current_page_;
         std::size_t current_offset_;
      };

   } // namespace details
} // namespace exprtk

#endif // EXPRTK_SRC_ARENA_ALLOCATOR_HPP
