#ifndef __SKETCH_h__
#define __SKETCH_h__

#ifdef __cplusplus
extern "C" {
#endif

size_t color_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    );
int color_from_string
    (const char *name, const char *uri, const char *query, const char *s);
int color_rgb_from_string (const char *r, const char *g, const char *b);

#ifdef __cplusplus
}
#endif

#endif
