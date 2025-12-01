#ifndef MKDBG_H
#define MKDBG_H

#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#else
#define _DEFAULT_SOURCE
#endif

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MKDBG_NATIVE_VERSION "0.1.0"
#define CONFIG_NAME ".mkdbg.toml"
#define STATE_DIR_NAME ".mkdbg"
#define INCIDENTS_DIR_NAME "incidents"
#define INCIDENT_CURRENT_NAME "current"
#define INCIDENT_META_NAME "incident.json"
#define MAX_REPOS 16
#define MAX_NAME 128
#define MAX_VALUE 1024
#define MAX_ATTACH_BREAKPOINTS 16
#define MAX_ATTACH_COMMANDS 16
#define MAX_U32_TEXT 16
#define SERIAL_BUF_SIZE 256
#define DEFAULT_BAUD 115200

/* ---- Data types ---- */

typedef struct {
  char name[MAX_NAME];
  char preset[MAX_NAME];
  char path[MAX_VALUE];
  char port[MAX_VALUE];
  char build_cmd[MAX_VALUE];
  char flash_cmd[MAX_VALUE];
  char hil_cmd[MAX_VALUE];
  char snapshot_cmd[MAX_VALUE];
  char attach_cmd[MAX_VALUE];
  char elf_path[MAX_VALUE];
  char snapshot_output[MAX_VALUE];
  char openocd_cfg[MAX_VALUE];
  char openocd_server_cmd[MAX_VALUE];
  char gdb[MAX_VALUE];
  char gdb_target[MAX_VALUE];
} RepoConfig;

typedef struct {
  int version;
  char default_repo[MAX_NAME];
  RepoConfig repos[MAX_REPOS];
  size_t repo_count;
} MkdbgConfig;

typedef struct {
  const char *preset;
  const char *name;
  const char *port;
  int force;
} InitOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
} DoctorOptions;

typedef struct {
  const char *name;
  const char *path;
  const char *preset;
  const char *port;
  const char *build_cmd;
  const char *flash_cmd;
  const char *hil_cmd;
  const char *snapshot_cmd;
  const char *attach_cmd;
  const char *elf_path;
  const char *snapshot_output;
  const char *openocd_cfg;
  const char *openocd_server_cmd;
  const char *gdb;
  const char *gdb_target;
  int make_default;
} RepoAddOptions;

typedef struct {
  const char *name;
} NameCommandOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *name;
  const char *port;
} IncidentOpenOptions;

typedef struct {
  int json;
} IncidentStatusOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
  const char *source_log;
  const char *output;
  int json;
  int dry_run;
} CaptureBundleOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
  const char *bundle_json;
  const char *source_log;
  const char *auto_refresh_s;
  const char *width;
  const char *height;
  int render_once;
  int dry_run;
} WatchOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
  const char *breakpoints[MAX_ATTACH_BREAKPOINTS];
  size_t breakpoint_count;
  const char *gdb_commands[MAX_ATTACH_COMMANDS];
  size_t gdb_command_count;
  double server_wait_s;
  int batch;
  int dry_run;
} AttachOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
  const char *address;
  const char *value;
  int dry_run;
} ProbeOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
  char **command;
  int command_argc;
  int dry_run;
} RunOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
  int dry_run;
} ActionOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *branch_name;
  const char *path;
  int dry_run;
} GitOptions;

typedef struct {
  const char *repo;
  const char *target;
  const char *port;
  int baud;
  double char_delay_ms;
  const char *message;
  int dry_run;
} SerialOptions;

typedef struct {
  char id[MAX_NAME];
  char name[MAX_NAME];
  char status[MAX_NAME];
  char repo[MAX_NAME];
  char port[MAX_VALUE];
  long opened_at;
} IncidentMetadata;

/* ---- util.c ---- */
void die(const char *fmt, ...);
void trim_in_place(char *s);
void copy_string(char *dst, size_t dst_size, const char *src);
void append_string(char *dst, size_t dst_size, const char *src);
void replace_all(char *dst, size_t dst_size, const char *src,
                 const char *needle, const char *replacement);
void format_u32_hex(const char *input, const char *label, char *out, size_t out_size);
const char *path_basename(const char *path);
void path_dirname(const char *path, char *out, size_t out_size);
void join_path(const char *a, const char *b, char *out, size_t out_size);
void resolve_path(const char *base, const char *raw, char *out, size_t out_size);
int path_exists(const char *path);
int path_executable(const char *path);
int ensure_dir(const char *path);
void print_check(int ok, const char *label, const char *detail, int *failed);
int command_program(const char *command, char *out, size_t out_size);
int search_path(const char *program);
int command_available(const char *command);

#endif /* MKDBG_H */
