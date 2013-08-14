#pragma once

#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/triangle.h>
#include <cg/primitives/segment.h>
#include <cg/operations/orientation.h>

namespace cg {
   enum v_type {SPLIT, MERGE, LEFT_REGULAR, RIGHT_REGULAR, START, END};

   v_type vertex_type(const contour_2::circulator_t &c) {
      auto cur = *c;
      auto prev = *(c - 1);
      auto next = *(c + 1);

      bool right = orientation(prev, cur, next) == CG_RIGHT;
      if (cur > prev && cur > next) return right ? SPLIT : START;
      if (cur < prev && cur < next) return right ? MERGE : END;
      return next > cur ? RIGHT_REGULAR : LEFT_REGULAR;
   }

   struct monotone_chain {
      bool left;
      std::vector<point_2> v;
      monotone_chain() {}

      monotone_chain(const segment_2 &s1, bool left) : left(left) {
         v.push_back(s1[0]);
         v.push_back(s1[1]);
      }
   };

   void add(std::vector<triangle_2> &result, std::vector<std::shared_ptr<monotone_chain>> &chains,
         const segment_2 &s1, bool left = false) {
      if (chains.size() == 0) {
         chains.push_back(std::shared_ptr<monotone_chain>(new monotone_chain(s1, left)));
         return;
      }
      for (auto &chain : chains) {
         auto &v = chain->v;
         auto &p1 = s1[1];
         if (v.size() == 2 && s1[0] == v[0] && s1[1] == v[1]) continue;
         if (s1[0] == v[0]) {
            //other side
            for (size_t i = 0; i < v.size() - 1; i++) {
               result.push_back(triangle_2(p1, v[i + 1], v[i]));
            }
            v.erase(v.begin(), v.end() - 1);
            v.push_back(p1);
            chain->left ^= 1;
         } else if (s1[0] == v.back()) {
            //same side
            orientation_t need = chain->left ? CG_RIGHT : CG_LEFT;
            while (v.size() > 1 && orientation(p1, v[v.size() - 1], v[v.size() - 2]) == need) {
               result.push_back(triangle_2(p1, v[v.size() - 1], v[v.size() - 2]));
               v.pop_back();
            }
            v.push_back(p1);
         }
      }
   }

   std::vector<triangle_2> triangulate(const std::vector<contour_2> &polygon) {
      std::vector<triangle_2> result;

      std::vector<contour_2::circulator_t> p;
      for (const contour_2 &c : polygon) {
         auto start = c.circulator();
         auto cur = start;
         do {
            p.push_back(cur++);
         } while (cur != start);
      }
      std::sort(p.begin(), p.end(),
      [](const contour_2::circulator_t &c1, const contour_2::circulator_t &c2) { return *c1 > *c2; });
      auto segment_comp = [](const segment_2 &s1, const segment_2 &s2) {
               if (s1[0].x < s2[0].x) {
                  auto res = orientation(s2[0], s2[1], s1[0]);
                  if (res != CG_COLLINEAR) return res == CG_LEFT;
               } else if (s2[0].x < s1[0].x) {
                  auto res = orientation(s1[0], s1[1], s2[0]);
                  if (res != CG_COLLINEAR) return res == CG_RIGHT;
               }
               if (s1[0] != s2[0]) return s1[0] < s2[0];
               return s1[1] < s2[1];
            };
      typedef std::vector<std::shared_ptr<monotone_chain>> chains_t;
      std::map<segment_2, std::pair<point_2, std::vector<std::shared_ptr<monotone_chain>>>,
         decltype(segment_comp)> helper(segment_comp);
      auto left_cont = [&helper, &result](const segment_2 &prev_edge, const point_2 &p, chains_t &res) {
               const auto &ej = helper.lower_bound(prev_edge);
               auto &old_helper = ej->second.first;
               auto &chains = ej->second.second;
               segment_2 new_seg(old_helper, p);
               add(result, chains, prev_edge, true);
               if (chains.size() == 2) {
                  add(result, chains, new_seg);
                  res[res.size() - 1] = chains[1];
               } else {
                  res[res.size() - 1] = chains[0];
               }
               helper[ej->first] = std::make_pair(p, res);
               helper.erase(prev_edge);
            };
      auto right_cont = [&helper, &result](const segment_2 &rev_cur_edge, const point_2 &p, chains_t &res) {
               segment_2 cur_vertex(p, p);
               const auto &ej = helper.lower_bound(cur_vertex);
               auto &old_helper = ej->second.first;
               auto &chains = ej->second.second;
               segment_2 new_seg(old_helper, p);
               add(result, chains, rev_cur_edge, false);
               res[0] = chains[0];
               if (chains.size() == 2) add(result, chains, new_seg);
               helper[ej->first] = std::make_pair(p, res);
               helper.erase(cur_vertex);
            };
      for (auto &c : p) {
         v_type type = vertex_type(c);
         segment_2 prev_edge(*(c - 1), *c);
         segment_2 cur_edge(*c, *(c + 1));
         segment_2 rev_cur_edge(*(c + 1), *c);
         segment_2 cur_vertex(*c, *c);
         if (type == SPLIT) {
            const auto &ej = helper.lower_bound(cur_vertex);
            auto &old_helper = ej->second.first;
            auto &chains = ej->second.second;
            segment_2 new_seg(old_helper, *c);
            chains_t new_chains;
            point_2 new_helper;
            new_helper = old_helper = *c;
            add(result, chains, new_seg, false);
            if (chains.size() == 2) {
               //merge
               new_chains.push_back(*(--chains.end()));
               chains.erase(--chains.end());
               helper[ej->first] = std::make_pair(old_helper, chains);
               helper[cur_edge] = std::make_pair(new_helper, new_chains);
            } else {
               //ordinary
               add(result, new_chains, new_seg, !chains[0]->left);
               if (chains[0]->left) {
                  helper[cur_edge] = std::make_pair(old_helper, chains);
                  helper[ej->first] = std::make_pair(new_helper, new_chains);
               } else {
                  helper[cur_edge] = std::make_pair(new_helper, new_chains);
                  helper[ej->first] = std::make_pair(old_helper, chains);
               }
            }
         }
         chains_t res;
         if (type == MERGE) res = chains_t(2);
         else if (type == LEFT_REGULAR || type == RIGHT_REGULAR || type == END) res = chains_t(1);

         if (type == MERGE) {
            left_cont(prev_edge, *c, res);
            right_cont(rev_cur_edge, *c, res);
         }
         if (type == END) {
            right_cont(rev_cur_edge, *c, res);
            left_cont(prev_edge, *c, res);
         }
         if (type == LEFT_REGULAR) left_cont(prev_edge, *c, res);
         if (type == RIGHT_REGULAR) right_cont(rev_cur_edge, *c, res);

         if (type == LEFT_REGULAR || type == START) helper[cur_edge] = std::make_pair(*c, res);
      }
      return result;
   }
}
