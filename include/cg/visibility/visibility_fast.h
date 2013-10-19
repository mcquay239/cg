#pragma once

#include <cg/primitives/contour.h>
#include <cg/primitives/point.h>
#include <cg/operations/orientation.h>
#include <cg/primitives/segment.h>
#include <cg/common/range.h>

#include <algorithm>
#include <vector>
#include <limits>
#include <stack>
#include <memory>


namespace cg
{
   template<class Scalar>
   struct vertex_2t
   {
      point_2t<Scalar> point;
      vertex_2t<Scalar> *father;
      vertex_2t<Scalar> *left;
      vertex_2t<Scalar> *right;
      vertex_2t<Scalar> *rightmost_son;

      segment_2t<Scalar> back_seg;
      segment_2t<Scalar> front_seg;

      segment_2t<Scalar> *vis;

      vertex_2t(point_2t<Scalar> p,
                segment_2t<Scalar> f_s = segment_2t<Scalar>(point_2t<Scalar>(0, 0), point_2t<Scalar>(0, 0)),
                segment_2t<Scalar> s_s = segment_2t<Scalar>(point_2t<Scalar>(0, 0), point_2t<Scalar>(0, 0)))
         : point(p)
         , father(nullptr)
         , left(nullptr)
         , right(nullptr)
         , rightmost_son(nullptr)
         , back_seg(f_s)
         , front_seg(s_s)
         , vis(nullptr)
      {}

      bool operator< (const vertex_2t<Scalar> &rhs) const
      {
         return rhs.point < point;
      }
   };

   template<class Scalar>
   bool before(vertex_2t<Scalar> *p, vertex_2t<Scalar> *q)
   {
      if (p->vis == nullptr)
      {
         return true;
      }
      orientation_t paq = orientation(p->point, p->vis->operator[](0), q->point);
      orientation_t pbq = orientation(p->point, p->vis->operator[](1), q->point);
      if (paq == CG_LEFT &&
            orientation(p->vis->operator [](0), p->vis->operator [](1), q->point)
            == CG_LEFT)
      {
         return true;
      }
      if (pbq == CG_LEFT &&
            orientation(p->vis->operator [](1), p->vis->operator [](0), q->point)
            == CG_LEFT)
      {
         return true;
      }
      return false;

   }

   template<class Scalar>
   bool before(vertex_2t<Scalar> &p, segment_2t<Scalar> &s)
   {
      if (p.vis == nullptr)
         return true;

      return orientation(min(*(p.vis)), max(*(p.vis)), min(s)) == CG_LEFT ||
             orientation(min(*(p.vis)), max(*(p.vis)), max(s)) == CG_LEFT;

   }

   template<class Scalar>
   void init_vis(vertex_2t<Scalar> &p, segment_2t<Scalar> &s)
   {
      if (((s[0].x <= p.point.x && s[1].x > p.point.x && orientation(s[0], p.point, s[1]) == CG_RIGHT) ||
         ((s[1].x <= p.point.x && s[0].x > p.point.x && orientation(s[1], p.point, s[0]) == CG_RIGHT))) &&
         ((p.back_seg != s) && (p.front_seg != s)) && before(p, s))
      {
         p.vis = &s;
      }
   }

   template<class Scalar>
   orientation_t orient(point_2t<Scalar> &a, point_2t<Scalar> &b, point_2t<Scalar> &c)
   {
      if (c.y == std::numeric_limits<double>::infinity())
      {
         if (a < b)
            return CG_LEFT;
         else
            return CG_RIGHT;
      }
      else
         return orientation(a, b, c);
   }


   template<class Scalar>
   void clean(vertex_2t<Scalar> *v)
   {
      if (v->left != nullptr)
         v->left->right = nullptr;
      if (v->right != nullptr)
         v->right->left = nullptr;
      if (v->father->rightmost_son == v)
         v->father->rightmost_son = v->left;

      v->left = nullptr;
      v->father = nullptr;
      v->right = nullptr;
      v->rightmost_son = nullptr;
   }

