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

#include <iostream> // for testing

namespace cg {
    enum v_type {SPLIT, MERGE, LEFT_REGULAR, RIGHT_REGULAR, START, END};

    v_type vertex_type(contour_2::circulator_t c) {
        auto cur = *c;
        c--;
        auto prev = *c;
        c++;
        c++;
        auto next = *c;

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
            auto p1 = s1[1];
            if (chain->v.size() == 2 && s1[0] == chain->v[0] && s1[1] == chain->v[1]) continue;
            if (s1[0] == chain->v[0]) {
                //other side
                for (size_t i = 0; i < v.size() - 1; i++) {
                    result.push_back(triangle_2(p1, v[i + 1], v[i]));
                }
                auto last = v.back();
                v.clear();
                v.push_back(last);
                v.push_back(p1);
                chain->left ^= 1;
            } else if (s1[0] == chain->v[chain->v.size() - 1]) {
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

    std::vector<triangle_2> triangulate(std::vector<contour_2> polygon) {
        typedef contour_2::circulator_t circulator;
        std::vector<triangle_2> result;

        std::vector<circulator> p;
        for (contour_2 &c : polygon) {
            auto start = c.circulator();
            auto cur = start;
            do {
                p.push_back(cur++);
            } while (cur != start);
        }
        std::sort(p.begin(), p.end(), [](const circulator &c1, const circulator &c2) {
                    if (c1->x != c2->x) return c1->x > c2->x;
                    return c1->y > c2->y;
                });
        auto pair_comp = [](const segment_2 &s1, const segment_2 &s2) {
            if (s1[0] != s2[0]) return s1[0] < s2[0];
            return s1[1] < s2[1];
        };
        auto segment_comp = [&pair_comp](const segment_2 &s1, const segment_2 &s2) {
                    if (s1[0].x < s2[0].x) {
                        auto res = orientation(s2[0], s2[1], s1[0]);
                        if (res == CG_COLLINEAR) return pair_comp(s1, s2);
                        return res == CG_RIGHT;
                    } else if (s2[0].x < s1[0].x) {
                        auto res = orientation(s1[0], s1[1], s2[0]); 
                        if (res == CG_COLLINEAR) return pair_comp(s1, s2);
                        return res == CG_LEFT;
                    } else {
                        if (s1[0].y != s2[0].y) return s1[0].y < s2[0].y;
                        return pair_comp(s1, s2);
                    }
                };
        std::map<segment_2, std::pair<point_2, std::vector<std::shared_ptr<monotone_chain>>>, decltype(segment_comp)> helper(segment_comp);
        for (auto &c : p) {
            v_type type = vertex_type(c);
            segment_2 prev_edge(*(c - 1), *c);
            segment_2 cur_edge(*c, *(c + 1)); 
            segment_2 rev_cur_edge(*(c + 1), *c);
            segment_2 rev_prev_edge(*c, *(c - 1));
            if (type == SPLIT) {
                std::cout << "SPLIT" << std::endl;
                auto ej = helper.upper_bound(segment_2(*c, *c));
                auto help = ej->second;
                segment_2 new_seg(help.first, *c);
                auto &chains = help.second;
                decltype(help) new_help;
                help.first = new_help.first = *c;
                if (chains.size() == 2) {
                    //merge
                    add(result, chains, new_seg);
                    new_help.second.push_back(*(--help.second.end()));
                    help.second.erase(--help.second.end());
                    helper[ej->first] = help;
                    helper[cur_edge] = new_help;
                } else {
                    //ordinary
                    add(result, chains, new_seg, false);
                    add(result, new_help.second, new_seg, !chains[0]->left);
                    if (chains[0]->left) {
                        helper[cur_edge] = help;
                        helper[ej->first] = new_help;
                    } else {
                        helper[cur_edge] = new_help;
                        helper[ej->first] = help;
                    }
                }
            }
            if (type == MERGE) {
                std::cout << "MERGE" << std::endl;
                auto ej = helper.find(prev_edge);
                auto help = ej->second;
                segment_2 new_seg(help.first, *c);
                auto &chains = help.second;
                add(result, chains, prev_edge, true);
                std::vector<std::shared_ptr<monotone_chain>> res(2);
                if (chains.size() == 2) {
                    add(result, chains, new_seg);
                    res[1] = chains[1];
                } else {
                    res[1] = chains[0];
                }
                helper.erase(ej);

                auto ej2 = helper.upper_bound(segment_2(*c, *c));
                auto help2 = ej2->second;
                segment_2 new_seg2(help2.first, *c);
                auto &chains2 = help2.second;
                add(result, chains2, rev_cur_edge, false);
                if (chains2.size() == 2) {
                    add(result, chains2, new_seg2);
                    res[0] = chains2[0];
                } else {
                    res[0] = chains2[0];
                }
                helper[ej2->first] = std::make_pair(*c, res);
            } 
            if (type == LEFT_REGULAR) { 
                std::cout << "LEFT REGULAR" << std::endl;
                auto ej = helper.find(prev_edge);
                auto help = ej->second;
                auto &chains = help.second;
                add(result, chains, prev_edge, true);
                std::vector<std::shared_ptr<monotone_chain>> res(1);
                res[0] = chains[0];
                if (chains.size() == 2) {
                    segment_2 new_seg(help.first, *c);
                    add(result, chains, new_seg);
                    res[0] = chains[1];
                }
                helper.erase(ej);
                helper[cur_edge] = std::make_pair(*c, res);
            }
             if (type == RIGHT_REGULAR) { 
                std::cout << "RIGHT REGULAR" << std::endl;
                auto ej = helper.upper_bound(segment_2(*c, *c));
                auto help = ej->second;
                auto &chains = help.second;
                add(result, chains, rev_cur_edge, false);
                std::vector<std::shared_ptr<monotone_chain>> res(1);
                res[0] = chains[0];
                if (chains.size() == 2) {
                    segment_2 new_seg(help.first, *c);
                    add(result, chains, new_seg);
                    res[0] = chains[0];
                }
                helper[ej->first] = std::make_pair(*c, res);
             }
            if (type == START) {
                std::cout << "START" << std::endl;
                helper[cur_edge] = std::make_pair(*c, std::vector<std::shared_ptr<monotone_chain>>(0));
            }
            if (type == END) { 
                std::cout << "END" << std::endl;
                auto ej = helper.find(prev_edge);
                auto help = ej->second;
                auto &chains = help.second;
                add(result, chains, prev_edge);
                add(result, chains, rev_cur_edge);
                if (chains.size() == 2) {
                    segment_2 new_seg(help.first, *c);
                    add(result, chains, new_seg);
                }
                helper.erase(ej);
            }
        }
        return result;
    }
}
