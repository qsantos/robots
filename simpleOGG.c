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

ALuint LoadOGG(const char* filename)
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

	ALuint bufferID;
	alGenBuffers(1, &bufferID);
	alBufferData(bufferID, format, buffer, size, freq);
	free(buffer);
	return bufferID;
}

ALuint sourceID;
ALuint bufferID;
void StartMusic(int* argc, char** argv, const char* filename)
{
	alutInit(argc, argv);
	bufferID = LoadOGG(filename);

	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alGenSources(1, &sourceID);
	alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSourcei(sourceID, AL_BUFFER, bufferID);

	alSourcePlay(sourceID);
}

void StopMusic()
{
	alSourceStop(sourceID);

	alDeleteBuffers(1, &bufferID);
	alDeleteSources(1, &sourceID);

	ALCcontext* context = alcGetCurrentContext();
	ALCdevice* device = alcGetContextsDevice(NULL);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	alutExit();
}
