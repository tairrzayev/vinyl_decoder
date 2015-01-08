#include <string.h>
#include "wav.h"

//TODO: determine payload size dynamically
struct wav_hdr mk_wav_hdr(uint32_t payload_size)
{
	struct wav_hdr hdr;
	const char* chunk_id = "RIFF";
	const char* format = "WAVE";
	const char* subchunk1_id = "fmt ";
	const char* subchunk2_id = "data";

	strncpy(hdr.chunk_id, chunk_id, strlen(chunk_id));
	hdr.chunk_size = 0x6424;
	strncpy(hdr.format, format, strlen(format));
	strncpy(hdr.subchunk1_id, subchunk1_id, strlen(subchunk1_id));
	hdr.subchunk1_size = 0x10;
	hdr.audio_format = 0x01;
	hdr.channels = 0x01;
	hdr.sample_rate = 0x1F40;
	hdr.byte_rate = 0x1F40;
	hdr.block_align = 0x01;
	hdr.bits_per_sample = 0x08;
	strncpy(hdr.subchunk2_id, subchunk2_id, strlen(subchunk2_id));
	hdr.subchunk2_size = 0x45A00;

	return hdr;
}
