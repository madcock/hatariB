#pragma once

// internal core stuffs that needs to be shared between core modules,
// but not with Hatari. Avoids a full rebuild for each change.

#define   CORE_ARRAY_SIZE(a_)   (sizeof(a_)/sizeof(a_[0]))

// core.c
extern retro_environment_t environ_cb;
extern retro_input_poll_t input_poll_cb;
extern retro_input_state_t input_state_cb;
extern retro_log_printf_t retro_log;
extern int core_video_aspect_mode;
extern bool core_video_changed;
extern bool core_option_hard_reset;

// core_file.c
extern void strcpy_trunc(char* dest, const char* src, unsigned int len);
extern void strcat_trunc(char* dest, const char* src, unsigned int len);

extern uint8_t* core_read_file(const char* filename, unsigned int* size_out);
extern bool core_write_file(const char* filename, unsigned int size, const uint8_t* data);
extern uint8_t* core_read_file_system(const char* filename, unsigned int* size_out);
extern uint8_t* core_read_file_save(const char* filename, unsigned int* size_out);
extern bool core_write_file_save(const char* filename, unsigned int size, const uint8_t* data);

extern void core_file_set_environment(retro_environment_t cb); // scans system/ folder, includes "tos.img" and everything in"hatarib/" (non-recursive)
extern int core_file_system_count(); // number of files found
extern const char* core_file_system_filename(int index); // file list, first is "tos.img" if it exists, and "hatarib/" files follow (with "hatarib/ prefix)
extern int core_file_system_dir_count();
extern const char* core_file_system_dirname(int index); // "hatarib/dir"
extern const char* core_file_system_dirlabel(int index); // "dir/"

// core_disk.c
extern void core_disk_set_environment(retro_environment_t cb);
extern void core_disk_init(void);
extern void core_disk_load_game(const struct retro_game_info *game);
extern void core_disk_unload_game(void);
extern void core_disk_serialize(void);
extern void core_disk_drive_toggle(void);
extern void core_disk_drive_reinsert(void); // used after cold reboot

extern bool core_disk_enable_b;
extern bool core_disk_save;

// core_config.c
extern void core_config_set_environment(retro_environment_t cb); // call after core_disk_set_environment (which scans system folder for TOS etc)
extern void core_config_apply(void);

// core_input.c
extern void core_input_set_environment(retro_environment_t cb);
extern void core_input_init(void);
extern void core_input_update(void); // call in retro_run, polls Libretro inputs and translates to events for hatari
extern void core_input_post(void); // call to force hatari to process the input queue
extern void core_input_finish(void); // call at end of retro_run
extern void core_input_serialize(void);

extern bool core_mouse_port;
extern bool core_host_keyboard;
extern bool core_host_mouse;
extern int core_autofire;
extern int core_stick_threshold;
extern int core_mouse_speed;
extern int core_mouse_dead;

// input mapping index

extern int core_joy_port_map[4];

#define CORE_INPUT_STICK_DPAD      0
#define CORE_INPUT_STICK_L         1
#define CORE_INPUT_STICK_R         2
extern int core_stick_map[4][3];

#define CORE_INPUT_BUTTON_A        0
#define CORE_INPUT_BUTTON_B        1
#define CORE_INPUT_BUTTON_X        2
#define CORE_INPUT_BUTTON_Y        3
#define CORE_INPUT_BUTTON_SELECT   4
#define CORE_INPUT_BUTTON_START    5
#define CORE_INPUT_BUTTON_L1       6
#define CORE_INPUT_BUTTON_R1       7
#define CORE_INPUT_BUTTON_L2       8
#define CORE_INPUT_BUTTON_R2       9
#define CORE_INPUT_BUTTON_L3       10
#define CORE_INPUT_BUTTON_R3       11
extern int core_button_map[4][12];

#define CORE_INPUT_OSKEY_CONFIRM   0
#define CORE_INPUT_OSKEY_CANCEL    1
#define CORE_INPUT_OSKEY_SHIFT     2
#define CORE_INPUT_OSKEY_POS       3
#define CORE_INPUT_OSKEY_MOVE      4
extern int core_oskey_map[4][5];
