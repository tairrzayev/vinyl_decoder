#include <stdint.h>
// WAV header
// http://soundfile.sapp.org/doc/WaveFormat/
struct wav_hdr
{
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
    char subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char subchunk2_id[4];
    uint32_t subchunk2_size;
} __attribute__((packed));

void init_wav_hdr(struct wav_hdr *hdr, uint32_t payload_size);
