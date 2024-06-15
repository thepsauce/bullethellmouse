#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

struct level {

};

void load_level(int fd);
void update_level(uint32_t ticks);
void update_level_menu(uint32_t ticks);

#endif
