/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk vs Native Benchmarks                                *
 * Author: Arash Partow (1999-2025)                           *
 * URL: https://www.partow.net/programming/exprtk/index.html  *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the MIT License.                   *
 * https://www.opensource.org/licenses/MIT                    *
 * SPDX-License-Identifier: MIT                               *
 *                                                            *
 **************************************************************
*/


#include <cstdio>
#include <cmath>
#include <fstream>
#include <string>
#include <deque>
#include <map>

#include "exprtk.hpp"


const std::string global_expression_list[] =
   {
      "(y + x)",
      "2 * (y + x)",
      "(2 * y + 2 * x)",
      "((1.23 * x^2) / y) - 123.123",
      "(y + x / y) * (x - y / x)",
      "x / ((x + y) + (x - y)) / y",
      "1 - ((x * y) + (y / x)) - 3",
      "(5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)",
      "1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55",
      "sin(2 * x) + cos(pi / y)",
      "1 - sin(2 * x) + cos(pi / y)",
      "sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)",
      "(x^2 / sin(2 * pi / y)) - x / 2",
      "x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y",
      "clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
      "max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))",
      "if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x"
   };

const std::size_t global_expression_list_size = sizeof(global_expression_list) / sizeof(std::string);

static const double global_lower_bound_x = -100.0;
static const double global_lower_bound_y = -100.0;
static const double global_upper_bound_x = +100.0;
static const double global_upper_bound_y = +100.0;
static const double global_delta         = 0.0111;


template <typename T,
          typename Allocator,
          template <typename,typename> class Sequence>
bool load_expression(exprtk::symbol_table<T>& symbol_table,
                     Sequence<exprtk::expression<T>,Allocator>& expr_seq)
{
   exprtk::parser<double> parser;

   for (std::size_t i = 0; i < global_expression_list_size; ++i)
   {
      exprtk::expression<double> expression;
      expression.register_symbol_table(symbol_table);

      if (!parser.compile(global_expression_list[i],expression))
      {
         printf("[load_expression] - Parser Error: %s\tExpression: %s\n",
                parser.error().c_str(),
                global_expression_list[i].c_str());

         return false;
      }

      expr_seq.push_back(expression);
   }

   return true;
}

template <typename T>
void run_exprtk_benchmark(T& x, T& y,
                          exprtk::expression<T>& expression,
                          const std::string& expr_string)
{
   T total = T(0);
   unsigned int count = 0;

   exprtk::timer timer;
   timer.start();

   for (x = global_lower_bound_x; x <= global_upper_bound_x; x += global_delta)
   {
      for (y = global_lower_bound_y; y <= global_upper_bound_y; y += global_delta)
      {
         total += expression.value();
         ++count;
      }
   }

   timer.stop();

   if (T(0) != total)
      printf("[exprtk] Total Time:%12.8f  Rate:%14.3f evals/sec Expression: %s\n",
             timer.time(),
             count / timer.time(),
             expr_string.c_str());
   else
      printf("run_exprtk_benchmark() - Error running benchmark for expression: %s\n",expr_string.c_str());
}

template <typename T> struct native;

template <typename T, typename NativeFunction>
void run_native_benchmark(T& x, T& y, NativeFunction f, const std::string& expr_string)
{
   T total = T(0);
   unsigned int count = 0;

   exprtk::timer timer;
   timer.start();

   for (x = global_lower_bound_x; x <= global_upper_bound_x; x += global_delta)
   {
      for (y = global_lower_bound_y; y <= global_upper_bound_y; y += global_delta)
      {
         total += f(x,y);
         ++count;
      }
   }

   timer.stop();

   if (T(0) != total)
      printf("[native] Total Time:%12.8f  Rate:%14.3f evals/sec Expression: %s\n",
             timer.time(),
             count / timer.time(),
             expr_string.c_str());
   else
      printf("run_native_benchmark() - Error running benchmark for expression: %s\n",expr_string.c_str());
}

