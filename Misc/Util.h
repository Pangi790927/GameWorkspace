#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <sstream>
#include <string>
#include <initializer_list>

#include <vector>
#include <chrono>
#include <set>

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef DEBUG
    #ifdef DEBUG_PRINT
        #include <iostream>
        #define DEBUG(x) std::cout << __FUNCTION__ << ": " << x << std::endl;
    #else
        #define DEBUG(x) ;
    #endif
#endif

#define DBG(fmt, ...) printf("[%s:%d] %s() :> " fmt "\n",\
        __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#include "demangle.h"
#include "except.h"

namespace Util {

	template <typename Type, int MAX_SIZE = 100>
	class StaticQueue {
	public:
		Type cont[MAX_SIZE];
		int start = 0;
		int end = 0;
		int size = 0;

		void insert (const Type& arg) {
			if (size < MAX_SIZE) {
				cont[start] = arg;
				start++;
				if (start >= MAX_SIZE)
					start = 0;
				size++;
			}
			else {
				cont[start] = arg;
				start++;
				end++;
				if (end >= MAX_SIZE)
					end = 0;
				if (start >= MAX_SIZE)
					start = 0;
			}
		}

		Type pop() {
			if (size > 0) {
				int toRet = end;
				size--;
				end++;
				if (end >= MAX_SIZE)
					end = 0;
				return cont[toRet];
			}
            throw std::runtime_error("Can't pop, size <= 0");
		}

		bool empty() {
			return size == 0;
		}
	};

    template <typename T>
    struct BinPack {
        int w = 0;
        int h = 0;
        struct rect_t {
            int x, y;
            int w, h;
            T data;

            bool intersect(const rect_t& oth) {
                if (x >= oth.x + oth.w || y >= oth.y + oth.h ||
                        oth.x >= x + w || oth.y >= y + h)
                    return false;
                return true;
            }
        };
        std::vector<rect_t> in_place;

        /* I am lazy, every added rectangle does the following:
            before adding it: try to place it in all corners and if it doesn't
            intersect anyone we are good to go else we are not. If no one is
            intersected that corner is remembered and the new area is computed.
            That corner with lowest area is used, by removing it and placing the
            new rectangle there. In the rightmost corner a new corner is placed.
            
            aaaaaaaaaaabbbbbbbbb#
            aaaaaaaaaaabbbbbbbbb 
            aaaaaaaaaaaxxxxx#
            aaaaaaaaaaaxxxxx
            cccccc#    xxxxx
            cccccc     xxxxx
            cccccc     #
            cccccc 
            ttttttttt#
            ttttttttt 
            ttttttttt 
            vvvvvvv#
            vvvvvvv 
            vvvvvvv 
            #

            # - denote corners where a new placement is tried
            not the best packing but I hope it does the job
        */
        struct point_t {
            int x, y;

            point_t(int x = 0, int y = 0) : x(x), y(y) {}

            bool operator < (const point_t& oth) const {
                if (x == oth.x)
                    return y < oth.y;
                return x < oth.x;
            }
        };
        std::set<point_t> corners;

        BinPack() {
            corners.insert(point_t(0, 0));
        }

