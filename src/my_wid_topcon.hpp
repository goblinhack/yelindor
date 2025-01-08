//
// Copyright Neil McGill, goblinhack@gmail.com
//

#pragma once

void    wid_topcon_fini(void);
uint8_t wid_topcon_init(void);
void    wid_topcon_log(std::string s);
void    wid_topcon_flush(void);

#include "my_wid.hpp"

//
// Global widgets.
//
extern Widp wid_topcon_input_line;
extern Widp wid_topcon_window;
extern Widp wid_topcon_container;