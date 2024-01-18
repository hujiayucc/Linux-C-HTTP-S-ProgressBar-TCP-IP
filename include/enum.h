#ifndef ENUM_H
#define ENUM_H
#ifdef  __cplusplus
extern "C" {
#endif

typedef enum {
    DEFAULT = 0,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37
} textcolor;

typedef enum {
    GET,
    POST,
    HEAD,
    DELETE,
    PUT
} methods;

#ifdef  __cplusplus
}
#endif // __cplusplus
#endif // ENUM_H