// Copyright [2015] Pebble Technology

#if defined(PBL_RECT)
// #if defined(dontrunthis)

#include <pebble.h>
#include "./ui.h"

#define TIME_ANGLE(time) time * (TRIG_MAX_ANGLE / 60)
#define HOUR_ANGLE(hour) hour * (TRIG_MAX_ANGLE / 12)

int32_t abs32(int32_t a) {return (a^(a>>31)) - (a>>31);}                  // Returns absolute value of A (only works on 32bit signed integers)

GPoint get_point_on_rect(int angle, GRect rect) {                         // Returns a point on rect made from center of rect and angle
  int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);               // Calculate once and store, to make quicker and cleaner
  int32_t dy = (sin > 0 ? (rect.size.h - 1) : (0 - rect.size.h)) / 2;     // Distance to top or bottom edge (from center)
  int32_t dx = (cos > 0 ? (rect.size.w - 1) : (0 - rect.size.w)) / 2;     // Distance to left or right edge (from center)
  if(abs32(dx * sin) < abs32(dy * cos))                                   // if (distance to vertical line) < (distance to horizontal line)
    dy = (dx * sin) / cos;                                                // calculate y position on left or right edge
   else                                                                   // else: (distance to top or bottom edge) < (distance to left or right edge)
    dx = (dy * cos) / sin;                                                // calculate x position on top or bottom line
  return GPoint(dx + rect.origin.x + (rect.size.w / 2), dy + rect.origin.y + (rect.size.h / 2));  // Return point on rect
}

void graphics_fill_radial_rect(GContext *ctx, GRect rect, int thickness, int angle_start, int angle_end) {
  if (rect.size.w>2 && rect.size.h>2) {
    GPoint end_point = get_point_on_rect(angle_end - TRIG_MAX_ANGLE / 4, rect);  // "- TRIG_MAX_ANGLE/4" fixes dumb difference on how...
    GPoint cursor  = get_point_on_rect(angle_start - TRIG_MAX_ANGLE / 4, rect);  // ... angle is seen between sin/cos and graphics_fill_radial

    do {
      if (cursor.x == rect.origin.x && cursor.y != rect.origin.y)
        graphics_fill_rect(ctx, GRect(cursor.x, --cursor.y, thickness, 2), 0, GCornerNone);
      else if (cursor.y == rect.size.h + rect.origin.y - 1)
        graphics_fill_rect(ctx, GRect(--cursor.x, cursor.y - thickness + 1, 2, thickness), 0, GCornerNone);
      else if (cursor.x == rect.origin.x + rect.size.w - 1)
        graphics_fill_rect(ctx, GRect(cursor.x - thickness + 1, cursor.y++, thickness, 2), 0, GCornerNone);
      else if (cursor.y == rect.origin.y)
        graphics_fill_rect(ctx, GRect(cursor.x++, cursor.y, 2, thickness), 0, GCornerNone);
    } while (cursor.x != end_point.x || cursor.y != end_point.y);
  }
}

// Draw an arc with given inner/outer radii
static void draw_arc(GContext *ctx, GRect rect, uint16_t thickness, uint32_t end_angle) {
  if (end_angle == 0) {
    graphics_fill_radial_rect(ctx, rect, thickness, 0, TRIG_MAX_ANGLE);
  } else {
    graphics_fill_radial_rect(ctx, rect, thickness, 0, end_angle);
  }
}

static GRect calculate_rect(Layer *layer, uint8_t arc_id) {
  uint8_t padding = 10;
  return grect_inset(layer_get_bounds(layer), GEdgeInsets(padding*(2*arc_id)));
}

// Handle representation for seconds
void draw_seconds(GContext *ctx, uint8_t seconds, Layer *layer) {
  draw_arc(ctx, calculate_rect(layer, 0), 14, TIME_ANGLE(seconds));
}

// Handle representation for minutes
void draw_minutes(GContext *ctx, uint8_t minutes, Layer *layer) {
  draw_arc(ctx, calculate_rect(layer, 1), 14, TIME_ANGLE(minutes));
}

// Handle representation for hours
void draw_hours(GContext *ctx, uint8_t hours, Layer *layer) {
  draw_arc(ctx, calculate_rect(layer, 2), 14, HOUR_ANGLE(hours));
}

#endif
