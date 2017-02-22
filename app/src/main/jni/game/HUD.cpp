#include "../common.h"
#include "HUD.h"

#include "flappy_birds.h"

namespace pegas {
    //===================================================================================================
    //  main menu screen
    //===================================================================================================
    HUDMainMenu::HUDMainMenu()
        :m_mainMenu(NULL)
    {

    }

    HUDMainMenu::~HUDMainMenu()
    {
        destroy();
    }

    bool HUDMainMenu::create(Atlas* atlas, SceneManager* sceneManager)
    {
        m_mainMenu = new SceneNode(sceneManager->getRootNode());
        m_mainMenu->setVisible(false);

        //game title
        SpritePtr spriteGameTitle = atlas->getSprite("main_menu_title");
        spriteGameTitle->setPivot(Sprite::k_pivotCenter);

        Rect2D screenRect = GameScreen::getScreenRect();
        float spriteScale = GameWorld::getSpriteScale();

        Matrix4x4 scale;
        scale.identity();
        scale.scale(spriteGameTitle->width() * spriteScale, spriteGameTitle->height() * spriteScale, 0.0f);

        Matrix4x4 translate;
        translate.identity();
        translate.translate(screenRect.width() * 0.5f, screenRect.height() * 0.3f, 0.0f);

        SceneNode* title = new SpriteSceneNode(spriteGameTitle, m_mainMenu);
        title->setZIndex(-6.0f);
        title->setTransform(scale * translate);
        title->setVisible(true);

        //fake start button
        SpritePtr spriteFakeStartButton = atlas->getSprite("button_start");
        spriteFakeStartButton->setPivot(Sprite::k_pivotCenter);

        scale.identity();
        scale.scale(spriteFakeStartButton->width() * spriteScale, spriteFakeStartButton->height() * spriteScale, 0.0f);

        translate.identity();
        translate.translate(screenRect.width() * 0.5f, screenRect.height() * 0.65f, 0.0f);

        SceneNode* fakeStartButton = new SpriteSceneNode(spriteFakeStartButton, m_mainMenu);
        fakeStartButton->setZIndex(-6.0f);
        fakeStartButton->setTransform(scale * translate);
        fakeStartButton->setVisible(true);


        //copyright
        //TODO: add later

        return true;
    }

    void HUDMainMenu::destroy()
    {
        if(m_mainMenu != NULL)
        {
            SceneNode* root = m_mainMenu->getParentNode();
            root->removeChild(root, true);

            m_mainMenu = NULL;
        }
    }

    void HUDMainMenu::show()
    {
        if(m_mainMenu != NULL)
        {
            m_mainMenu->setVisible(true);
        }
    }

    void HUDMainMenu::hide()
    {
        if(m_mainMenu != NULL)
        {
            m_mainMenu->setVisible(false);
        }
    }

    //===================================================================================================
    //  get ready screen
    //===================================================================================================
    HUDGetReady::HUDGetReady()
        :m_getReadyScreen(NULL)
    {

    }

    HUDGetReady::~HUDGetReady()
    {
        destroy();
    }

    bool HUDGetReady::create(Atlas* atlas, SceneManager* sceneManager)
    {
        SceneNode* rootSceneNode = sceneManager->getRootNode();
        m_getReadyScreen = new SceneNode(rootSceneNode);

        Rect2D screenRect = GameScreen::getScreenRect();
        float spriteScale = GameWorld::getSpriteScale();

        SpritePtr spriteGetReady = atlas->getSprite("get_ready");
        spriteGetReady->setPivot(Sprite::k_pivotCenter);

        SpritePtr spriteTutorial = atlas->getSprite("tutorial");
        spriteTutorial->setPivot(Sprite::k_pivotCenter);

        Matrix4x4 scale, translate, translate2;

        scale.identity();
        scale.scale(spriteGetReady->width() * spriteScale, spriteGetReady->height() * spriteScale, 1.0f);

        translate.identity();
        translate.translate(screenRect.width() * 0.5f, screenRect.height() * 0.3f, 0.0f);

        SceneNode* sceneNodeGetReady = new SpriteSceneNode(spriteGetReady, m_getReadyScreen);
        sceneNodeGetReady->setTransform(scale * translate);
        sceneNodeGetReady->setZIndex(-6.0f);

        scale.identity();
        scale.scale(spriteTutorial->width() * spriteScale, spriteTutorial->height() * spriteScale, 1.0f);

        translate2.identity();
        translate2.translate(0.0f, (spriteTutorial->height() + spriteGetReady->height()) * spriteScale * 0.7f, 0.0f);

        SceneNode* sceneNodeTutorial = new SpriteSceneNode(spriteTutorial, m_getReadyScreen);
        sceneNodeTutorial->setTransform(scale * translate * translate2);
        sceneNodeTutorial->setZIndex(-6.0f);

        return true;
    }