template <typename T>
bool run_parse_benchmark(exprtk::symbol_table<T>& symbol_table)
{
   static const std::size_t rounds = 100000;
   exprtk::parser<double>     parser;
   exprtk::expression<double> expression;

   expression.register_symbol_table(symbol_table);

   for (std::size_t i = 0; i < global_expression_list_size; ++i)
   {
      exprtk::timer timer;
      timer.start();

      for (std::size_t r = 0; r < rounds; ++r)
      {
         if (!parser.compile(global_expression_list[i],expression))
         {
            printf("[run_parse_benchmark] - Parser Error: %s\tExpression: %s\n",
                   parser.error().c_str(),
                   global_expression_list[i].c_str());

            return false;
         }
      }

      timer.stop();

      printf("[parse] Total Time:%12.8f  Rate:%14.3f parse/sec Expression: %s\n",
             timer.time(),
             rounds / timer.time(),
             global_expression_list[i].c_str());
   }

   return true;
}

const double pi = 3.141592653589793238462643383279502;

template <typename T>
struct native
{
   typedef typename exprtk::details::functor_t<T> functor_t;
   typedef typename functor_t::Type Type;

   static inline T avg(Type x, Type y)
   {
      return (x + y) / T(2);
   }

   static inline T clamp(const Type l, const Type v, const Type u)
   {
      return ((v < l) ? l : ((v > u) ? u : v));
   }

   static inline T func00(Type x, Type y)
   {
      return (y + x);
   }

   static inline T func01(Type x, Type y)
   {
      return T(2) * (y + x);
   }

   static inline T func02(Type x, Type y)
   {
      return (T(2) * y + T(2) * x);
   }

   static inline T func03(Type x, Type y)
   {
      return ((T(1.23) * (x * x)) / y) - T(123.123);
   }

   static inline T func04(Type x, Type y)
   {
      return (y + x / y) * (x - y / x);
   }

   static inline T func05(Type x, Type y)
   {
      return x / ((x + y) + (x - y)) / y;
   }

   static inline T func06(Type x, Type y)
   {
      return T(1) - ((x * y) + (y / x)) - T(3);
   }

   static inline T func07(Type x, Type y)
   {
      return (T(5.5) + x) + (T(2) * x - T(2) / T(3) * y) * (x / T(3) + y / T(4)) + (y + T(7.7));
   }

   static inline T func08(Type x, Type y)
   {
      using namespace std;
      return (T(1.1)*pow(x,T(1))+T(2.2)*pow(y,T(2))-T(3.3)*pow(x,T(3))+T(4.4)*pow(y,T(15))-T(5.5)*pow(x,T(23))+T(6.6)*pow(y,T(55)));
   }

   static inline T func09(Type x, Type y)
   {
      return std::sin(T(2) * x) + std::cos(pi / y);
   }

   static inline T func10(Type x, Type y)
   {
      return T(1) - std::sin(T(2) * x) + std::cos(pi / y);
   }

   static inline T func11(Type x, Type y)
   {
      return std::sqrt(T(111.111) - std::sin(T(2) * x) + std::cos(pi / y) / T(333.333));
   }

   static inline T func12(Type x, Type y)
   {
      return ((x * x) / std::sin(T(2) * pi / y)) - x / T(2);
   }

   static inline T func13(Type x, Type y)
   {
      return (x + (std::cos(y - std::sin(T(2) / x * pi)) - std::sin(x - std::cos(T(2) * y / pi))) - y);
   }

   static inline T func14(Type x, Type y)
   {
      return clamp(T(-1), std::sin(T(2) * pi * x) + std::cos(y / T(2) * pi), + T(1));
   }

   static inline T func15(Type x, Type y)
   {
      return std::max(T(3.33), std::min(sqrt(T(1) - std::sin(T(2) * x) + std::cos(pi / y) / T(3)), T(1.11)));
   }

   static inline T func16(Type x, Type y)
   {
      return (((y + (x * T(2.2))) <= (x + y + T(1.1))) ? x - y : x * y) + T(2) * pi / x;
   }
};

double pgo_primer();
void perform_file_based_benchmark(const std::string& file_name, const std::size_t& rounds = 100000);
void perform_special_benchmark(const std::size_t& rounds, bool benchmark_parse = false);

