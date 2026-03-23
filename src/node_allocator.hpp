#ifndef EXPRTK_SRC_NODE_ALLOCATOR_HPP
#define EXPRTK_SRC_NODE_ALLOCATOR_HPP

#include "nodes_base.hpp"
#include "arena_allocator.hpp"

namespace exprtk
{
   namespace details
   {

      class node_allocator
      {
      public:

         node_allocator()
         : arena_(0)
         {}

         void set_arena(arena_allocator* arena)
         {
            arena_ = arena;
         }

         arena_allocator* get_arena() const
         {
            return arena_;
         }

      private:

         template <typename node_type>
         static inline void destroy_arena_node(expression_node<typename node_type::value_type>* node)
         {
            static_cast<node_type*>(node)->~node_type();
         }

         template <typename node_type>
         inline void mark_arena_node(node_type* node) const
         {
            node->arena_managed_    = true;
            node->arena_destroy_fn_ = &destroy_arena_node<node_type>;
         }

         template <typename node_type>
         inline node_type* arena_new() const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type();
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type();
         }

         template <typename node_type, typename T1>
         inline node_type* arena_new(T1& t1) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1);
         }

         template <typename node_type, typename T1>
         inline node_type* arena_new_c(const T1& t1) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1);
         }

         template <typename node_type, typename T1, typename T2>
         inline node_type* arena_new(const T1& t1, const T2& t2) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2);
         }

         template <typename node_type, typename T1, typename T2>
         inline node_type* arena_new_cr(const T1& t1, T2& t2) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2);
         }

         template <typename node_type, typename T1, typename T2>
         inline node_type* arena_new_rc(T1& t1, const T2& t2) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2);
         }

         template <typename node_type, typename T1, typename T2>
         inline node_type* arena_new_rr(T1& t1, T2& t2) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2);
         }

         template <typename node_type, typename T1, typename T2>
         inline node_type* arena_new_tt(T1 t1, T2 t2) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2);
         }

         template <typename node_type, typename T1, typename T2, typename T3>
         inline node_type* arena_new_ttt(T1 t1, T2 t2, T3 t3) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4>
         inline node_type* arena_new_tttt(T1 t1, T2 t2, T3 t3, T4 t4) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4);
         }

         template <typename node_type, typename T1, typename T2, typename T3>
         inline node_type* arena_new_rrr(T1& t1, T2& t2, T3& t3) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4>
         inline node_type* arena_new_rrrr(T1& t1, T2& t2, T3& t3, T4& t4) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4, typename T5>
         inline node_type* arena_new_rrrrr(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4, t5);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4, t5);
         }

         template <typename node_type, typename T1, typename T2, typename T3>
         inline node_type* arena_new_ccc(const T1& t1, const T2& t2, const T3& t3) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4>
         inline node_type* arena_new_cccc(const T1& t1, const T2& t2, const T3& t3, const T4& t4) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4, typename T5>
         inline node_type* arena_new_ccccc(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4, t5);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4, t5);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
         inline node_type* arena_new_cccccc(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4, t5, t6);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4, t5, t6);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
         inline node_type* arena_new_ccccccc(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4, t5, t6, t7);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4, t5, t6, t7);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
         inline node_type* arena_new_cccccccc(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4, t5, t6, t7, t8);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4, t5, t6, t7, t8);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
         inline node_type* arena_new_ccccccccc(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4, t5, t6, t7, t8, t9);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4, t5, t6, t7, t8, t9);
         }

         template <typename node_type, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
         inline node_type* arena_new_cccccccccc(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10) const
         {
            if (arena_)
            {
               const std::size_t align = alignof(node_type);
               void* mem = arena_->allocate(sizeof(node_type), align);
               if (mem)
               {
                  node_type* node = ::new (mem) node_type(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
                  mark_arena_node(node);
                  return node;
               }
            }
            return new node_type(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
         }

         arena_allocator* arena_;

      public:

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[1])
         {
            expression_node<typename ResultNode::value_type>* result =
               allocate<ResultNode>(operation, branch[0]);
            result->node_depth();
            return result;
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[2])
         {
            expression_node<typename ResultNode::value_type>* result =
               allocate<ResultNode>(operation, branch[0], branch[1]);
            result->node_depth();
            return result;
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[3])
         {
            expression_node<typename ResultNode::value_type>* result =
               allocate<ResultNode>(operation, branch[0], branch[1], branch[2]);
            result->node_depth();
            return result;
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[4])
         {
            expression_node<typename ResultNode::value_type>* result =
               allocate<ResultNode>(operation, branch[0], branch[1], branch[2], branch[3]);
            result->node_depth();
            return result;
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[5])
         {
            expression_node<typename ResultNode::value_type>* result =
               allocate<ResultNode>(operation, branch[0],branch[1], branch[2], branch[3], branch[4]);
            result->node_depth();
            return result;
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[6])
         {
            expression_node<typename ResultNode::value_type>* result =
               allocate<ResultNode>(operation, branch[0], branch[1], branch[2], branch[3], branch[4], branch[5]);
            result->node_depth();
            return result;
         }

         template <typename node_type>
         inline expression_node<typename node_type::value_type>* allocate() const
         {
            return arena_new<node_type>();
         }

         template <typename node_type,
                   typename Type,
                   typename Allocator,
                   template <typename, typename> class Sequence>
         inline expression_node<typename node_type::value_type>* allocate(const Sequence<Type,Allocator>& seq) const
         {
            expression_node<typename node_type::value_type>*
            result = (new node_type(seq));
            result->node_depth();
            return result;
         }

         template <typename node_type, typename T1>
         inline expression_node<typename node_type::value_type>* allocate(T1& t1) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new<node_type>(t1);
            result->node_depth();
            return result;
         }

         template <typename node_type, typename T1>
         inline expression_node<typename node_type::value_type>* allocate_c(const T1& t1) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_c<node_type>(t1);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new<node_type>(t1, t2);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_cr(const T1& t1, T2& t2) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_cr<node_type>(t1, t2);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_rc(T1& t1, const T2& t2) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_rc<node_type>(t1, t2);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_rr(T1& t1, T2& t2) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_rr<node_type>(t1, t2);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_tt(T1 t1, T2 t2) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_tt<node_type, T1, T2>(t1, t2);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate_ttt(T1 t1, T2 t2, T3 t3) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_ttt<node_type, T1, T2, T3>(t1, t2, t3);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate_tttt(T1 t1, T2 t2, T3 t3, T4 t4) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_tttt<node_type, T1, T2, T3, T4>(t1, t2, t3, t4);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate_rrr(T1& t1, T2& t2, T3& t3) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_rrr<node_type>(t1, t2, t3);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate_rrrr(T1& t1, T2& t2, T3& t3, T4& t4) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_rrrr<node_type>(t1, t2, t3, t4);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3, typename T4, typename T5>
         inline expression_node<typename node_type::value_type>* allocate_rrrrr(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_rrrrr<node_type>(t1, t2, t3, t4, t5);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_ccc<node_type>(t1, t2, t3);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_cccc<node_type>(t1, t2, t3, t4);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4, typename T5>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_ccccc<node_type>(t1, t2, t3, t4, t5);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4, typename T5, typename T6>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_cccccc<node_type>(t1, t2, t3, t4, t5, t6);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6, typename T7>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6,
                                                                          const T7& t7) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_ccccccc<node_type>(t1, t2, t3, t4, t5, t6, t7);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6,
                   typename T7, typename T8>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6,
                                                                          const T7& t7, const T8& t8) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_cccccccc<node_type>(t1, t2, t3, t4, t5, t6, t7, t8);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6,
                   typename T7, typename T8, typename T9>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6,
                                                                          const T7& t7, const T8& t8,
                                                                          const T9& t9) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_ccccccccc<node_type>(t1, t2, t3, t4, t5, t6, t7, t8, t9);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6,
                   typename T7, typename T8,
                   typename T9, typename T10>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const  T2&  t2,
                                                                          const T3& t3, const  T4&  t4,
                                                                          const T5& t5, const  T6&  t6,
                                                                          const T7& t7, const  T8&  t8,
                                                                          const T9& t9, const T10& t10) const
         {
            expression_node<typename node_type::value_type>*
            result = arena_new_cccccccccc<node_type>(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2, T3 t3) const
         {
            expression_node<typename node_type::value_type>* result;
            if (arena_)
            {
               void* mem = arena_->allocate(sizeof(node_type), alignof(node_type));
               if (mem)
               {
                  result = ::new (mem) node_type(t1, t2, t3);
                  mark_arena_node(static_cast<node_type*>(result));
                  result->node_depth();
                  return result;
               }
            }
            result = new node_type(t1, t2, t3);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2,
                                                                               T3 t3, T4 t4) const
         {
            expression_node<typename node_type::value_type>* result;
            if (arena_)
            {
               void* mem = arena_->allocate(sizeof(node_type), alignof(node_type));
               if (mem)
               {
                  result = ::new (mem) node_type(t1, t2, t3, t4);
                  mark_arena_node(static_cast<node_type*>(result));
                  result->node_depth();
                  return result;
               }
            }
            result = new node_type(t1, t2, t3, t4);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2,
                                                                               T3 t3, T4 t4,
                                                                               T5 t5) const
         {
            expression_node<typename node_type::value_type>* result;
            if (arena_)
            {
               void* mem = arena_->allocate(sizeof(node_type), alignof(node_type));
               if (mem)
               {
                  result = ::new (mem) node_type(t1, t2, t3, t4, t5);
                  mark_arena_node(static_cast<node_type*>(result));
                  result->node_depth();
                  return result;
               }
            }
            result = new node_type(t1, t2, t3, t4, t5);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2,
                                                                               T3 t3, T4 t4,
                                                                               T5 t5, T6 t6) const
         {
            expression_node<typename node_type::value_type>* result;
            if (arena_)
            {
               void* mem = arena_->allocate(sizeof(node_type), alignof(node_type));
               if (mem)
               {
                  result = ::new (mem) node_type(t1, t2, t3, t4, t5, t6);
                  mark_arena_node(static_cast<node_type*>(result));
                  result->node_depth();
                  return result;
               }
            }
            result = new node_type(t1, t2, t3, t4, t5, t6);
            result->node_depth();
            return result;
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6, typename T7>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2,
                                                                               T3 t3, T4 t4,
                                                                               T5 t5, T6 t6,
                                                                               T7 t7) const
         {
            expression_node<typename node_type::value_type>* result;
            if (arena_)
            {
               void* mem = arena_->allocate(sizeof(node_type), alignof(node_type));
               if (mem)
               {
                  result = ::new (mem) node_type(t1, t2, t3, t4, t5, t6, t7);
                  mark_arena_node(static_cast<node_type*>(result));
                  result->node_depth();
                  return result;
               }
            }
            result = new node_type(t1, t2, t3, t4, t5, t6, t7);
            result->node_depth();
            return result;
         }

         template <typename T>
         void inline free(expression_node<T>*& e) const
         {
            exprtk_debug(("node_allocator::free() - deleting expression_node "
                          "type: %03d addr: %p\n",
                          static_cast<int>(e->type()),
                          reinterpret_cast<void*>(e)));
            e->destroy_self();
            e = 0;
         }
      };

      inline void load_operations_map(std::multimap<std::string,details::base_operation_t,details::ilesscompare>& m)
      {
         #define register_op(Symbol, Type, Args)                                             \
         m.insert(std::make_pair(std::string(Symbol),details::base_operation_t(Type,Args))); \

         register_op("abs"       , e_abs     , 1)
         register_op("acos"      , e_acos    , 1)
         register_op("acosh"     , e_acosh   , 1)
         register_op("asin"      , e_asin    , 1)
         register_op("asinh"     , e_asinh   , 1)
         register_op("atan"      , e_atan    , 1)
         register_op("atanh"     , e_atanh   , 1)
         register_op("ceil"      , e_ceil    , 1)
         register_op("cos"       , e_cos     , 1)
         register_op("cosh"      , e_cosh    , 1)
         register_op("exp"       , e_exp     , 1)
         register_op("expm1"     , e_expm1   , 1)
         register_op("floor"     , e_floor   , 1)
         register_op("log"       , e_log     , 1)
         register_op("log10"     , e_log10   , 1)
         register_op("log2"      , e_log2    , 1)
         register_op("log1p"     , e_log1p   , 1)
         register_op("round"     , e_round   , 1)
         register_op("sin"       , e_sin     , 1)
         register_op("sinc"      , e_sinc    , 1)
         register_op("sinh"      , e_sinh    , 1)
         register_op("sec"       , e_sec     , 1)
         register_op("csc"       , e_csc     , 1)
         register_op("sqrt"      , e_sqrt    , 1)
         register_op("tan"       , e_tan     , 1)
         register_op("tanh"      , e_tanh    , 1)
         register_op("cot"       , e_cot     , 1)
         register_op("rad2deg"   , e_r2d     , 1)
         register_op("deg2rad"   , e_d2r     , 1)
         register_op("deg2grad"  , e_d2g     , 1)
         register_op("grad2deg"  , e_g2d     , 1)
         register_op("sgn"       , e_sgn     , 1)
         register_op("not"       , e_notl    , 1)
         register_op("erf"       , e_erf     , 1)
         register_op("erfc"      , e_erfc    , 1)
         register_op("ncdf"      , e_ncdf    , 1)
         register_op("frac"      , e_frac    , 1)
         register_op("trunc"     , e_trunc   , 1)
         register_op("atan2"     , e_atan2   , 2)
         register_op("mod"       , e_mod     , 2)
         register_op("logn"      , e_logn    , 2)
         register_op("pow"       , e_pow     , 2)
         register_op("root"      , e_root    , 2)
         register_op("roundn"    , e_roundn  , 2)
         register_op("equal"     , e_equal   , 2)
         register_op("not_equal" , e_nequal  , 2)
         register_op("hypot"     , e_hypot   , 2)
         register_op("shr"       , e_shr     , 2)
         register_op("shl"       , e_shl     , 2)
         register_op("clamp"     , e_clamp   , 3)
         register_op("iclamp"    , e_iclamp  , 3)
         register_op("inrange"   , e_inrange , 3)
         #undef register_op
      }

   } // namespace details

} // namespace exprtk

#endif // EXPRTK_SRC_NODE_ALLOCATOR_HPP