    void HUDGetReady::destroy()
    {
        if(m_getReadyScreen != NULL)
        {
            SceneNode* root = m_getReadyScreen->getParentNode();
            root->removeChild(m_getReadyScreen, true);

            m_getReadyScreen = NULL;
        }
    }

    void HUDGetReady::update(float dt)
    {
        //do nothing
    }

    void HUDGetReady::show()
    {
        if(m_getReadyScreen != NULL)
        {
            m_getReadyScreen->setVisible(true);
        }
    }

    void HUDGetReady::hide()
    {
        if(m_getReadyScreen != NULL)
        {
            m_getReadyScreen->setVisible(false);
        }
    }

    //==================================================================================================
    // game over screen
    //==================================================================================================
    HUDGameOver::HUDGameOver()
        :m_gameOverScreen(NULL),
         m_bestScores(HUDScores::k_smallDigits),
         m_currentScores(HUDScores::k_smallDigits)
    {

    }

    HUDGameOver::~HUDGameOver()
    {
        destroy();
    }

    bool HUDGameOver::create(Atlas* atlas, SceneManager* sceneManager)
    {
        SceneNode* rootSceneNode = sceneManager->getRootNode();
        m_gameOverScreen = new SceneNode(rootSceneNode);
        m_gameOverScreen->setVisible(false);

        SpritePtr spriteGameOver = atlas->getSprite("game_over");
        spriteGameOver->setPivot(Sprite::k_pivotCenter);

        SpritePtr spriteScoresPanel = atlas->getSprite("scores_panel");
        spriteScoresPanel->setPivot(Sprite::k_pivotCenter);

        Matrix4x4 scale, translate, translate2;
        Rect2D screenRect = GameScreen::getScreenRect();
        float spriteScale = GameWorld::getSpriteScale();

        //game over
        scale.identity();
        scale.scale(spriteGameOver->width() * spriteScale, spriteGameOver->height() * spriteScale, 1.0f);

        translate.identity();
        translate.translate(screenRect.width() * 0.5f, screenRect.height() * 0.2f, 0.0f);

        SceneNode* sceneNodeGameOver = new SpriteSceneNode(spriteGameOver, m_gameOverScreen);
        sceneNodeGameOver->setZIndex(-6.0f);
        sceneNodeGameOver->setTransform(scale * translate);

        //score board pad
        scale.identity();
        scale.scale(spriteScoresPanel->width() * spriteScale, spriteScoresPanel->height() * spriteScale, 1.0f);

        translate2.identity();
        translate2.translate(0.0f, (spriteScoresPanel->height() + spriteGameOver->height()) * spriteScale * 0.7f, 0.0f);

        SceneNode* sceneNodeScoresPanel = new SpriteSceneNode(spriteScoresPanel, m_gameOverScreen);
        sceneNodeScoresPanel->setZIndex(-6.0f);
        sceneNodeScoresPanel->setTransform(scale * translate * translate2);

        //fake start button
        SpritePtr spriteFakeStartButton = atlas->getSprite("button_start");
        spriteFakeStartButton->setPivot(Sprite::k_pivotCenter);

        scale.identity();
        scale.scale(spriteFakeStartButton->width() * spriteScale, spriteFakeStartButton->height() * spriteScale, 0.0f);

        Matrix4x4 translate5;
        translate5.identity();
        translate5.translate(0.0f, (spriteFakeStartButton->height() + spriteScoresPanel->height()) * spriteScale * 0.6f, 0.0f);

        SceneNode* fakeStartButton = new SpriteSceneNode(spriteFakeStartButton, m_gameOverScreen);
        fakeStartButton->setZIndex(-6.0f);
        fakeStartButton->setTransform(scale * translate * translate2 * translate5);
        fakeStartButton->setVisible(true);

        //score digits
        Matrix4x4 scoresPanelTransform = sceneNodeScoresPanel->getWorldTransform();
        Vector3 currentScoresPos, bestScoresPos;
        currentScoresPos._x = scoresPanelTransform._41;
        currentScoresPos._y = scoresPanelTransform._42;
        bestScoresPos = currentScoresPos;

        currentScoresPos._x += spriteScoresPanel->width() * spriteScale * 0.33f;
        currentScoresPos._y -= spriteScoresPanel->height() * spriteScale * 0.13f;

        bestScoresPos._x += spriteScoresPanel->width() * spriteScale * 0.33f;
        bestScoresPos._y += spriteScoresPanel->height() * spriteScale * 0.22f;

        m_currentScores.create(atlas, currentScoresPos, m_gameOverScreen);
        m_bestScores.create(atlas, bestScoresPos, m_gameOverScreen);

        //medals
        SpritePtr spriteBronzeMedal = atlas->getSprite("medal_bronze");
        spriteBronzeMedal->setPivot(Sprite::k_pivotCenter);

        SpritePtr spriteSilverMedal = atlas->getSprite("medal_silver");
        spriteSilverMedal->setPivot(Sprite::k_pivotCenter);

        SpritePtr spriteGoldMedal = atlas->getSprite("medal_gold");
        spriteGoldMedal->setPivot(Sprite::k_pivotCenter);

        SpritePtr spriteNewMedal = atlas->getSprite("medal_new");
        spriteNewMedal->setPivot(Sprite::k_pivotCenter);

        scale.identity();
        scale.scale(spriteBronzeMedal->width() * spriteScale, spriteBronzeMedal->height() * spriteScale, 0.0f);

        Matrix4x4 translate3;
        translate3.identity();
        translate3.translate(-(spriteScoresPanel->width() * spriteScale * 0.28f),
                             (spriteScoresPanel->height() * spriteScale * 0.051f), 0.0f);

        Matrix4x4 medalPosition = scale * translate * translate2 * translate3;

        m_bronzeMedal = new SpriteSceneNode(spriteBronzeMedal, m_gameOverScreen);
        m_bronzeMedal->setZIndex(-5.0f);
        m_bronzeMedal->setTransform(medalPosition);
        m_bronzeMedal->setVisible(false);

        m_silverMedal = new SpriteSceneNode(spriteSilverMedal, m_gameOverScreen);
        m_silverMedal->setZIndex(-5.0f);
        m_silverMedal->setTransform(medalPosition);
        m_silverMedal->setVisible(false);

        m_goldMedal = new SpriteSceneNode(spriteGoldMedal, m_gameOverScreen);
        m_goldMedal->setZIndex(-5.0f);
        m_goldMedal->setTransform(medalPosition);
        m_goldMedal->setVisible(false);

        scale.identity();
        scale.scale(spriteNewMedal->width() * spriteScale, spriteNewMedal->height() * spriteScale, 0.0f);

        Matrix4x4 translate4;
        translate4.identity();
        translate4.translate(0.0f, (spriteBronzeMedal->height() + spriteNewMedal->height()) * spriteScale * 0.55f, 0.0f);
        medalPosition = scale * translate * translate2 * translate3 * translate4;

        m_newMedal = new SpriteSceneNode(spriteNewMedal, m_gameOverScreen);
        m_newMedal->setZIndex(-5.0f);
        m_newMedal->setTransform(medalPosition);
        m_newMedal->setVisible(false);

        return true;
    }

