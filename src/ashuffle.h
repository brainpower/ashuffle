#ifndef __ASHUFFLE_ASHUFFLE_H__
#define __ASHUFFLE_ASHUFFLE_H__

#include <stdio.h>

#include <mpd/client.h>

#include "args.h"
#include "list.h"
#include "shuffle.h"

extern const int WINDOW_SIZE;

// `MPD_PORT` environment variables. If a password is needed, no password can
// be found in MPD_HOST, then `getpass_f' will be used to prompt the user
// for a password. If `getpass_f' is NULL, the a default password prompt
// (based on getpass) will be used.
struct mpd_connection* ashuffle_connect(struct ashuffle_options* options,
                                        char* (*getpass_f)());

// Build a `shuffle_chain` of songs from URIs in the given file.
int build_songs_file(struct mpd_connection* mpd, struct list* ruleset,
                     FILE* input, struct shuffle_chain* songs, bool check);

// Build a `shuffle_chain` of songs, by querying the given MPD instance.
int build_songs_mpd(struct mpd_connection* mpd, struct list* ruleset,
                    struct shuffle_chain* songs);

// Add a single random song from the given shuffle chain to the given MPD
// instance.
void shuffle_single(struct mpd_connection* mpd, struct shuffle_chain* songs);

struct shuffle_test_delegate {
    bool skip_init;
    bool (*until_f)();
};

// Use the MPD `idle` command to queue songs random songs when the current
// queue finishes playing. This is the core loop of `ashuffle`. The tests
// delegate is used during tests to observe loop effects. It should be set to
// NULL during normal operations.
int shuffle_loop(struct mpd_connection* mpd, struct shuffle_chain* songs,
                 struct ashuffle_options* options,
                 struct shuffle_test_delegate*);
#endif
