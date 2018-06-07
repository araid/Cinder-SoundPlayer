#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/params/Params.h"
#include "rph/SoundPlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MultiChannelApp : public App {
public:
    void setup();
    void draw();
    void keyDown( KeyEvent event );
    void updateVolume();
    void updatePan();
    
    params::InterfaceGlRef   mParams;
    rph::SoundPlayerMultiRef mSound;
    int                      mNumChannels;
    float                    mPan;
    float                    mVolume;
    std::map<int, float>     mChannelVolumes;
};

void MultiChannelApp::setup()
{
    setWindowSize(300, 300);
    
    // List audio devices
    audio::Device::printDevicesToString();
    
    // Load and setup sound players
    mNumChannels = audio::Device::getDefaultOutput()->getNumOutputChannels();
    mSound = rph::SoundPlayerMulti::create(loadAsset("8bit.mp3"), mNumChannels);    // loadResource works too
    mSound->setLoop(true);
    mVolume = 1.0;
    mPan = 0.0;
    
    // Create buttons for playback
    mParams = params::InterfaceGl::create("SoundPlayerMulti", ivec2(260, 260));
    mParams->addButton("Play", std::bind(&rph::SoundPlayerMulti::play, mSound));     // mSound.play()
    mParams->addButton("Pause", std::bind(&rph::SoundPlayerMulti::pause, mSound));   // mSound.pause()
    mParams->addButton("Stop", std::bind(&rph::SoundPlayerMulti::stop, mSound));     // mSound.stop()
    mParams->addParam("Volume", &mVolume).min(0.0f).max(5.0f).step(0.1f).updateFn([this] {
        mSound->setVolume(mVolume);
    });
    mParams->addParam("Pan", &mPan).min(-1.0f).max(1.0f).step(0.05f).updateFn([this] {
        mSound->setPan(mPan);
    });
    mParams->addText(" ");
    
    for (int i = 0; i < mNumChannels; i++) {
        mChannelVolumes[i] = 1.0f;
        
        mParams->addParam("Channel " + to_string(i) + " Volume", &mChannelVolumes.at(i)).min(0.0f).max(5.0f).step(0.1f).updateFn([this, i] {
            mSound->setChannelVolume(i, mChannelVolumes.at(i));
        });
    }
}

void MultiChannelApp::keyDown( KeyEvent event )
{
    switch (event.getCode()) {
        case KeyEvent::KEY_SPACE:
            if (mSound->isPlaying()) mSound->pause();
            else                     mSound->play();
            break;
        case KeyEvent::KEY_UP:
            mVolume += 0.1; // we update this so it shows in params, but we dont need to
            mSound->setVolume(mVolume);
            break;
        case KeyEvent::KEY_DOWN:
            mVolume -= 0.1;
            mSound->setVolume(mVolume);
            break;
        case KeyEvent::KEY_RIGHT:
            mPan += 0.1;
            mSound->setPan(mPan);
            break;
        case KeyEvent::KEY_LEFT:
            mPan -= 0.1;
            mSound->setPan(mPan);
            break;
        default:
            break;
    }
}

void MultiChannelApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    mParams->draw();
}

CINDER_APP( MultiChannelApp, RendererGl )