    void HUDGameOver::destroy()
    {
        m_currentScores.destroy();
        m_bestScores.destroy();

        if(m_gameOverScreen != NULL)
        {
            SceneNode* root = m_gameOverScreen->getParentNode();
            root->removeChild(root, true);

            m_gameOverScreen = NULL;
        }
    }

    void HUDGameOver::update(float dt)
    {
        //do nothing
    }

    void HUDGameOver::show(float currentScores, float bestScores, Medal medal, bool isNew)
    {
        if(m_gameOverScreen != NULL)
        {
            m_currentScores.setScores(currentScores);
            m_currentScores.show();

            m_bestScores.setScores(bestScores);
            m_bestScores.show();

            switch (medal)
            {
                case k_medalBronze:
                    m_bronzeMedal->setVisible(true);
                    break;
                case k_medalGold:
                    m_goldMedal->setVisible(true);
                    break;
                case k_medalSilver:
                    m_silverMedal->setVisible(true);
                    break;
                default:
                    break;
            };

            if(isNew)
            {
                m_newMedal->setVisible(true);
            }

            m_gameOverScreen->setVisible(true);
        }
    }

    void HUDGameOver::hide()
    {
        if(m_gameOverScreen != NULL)
        {
            m_gameOverScreen->setVisible(false);

            m_bronzeMedal->setVisible(false);
            m_silverMedal->setVisible(false);
            m_goldMedal->setVisible(false);
            m_newMedal->setVisible(false);
        }
    }

