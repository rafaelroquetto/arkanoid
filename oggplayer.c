#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <AL/alc.h>
#include <AL/al.h>
#include <vorbis/vorbisfile.h>

#include "oggplayer.h"
#include "panic.h"
#include "bytearray.h"

static ALCdevice *al_device = NULL;
static ALCcontext *al_context = NULL;;

enum { BUFSIZE = 4096, NBUFFERS = 4 };

struct ogg_buffer
{
    ALuint id;
    struct byte_array *data;
};

struct ogg_context
{
    int num_samples;
    int rate;
    int playing;

    float gain;

    OggVorbis_File ogg_stream;

    ALenum format;
    ALuint source;

    struct ogg_buffer buffers[NBUFFERS];
};

static void
ogg_buffer_replace_data(struct ogg_buffer *b, struct byte_array *data)
{
    if (b->data != NULL) {
        byte_array_free(b->data);
        b->data = NULL;
    }

    if (byte_array_size(data) > 0) {
        b->data = data;
    } else  {
        byte_array_free(data);
    }
}

static void
enqueue_buffer(const struct ogg_context *ctx, struct ogg_buffer *b)
{
    if (b->data == NULL)
        return;

    alBufferData(b->id, ctx->format, byte_array_data(b->data), byte_array_size(b->data), ctx->rate);
    alSourceQueueBuffers(ctx->source, 1, &b->id);
}

static struct ogg_buffer *
find_buffer(struct ogg_context *ctx, ALuint id)
{
    int i;

    for (i = 0; i < NBUFFERS; ++i) {
        struct ogg_buffer *b = &ctx->buffers[i];

        if (b->id == id)
            return b;
    }

    return NULL;
}

static struct byte_array *
read_ogg_stream(OggVorbis_File *stream)
{
    static char data[512];

    struct byte_array *b = byte_array_new(BUFSIZE);

    while (byte_array_size(b) < BUFSIZE) {
        int section;
        long r = ov_read(stream, data, sizeof (data), 0, 2, 1, &section);

        if (r < 0)
            panic("failed to read ogg stream");

        if (r == 0)
            break;

        byte_array_append(b, data, r);
    }

    return b;
}


struct ogg_context *
ogg_context_new(void)
{
    struct ogg_context *ctx = malloc(sizeof *ctx);
    memset(ctx, 0, sizeof *ctx);

    alGenSources(1, &ctx->source);

    int i;

    for (i = 0; i < NBUFFERS; ++i)
        alGenBuffers(1, &ctx->buffers[i].id);

    return ctx;
}

void
ogg_context_free(struct ogg_context *ctx)
{
    ogg_context_close(ctx);

    alDeleteSources(1, &ctx->source);

    int i;

    for (i = 0; i < NBUFFERS; ++i) {
        alDeleteBuffers(1, &ctx->buffers[i].id);

        if (ctx->buffers[i].data != NULL)
            byte_array_free(ctx->buffers[i].data);
    }


    free(ctx);
}

void
ogg_context_set_gain(struct ogg_context *ctx, float gain)
{
    ctx->gain = gain;

    alSourcef(ctx->source, AL_GAIN, gain);
}

struct ogg_context *
ogg_context_open(const char *path)
{
    struct ogg_context *ctx = ogg_context_new();

    if (ov_fopen(path, &ctx->ogg_stream) < 0)
        panic("failed to open ogg file");

    const vorbis_info *info = ov_info(&ctx->ogg_stream, -1);

    switch (info->channels) {
    case 1:
        ctx->format = AL_FORMAT_MONO16;
        break;
    case 2:
        ctx->format = AL_FORMAT_STEREO16;
        break;
    default:
        panic("invalid number of channels");
    }

    ctx->rate = info->rate;
    ctx->num_samples = ov_pcm_total(&ctx->ogg_stream, -1);

    return ctx;
}

void
ogg_context_start(struct ogg_context *ctx)
{
    if (ctx->playing) {
        ogg_context_stop(ctx);
    }

    int i;

    for (i = 0; i < NBUFFERS; ++i) {
        struct ogg_buffer *buffer = &ctx->buffers[i];
        struct byte_array *data = read_ogg_stream(&ctx->ogg_stream);

        ogg_buffer_replace_data(buffer, data);
        enqueue_buffer(ctx, buffer);
    }

    ctx->playing = 1;

    alSourcePlay(ctx->source);
}

void
ogg_context_stop(struct ogg_context *ctx)
{
    if (!ctx->playing)
        return;

    alSourceStop(ctx->source);

    ALint num_processed;

    while (alGetSourcei(ctx->source, AL_BUFFERS_PROCESSED, &num_processed), num_processed > 0) {
        ALuint id;
        alSourceUnqueueBuffers(ctx->source, 1, &id);
    }

    ov_raw_seek(&ctx->ogg_stream, 0);

    ctx->playing = 0;
}

void
ogg_context_close(struct ogg_context *ctx)
{
    ogg_context_stop(ctx);
    ov_clear(&ctx->ogg_stream);
}

void
ogg_context_update(void *obj)
{
    struct ogg_context *ctx = (struct ogg_context *) obj;
    if (!ctx->playing)
        return;

    ALint num_processed;

    while (alGetSourcei(ctx->source, AL_BUFFERS_PROCESSED, &num_processed), num_processed > 0) {
        ALuint id;

        alSourceUnqueueBuffers(ctx->source, 1, &id);

        struct ogg_buffer *b = find_buffer(ctx, id);

        if (b == NULL) {
            fprintf(stderr, "no buffer with id %d was found, weird...\n", id);
            continue;
        }

        struct byte_array *data = read_ogg_stream(&ctx->ogg_stream);
        ogg_buffer_replace_data(b, data);
        enqueue_buffer(ctx, b);
    }

    ALint state;

    alGetSourcei(ctx->source, AL_SOURCE_STATE, &state);

    if (state != AL_PLAYING) {
        ALint queued;

        alGetSourcei(ctx->source, AL_BUFFERS_QUEUED, &queued);

        if (queued == 0) {
            ov_raw_seek(&ctx->ogg_stream, 0);
            ctx->playing = 0;
        } else {
            alSourcePlay(ctx->source);
        }
    }
}

void
ogg_init(void)
{
    if ((al_device = alcOpenDevice(NULL)) == NULL)
        panic("alcOpenDevice failed");

    if ((al_context = alcCreateContext(al_device, NULL)) == NULL)
        panic("alcCreateContext failed");

    alcMakeContextCurrent(al_context);
    alGetError();
}

void
ogg_release(void)
{
    alcMakeContextCurrent(NULL);
    alcDestroyContext(al_context);
    alcCloseDevice(al_device);
}

