/*
 * flappy_birds.h
 *
 *  Created on: 3 ���. 2016 �.
 *      Author: merelalkar
 */

#ifndef GAME_FLAPPY_BIRDS_H_
#define GAME_FLAPPY_BIRDS_H_

#include "game_screen.h"
#include "HUD.h"

namespace pegas
{
	class GameWorld: public GameObject, public IEventListener, public IMouseController
	{
	public:
		static const std::string k_name;

		static float getSpriteScale();
		static float getColumnVelocity();
		static float getBornLine();
		static float getDeadLine();
		static float getColumnWindowHeight();
	public:
		GameWorld();

		virtual std::string getName() { return k_name; }
		virtual ListenerType getListenerName() { return k_name; }

		bool isMenuMode() const { return m_mainMenuMode; }
		bool isGameStarted() const { return m_gameStarted; }

		virtual void onCreate(IPlatformContext* context, void* pData);
		virtual void onDestroy(IPlatformContext* context);
		virtual void onCreateSceneNode(Atlas* atlas, SceneManager* sceneManager, const Vector3& spawnPoint);
		virtual void onCreateCollisionHull(IPhysics* physicsManager);
		virtual void handleEvent(EventPtr evt);
		virtual void update(MILLISECONDS deltaTime);

		virtual void onMouseButtonDown(MouseButton button, float x, float y, MouseFlags flags);
		virtual void onMouseButtonUp(MouseButton button, float x, float y, MouseFlags flags) {}
		virtual void onMouseMove(float x, float y, MouseFlags flags) {}
		virtual void onMouseWheel(NumNothes wheel, MouseFlags flags) {}
	private:
		void initHUD(Atlas* atlas, SceneManager* sceneManager);
		void spawnNewColumn();

		Vector3 m_spawnPosition;
		int     m_columnsSpawned;
		float 	m_offset;
		bool	m_gameStarted;
		bool    m_mainMenuMode;

		HUDGetReady 	m_getReadyScreen;
		HUDGameOver 	m_gameOverScreen;
		HUDBlindFlash 	m_blindFlash;
		HUDScores	  	m_scores;
		HUDMainMenu     m_mainMenu;
		HUDFader		m_fader;

		int m_bestScores;
		bool m_newBronzeMedal;
		bool m_newSilverMedal;
		bool m_newGoldMedal;
	private:
		static float s_columnVelocity;
		static float s_spriteScale;
		static float s_bornLine;
		static float s_deadLine;
		static float s_columnWindowHeight;

		static int k_bronzeMedalScores;
		static int k_silverMedalScores;
		static int k_goldMedalScores;
	};

	class Background: public GameObject
	{
	public:
		static const std::string k_name;
	public:
		virtual std::string getName() { return k_name; }
		virtual void onCreateSceneNode(Atlas* atlas,
					SceneManager* sceneManager, const Vector3& spawnPoint);
	};

	class Ground: public GameObject, public IEventListener
	{
	public:
		static const std::string k_name;

		static float getGroundLevel();
	public:
		Ground(): m_isMoving(false) {}

		virtual std::string getName() { return k_name; }
		virtual void onCreateSceneNode(Atlas* atlas, SceneManager* sceneManager, const Vector3& spawnPoint);
		virtual void update(MILLISECONDS deltaTime);

		virtual void onCreate(IPlatformContext* context, void* pData);
		virtual void onDestroy(IPlatformContext* context);

		virtual ListenerType getListenerName() { return k_name; }
		virtual void handleEvent(EventPtr evt);

	private:
		static float s_groundLevel;

		SceneNode*  m_sceneNodes[2];
		bool		m_isMoving;
	};

	class Column: public GameObject, public IEventListener
	{
	public:
		static const std::string k_name;

	public:
		Column();

		virtual std::string getName() { return k_name; }
		virtual void onCreateSceneNode(Atlas* atlas, SceneManager* sceneManager, const Vector3& spawnPoint);
		virtual void update(MILLISECONDS deltaTime);

		virtual void onCreate(IPlatformContext* context, void* pData);
		virtual void onDestroy(IPlatformContext* context);

		virtual ListenerType getListenerName() { return k_name; }
		virtual void handleEvent(EventPtr evt);

	private:
		enum
		{
			k_up = 0,
			k_down,
			k_window,
			k_max
		};

		SceneNode*  		m_sceneNodes[k_max];
		Vector3     		m_currentPosition;
		bool				m_isMoving;
		bool 				m_isAboutToDestroy;
	};

	class Bird: public GameObject, public IMouseController, public IEventListener
	{
	public:
		static const std::string k_name;
		static const int32 k_collisionGroup;

	public:
		Bird();

		virtual std::string getName() { return k_name; }

		virtual void onCreate(IPlatformContext* context, void* pData);
		virtual void onDestroy(IPlatformContext* context);
		virtual void onCreateSceneNode(Atlas* atlas,
										   SceneManager* sceneManager,
										   const Vector3& spawnPoint);
		virtual void onCreateCollisionHull(IPhysics* physicsManager);
		virtual void onCollission(GameObject* other);
		virtual void update(MILLISECONDS deltaTime);

		virtual ListenerType getListenerName() { return k_name; }
		virtual void handleEvent(EventPtr evt);

		virtual void onMouseButtonDown(MouseButton button, float x, float y, MouseFlags flags);
		virtual void onMouseButtonUp(MouseButton button, float x, float y, MouseFlags flags) {}
		virtual void onMouseMove(float x, float y, MouseFlags flags) {}
		virtual void onMouseWheel(NumNothes wheel, MouseFlags flags) {}

	private:
		void impuls();
		void  updateNodePosition(float offset, bool absolute = false);
		void  setAngle(float angle);
		float interpollateAngle(float velocity);
		float checkUpBound(float offset);
		float checkDownBound(float offset);
		bool  isOnTheGround(float offset);

		enum
		{
			k_modeIdle = 0,
			k_modeFly,
			k_modeFall,
			k_modeShock,
			k_modeDead
		};

		IPhysics* m_physicsManager;

		Vector3 		m_position;
		Matrix4x4		m_size;

		float 	    	m_radius;
		float 			m_impulsVelocity;
		float			m_fallVelocity;
		float			m_impulsAngle;
		float			m_fallAngle;
		float 			m_velocity;
		float 			m_gravity;

		SceneNode* 		m_birdNode;
		ProcessPtr		m_animation;
		int 			m_mode;
		bool			m_isAboutToDestroy;
		bool 			m_mainMenuMode;
	};


	class CollidableObject : public GameObject, public SceneNodeEventListener
	{
	public:
		CollidableObject();

		virtual void onCreate(IPlatformContext* context, void* pData);
		virtual void onCreateCollisionHull(IPhysics* physicsManager);
		virtual void onDestroy(IPlatformContext* context);

		virtual void onTransformChanged(SceneNode *sender);
		virtual void onNodeRemoved(SceneNode* sender);

	private:
		IPhysics* m_physicsManager;
	};

	class Trigger: public CollidableObject
	{
	public:
		static const std::string k_name;
		static const int32 k_collisionGroup;

	public:
		Trigger(): m_isTriggered(false) {}
		virtual std::string getName() { return k_name; }

		virtual void onCollission(GameObject* other);

	private:
		bool m_isTriggered;
	};

	class Obstacle: public CollidableObject
	{
	public:
		static const std::string k_name;
		static const int32 k_collisionGroup;

	public:
		Obstacle() {}
		virtual std::string getName() { return k_name; }
	};
}


#endif /* GAME_FLAPPY_BIRDS_H_ */