    //=============================================================================================
    //blind flash
    //=============================================================================================
    HUDBlindFlash::HUDBlindFlash()
        :m_flashSceneNode(NULL)
    {

    }

    HUDBlindFlash::~HUDBlindFlash()
    {
        destroy();
    }

    bool HUDBlindFlash::create(Atlas* atlas, SceneManager* sceneManager)
    {
        m_flashSprite = atlas->getSprite("white_quad");
        m_flashSprite->setPivot(Sprite::k_pivotCenter);
        m_flashSprite->setAlpha(0.0f);

        m_flashSceneNode = new SpriteSceneNode(m_flashSprite, sceneManager->getRootNode());

        Rect2D screenRect = GameScreen::getScreenRect();

        Matrix4x4 scale;
        scale.identity();
        scale.scale(screenRect.width(), screenRect.height() * 1.2f, 0.0f);

        Matrix4x4 translate;
        translate.identity();
        translate.translate(screenRect.width() * 0.5f, screenRect.height() * 0.5f, 0.0f);

        m_flashSceneNode->setTransform(scale * translate);
        m_flashSceneNode->setZIndex(-5.0f);

        return true;
    }

    void HUDBlindFlash::destroy()
    {
        if(m_flashSceneNode != NULL)
        {
            SceneNode* root = m_flashSceneNode->getParentNode();
            root->removeChild(m_flashSceneNode, true);
            m_flashSceneNode = NULL;
        }
    }

    void HUDBlindFlash::blind()
    {
        if(m_flashSprite != NULL)
        {
            m_flashSprite->setAlpha(1.0f);
        }
    }

    void HUDBlindFlash::update(float dt)
    {
        if(m_flashSprite != NULL)
        {
            float alpha = m_flashSprite->getAlpha();

            alpha -= 1.5f * dt;
            if (alpha < 0.0f)
            {
                alpha = 0.0f;
            }

            m_flashSprite->setAlpha(alpha);
        }
    }

    //====================================================================================
    //  HUDScores
    //====================================================================================
    HUDScores::HUDScores(DigitSize digitSize,  int numDigits)
        :m_digitSize(digitSize), m_numDigits(numDigits), m_boardSceneNode(NULL)
    {

    }

    HUDScores::~HUDScores()
    {
        destroy();
    }

    bool HUDScores::create(Atlas* atlas, const Vector3& position, SceneNode* parentNode)
    {
        Matrix4x4 scale, trans;

        trans.identity();
        trans.translate(position._x, position._y, 0.0f);

        m_boardSceneNode = new SceneNode(parentNode);
        m_boardSceneNode->setTransform(trans);
        m_boardSceneNode->setVisible(false);

        float spriteScale = GameWorld::getSpriteScale();
        float digitWidth, digitHeight, digitMargin, boardWidth;

        for(int32 i = 0; i < m_numDigits; i++)
        {
            SpritePtr spriteDigit;
            if(m_digitSize == k_bigDigits)
            {
                spriteDigit = atlas->getSprite("digit_big");
            }

            if(m_digitSize == k_smallDigits)
            {
                spriteDigit = atlas->getSprite("digit_small");
            }

            if(spriteDigit != NULL) {

                if(i == 0)
                {
                    digitWidth = spriteDigit->width() * spriteScale;
                    digitHeight = spriteDigit->height() * spriteScale;
                    digitMargin = digitWidth * 0.1f;
                    boardWidth = (digitWidth + digitMargin) * m_numDigits - digitMargin;
                }

                float offset = -(boardWidth * 0.5f) + (m_numDigits - i - 1) * (digitWidth + digitMargin);

                spriteDigit->setPivot(Sprite::k_pivotCenter);
                spriteDigit->setCurrentFrame(0);

                scale.identity();
                scale.scale(digitWidth, digitHeight, 1.0f);

                trans.identity();
                trans.translate(offset, 0.0f, 0.0f);

                SceneNode* sceneNodeDigit = new SpriteSceneNode(spriteDigit, m_boardSceneNode);
                sceneNodeDigit->setTransform(scale * trans);
                sceneNodeDigit->setZIndex(-5.0f);
                sceneNodeDigit->setVisible(false);

                m_digits.push_back(spriteDigit);
                m_digitNodes.push_back(sceneNodeDigit);
            } else {
                LOGE("AHTUNG!!!! m_digits[%d] = 0x%X", i, m_digits[i].get());
                return false;
            }
        }//for(int32 i = 0; i < k_numDigits; i++)



        return true;
    }

