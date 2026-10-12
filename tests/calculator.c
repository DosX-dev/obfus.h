#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "../include/obfus.h"

typedef struct {
    const char *start, *p, *error;
    double ans;
    unsigned depth;
} Parser;
__declspec(dllexport) double calc_sum(Parser *s);
__declspec(dllexport) double calc_unary(Parser *s);
__declspec(dllexport) double calc_atom(Parser *s);
static void fail(Parser *s, const char *message) {
    if (!s->error) s->error = message;
}
static void spaces(Parser *s) {
    while (*s->p == ' ' || *s->p == '\t' || *s->p == '\n' || *s->p == '\r') ++s->p;
}
static int take(Parser *s, int ch) {
    spaces(s);
    if (*s->p != ch) return 0;
    ++s->p;
    return 1;
}
static int letter(int ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
static double atom_base(Parser *s) {
    spaces(s);
    if (take(s, '(')) {
        double value = calc_sum(s);
        if (!take(s, ')')) fail(s, "expected closing parenthesis");
        return value;
    }
    if (letter((unsigned char)*s->p)) {
        char name[16];
        unsigned n = 0;
        while (letter((unsigned char)*s->p)) {
            if (n + 1 < sizeof name)
                name[n++] = *s->p;
            else
                fail(s, "identifier too long");
            ++s->p;
        }
        name[n] = 0;
        if (!strcmp(name, "pi")) return 3.14159265358979323846;
        if (!strcmp(name, "e")) return 2.71828182845904523536;
        if (!strcmp(name, "ans")) return s->ans;
        if (!take(s, '(')) {
            fail(s, "expected function arguments");
            return 0;
        }
        double a = calc_sum(s), b = 0;
        int two = take(s, ',');
        if (two) b = calc_sum(s);
        if (!take(s, ')')) fail(s, "expected closing parenthesis");
        if (two) {
            if (!strcmp(name, "min")) return a < b ? a : b;
            if (!strcmp(name, "max")) return a > b ? a : b;
            if (!strcmp(name, "pow")) return pow(a, b);
        } else {
            if (!strcmp(name, "sin")) return sin(a);
            if (!strcmp(name, "cos")) return cos(a);
            if (!strcmp(name, "tan")) return tan(a);
            if (!strcmp(name, "sqrt")) return sqrt(a);
            if (!strcmp(name, "abs")) return fabs(a);
            if (!strcmp(name, "log")) return log(a);
            if (!strcmp(name, "exp")) return exp(a);
            if (!strcmp(name, "floor")) return floor(a);
            if (!strcmp(name, "ceil")) return ceil(a);
        }
        fail(s, "unknown function or argument count");
        return 0;
    }
    char *end;
    errno = 0;
    double value = strtod(s->p, &end);
    if (end == s->p) {
        fail(s, "expected number or function");
        return 0;
    }
    if (errno == ERANGE) fail(s, "number out of range");
    s->p = end;
    return value;
}
__declspec(dllexport) double calc_atom(Parser *s) {
    double value = atom_base(s);
    while (take(s, '!')) {
        if (value < 0 || value > 170 || floor(value) != value) {
            fail(s, "invalid factorial");
            break;
        }
        double product = 1;
        for (unsigned i = 2; i <= (unsigned)value; ++i) product *= i;
        value = product;
    }
    return value;
}
__declspec(dllexport) double calc_unary(Parser *s) {
    if (++s->depth > 64) {
        --s->depth;
        fail(s, "expression too deep");
        return 0;
    }
    double value;
    if (take(s, '+'))
        value = calc_unary(s);
    else if (take(s, '-'))
        value = -calc_unary(s);
    else {
        value = calc_atom(s);
        if (take(s, '^')) value = pow(value, calc_unary(s));
    }
    --s->depth;
    return value;
}
static double product(Parser *s) {
    double value = calc_unary(s);
    for (;;) {
        spaces(s);
        int op = *s->p;
        if (op != '*' && op != '/' && op != '%') return value;
        ++s->p;
        double right = calc_unary(s);
        if (op != '*' && right == 0) {
            fail(s, "division by zero");
            return 0;
        }
        if (op == '*')
            value *= right;
        else if (op == '/')
            value /= right;
        else
            value = fmod(value, right);
    }
}
__declspec(dllexport) double calc_sum(Parser *s) {
    double value = product(s);
    for (;;) {
        spaces(s);
        int op = *s->p;
        if (op != '+' && op != '-') return value;
        ++s->p;
        double right = product(s);
        value = op == '+' ? value + right : value - right;
    }
}
__declspec(dllexport) int calc_eval(const char *text, double previous, double *out) {
    if (!text || !out || strlen(text) > 4096) return 0;
    Parser s = {text, text, NULL, previous, 0};
    errno = 0;
    double value = calc_sum(&s);
    spaces(&s);
    if (*s.p) fail(&s, "unexpected trailing input");
    if (value != value || fabs(value) > DBL_MAX || errno == EDOM || errno == ERANGE) fail(&s, "math domain or range error");
    if (s.error) {
        fprintf(stderr, "Error at %u: %s\n", (unsigned)(s.p - s.start), s.error);
        return 0;
    }
    *out = value;
    return 1;
}
int main(int argc, char **argv) {
    char line[4098];
    double ans = 0;
    if (argc > 1) {
        if (!calc_eval(argv[1], ans, &ans)) return 1;
        if (printf("%.15g\n", ans) < 0 || fflush(stdout)) return 2;
        return 0;
    }
    puts("Calculator: + - * / % ^, factorial, pi/e/ans, functions; quit to exit.");
    while (fgets(line, sizeof line, stdin)) {
        if (!strcmp(line, "quit\n") || !strcmp(line, "quit")) break;
        if (calc_eval(line, ans, &ans)) {
            printf("= %.15g\n", ans);
            fflush(stdout);
        }
    }
    return 0;
}
