#ifndef _USER_LAYOUT_H_
#define _USER_LAYOUT_H_

typedef struct
{
    const char *name;
    void (*enter)(void);
    void (*quit)(void);
} layout_info;

#define LAYOUT_DEFINE(x) layout_info layout_##x = {       \
                             .name = #x,                  \
                             .enter = layout_##x##_enter, \
                             .quit = layout_##x##_quit};

#define LAYOUT_EXTERN(x) extern layout_info layout_##x;

#define LAYOUT_ENTER_FUNC(x) static void layout_##x##_enter(void)

#define LAYOUT_QUIT_FUNC(x) static void layout_##x##_quit(void)

#define pLAYOUT(x) (&layout_##x)

#define LAYOUT_GOTO(x, r)     \
    _layout_goto(pLAYOUT(x)); \
    return r;

int _layout_goto(const layout_info *layout);

const layout_info *curr_layout_get(void);

const layout_info *prev_layout_get(void);

const layout_info *next_layout_get(void);

LAYOUT_EXTERN(logo)
LAYOUT_EXTERN(home)
LAYOUT_EXTERN(monitor)

#endif // _USER_LAYOUT_H_