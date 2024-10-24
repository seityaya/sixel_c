#include "mtime.h"

realtime_t get_time_real(){
    realtime_t time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time;
}

realtime_t time_dif(realtime_t beg, realtime_t end){
    realtime_t time;
    time.tv_sec =  beg.tv_sec - end.tv_sec;
    time.tv_nsec = beg.tv_nsec - end.tv_nsec;
//  if (time.tv_nsec < 0) { //TODO
//      time.tv_sec -= 1;
//      time.tv_nsec = (time.tv_nsec * -1) % NANOS;
//  }
    return time;
}

realtime_t time_sum(realtime_t beg, realtime_t end){
    realtime_t time;
    time.tv_sec = end.tv_sec + beg.tv_sec;
    time.tv_nsec = end.tv_nsec + beg.tv_nsec;
//  if (time.tv_nsec > NANOS) { //TODO
//      time.tv_sec += 1;
//      time.tv_nsec = time.tv_nsec % NANOS;
//  }
    return time;
}

realtime_t time_build(uint64_t sec, uint64_t nsec){
    realtime_t time;
    time.tv_sec = sec;
    time.tv_nsec = nsec;
    return time;
}

long double time_convert(realtime_t time){
    return (long double)(time.tv_sec)  + (long double)(time.tv_nsec) / NANOS;
}

void time_sleep(realtime_t time) {
    realtime_t save;
    clock_gettime(CLOCK_REALTIME, &save);
    realtime_t leap = time_sum(save, time);
    while(true) {
        realtime_t curr;
        clock_gettime(CLOCK_REALTIME, &curr);
        realtime_t chck = time_dif(leap, curr);
//      printf("> %25.10Lf\n", convert_time(save));
//      printf("> %25.10Lf\n", convert_time(leap));
//      printf("_ %25.10Lf\n", convert_time(chck));
        if(time_convert(chck) < 0){
            return;
        }
    }
}
