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
/* Predicates */
extern cons *lnull(cons *env);
extern cons *lnumberp(cons *env);
/* List operations */
extern cons *lcons(cons *env);
extern cons *lcar(cons *env);
extern cons *lrplaca(cons *env);
extern cons *lcdr(cons *env);
extern cons *lrplacd(cons *env);
extern cons *llist(cons *env);
/* Mathematics */
extern cons *ladd(cons *env);
extern cons *lsubtract(cons *env);
extern cons *lmultiply(cons *env);
/* Environment */
extern cons *lintern(cons *env);
extern cons *lfind_package(cons *env);
extern cons *lfind_class(cons *env);
/* Equality */
extern cons *lchareq(cons *env);
extern cons *lcharequal(cons *env);
extern cons *lstringeq(cons *env);
extern cons *lstringequal(cons *env);
extern cons *leq (cons *env);
extern cons *leql (cons *env);
extern cons *lequal (cons *env);
/* Flow Control */
extern cons *lcond (cons *env);
/* Assignment */
extern cons *ldefun(cons *env);
extern cons *llet(cons *env);
extern cons *llet_star(cons *env);
extern cons *lsetq(cons *env);
/* Evaluation */
extern cons *lquote(cons *env);
extern cons *leval(cons *env);
extern cons *lprogn(cons *env);
extern cons *lfunction(cons *env);
extern cons *ltype_of(cons *env);
/* Reading */
extern cons *lread_char(cons *env);
extern cons *lread_list(cons *env);
#endif
