/**
 * \defgroup Time related functions
 *
 * This rolls the necessary definition for getting/setting time and
 * managing local time into one include file, on posix systems this
 * lives in at least two include files, time.h and sys/time.h
 *
 * @{
 */

/**
 * \file
 *         Definitions for the time module
 *      
 * \author
 *         Ralf Schlatterbeck <rsc@tux.runtux.com>
 */

#ifndef xtime_h
#define xtime_h

/* This is a time.h implementation but to avoid name-clashes with libs
 * trying to be helpfull we add the prefix x
 */

#ifdef LOCAL_COMPILE
#define clock_seconds() 1
#define clock_time() 1
#endif

#define DEFAULT_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

typedef signed long long xtime_t;
typedef signed long      suseconds_t;

#ifdef __cplusplus
extern "C" {
#endif

/* tm_gmtoff and tm_zone are BSD additions */
struct xtm {
    uint32_t    tm_year;        /* year */
    uint16_t    tm_yday;        /* day in the year */
    uint8_t     tm_sec;         /* seconds */
    uint8_t     tm_min;         /* minutes */
    uint8_t     tm_hour;        /* hours */
    uint8_t     tm_mday;        /* day of the month */
    uint8_t     tm_mon;         /* month */
    uint8_t     tm_wday;        /* day of the week */
    uint8_t     tm_isdst;       /* daylight saving time */
    int32_t     tm_gmtoff;      /* Seconds east of UTC */
    const char *tm_zone;        /* Timezone abbreviation */
};

struct xtimeval {
    xtime_t     tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
};

struct timezone {
    int16_t tz_minuteswest;     /* minutes west of Greenwich */
    int     tz_dsttime;         /* type of DST correction, unused */
};

struct xtm *xgmtime      (const xtime_t *timep);
struct xtm *xgmtime_r    (const xtime_t *timep, struct xtm *result);
struct xtm *xlocaltime   (const xtime_t *timep);
struct xtm *xlocaltime_r (const xtime_t *timep, struct xtm *result);

int xgettimeofday       (struct xtimeval *tv, struct timezone *tz);
int xsettimeofday       (const struct xtimeval *tv, const struct timezone *tz);

/*
 * Maximum length of all timezone names, this is much longer in UNIX
 * implementations but we have limited space here. Note that the length
 * includes a trailing \0 byte for each timezone name.
 */
#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS 16
#endif

/* Maximum length of buffer to reserve for timezone string */
#define MAXTZLEN (TZ_MAX_CHARS+9+2*(2+6+1+8)+1)

int set_tz (const char *tzstring);
const char *get_tz (char *buffer, size_t buflen);
size_t len_tz (void);

#ifdef __cplusplus
}
#endif

#endif // xtime_h
/** @} */
