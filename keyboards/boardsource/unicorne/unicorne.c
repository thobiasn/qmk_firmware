// Copyright 2023 jack (@waffle87)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "unicorne.h"
#include <stdio.h>

#define frame_size 512
#define logo_glitch_count 5
#define logo_glitch_dirty_count 2
#define text_glitch_count 7
#define text_glitch_dirty_count 3

#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return rotation;
}

static bool glitch = true;
static bool dirty = false;
static uint8_t frame_count = 15;
static bool show_text = true;
static uint16_t arasaka_timer;

void arasaka_text_clean(void)
{
  oled_write_raw_P(text_clean, frame_size);
}

void arasaka_text_glitch_dirty(void)
{
  oled_write_raw_P(text_glitch_dirty[rand() % text_glitch_dirty_count], frame_size);
}

void arasaka_text_glitch(bool can_be_dirty)
{
  uint8_t frame = can_be_dirty
    ? rand() % (text_glitch_count + text_glitch_dirty_count)
    : rand() % text_glitch_count
  ;

  if (frame < text_glitch_count) {
    oled_write_raw_P(text_glitch[frame], frame_size);

    return;
  }

  arasaka_text_glitch_dirty();
}

void arasaka_logo_clean(void)
{
    oled_write_raw_P(logo_clean, frame_size);
}

void arasaka_logo_glitch_dirty(void)
{
  oled_write_raw_P(logo_glitch_dirty[rand() % logo_glitch_dirty_count], frame_size);
}

void arasaka_logo_glitch(bool can_be_dirty)
{
  uint8_t frame = can_be_dirty
    ? rand() % (logo_glitch_count + logo_glitch_dirty_count)
    : rand() % logo_glitch_count
  ;

  if (frame < logo_glitch_count) {
    oled_write_raw_P(logo_glitch[frame], frame_size);

    return;
  }

  arasaka_logo_glitch_dirty();
}

void arasaka_draw(void) {
  uint16_t timer = timer_elapsed(arasaka_timer);

  if (timer < 150) {
    show_text ? arasaka_text_glitch_dirty() : arasaka_logo_glitch_dirty();

    return;
  }

  if (timer < 250) {
    show_text ? arasaka_text_glitch(true) : arasaka_logo_glitch(true);

    return;
  }

  if (timer > 9750 && timer < 9850) {
    show_text ? arasaka_text_glitch(true) : arasaka_logo_glitch(true);

    return;
  }

  if (timer > 9850 && timer < 10000) {
    show_text ? arasaka_text_glitch_dirty() : arasaka_logo_glitch_dirty();

    return;
  }

  if (timer > 10000) {
    show_text = !show_text;
    arasaka_timer = timer_read();
  }

  if (glitch && 0 != frame_count) {
    frame_count--;
    show_text ? arasaka_text_glitch(true) : arasaka_logo_glitch(true);

    return;
  }

  glitch = false;
  dirty = false;

  show_text ? arasaka_text_clean() : arasaka_logo_clean();

  if (1 == rand() % 60) {
    glitch = true;
    frame_count = 1 + rand() % 4;

    return;
  }

  if (1 == rand() % 60) {
    glitch = true;
    frame_count = 1 + rand() % 10;
    dirty = frame_count > 5;
  }
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (is_keyboard_master()) {
        switch (get_highest_layer(layer_state)) {
            case 0:
                oled_write_raw(layer_zero, sizeof(layer_zero));
                break;
            case 1:
                oled_write_raw(layer_one, sizeof(layer_one));
                break;
            case 2:
                oled_write_raw(layer_two, sizeof(layer_two));
                break;
            case 3:
                oled_write_raw(layer_three, sizeof(layer_three));
                break;
        }
    } else {
        //oled_write_raw(logo, sizeof(logo));
        oled_set_brightness(0);
        arasaka_draw();
    }
    return false;
}
#endif
