/*
 * SoundPlayer.h
 *
 * Created by Adria Navarro on 9/8/14.
 *
 * Copyright (c) 2014, Red Paper Heart, All rights reserved.
 * This code is intended for use with the Cinder C++ library: http://libcinder.org
 *
 * To contact Red Paper Heart, email hello@redpaperheart.com or tweet @redpaperhearts
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include "cinder/app/App.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/PanNode.h"
#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/Timeline.h"

using namespace ci;

namespace rph {
    
    typedef std::shared_ptr<class SoundPlayer> SoundPlayerRef;
    typedef std::shared_ptr<class SoundPlayerMulti> SoundPlayerMultiRef;

    class SoundPlayer {
      public:
        static SoundPlayerRef create(const DataSourceRef &source, size_t maxFramesForBufferPlayback = 20000);
        
        SoundPlayer() {};
        SoundPlayer(const DataSourceRef &source, size_t maxFramesForBufferPlayback = 20000);
        ~SoundPlayer() {};

        // playback
        void play()                                             { mPlayer->enable(); }      // plays from wherever it was
        void start()                                            { mPlayer->start(); }       // plays from beginning
        void stop()                                             { mPlayer->stop(); }
        void pause()                                            { mPlayer->disable(); }     // TODO: stop timers and deal with ramps
        void setLoop(bool loop)                                 { mPlayer->setLoopEnabled(loop); }
        void goToFrame(size_t frame)                            { mPlayer->seek(frame); }
        void goToTime(double seconds)                           { mPlayer->seekToTime(seconds); }
        void goToPercent(double percent)                        { mPlayer->seekToTime(percent * getDuration()); }   // 0.0 - 1.0

        // volume
        virtual void setVolume(float volume)                            { mGain->setValue(volume); }
        void fadeTo(float volume, float seconds)                { fade(mGain->getValue(), volume, seconds); }
        void fadeOutAndStop(float seconds);
        void fadeOutAndPause(float seconds);
        void fadeInAndPlay(float seconds, float volume = 1.0f);
        void fadeInAndStart(float seconds, float volume = 1.0f);
        void fade(float from, float to, float seconds, float delay = 0.0f);
        
        // pan
        void setPan(float pos)                                  { mPan->setPos(pos); }
        void panTo(float pos, float seconds)                    { pan(mPan->getPos(), pos, seconds); }
        void pan(float from, float to, float seconds);
        
        // getters
        bool isPlaying() const                                  { return mPlayer->isEnabled() && !isFinished(); }
        bool isLooping() const                                  { return mPlayer->isLoopEnabled(); }
        bool isFinished() const                                 { return mPlayer->isEof(); }
        float getVolume() const                                 { return mGain->getValue(); }
        float getPan() const                                    { return mPan->getPos(); }
        float getDuration() const                               { return mPlayer->getNumSeconds(); }
        size_t getDurationFrames() const                        { return mPlayer->getNumFrames(); }
        
        audio::SamplePlayerNodeRef getPlayerNode()              { return mPlayer; }
        
      protected:
        audio::SamplePlayerNodeRef  mPlayer;
        audio::GainNodeRef			mGain;
        audio::Pan2dNodeRef         mPan;
        Anim<float>                 mStopTimer;
    };
    
    
    class SoundPlayerMulti : public SoundPlayer {
      public:
        static SoundPlayerMultiRef create(const DataSourceRef &source, int numChannels, size_t maxFramesForBufferPlayback = 20000);
        
        SoundPlayerMulti() {};
        SoundPlayerMulti(const DataSourceRef &source, int numChannels, size_t maxFramesForBufferPlayback = 20000);
        ~SoundPlayerMulti() {};
        
        void setChannelVolume(int ch, float volume)             { mGains.at(ch)->setValue(volume); }
        float getChannelVolume(int ch) const                    { return mGains.at(ch)->getValue(); }
        
      protected:
        int                             mNumChannels;
        audio::ChannelRouterNodeRef     mRouter;
        std::vector<audio::GainNodeRef> mGains;
        
    };
}

