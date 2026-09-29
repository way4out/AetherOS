#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void aether_messaging_init(void);
void aether_messaging_tick(void);
const char* aether_messaging_contact(void);
const char* aether_messaging_platform(void);
const char* aether_messaging_color_name(void);
unsigned aether_messaging_color(void);
const char* aether_messaging_text(void);
unsigned aether_messaging_text_len(void);
unsigned aether_messaging_cursor(void);
unsigned aether_messaging_sent(void);
unsigned aether_messaging_received(void);
unsigned aether_messaging_queued(void);
unsigned aether_messaging_connected(void);
void aether_messaging_cycle_contact(void);
void aether_messaging_cycle_platform(void);
void aether_messaging_cursor_up(void);
void aether_messaging_cursor_down(void);
void aether_messaging_backspace(void);
void aether_messaging_append_char(char c);
void aether_messaging_clear(void);
int aether_messaging_send(void);
void aether_messaging_simulate_incoming(void);
#ifdef __cplusplus
}
#endif
