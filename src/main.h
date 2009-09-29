#ifndef __AW_MAIN_H__
#define __AW_MAIN_H__ 1

#include <libawui/hildon.h>
#include <libawui/web.h>

void
aw_main_login         (GtkWidget *window,
                       AwSession *session);
void
aw_main_solve_captcha (GtkWidget *window,
                       AwSession *session);

#endif /* __AW_MAIN_H__ */
