#ifndef __PRINTF_API_H__
#define __PRINTF_API_H__

struct printf_api {
    void (* _printf_init)(void);
    int (* _printf)(const char * fmt, ...);
};

/* NB: The printf module requires the serial module. */
void printf_module_install(struct printf_api *tbl);
int printf(const char *fmt, ...);


#endif /* __PRINTF_API_H__ */