int main(int argc, char* argv[])
{
   if (argc >= 2)
   {
      const std::string arg_1 = argv[1];

      if (arg_1 == "special_parse") {
         size_t rounds = argc >= 3 ? atoi(argv[2]) : 34567;
         perform_special_benchmark(rounds, true);
         return 0;
      }

      if (arg_1 == "special_eval") {
         size_t rounds = argc >= 3 ? atoi(argv[2]) : 3456789;
         perform_special_benchmark(rounds, false);
         return 0;
      }

      if (argc == 2)
         perform_file_based_benchmark(arg_1);
      else
         perform_file_based_benchmark(arg_1,atoi(argv[2]));

      return 0;
   }

   pgo_primer();

   double x = 0;
   double y = 0;

   exprtk::symbol_table<double> symbol_table;
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);

   std::deque<exprtk::expression<double> > compiled_expr_list;

   if (!load_expression(symbol_table,compiled_expr_list))
   {
      return 1;
   }

   {
      printf("--- EXPRTK ---\n");
      for (std::size_t i = 0; i < compiled_expr_list.size(); ++i)
      {
         run_exprtk_benchmark(x,y,compiled_expr_list[i],global_expression_list[i]);
      }
   }

   {
      printf("--- NATIVE ---\n");
      run_native_benchmark(x, y, native<double>::func00,global_expression_list[ 0]);
      run_native_benchmark(x, y, native<double>::func01,global_expression_list[ 1]);
      run_native_benchmark(x, y, native<double>::func02,global_expression_list[ 2]);
      run_native_benchmark(x, y, native<double>::func03,global_expression_list[ 3]);
      run_native_benchmark(x, y, native<double>::func04,global_expression_list[ 4]);
      run_native_benchmark(x, y, native<double>::func05,global_expression_list[ 5]);
      run_native_benchmark(x, y, native<double>::func06,global_expression_list[ 6]);
      run_native_benchmark(x, y, native<double>::func07,global_expression_list[ 7]);
      run_native_benchmark(x, y, native<double>::func08,global_expression_list[ 8]);
      run_native_benchmark(x, y, native<double>::func09,global_expression_list[ 9]);
      run_native_benchmark(x, y, native<double>::func10,global_expression_list[10]);
      run_native_benchmark(x, y, native<double>::func11,global_expression_list[11]);
      run_native_benchmark(x, y, native<double>::func12,global_expression_list[12]);
      run_native_benchmark(x, y, native<double>::func13,global_expression_list[13]);
      run_native_benchmark(x, y, native<double>::func14,global_expression_list[14]);
      run_native_benchmark(x, y, native<double>::func15,global_expression_list[15]);
      run_native_benchmark(x, y, native<double>::func16,global_expression_list[16]);
   }

   {
      printf("--- PARSE ----\n");
      run_parse_benchmark(symbol_table);
   }

   return 0;
}

double pgo_primer()
{
   static const double lower_bound_x = -50.0;
   static const double lower_bound_y = -50.0;
   static const double upper_bound_x = +50.0;
   static const double upper_bound_y = +50.0;
   static const double delta = 0.07;

   double total = 0.0;

   for (double x = lower_bound_x; x <= upper_bound_x; x += delta)
   {
      for (double y = lower_bound_y; y <= upper_bound_y; y += delta)
      {
         total += native<double>::func00(x,y);
         total += native<double>::func01(x,y);
         total += native<double>::func02(x,y);
         total += native<double>::func03(x,y);
         total += native<double>::func04(x,y);
         total += native<double>::func05(x,y);
         total += native<double>::func06(x,y);
         total += native<double>::func07(x,y);
         total += native<double>::func08(x,y);
         total += native<double>::func09(x,y);
         total += native<double>::func10(x,y);
         total += native<double>::func11(x,y);
         total += native<double>::func12(x,y);
         total += native<double>::func13(x,y);
         total += native<double>::func14(x,y);
         total += native<double>::func15(x,y);
         total += native<double>::func16(x,y);
      }
   }

   return total;
}

inline std::size_t load_expression_file(const std::string& file_name, std::deque<std::string>& expression_list)
{
   std::ifstream stream(file_name.c_str());

   if (!stream) return 0;

   std::string buffer;
   buffer.reserve(1024);

   while (std::getline(stream,buffer))
   {
      if (buffer.empty())
         continue;
      else if ('#' == buffer[0])
         continue;

      expression_list.push_back(buffer);
   }

   return expression_list.size();
}