   template<class Scalar>
   void add(vertex_2t<Scalar> *p, vertex_2t<Scalar> *q, std::vector<segment_2t<Scalar>> &seg, bool &remove_redundant_edges)
   {
      if (!remove_redundant_edges)
      {
         seg.push_back(segment_2t<Scalar>(p->point, q->point));
         return;
      }
      if (p->back_seg.is_point() || q->back_seg.is_point())
      {
         seg.push_back(segment_2t<Scalar>(p->point, q->point));
         return;
      }
      if (p->back_seg[0] == p->front_seg[1] && q->back_seg[0] == q->front_seg[1])
      {
         seg.push_back(segment_2t<Scalar>(p->point, q->point));
         return;
      }
      if (p->back_seg[0] == p->front_seg[1] && q->back_seg[0] != q->front_seg[1] &&
          orientation(q->back_seg[0], q->back_seg[1], p->point) !=
          orientation(q->front_seg[0], q->front_seg[1], p->point))
      {
         seg.push_back(segment_2t<Scalar>(p->point, q->point));
         return;
      }
      if (p->back_seg[0] != p->front_seg[1] && q->back_seg[0] == q->front_seg[1] &&
          orientation(p->back_seg[0], p->back_seg[1], q->point) !=
          orientation(p->front_seg[0], p->front_seg[1], q->point))
      {
         seg.push_back(segment_2t<Scalar>(p->point, q->point));
         return;
      }
      if (orientation(p->back_seg[0], p->back_seg[1], q->point) !=
          orientation(p->front_seg[0], p->front_seg[1], q->point) &&
          orientation(q->back_seg[0], q->back_seg[1], p->point) !=
          orientation(q->front_seg[0], q->front_seg[1], p->point))
      {
         seg.push_back(segment_2t<Scalar>(p->point, q->point));
         return;
      }
   }

   template<class Scalar>
   void report(vertex_2t<Scalar> *p, vertex_2t<Scalar> *q, std::vector<segment_2t<Scalar>> &seg, bool &remove_redundant_edges)
   {
      if (p->back_seg.is_point() || p->back_seg[0] == p->front_seg[1])
      {
         add(p, q, seg, remove_redundant_edges);
      }
      if (orientation(p->back_seg[0], p->back_seg[1], p->front_seg[1]) == CG_LEFT)
      {
         if (orientation(p->back_seg[0], p->back_seg[1], q->point) == CG_RIGHT ||
               orientation(p->front_seg[0], p->front_seg[1], q->point) == CG_RIGHT)
         {
            add(p, q, seg, remove_redundant_edges);
         }
      }
      else
      {
         if (orientation(p->back_seg[0], p->back_seg[1], q->point) == CG_RIGHT &&
               orientation(p->front_seg[0], p->front_seg[1], q->point) == CG_RIGHT)
         {
            add(p, q, seg, remove_redundant_edges);
         }
      }
   }

   template<class Scalar>
   void handle(vertex_2t<Scalar> *p, vertex_2t<Scalar> *q, std::vector<segment_2t<Scalar>> &seg, bool &remove_redundant_edges)
   {
      if (q->point == p->back_seg[0] || q->point == p->front_seg[1])
      {
         if (orientation(p->point, q->point, q->back_seg[0]) == CG_LEFT)
            p->vis = &(q->back_seg);
         else if (orientation(p->point, q->point, q->front_seg[1]) == CG_LEFT)
            p->vis = &(q->front_seg);
         else
            p->vis = q->vis;
         return;
      }
      if (p->vis != nullptr)
      {
         if (*(p->vis) == q->back_seg || *(p->vis) == q->front_seg)
         {
            orientation_t pqa = orientation(p->point, q->point, q->back_seg[0]);
            orientation_t pqb = orientation(p->point, q->point, q->front_seg[1]);
            if (pqa != CG_LEFT && pqb != CG_LEFT)
            {
               p->vis = q->vis;
            }
            else if (pqa == CG_LEFT)
            {
               if (!(q->back_seg.is_point()))
                  p->vis = &(q->back_seg);
            }
            else
            {
               if (!(q->front_seg.is_point()))
                  p->vis = &(q->front_seg);
            }
            report(p, q, seg, remove_redundant_edges);
            return;
         }
      }
      if (before(p, q))
      {
         if (q->back_seg[0] != q->front_seg[1])
         {
            if (orientation(q->back_seg[0], q->back_seg[1], q->front_seg[1]) == CG_LEFT)
            {
               if (!(q->back_seg.is_point()))
                  p->vis = &(q->back_seg);
            }
            else
            {
               if (!(q->front_seg.is_point()))
                  p->vis = &(q->front_seg);
            }
         }
         else if (!(q->back_seg.is_point()))
         {
            if (orientation(p->point, q->back_seg[0], q->back_seg[1]) != CG_COLLINEAR)
               p->vis = &(q->back_seg);
         }
         report(p, q, seg, remove_redundant_edges);
      }

   }

