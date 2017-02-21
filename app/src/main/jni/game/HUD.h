//
// Created by merelalkar on 17.02.2017.
//
#ifndef GAME_HUD_H
#define GAME_HUD_H

#include "game_screen.h"

namespace pegas
{
    class HUDScores
    {
    public:
        enum DigitSize
        {
            k_bigDigits,
            k_smallDigits
        };
    public:
        HUDScores(DigitSize digitSize,  int numDigits = 3);
        ~HUDScores();

        bool create(Atlas* atlas, const Vector3& position, SceneNode* parentNode);
        void destroy();
        void resetScores();
        void addScores(int scrores = 1);
        void show();
        void hide();

        void setScores(int scores);
        int getScores() const { return m_scores; }

    private:
        void updateBoard();

        int32 m_scores;

        DigitSize   m_digitSize;
        int         m_numDigits;

        SceneNode* m_boardSceneNode;
        std::vector<SceneNode*> m_digitNodes;
        std::vector<SpritePtr> m_digits;
    };

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
        enum Medal
        {
            k_medalNone,
            k_medalBronze,
            k_medalSilver,
            k_medalGold
        };
    public:
        HUDGameOver();
        ~HUDGameOver();

        bool create(Atlas* atlas, SceneManager* sceneManager);
        void update(float dt);
        void destroy();
        void show(float currentScores, float bestScores, Medal medal = k_medalNone, bool isNew = false);
        void hide();

    private:
        SceneNode* m_gameOverScreen;
        SceneNode* m_bronzeMedal;
        SceneNode* m_silverMedal;
        SceneNode* m_goldMedal;
        SceneNode* m_newMedal;
        HUDScores  m_bestScores;
        HUDScores  m_currentScores;
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
