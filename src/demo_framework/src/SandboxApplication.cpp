/**
 * Copyright (c) 2013 David Young dayoung@goliathdesigns.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *  claim that you wrote the original software. If you use this software
 *  in a product, an acknowledgment in the product documentation would be
 *  appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *  misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source
 *  distribution.
 */

#include "PrecompiledHeaders.h"

#include "demo_framework/include/DebugDrawer.h"
#include "demo_framework/include/LuaFileManager.h"
#include "demo_framework/include/LuaFilePtr.h"
#include "demo_framework/include/Sandbox.h"
#include "demo_framework/include/SandboxApplication.h"

SandboxApplication::SandboxApplication(const Ogre::String& applicationTitle)
    : BaseApplication(applicationTitle),
    lastSandboxId_(-1)
{
    timer_.reset();

    lastUpdateTimeInMicro_ = timer_.getMilliseconds();
    lastUpdateCallTime_ = timer_.getMilliseconds();
}

SandboxApplication::~SandboxApplication()
{
}

void SandboxApplication::AddResourceLocation(const Ogre::String& location)
{
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        location, "FileSystem");
}

void SandboxApplication::Cleanup()
{
    if (sandbox_)
    {
        sandbox_->Cleanup();
    }
}

void SandboxApplication::CreateSandbox(const Ogre::String& sandboxLuaScript)
{
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "Sandbox: Creating sandbox \"" + sandboxLuaScript + "\"",
        Ogre::LML_NORMAL);

    LuaFilePtr script = luaFileManager_->load(
        sandboxLuaScript,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// TODO(zolo)
    Ogre::SceneNode* const sandboxNode =
        GetSceneManager()->getRootSceneNode()->createChildSceneNode();

	// TODO(zolo) if sandbox exist, delete it and recreate
    sandbox_ = std::make_unique<Sandbox>(GenerateSandboxId(), sandboxNode, GetCamera());

    sandbox_->LoadScript(
        script->GetData(), script->GetDataLength(), script->getName().c_str());

    sandbox_->Initialize();

    Ogre::LogManager::getSingletonPtr()->logMessage(
        "Sandbox: Finished creating sandbox \"" + sandboxLuaScript + "\"",
        Ogre::LML_NORMAL);
}

void SandboxApplication::Draw()
{
    const long long currentTime = timer_.getMicroseconds();

    sandbox_->SetProfileTime(
        Sandbox::RENDER_TIME, currentTime - lastUpdateCallTime_);

    lastUpdateCallTime_ = currentTime;
}

int SandboxApplication::GenerateSandboxId()
{
    return ++lastSandboxId_;
}

Sandbox* SandboxApplication::GetSandbox()
{
    return sandbox_.get();
}

void SandboxApplication::HandleKeyPress(
    const OIS::KeyCode keycode, unsigned int key)
{
    if (sandbox_)
    {
        sandbox_->HandleKeyPress(keycode, key);
    }
}

void SandboxApplication::HandleKeyRelease(
    const OIS::KeyCode keycode, unsigned int key)
{
    if (sandbox_)
    {
        sandbox_->HandleKeyRelease(keycode, key);
    }
}

void SandboxApplication::HandleMouseMove(const int width, const int height)
{
    if (sandbox_)
    {
        sandbox_->HandleMouseMove(width, height);
    }
}

void SandboxApplication::HandleMousePress(
    const int width, const int height, const OIS::MouseButtonID button)
{
    if (sandbox_)
    {
        sandbox_->HandleMousePress(width, height, button);
    }
}

void SandboxApplication::HandleMouseRelease(
    const int width, const int height, const OIS::MouseButtonID button)
{
    if (sandbox_)
    {
        sandbox_->HandleMouseRelease(width, height, button);
    }
}

void SandboxApplication::Initialize()
{
    luaFileManager_ = std::make_unique<LuaFileManager>();

    const Ogre::ColourValue ambient(0.0f, 0.0f, 0.0f);

    GetRenderWindow()->getViewport(0)->setBackgroundColour(ambient);
    GetSceneManager()->setAmbientLight(ambient);
    GetSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    GetCamera()->setFarClipDistance(1000.0f);
    GetCamera()->setNearClipDistance(0.1f);
    GetCamera()->setAutoAspectRatio(true);

    GetRenderWindow()->setDeactivateOnFocusChange(false);

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(4);

    lastUpdateTimeInMicro_ = timer_.getMilliseconds();

    AddResourceLocation("../../../src/demo_framework/script");

	mSilverback = std::make_shared<Gorilla::Silverback>();
}

void SandboxApplication::ShowDebugText() {
	auto mScreen = mSilverback->createScreen(
		GetCamera()->getViewport(), "fonts/dejavu/dejavu");
	auto mLayer = mScreen->createLayer(0);

#ifdef NDEBUG
#define BUILD_TYPE "RELEASE"
#else
#define BUILD_TYPE "DEBUG"
#endif

	auto text = mLayer->createMarkupText(
		91,
		mScreen->getWidth(),
		mScreen->getHeight(),
		"Learning Game AI Programming with Lua v1.0 " BUILD_TYPE " " __TIMESTAMP__);

	text->left(mScreen->getWidth() - text->maxTextWidth() - 4);
	text->top(mScreen->getHeight() -
		mScreen->getAtlas()->getGlyphData(9)->mLineHeight - 4);

	mLayer->setVisible(true);
	// mLayer->setVisible(false);
}

void SandboxApplication::Update()
{
    // The sandbox simulation will update 30 times per second.
    static const long long updatePerSecondInMicros = 1000000 / 30;

    const long long currentTimeInMicro = timer_.getMicroseconds();

    const long long timeDeltaInMicros =
        currentTimeInMicro - lastUpdateTimeInMicro_;

    if (sandbox_ && timeDeltaInMicros >= updatePerSecondInMicros)
    {
        sandbox_->SetProfileTime(
            Sandbox::TOTAL_SIMULATION_TIME,
            currentTimeInMicro - lastUpdateTimeInMicro_);

        // Flush out the previous debug graphics.
        DebugDrawer::getSingleton().clear();

        // Fixed time step regardless of actual time that has passed.
        sandbox_->Update(static_cast<int>(updatePerSecondInMicros / 1000));

        lastUpdateTimeInMicro_ = currentTimeInMicro;

        DebugDrawer::getSingleton().build();

        sandbox_->SetProfileTime(
            Sandbox::SIMULATION_TIME,
            timer_.getMicroseconds() - currentTimeInMicro);
    }
}