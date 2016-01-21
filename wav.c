#include <string.h>
#include "wav.h"

#define CHUNK_ID "RIFF"
#define FORMAT "WAVE"
#define SUBCHUNK1_ID "fmt "
#define SUBCHUNK2_ID "data"

//TODO: determine payload size dynamically
void init_wav_hdr(struct wav_hdr *hdr, uint32_t payload_size)
{
	memset(hdr, 0, sizeof(*hdr));
	strncpy(hdr->chunk_id, CHUNK_ID, strlen(CHUNK_ID));
	hdr->chunk_size = 0x6424;
	strncpy(hdr->format, FORMAT, strlen(FORMAT));
	strncpy(hdr->subchunk1_id, SUBCHUNK1_ID, strlen(SUBCHUNK1_ID));
	hdr->subchunk1_size = 0x10;
	hdr->audio_format = 0x01;
	hdr->channels = 0x01;
	hdr->sample_rate = 0x1F40;
	hdr->byte_rate = 0x1F40;
	hdr->block_align = 0x01;
	hdr->bits_per_sample = 0x08;
	strncpy(hdr->subchunk2_id, SUBCHUNK2_ID, strlen(SUBCHUNK2_ID));
	hdr->subchunk2_size = 0x45A00;
}
