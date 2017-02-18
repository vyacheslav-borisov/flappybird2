//
// Created by merelalkar on 17.02.2017.
//
#ifndef GAME_HUD_H
#define GAME_HUD_H

#include "game_screen.h"

namespace pegas
{
    class HUDGetReady
    {
    public:
        HUDGetReady();
        ~HUDGetReady();

        bool create(Atlas* atlas, SceneManager* sceneManager);
        void update(float dt);
        void destroy();
        void show();
        void hide();

    private:
        SceneNode* m_getReadyScreen;
    };

    class HUDGameOver
    {
    public:
        HUDGameOver();
        ~HUDGameOver();

        bool create(Atlas* atlas, SceneManager* sceneManager);
        void update(float dt);
        void destroy();
        void show(float currentScores, float bestScores);
        void hide();

    private:
        SceneNode* m_gameOverScreen;
        bool       m_initialized;
    };

    class HUDBlindFlash
    {
    public:
        HUDBlindFlash();
        ~HUDBlindFlash();

        bool create(Atlas* atlas, SceneManager* sceneManager);
        void update(float dt);
        void destroy();
        void blind();

    private:
        SceneNode* m_flashSceneNode;
        SpritePtr  m_flashSprite;
    };

}

#endif //GAME_HUD_H
