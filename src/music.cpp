// DR ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2023 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------

#include "music_manager.hpp"
#include "music_tune.hpp"

#include "doomtype.h"
#include "music.h"

using namespace srb2::music;

namespace
{

TuneManager g_tunes;

}; // namespace

void Music_Init(void)
{
	{
		Tune& tune = g_tunes.insert("level");

		tune.priority = 1;
		tune.resume_fade_in = 750;
		tune.sync = true;
		tune.credit = true;
	}

	{
		Tune& tune = g_tunes.insert("position");

		tune.priority = 10;
		tune.fade_out = 3500;
	}

	{
		Tune& tune = g_tunes.insert("grow");

		tune.song = "kgrow";
		tune.priority = 20;
		tune.resume_fade_in = 200;
	}

	{
		Tune& tune = g_tunes.insert("invinc");

		tune.song = "kinvnc";
		tune.priority = 21;
	}

	{
		Tune& tune = g_tunes.insert("finish");

		tune.priority = 30;
		tune.loop = false;
	}

	{
		Tune& tune = g_tunes.insert("comeon");

		tune.song = "chalng";
		tune.priority = 35;
		tune.loop = false;
	}

	{
		Tune& tune = g_tunes.insert("intermission");

		tune.song = "racent";
		tune.priority = 40;
	}

	{
		Tune& tune = g_tunes.insert("vote");

		tune.song = "vote";
		tune.priority = 50;
		tune.credit = true;
	}

	{
		Tune& tune = g_tunes.insert("vote_suspense");

		tune.song = "voteea";
		tune.priority = 51;
	}

	{
		Tune& tune = g_tunes.insert("vote_end");

		tune.song = "voteeb";
		tune.priority = 52;
		tune.loop = false;
	}

	{
		Tune& tune = g_tunes.insert("wait");

		tune.song = "WAIT2J";
		tune.priority = 60;
	}

	{
		Tune& tune = g_tunes.insert("title");

		tune.song = "_title";
		tune.priority = 100;
		tune.resist = true;
	}

	{
		Tune& tune = g_tunes.insert("menu");

		tune.priority = 100;
		tune.credit = true;
	}

	{
		Tune& tune = g_tunes.insert("menu_nocred");

		tune.priority = 100;
	}

	{
		Tune& tune = g_tunes.insert("credits");

		tune.priority = 100;
		tune.song = "_creds";
		tune.fade_out = 250;
		tune.loop = false;
		tune.credit = true;
	}

	{
		Tune& tune = g_tunes.insert("shore");

		tune.priority = 100;
		tune.loop = false;
	}

	{
		Tune& tune = g_tunes.insert("stereo");

		tune.priority = 1000;
		tune.resist = true;
		tune.keep_open = true;
		tune.credit = true;
	}

	{
		Tune& tune = g_tunes.insert("stereo_fade");

		tune.priority = 1000;
		tune.fade_out = 5000;
		tune.fade_out_inclusive = false;
		tune.resist = true;
		tune.keep_open = true;
		tune.credit = true;
	}
}

void Music_Tick(void)
{
	g_tunes.tick();
}

void Music_Play(const char* id)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		tune->play();
		g_tunes.tick(); // play this immediately
	}
}

void Music_DelayEnd(const char* id, tic_t duration)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		tune->delay_end(duration);

		if (tune->time_remaining() <= detail::msec_to_tics(tune->fade_out))
		{
			// If this action would cause a fade out, start
			// fading immediately.
			g_tunes.tick();
		}
	}
}

void Music_Seek(const char* id, tic_t set)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		tune->seek = set;
		tune->needs_seek = true;
	}
}

void Music_Stop(const char* id)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		g_tunes.stop(*tune);
		g_tunes.tick();
	}
}

void Music_Pause(const char* id)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		tune->pause();
		g_tunes.pause_unpause();
	}
}

void Music_UnPause(const char* id)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		tune->unpause();
		g_tunes.pause_unpause();
	}
}

void Music_PauseAll(void)
{
	g_tunes.for_each([](Tune& tune) { tune.pause(); });
	g_tunes.pause_unpause();
}

void Music_UnPauseAll(void)
{
	g_tunes.for_each([](Tune& tune) { tune.unpause(); });
	g_tunes.pause_unpause();
}

void Music_StopAll(void)
{
	g_tunes.stop_all();
	g_tunes.tick();
}

void Music_Remap(const char* id, const char* song)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		tune->song = song;
	}
}

boolean Music_Playing(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune && tune->playing();
}

boolean Music_Paused(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune && tune->paused();
}

tic_t Music_Elapsed(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune ? tune->elapsed() : 0u;
}

tic_t Music_DurationLeft(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune ? tune->time_remaining() : 0u;
}

tic_t Music_TotalDuration(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune ? tune->duration() : 0u;
}

void Music_Loop(const char* id, boolean loop)
{
	Tune* tune = g_tunes.find(id);

	if (tune)
	{
		// FIXME: this has no effect if the song is already
		// playing
		tune->loop = loop;
	}
}

boolean Music_CanLoop(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune && tune->loop;
}

boolean Music_CanEnd(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune && tune->can_end();
}

const char* Music_Song(const char* id)
{
	const Tune* tune = g_tunes.find(id);
	return tune ? tune->song.c_str() : "";
}

const char* Music_CurrentSong(void)
{
	return g_tunes.current_song().c_str();
}

const char* Music_CurrentId(void)
{
	return g_tunes.current_id();
}