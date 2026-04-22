#define BUTTON_DOUBLE_PRESS_WINDOW_MS    500
#define BUTTON_DEBOUNCE_MS               50

typedef struct {
    uint32_t last_press_time;
    uint32_t press_count;
    bool is_pressed;
} button_state_t;

typedef enum {
  BUTTON_NONE = 0,
  BUTTON_SINGLE_PRESS,
  BUTTON_DOUBLE_PRESS
} button_press_type_t;

typedef void (*button_callback_t)(button_press_type_t);
void button_init();
void button_press(button_callback_t callback);
