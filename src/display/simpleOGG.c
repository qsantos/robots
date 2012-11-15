/*\
 *  This is an awesome programm simulating awesome battles of awesome robot tanks
 *  Copyright (C) 2012  Quentin SANTOS
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*/

#include <assert.h>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

#include "../common.h"

void LoadOGG(const char* filename, ALuint bufferID)
{
	FILE* f = fopen(filename, "r"); assert(f);
	OggVorbis_File oggFile;
	assert (ov_open(f, &oggFile, NULL, 0) == 0);

	vorbis_info* pInfo = ov_info(&oggFile, -1);
	ALenum  format = pInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	ALsizei freq   = pInfo->rate;

	size_t available = 0;
	size_t size      = 0;
	char*  buffer    = NULL;
	while (1)
	{
		if (size + 1024 >= available)
		{
			available += 1024;
			buffer = (char*) realloc(buffer, available);
			assert(buffer);
		}
		int bitStream;
		long bytes = ov_read(&oggFile, buffer+size, available-size, 0, 2, 1, &bitStream);
		assert(bytes >= 0);
		if (!bytes)
			break;

		size += bytes;
	}

	ov_clear(&oggFile);

	alBufferData(bufferID, format, buffer, size, freq);
	free(buffer);
}

void AL_Init(int* argc, char** argv)
{
	alutInit(argc, argv);
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
}

static ALuint* sources  = NULL;
static ALuint* buffers  = NULL;
static u32     n_sounds = 0;
static u32     a_sounds = 0;
void AL_Play(const char* filename, char loop)
{
	s32 soundID = -1;
	for (u32 i = 0; i < n_sounds; i++)
	{
		ALint state;
		alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED)
		{
			soundID = i;
			break;
		}
	}
	if (soundID < 0)
	{
		if (n_sounds >= a_sounds)
		{
			a_sounds = a_sounds ? 2*a_sounds : 1;
			sources = REALLOC(sources, ALuint, a_sounds);
			buffers = REALLOC(buffers, ALuint, a_sounds);
		}
		alGenSources(1, &sources[n_sounds]);
		ALuint sourceID = sources[n_sounds];
		alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSourcei(sourceID, AL_LOOPING, loop);

		alGenBuffers(1, &buffers[n_sounds]);

		soundID = n_sounds++;
	}

	LoadOGG(filename, buffers[soundID]);
	alSourcei(sources[soundID], AL_BUFFER, buffers[soundID]);
	alSourcePlay(sources[soundID]);
}

void AL_Exit()
{
	for (u32 i = 0; i < n_sounds; i++)
	{
		alSourceStop(sources[i]);
		alDeleteBuffers(1, &buffers[i]);
		alDeleteSources(1, &sources[i]);
	}

	ALCcontext* context = alcGetCurrentContext();
	ALCdevice* device = alcGetContextsDevice(NULL);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	alutExit();
}