    void HUDScores::destroy()
    {
        if(m_boardSceneNode != NULL)
        {
            SceneNode *rootNode = m_boardSceneNode->getParentNode();
            rootNode->removeChild(m_boardSceneNode, true);
            m_boardSceneNode = NULL;
        }
    }

    void HUDScores::resetScores()
    {
        setScores(0);
    }

    void HUDScores::setScores(int scores)
    {
        m_scores = scores;
        updateBoard();
    }

    void HUDScores::addScores(int scrores)
    {
        m_scores+= scrores;
        updateBoard();
    }

    void HUDScores::show()
    {
        if(m_boardSceneNode != NULL)
        {
            m_boardSceneNode->setVisible(true);
        }
    }

    void HUDScores::hide()
    {
        if(m_boardSceneNode != NULL)
        {
            m_boardSceneNode->setVisible(false);
        }
    }

    void HUDScores::updateBoard()
    {
        if(m_boardSceneNode == NULL)
        {
            return;
        }

        for(int32 i = 0; i < m_digitNodes.size(); i++)
        {
            m_digitNodes[i]->setVisible(false);
        }

        int32 scores = m_scores;
        int32 index = 0;
        do
        {
            int32 digit = scores % 10;
            if(index < m_numDigits) {
                m_digitNodes[index]->setVisible(true);
                m_digits[index]->setCurrentFrame(digit);
            }
            index++;
            scores = scores / 10;
        }while(scores > 0);
    }

    //=====================================================================================================
    //
    //=====================================================================================================
    HUDFader::HUDFader()
        :m_fader(NULL), m_time(0.0f), m_leftTime(0.0f), m_mode(k_None)
    {

    }

    /*
     * SceneNode*  m_fader;
        float       m_time;
        int32       m_mode;
     * */

    HUDFader::~HUDFader()
    {
        destroy();
    }

    bool HUDFader::create(Atlas* atlas, SceneManager* sceneManager)
    {
        m_spriteFader = atlas->getSprite("black_quad");
        m_spriteFader->setPivot(Sprite::k_pivotLeftTop);

        m_fader = new SpriteSceneNode(m_spriteFader, sceneManager->getRootNode());
        m_fader->setVisible(false);
        m_fader->setZIndex(-5.0f);

        Rect2D screenRect = GameScreen::getScreenRect();
        Matrix4x4 scale;
        scale.identity();
        scale.scale(screenRect.width(), screenRect.height(), 0.0f);

        m_fader->setTransform(scale);

        return true;
    }

    void HUDFader::update(float dt)
    {
        if(m_fader == NULL) return;
        if(m_mode == k_None) return;
        if(m_leftTime <= 0.0f) return;

        if(m_mode == k_Fadein && m_time > 0.0f)
        {
            m_leftTime -= dt;
            float alpha = 1.0 - (m_leftTime / m_time);
            if(alpha > 1.0f) alpha = 1.0f;

            m_spriteFader->setAlpha(alpha);
        }

        if(m_mode == k_Fadeout && m_time > 0.0f)
        {
            m_leftTime -= dt;
            float alpha = m_leftTime / m_time;
            if(alpha < 0.0f) alpha = 0.0f;

            m_spriteFader->setAlpha(alpha);
        }

        if(m_leftTime <= 0.0f)
        {
            m_mode = k_None;
        }
    }

    void HUDFader::destroy()
    {
        if(m_fader != NULL)
        {
            SceneNode* root = m_fader->getParentNode();
            root->removeChild(m_fader, true);

            m_fader = NULL;
        }
    }

    void HUDFader::fadein(float time)
    {
        if(m_fader == NULL) return;

        m_time = m_leftTime = time;
        m_mode = k_Fadein;
        m_spriteFader->setAlpha(0.0f);
        m_fader->setVisible(true);
    }

    void HUDFader::fadeout(float time)
    {
        if(m_fader == NULL) return;

        m_time = m_leftTime = time;
        m_mode = k_Fadeout;
        m_spriteFader->setAlpha(1.0f);
        m_fader->setVisible(true);
    }
}
