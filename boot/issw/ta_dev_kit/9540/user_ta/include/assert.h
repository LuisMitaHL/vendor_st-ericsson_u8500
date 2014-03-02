#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG

#define assert(expr) ((void)(0))

#else

/* TODO implement proper assert */
#define assert(expr) do { if (!(expr)) { abort(); } } while(0)

#endif

#endif /*ASSERT_H*/
