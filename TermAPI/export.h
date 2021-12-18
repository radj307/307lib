
#ifndef TERMAPI_EXPORT_H
#define TERMAPI_EXPORT_H

#ifdef TERMAPI_STATIC_DEFINE
#  define TERMAPI_EXPORT
#  define TERMAPI_NO_EXPORT
#else
#  ifndef TERMAPI_EXPORT
#    ifdef TermAPI_EXPORTS
        /* We are building this library */
#      define TERMAPI_EXPORT 
#    else
        /* We are using this library */
#      define TERMAPI_EXPORT 
#    endif
#  endif

#  ifndef TERMAPI_NO_EXPORT
#    define TERMAPI_NO_EXPORT 
#  endif
#endif

#ifndef TERMAPI_DEPRECATED
#  define TERMAPI_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef TERMAPI_DEPRECATED_EXPORT
#  define TERMAPI_DEPRECATED_EXPORT TERMAPI_EXPORT TERMAPI_DEPRECATED
#endif

#ifndef TERMAPI_DEPRECATED_NO_EXPORT
#  define TERMAPI_DEPRECATED_NO_EXPORT TERMAPI_NO_EXPORT TERMAPI_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef TERMAPI_NO_DEPRECATED
#    define TERMAPI_NO_DEPRECATED
#  endif
#endif

#endif /* TERMAPI_EXPORT_H */
