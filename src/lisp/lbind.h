/* lbind.h
 *
 * Daniel Ralston, 2009
 *
 * Function prototypes for Lisp interfaces to C functions.
 *
 * This code is released under the GNU GPL General Public License.
 */

#ifndef LBIND_H
#define LBIND_H
extern cons *lnull(cons *env);
extern cons *lnumberp(cons *env);
extern cons *lcons(cons *env);
extern cons *lcar(cons *env);
extern cons *lrplaca(cons *env);
extern cons *lcdr(cons *env);
extern cons *lrplacd(cons *env);
extern cons *llist(cons *env);

extern cons *ladd(cons *env);
extern cons *lsubtract(cons *env);
extern cons *lmultiply(cons *env);

extern cons *lintern(cons *env);
extern cons *lfind_package(cons *env);
extern cons *lfind_class(cons *env);

extern cons *lchareq(cons *env);
extern cons *lcharequal(cons *env);
extern cons *lstringeq(cons *env);
extern cons *lstringequal(cons *env);

extern cons *leq (cons *env);
extern cons *leql (cons *env);
extern cons *lequal (cons *env);

extern cons *lhash (cons *env);

extern cons *llet(cons *env);
extern cons *llet_star(cons *env);
extern cons *lsetq(cons *env);

extern cons *lcond (cons *env);
extern cons *lquote(cons *env);
extern cons *leval(cons *env);
extern cons *lprogn(cons *env);
extern cons *lfunction(cons *env);
extern cons *ltype_of(cons *env);

extern cons *ldefun(cons *env);
extern cons *lsetq(cons *env);

extern cons *lread_char(cons *env);
extern cons *lread_list(cons *env);
#endif
