#ifndef OGGPLAYER_H
#define OGGPLAYER_H

struct ogg_context;

struct ogg_context * ogg_context_new(void);
struct ogg_context * ogg_context_open(const char *path);

void ogg_context_free(struct ogg_context *ctx);
void ogg_context_set_gain(struct ogg_context *ctx, float gain);
void ogg_context_start(struct ogg_context *ctx);
void ogg_context_stop(struct ogg_context *ctx);
void ogg_context_close(struct ogg_context *ctx);
void ogg_context_update(void *ctx);
void ogg_init(void);
void ogg_release(void);

#endif /* OGGPLAYER_H */