void perform_file_based_benchmark(const std::string& file_name, const std::size_t& rounds)
{
   std::deque<std::string> expr_str_list;

   if (0 == load_expression_file(file_name,expr_str_list))
   {
      printf("Failed to load any expressions from: %s\n", file_name.c_str());
      return;
   }

   typedef exprtk::symbol_table<double> symbol_table_t;
   typedef exprtk::expression<double>   expression_t;
   typedef exprtk::parser<double>       parser_t;

   std::deque<expression_t> expression_list;

   symbol_table_t symbol_table;

   double a = 1.1;
   double b = 2.2;
   double c = 3.3;
   double x = 2.123456;
   double y = 3.123456;
   double z = 4.123456;
   double w = 5.123456;

   exprtk::rtl::vecops::package<double> vector_package;

   symbol_table.add_variable("a", a);
   symbol_table.add_variable("b", b);
   symbol_table.add_variable("c", c);

   symbol_table.add_variable("x", x);
   symbol_table.add_variable("y", y);
   symbol_table.add_variable("z", z);
   symbol_table.add_variable("w", w);

   exprtk::polynomial<double, 1> poly01;
   exprtk::polynomial<double, 2> poly02;
   exprtk::polynomial<double, 3> poly03;
   exprtk::polynomial<double, 4> poly04;
   exprtk::polynomial<double, 5> poly05;
   exprtk::polynomial<double, 6> poly06;
   exprtk::polynomial<double, 7> poly07;
   exprtk::polynomial<double, 8> poly08;
   exprtk::polynomial<double, 9> poly09;
   exprtk::polynomial<double,10> poly10;
   exprtk::polynomial<double,11> poly11;
   exprtk::polynomial<double,12> poly12;

   symbol_table.add_function("poly01", poly01);
   symbol_table.add_function("poly02", poly02);
   symbol_table.add_function("poly03", poly03);
   symbol_table.add_function("poly04", poly04);
   symbol_table.add_function("poly05", poly05);
   symbol_table.add_function("poly06", poly06);
   symbol_table.add_function("poly07", poly07);
   symbol_table.add_function("poly08", poly08);
   symbol_table.add_function("poly09", poly09);
   symbol_table.add_function("poly10", poly10);
   symbol_table.add_function("poly11", poly11);
   symbol_table.add_function("poly12", poly12);

   symbol_table.add_package(vector_package);


   static double e = exprtk::details::numeric::constant::e;
   symbol_table.add_variable("e", e, true);

   symbol_table.add_constants();

   {
      parser_t parser;

      for (std::size_t i = 0; i < expr_str_list.size(); ++i)
      {
         expression_t expression;
         expression.register_symbol_table(symbol_table);

         if (!parser.compile(expr_str_list[i],expression))
         {
            printf("[perform_file_based_benchmark] - Parser Error: %s\tExpression: %s\n",
                   parser.error().c_str(),
                   expr_str_list[i].c_str());

            return;
         }

         expression_list.push_back(expression);
      }
   }

   exprtk::timer total_timer;
   exprtk::timer timer;

   double single_eval_total_time = 0.0;

   total_timer.start();

   for (std::size_t i = 0; i < expression_list.size(); ++i)
   {
      expression_t& expression = expression_list[i];

      a = 1.1;
      b = 2.2;
      c = 3.3;
      x = 2.123456;
      y = 3.123456;
      z = 4.123456;
      w = 5.123456;

      timer.start();
      double sum = 0.0;

      for (std::size_t r = 0; r < rounds; ++r)
      {
         sum += expression.value();
         std::swap(a,b);
         std::swap(x,y);
      }

      timer.stop();

      printf("Expression %3d of %3d %9.3f ns\t%10d ns\t(%30.10f)  '%s'\n",
             static_cast<int>(i + 1),
             static_cast<int>(expression_list.size()),
             (timer.time() * 1000000000.0) / (1.0 * rounds),
             static_cast<int>(timer.time() * 1000000000.0),
             sum,
             expr_str_list[i].c_str());

      fflush(stdout);

      single_eval_total_time += (timer.time() * 1000000000.0) / (1.0 * rounds);
   }

   total_timer.stop();

   printf("[*] Number Of Evals:        %15.0f\n",
          rounds * (expression_list.size() * 1.0));

   printf("[*] Total Time:             %9.3fsec\n",
          total_timer.time());

   printf("[*] Total Single Eval Time: %9.3fms\n",
          single_eval_total_time / 1000000.0);
}