   template<class Scalar>
   void rotation_tree(std::vector<vertex_2t<Scalar>> &vec, std::vector<segment_2t<Scalar>> &seg, bool &remove_redundant_edges)
   {
      std::stack<vertex_2t<Scalar> *> st;
      std::sort(vec.begin(), vec.end());

      Scalar max_x = 0;
      for (vertex_2t<Scalar> v : vec)
         if (v.point.x > max_x)
            max_x = v.point.x;

      std::shared_ptr<vertex_2t<Scalar>> p_inf(new vertex_2t<Scalar>(point_2t<Scalar>(max_x + 1, (Scalar) std::numeric_limits<double>::infinity())));
      std::shared_ptr<vertex_2t<Scalar>> n_inf(new vertex_2t<Scalar>(point_2t<Scalar>(max_x + 1, (Scalar) std::numeric_limits<double>::infinity() * (-1))));

      p_inf->rightmost_son = n_inf.get();
      n_inf->father = p_inf.get();
      {
         auto it = vec.begin();
         it->father = n_inf.get();
         n_inf->rightmost_son = &(*it++);

         for (; it != vec.end(); ++it)
         {
            it->left = n_inf->rightmost_son;
            n_inf->rightmost_son->right = &(*it);
            n_inf->rightmost_son = &(*it);
            it->father = n_inf.get();
         }
      }
      st.push(&(vec[0]));
      while(!st.empty())
      {
         vertex_2t<Scalar> *p = st.top(); st.pop();
         vertex_2t<Scalar> *p_r = p->right;
         vertex_2t<Scalar> *q = p->father;
         vertex_2t<Scalar> *z = q->left;
         if (q != n_inf.get())
            handle(p, q, seg, remove_redundant_edges);
         clean(p);

         if (z == nullptr)
         {
            q->left = p;
            p->right = q;
            p->father = q->father;
         }
         else if (orient(p->point, z->point, z->father->point) != CG_LEFT)
         {
            p->left = q->left;
            q->left->right = p;
            q->left = p;
            p->right = q;
            p->father = q->father;
         }
         else
         {
            while(z->rightmost_son != nullptr && orient(p->point, z->rightmost_son->point, z->point) != CG_RIGHT)
               z = z->rightmost_son;

            if (z->rightmost_son != nullptr)
            {
               z->rightmost_son->right = p;
               p->left = z->rightmost_son;
            }
            z->rightmost_son = p;
            p->father = z;
            if (!st.empty() && st.top() == z)
               st.pop();
         }
         if (p->left == nullptr && p->father != p_inf.get())
            st.push(p);
         if (p_r != nullptr)
            st.push(p_r);
      }
   }

   template<class Scalar>
   std::vector<segment_2t<Scalar>> get_visibility_graph(std::vector<contour_2t<Scalar>> &v_cont, bool remove_redundant_edges = true)
   {
      std::vector<segment_2t<Scalar>> v_seg;
      std::vector<vertex_2t<Scalar>> v_vert;
      for (contour_2t<Scalar> contour : v_cont)
      {
         common::range_circulator<contour_2t<Scalar>> circulation(contour);
         for (int i = 0; i < contour.vertices_num(); i++)
         {
            point_2t<Scalar> l = *(--circulation);
            point_2t<Scalar> p = *(++circulation);
            point_2t<Scalar> r = *(++circulation);
            segment_2t<Scalar> s1(l, p);
            segment_2t<Scalar> s2(p, r);
            v_vert.push_back(vertex_2t<Scalar>(p, s1, s2));
            v_seg.push_back(s2);

         }
      }

      for (auto it = v_vert.begin(); it != v_vert.end(); ++it)
         for (auto its = v_seg.begin(); its != v_seg.end(); ++its)
            init_vis(*it, *its);

      if (!v_vert.empty())
         rotation_tree(v_vert, v_seg, remove_redundant_edges);

      return v_seg;
   }
}