        void insert(int user_w, int user_h, T data) {
            rect_t rect;
            rect.w = user_w;
            rect.h = user_h;
            rect.data = data;
            point_t best_corner;
            int best_x = w + rect.w;
            int best_y = h + rect.h;
            int best = best_x + best_y;
            for (auto &&corner : corners) {
                bool invalid = false;
                for (auto &&placed : in_place) {
                    rect.x = corner.x;
                    rect.y = corner.y;
                    if (placed.intersect(rect)) {
                        invalid = true;
                        break;
                    }
                }
                if (!invalid) {
                    int local_best = std::max(rect.x + rect.w, w) +
                        std::max(rect.y + rect.h, h);
                    if (local_best < best ||
                            (local_best == best && (corner.x < best_x)) ||
                            (local_best == best && (corner.y < best_y))) {
                        best = local_best;
                        best_corner = corner;
                        best_x = corner.x;
                        best_y = corner.y;
                    }
                }
            }
            int new_w = std::max(best_corner.x + rect.w, w);
            int new_h = std::max(best_corner.y + rect.h, h);
            if (new_h > h)
                corners.insert(point_t(0, new_h));
            if (new_w > w)
                corners.insert(point_t(new_w, 0));
            corners.erase(best_corner);
            rect.x = best_corner.x;
            rect.y = best_corner.y;
            in_place.push_back(rect);
            std::vector<point_t> to_erase;
            for (auto &&corner : corners) {
                rect_t corner_rec;
                corner_rec.x = corner.x;
                corner_rec.w = 1;
                corner_rec.y = corner.y;
                corner_rec.h = 1;
                if (corner_rec.intersect(rect))
                    to_erase.push_back(corner);
            }
            for (auto &&to_del : to_erase)
                corners.erase(to_del);
            add_corner(rect.x + rect.w, rect.y);
            add_corner(rect.x, rect.y + rect.h);

            w = new_w;
            h = new_h;
        }

        void add_corner(int x, int y) {
            /* TODO: fix this line bellow, rect_t could hold any type of data*/
            rect_t rect;
            rect.x = x;
            rect.w = 1;
            rect.y = y;
            rect.h = 1;
            bool invalid = false;
            for (auto &&placed : in_place)
                if (rect.intersect(placed))
                    invalid = true;
            if (!invalid)
                corners.insert(point_t(rect.x, rect.y));
        }

        auto begin() {
            return in_place.begin();
        }

        auto end() {
            return in_place.end();
        }
    };

    //Text not by const reference
    //so that the function can be used with a character array as argument
    template <typename Type>
    Type stringToNumber ( const std::string &Text )
    {
        std::stringstream ss(Text);
        Type result;
        return ss >> result ? result : 0;
    }

    template <typename Type>
    std::string numberToString ( Type Number )
    {
        std::stringstream ss;
        ss << Number;
        return ss.str();
    }

    template <typename Type, typename... Args>
    bool eq (Type a, Type b, Args... args) {
        return a == b && eq(args...);
    }

    template <typename Type>
    bool eq (Type a, Type b, Type c) {
        return a == b && b == c;
    }

    template <typename Type>
    bool eq (Type a, Type b) {
        return a == b;
    }

    template <typename Type>
    bool isEqualToAny (Type val, const std::initializer_list<Type>& list) {
        for (const auto& i : list) {
            if (val == i) {
                return true;
            }
        }
        return false;
    }

    float PI = 3.141592653589;

    float toRadians (float angle) {
        return angle * PI / 180.0f;
    }

    std::string getOpenGLError() {
        GLenum errCode;
        const unsigned char *errString = NULL;
        
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            errString = gluErrorString(errCode);
            return "OpenGL Error: " + std::string((const char*)errString) + " : " + Util::numberToString(errCode);
        }
        else {
            return "NO_ERROR";
        }
    }

    template <typename Out>
    void split(const std::string &s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        
        while (getline(ss, item, delim)) {
            *(result++) = item;
        }

        if (ss.str() != "") {
            *(result++) = ss.str();   
        }
    }


	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	template< typename T >
	struct array_deleter
	{
		void operator ()( T const * p)
		{ 
			delete[] p; 
		}
	};

	template <typename A, typename B>
	struct same_class {
		const static bool value = false;
	};

	template <typename A>
	struct same_class<A, A> {
		const static bool value = true;
	};

	template <typename A, typename B, int value>
	struct if_true {
		using type = A;
	};

	template <typename A, typename B>
	struct if_true<A, B, false> {
		using type = B;
	};

    uint64_t get_time_ms() {
        using namespace std::chrono;
        milliseconds ms = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
        );
        return ms.count();
    }
}


#endif // UTIL_H_INCLUDED