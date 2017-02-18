#include "../common.h"
#include "HUD.h"

#include "flappy_birds.h"

namespace pegas {
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
        sceneNodeGetReady->setTransfrom(scale * translate);
        sceneNodeGetReady->setZIndex(-6.0f);

        scale.identity();
        scale.scale(spriteTutorial->width() * spriteScale, spriteTutorial->height() * spriteScale, 1.0f);

        translate2.identity();
        translate2.translate(0.0f, (spriteTutorial->height() + spriteGetReady->height()) * spriteScale * 0.7f, 0.0f);

        SceneNode* sceneNodeTutorial = new SpriteSceneNode(spriteTutorial, m_getReadyScreen);
        sceneNodeTutorial->setTransfrom(scale * translate * translate2);
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
        :m_gameOverScreen(NULL)
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

        scale.identity();
        scale.scale(spriteGameOver->width() * spriteScale, spriteGameOver->height() * spriteScale, 1.0f);

        translate.identity();
        translate.translate(screenRect.width() * 0.5f, screenRect.height() * 0.3f, 0.0f);

        SceneNode* sceneNodeGameOver = new SpriteSceneNode(spriteGameOver, m_gameOverScreen);
        sceneNodeGameOver->setZIndex(-6.0f);
        sceneNodeGameOver->setTransfrom(scale * translate);

        scale.identity();
        scale.scale(spriteScoresPanel->width() * spriteScale, spriteScoresPanel->height() * spriteScale, 1.0f);

        translate2.identity();
        translate2.translate(0.0f, (spriteScoresPanel->height() + spriteGameOver->height()) * spriteScale * 0.7f, 0.0f);

        SceneNode* sceneNodeScoresPanel = new SpriteSceneNode(spriteScoresPanel, m_gameOverScreen);
        sceneNodeScoresPanel->setZIndex(-6.0f);
        sceneNodeScoresPanel->setTransfrom(scale * translate * translate2);

        return true;
    }

    void HUDGameOver::destroy()
    {
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

    void HUDGameOver::show(float currentScores, float bestScores)
    {
        if(m_gameOverScreen != NULL)
        {
            m_gameOverScreen->setVisible(true);
        }
    }

    void HUDGameOver::hide()
    {
        if(m_gameOverScreen != NULL)
        {
            m_gameOverScreen->setVisible(false);
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
        SceneNode* rootSceneNode = sceneManager->getRootNode();
        Rect2D screenRect = GameScreen::getScreenRect();

        m_flashSprite = atlas->getSprite("white_quad");
        m_flashSprite->setPivot(Sprite::k_pivotLeftTop);
        m_flashSprite->setAlpha(0.0f);

        m_flashSceneNode = new SpriteSceneNode(m_flashSprite, rootSceneNode);

        Matrix4x4 scale;
        scale.identity();
        scale.scale(screenRect.width(), screenRect.height(), 1.0f);

        m_flashSceneNode->setTransfrom(scale);
        m_flashSceneNode->setZIndex(-5.0f);

        return true;
    }

    void HUDBlindFlash::destroy()
    {
        if(m_flashSceneNode != NULL)
        {
            SceneNode* root = m_flashSceneNode->getParentNode();
            root->removeChild(m_flashSceneNode, true);
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

}