template<typename T>
class RoundZero : public exprtk::ifunction<T> {
public:
    RoundZero() : exprtk::ifunction<T>(3) {}
    inline T operator()(const T& val, const T& bound, const T& default_val) {
        if (std::abs(val) < bound) {
            return default_val;
        }
        return val;
    }
};

template<typename T>
class GetInRange : public exprtk::ifunction<T> {
public:
    GetInRange() : exprtk::ifunction<T>(3) {}
    inline T operator()(const T& val, const T& min_val, const T& max_val) {
        T new_val = std::max(val, min_val);
        new_val = std::min(new_val, max_val);
        return new_val;
    }
};

template<typename T>
class BottomLimit : public exprtk::ifunction<T> {
public:
    BottomLimit() : exprtk::ifunction<T>(1) {}
    inline T operator()(const T& val) {
        if (val < 1e-6) {
            return 1e-6;
        }
        return val;
    }
};

template<typename T>
class ZeroCeiling : public exprtk::ifunction<T> {
public:
    ZeroCeiling() : exprtk::ifunction<T>(1) {}
    inline T operator()(const T& val) {
        if (val < 0) {
            return 0;
        }
        return val;
    }
};

template<typename T>
class TransPos : public exprtk::ifunction<T> {
public:
    TransPos() : exprtk::ifunction<T>(1) {}
    inline T operator()(const T& val) {
        if (val < 1) {
            return 10000;
        }
        return val;
    }
};

void perform_special_benchmark(const std::size_t& rounds, bool benchmark_parse)
{

   exprtk::symbol_table<double> symbol_table;

   static RoundZero<double> round_zero;
   symbol_table.add_function("round_zero", round_zero);

   static GetInRange<double> get_in_range;
   symbol_table.add_function("get_in_range", get_in_range);

   static BottomLimit<double> bottom_limit;
   symbol_table.add_function("bottom_limit", bottom_limit);

   static ZeroCeiling<double> zero_ceiling;
   symbol_table.add_function("zero_ceiling", zero_ceiling);
   symbol_table.add_function("trans_pos", zero_ceiling);   

   static const char* complex_expression = R"CEXPR(
e := get_in_range(e,0,1);
f := get_in_range(f,0,1);
n := get_in_range(n,0,1);
p := get_in_range(p,0,1);
r := get_in_range(r,0,1);
x := get_in_range(x,0,1);
ab := get_in_range(ab,0,1);
al := 1;
an := 1;
ao := 1;
ap := 1;
aq := 1;
ar := 1;
as := 1;
at := 1;
au := 1;
av := 1;
aw := 1;
bj := get_in_range(bj,0,1);
bk := get_in_range(bk,0,1);
bl := 1;
bo := get_in_range(bo,0,200);
bp := get_in_range(bp,0,1);
bu := get_in_range(bu,0,1);
m := f;
o := n;
q := p;
s := r;
bt := bp;
by := bu;
cc := bo;
f := bottom_limit(m * i + h)^g;
l := bottom_limit(1 / (1 + ba))^k;
n := bottom_limit(m * (s + q + bk) * (0.8*n + 0.2*ab) * c + b)^a;
t := bottom_limit(m * (s + q + bk) * w + v)^u;
x := bottom_limit(x * aa + z)^y;
ag := bottom_limit(m * (s + q + bk) * (0.8*o + 0.2*ab) * min(480, if((ak == 0), aj, ak)+1)/480 * ae*(1-e+0.16) + ad)^ac;
ai := bottom_limit(1 / (1 + bb))^ah;
az := bottom_limit(1 / (1 + bc))^ay;
bf := bottom_limit(bj * bi + bh)^bg;
bo := bottom_limit(m * bo * cb + ca)^bz;
bp := bottom_limit(bp*bs+br)^bq;
bu := bottom_limit(bu*bx+bw)^bv;
ba := ba;
bb := bb;
bc := bc;
bd := ak;
be := bottom_limit(m * (s + q + bk) * o);
(f*t*bo*n*ag*l*az*ai*an*bf*ap*aq*ar*as*at*au*av*aw*ao*bp*bu*x) ^ 0.5;
)CEXPR";

   std::map<std::string, double> default_vars = {
      {"a", 23.7498},
      {"b", 1.0},
      {"c", 10438.119},
      {"d", 62667.409091},
      {"e", 0.0},
      {"f", 1.212228684328808e-16},
      {"g", 15.52932},
      {"h", 1e-08},
      {"i", 12.9812},
      {"j", 0.0072737499140203},
      {"k", 1.0},
      {"l", 0.002717391304347826},
      {"m", 0.0072737499140203},
      {"n", 1.0},
      {"o", 0.0},
      {"p", 0.0},
      {"q", 0.0},
      {"r", 0.0},
      {"s", 0.0},
      {"t", 1.0},
      {"u", 0.0},
      {"v", 1.0},
      {"w", 3.3754},
      {"x", 1.0671664585969973},
      {"y", 0.4},
      {"z", 1.0},
      {"aa", 0.3},
      {"ab", 0.0},
      {"ac", 18.0},
      {"ad", 1.0},
      {"ae", 100000.0},
      {"af", 1e-06},
      {"ag", 1.0},
      {"ah", 2.6},
      {"ai", 2.1320415859514218e-07},
      {"aj", 3.8753204937},
      {"ak", 3.77721},
      {"al", 1.0},
      {"am", 0.0},
      {"an", 1.0},
      {"ao", 1.0},
      {"ap", 1.0},
      {"aq", 1.0},
      {"ar", 1.0},
      {"as", 1.0},
      {"at", 1.0},
      {"au", 1.0},
      {"av", 1.0},
      {"aw", 1.0},
      {"ax", 1e-06},
      {"ay", 1.2},
      {"az", 0.0008336478176364468},
      {"ba", 367.0},
      {"bb", 367.0},
      {"bc", 367.0},
      {"bd", 3.77721},
      {"be", 1e-06},
      {"bf", 2.1489911103020884},
      {"bg", 43.48179},
      {"bh", 1.0},
      {"bi", 16.1136},
      {"bj", 0.0011015052441507578},
      {"bk", 0.0},
      {"bl", 1.0},
      {"bm", 0.0},
      {"bn", 0.5588802695274353},
      {"bo", 710.1982778818003},
      {"bp", 4.223250444224022},
      {"bq", 0.898},
      {"br", 2.2389},
      {"bs", 4.4369},
      {"bt", 0.6164584159851074},
      {"bu", 2.897342670425497},
      {"bv", 2.1587},
      {"bw", 0.4517},
      {"bx", 4.5996},
      {"by", 0.25767087936401367},
      {"bz", 6.0438},
      {"ca", 1.0},
      {"cb", 13.9199},
      {"cc", 19.391334533691406}
   };

   for (const auto& kv : default_vars)
   {
      if (!symbol_table.create_variable(kv.first, kv.second))
      {
         printf("[perform_special_benchmark] - Failed to register variable: %s\n", kv.first.c_str());
         return;
      }
   }

   exprtk::expression<double> expression;
   expression.register_symbol_table(symbol_table);

   exprtk::timer timer;
   exprtk::parser<double> parser;

   if (benchmark_parse) {
      timer.start();
      for (std::size_t r = 0; r < rounds; ++r)
      {
         if (!parser.compile(complex_expression, expression))
         {
            printf("[perform_special_benchmark] - Parser Error: %s\n",
                   parser.error().c_str());
            return;
         }
      }
      timer.stop();
   
      printf("[perform_special_benchmark-parse] Total Time:%12.8f  Rate:%14.3f parse/sec\n",
             timer.time(),
             rounds / timer.time());
      return;
   } 

   
   if (!parser.compile(complex_expression, expression))
   {
      printf("[perform_special_benchmark] - Parser Error: %s\n", parser.error().c_str());
      return;
   }

   double total = 0.0;
   timer.start();

   std::vector<std::string> vars = {"e", "f", "n", "p", "r", "x", "ab", "bj", "bk",
       "bo", "bp", "bu"};
   for (std::size_t r = 0; r < rounds; ++r)
   {
      for (auto& variable : vars) {
         symbol_table.variable_ref(variable) = default_vars.find(variable)->second + global_delta;
      }
      total += expression.value();
   }

   timer.stop();

   if (0.0 != total)
   {
      printf("[perform_special_benchmark-eval] Total Time:%12.8f  Rate:%14.3f evals/sec\n",
             timer.time(),
             rounds / timer.time());
   } else {
      printf("[perform_special_benchmark-eval] - Error running benchmark (zero total)\n");
   }
}
