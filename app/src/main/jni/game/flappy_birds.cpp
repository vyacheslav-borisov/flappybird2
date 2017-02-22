#include "../common.h"
#include "flappy_birds.h"

#include "../app/includes.h"

namespace pegas
{
	struct Event_Create_NextColumn: public Event
	{
	public:
		Event_Create_NextColumn()
		{
			LOGI("Event_Create_NextColumn()");
		}

		virtual EventType getType() const { return k_type; }
		static const EventType k_type;
	};
	const EventType Event_Create_NextColumn::k_type = "Event_Create_NextColumn";

	struct Event_Add_Scores: public Event
	{
	public:
		Event_Add_Scores()
		{
			LOGI("Event_Add_Scores()");
		}

		virtual EventType getType() const { return k_type; }
		static const EventType k_type;
	};
	const EventType Event_Add_Scores::k_type = "Event_Add_Scores";

	struct Event_Show_Scores: public Event
	{
	public:
		Event_Show_Scores(int32 scores)
				:_scores(scores)
		{
			LOGI("Event_Show_Scores()");
		}

		virtual EventType getType() const { return k_type; }
		static const EventType k_type;

		int _scores;
	};
	const EventType Event_Show_Scores::k_type = "Event_Show_Scores";

	struct Event_Bird_Dead: public Event
	{
	public:
		Event_Bird_Dead()
		{
			LOGI("Event_Bird_Dead()");
		}

		virtual EventType getType() const { return k_type; }
		static const EventType k_type;
	};
	const EventType Event_Bird_Dead::k_type = "Event_Bird_Dead";

	struct Event_Show_Flash: public Event
	{
	public:
		Event_Show_Flash()
		{
			LOGI("Event_Show_Flash()");
		}

		virtual EventType getType() const { return k_type; }
		static const EventType k_type;
	};
	const EventType Event_Show_Flash::k_type = "Event_Show_Flash";

	struct Event_Init_HUD: public Event
	{
	public:
		Event_Init_HUD(Atlas* atlas, SceneManager* sceneManager)
			:_atlas(atlas), _sceneManager(sceneManager)
		{
			LOGI("Event_Init_HUD()");
		}

		virtual EventType getType() const { return k_type; }
		static const EventType k_type;

		Atlas* _atlas;
		SceneManager* _sceneManager;
	};
	const EventType Event_Init_HUD::k_type = "Event_Init_HUD";


	//==============================================================================
	const int32 Bird::k_collisionGroup = 1;
	const int32 Obstacle::k_collisionGroup = 2;
	const int32 Trigger::k_collisionGroup = 3;

	//===============================================================================
	const std::string GameWorld::k_name = "game_world";

	float GameWorld::s_columnVelocity = 0.0f;
	float GameWorld::s_spriteScale = 0.0f;
	float GameWorld::s_bornLine = 0.0f;
	float GameWorld::s_deadLine = 0.0f;
	float GameWorld::s_columnWindowHeight = 0.0f;

	int GameWorld::k_bronzeMedalScores = 30;
	int GameWorld::k_silverMedalScores = 55;
	int GameWorld::k_goldMedalScores = 100;

	float GameWorld::getSpriteScale()
	{
		return s_spriteScale;
	}

	float GameWorld::getColumnVelocity()
	{
		return s_columnVelocity;
	}

	float GameWorld::getBornLine()
	{
		return s_bornLine;
	}

	float GameWorld::getDeadLine()
	{
		return s_deadLine;
	}

	float GameWorld::getColumnWindowHeight()
	{
		return s_columnWindowHeight;
	}

	////////////////////////////////////////////////////////////////////////////////////
	GameWorld::GameWorld()
		:m_gameStarted(false),
		 m_mainMenuMode(true),
		 m_columnsSpawned(0),
		 m_scores(HUDScores::k_bigDigits),
		 m_bestScores(0),
		 m_newBronzeMedal(true),
		 m_newSilverMedal(true),
		 m_newGoldMedal(true)
	{
	}

	void GameWorld::onCreate(IPlatformContext* context, void* pData)
	{
		GameObject::onCreate(context, pData);

		EventManager* eventManager = context->getEventManager();
		eventManager->addEventListener(this, Event_Game_Start::k_type);
		eventManager->addEventListener(this, Event_Game_Restart::k_type);
		eventManager->addEventListener(this, Event_Game_Stop::k_type);
		eventManager->addEventListener(this, Event_Create_NextColumn::k_type);
		eventManager->addEventListener(this, Event_Show_Flash::k_type);
		eventManager->addEventListener(this, Event_Bird_Dead::k_type);
		eventManager->addEventListener(this, Event_Add_Scores::k_type);
		eventManager->addEventListener(this, Event_Init_HUD::k_type);
		eventManager->addEventListener(this, Event_GUI_FadeinComplete::k_type);


		eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Background::k_name)));
		eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Ground::k_name)));
		eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Bird::k_name, (void*)this)));
	}

	void GameWorld::onDestroy(IPlatformContext* context)
	{
		m_getReadyScreen.destroy();
		m_gameOverScreen.destroy();
		m_blindFlash.destroy();
		m_scores.destroy();
		m_fader.destroy();
		m_mainMenu.destroy();

		EventManager* eventManager = context->getEventManager();
		eventManager->removeEventListener(this);

		context->removeMouseController(this);

		GameObject::onDestroy(context);
	}

	void GameWorld::onCreateSceneNode(Atlas* atlas, SceneManager* sceneManager, const Vector3& spawnPoint)
	{
		Rect2D screenRect = GameScreen::getScreenRect();
		float screenWidth = screenRect.width();

		SpritePtr background = atlas->getSprite("background_day");
		s_spriteScale = screenWidth / background->width();
		s_columnVelocity = -(screenWidth * 0.5f);

		SpritePtr column = atlas->getSprite("column_green_up");
		float columnWidth = column->width() * s_spriteScale;
		m_offset = columnWidth * 4.0f;
		s_bornLine = screenRect.width() + columnWidth;
		s_deadLine = -columnWidth;
		m_spawnPosition._x = s_bornLine;

		SpritePtr bird = atlas->getSprite("bird");
		s_columnWindowHeight = bird->height() * s_spriteScale * 3.5;

		initHUD(atlas, sceneManager);
		//EventManager* eventManager = m_context->getEventManager();
		//eventManager->pushEventToQueye(EventPtr(new Event_Init_HUD(atlas, sceneManager)));
	}

	void GameWorld::initHUD(Atlas* atlas, SceneManager* sceneManager)
	{
		Rect2D screenRect = GameScreen::getScreenRect();
		Vector3 scoresPosition;
		scoresPosition._x = screenRect.width() * 0.6f;
		scoresPosition._y = screenRect.height() * 0.15f;

		m_getReadyScreen.create(atlas, sceneManager);
		m_gameOverScreen.create(atlas, sceneManager);
		m_blindFlash.create(atlas, sceneManager);
		m_scores.create(atlas, scoresPosition, sceneManager->getRootNode());
		m_fader.create(atlas, sceneManager);
		m_mainMenu.create(atlas, sceneManager);

		m_getReadyScreen.hide();
		m_gameOverScreen.hide();
		m_scores.hide();
		m_mainMenu.show();

		m_context->addMouseController(this);
	}

	void GameWorld::onCreateCollisionHull(IPhysics* physicsManager)
	{
		physicsManager->setCollisionGroupFlag(Bird::k_collisionGroup, true);
		physicsManager->setCollisionGroupFlag(Obstacle::k_collisionGroup, false);
		physicsManager->setCollisionGroupFlag(Trigger::k_collisionGroup, false);

		physicsManager->setCollisionPairGroupFlag(Bird::k_collisionGroup, Obstacle::k_collisionGroup, true);
		physicsManager->setCollisionPairGroupFlag(Bird::k_collisionGroup, Trigger::k_collisionGroup, true);
		physicsManager->setCollisionPairGroupFlag(Trigger::k_collisionGroup, Obstacle::k_collisionGroup, false);
	}

	void GameWorld::onMouseButtonDown(MouseButton button, float x, float y, MouseFlags flags)
	{
		if(!m_mainMenuMode) return;

		m_mainMenuMode = false;
		m_fader.fadein(1.0f);

		Waiting* waiting = new Waiting(1.1f);
		waiting->addFinalEvent(EventPtr(new Event_GUI_FadeinComplete()));

		ProcessManager* processManager = m_context->getProcessManager();
		processManager->attachProcess(ProcessPtr(waiting));
	}

	void GameWorld::handleEvent(EventPtr evt)
	{
		LOGI("GameWorld::handleEvent");

		if(evt->getType() == Event_Init_HUD::k_type)
		{
			Event_Init_HUD* pEvent = evt->cast<Event_Init_HUD>();
			initHUD(pEvent->_atlas, pEvent->_sceneManager);
		}

		if(evt->getType() == Event_GUI_FadeinComplete::k_type)
		{
			m_fader.fadeout(1.0f);

			EventManager* eventManager = m_context->getEventManager();
			eventManager->pushEventToQueye(EventPtr(new Event_Game_Restart()));
		}

		if(evt->getType() == Event_Game_Start::k_type)
		{
			LOGI("evt->getType() == Event_Game_Start::k_type");

			m_gameStarted = true;
			m_spawnPosition._x = s_bornLine;

			m_getReadyScreen.hide();
			m_scores.resetScores();
			m_scores.show();

			for(int i = 0; i < 4; i++)
			{
				spawnNewColumn();
			}
		}

		if(evt->getType() == Event_Game_Restart::k_type)
		{
			LOGI("evt->getType() == Event_Game_Restart::k_type");

			EventManager* eventManager = m_context->getEventManager();
			eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Bird::k_name)));

			m_mainMenu.hide();
			m_gameOverScreen.hide();
			m_getReadyScreen.show();
		}

		if(evt->getType() == Event_Game_Stop::k_type)
		{
			LOGI("evt->getType() == Event_Game_Stop::k_type");

			m_gameStarted = false;
		}

		if(evt->getType() == Event_Create_NextColumn::k_type)
		{
			LOGI("evt->getType() == Event_Create_NextColumn::k_type");

			spawnNewColumn();
		}

		if(evt->getType() == Event_Show_Flash::k_type)
		{
			LOGI("evt->getType() == Event_Show_Flash::k_type");

			m_blindFlash.blind();
		}

		if(evt->getType() == Event_Bird_Dead::k_type)
		{
			LOGI("evt->getType() == Event_Bird_Dead::k_type");

			int scores = m_scores.getScores();
			m_scores.hide();

			if(scores > m_bestScores)
			{
				m_bestScores = scores;
			}

			HUDGameOver::Medal medal = HUDGameOver::k_medalNone;
			bool newMedal = false;
			if(scores > k_goldMedalScores)
			{
				medal = HUDGameOver::k_medalGold;
				newMedal = m_newGoldMedal;
				m_newGoldMedal = false;
			}else if(scores > k_silverMedalScores)
			{
				medal = HUDGameOver::k_medalSilver;
				newMedal = m_newSilverMedal;
				m_newSilverMedal = false;
			}else if(scores > k_bronzeMedalScores)
			{
				medal = HUDGameOver::k_medalBronze;
				newMedal = m_newBronzeMedal;
				m_newBronzeMedal = false;
			}

			m_gameOverScreen.show(scores, m_bestScores, medal, newMedal);
		}

		if(evt->getType() == Event_Add_Scores::k_type)
		{
			LOGI("evt->getType() == Event_Add_Scores::k_type");
			m_scores.addScores();
		}
	}

	void GameWorld::update(MILLISECONDS deltaTime)
	{
		float dt = deltaTime / 1000.0f;

		m_getReadyScreen.update(dt);
		m_gameOverScreen.update(dt);
		m_blindFlash.update(dt);
		m_fader.update(dt);

		if (m_gameStarted)
		{
			float offset = getColumnVelocity() * dt;
			m_spawnPosition._x += offset;
		}
	}

	void GameWorld::spawnNewColumn()
	{
		//LOGW_TAG("Pegas_debug", "GameWorld::spawnNewColumn");

		static int k_columnsInPeriod = 10;

		Rect2D screenRect = GameScreen::getScreenRect();
		float borderDown = Ground::getGroundLevel() - getColumnWindowHeight();
		float borderUp = getColumnWindowHeight();

		float t = (m_columnsSpawned % k_columnsInPeriod) / (1.0f * k_columnsInPeriod);
		float phase = t * Math::PI * 2.0f;
		float amplitude = borderDown - borderUp;
		float noiseAmplitude = amplitude * 0.3f;
		float baseVerticalOffset = borderUp + (amplitude * std::sin(phase));
		float noise = Math::rand(-noiseAmplitude, noiseAmplitude);
		baseVerticalOffset += noise;

		if(baseVerticalOffset > borderDown)
		{
			baseVerticalOffset = borderDown;
		}

		if(baseVerticalOffset < borderUp)
		{
			baseVerticalOffset = borderUp;
		}

		m_spawnPosition._y = baseVerticalOffset;

		EventManager* eventManager = m_context->getEventManager();
		eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Column::k_name, m_spawnPosition)));

		m_spawnPosition._x += m_offset;
		m_columnsSpawned++;
	}

	//===============================================================================
	const std::string Background::k_name = "background";

	void Background::onCreateSceneNode(Atlas* atlas,
									   SceneManager* sceneManager,
									   const Vector3& spawnPoint)
	{
		LOGI("obtaining background sprite...");
		SpritePtr background = atlas->getSprite("background_day");
		background->setPivot(Sprite::k_pivotLeftTop);

		Rect2D screenRect = GameScreen::getScreenRect();

		Matrix4x4 scale;
		scale.identity();
		scale.scale(screenRect.width(), screenRect.height(), 0.0f);

		LOGI("creating background scene node...");
		SpriteSceneNode* backgroundSceneNode = new SpriteSceneNode(background, sceneManager->getRootNode());
		backgroundSceneNode->setTransform(scale);
		backgroundSceneNode->setZIndex(-10.0f);
	}

	//===============================================================================
	const std::string Ground::k_name = "ground";

	float Ground::s_groundLevel = 0;
	float Ground::getGroundLevel()
	{
		return s_groundLevel;
	}

	void Ground::onCreate(IPlatformContext* context, void* pData)
	{
		GameObject::onCreate(context, pData);

		EventManager* eventManager = context->getEventManager();
		eventManager->addEventListener(this, Event_Game_Stop::k_type);
		eventManager->addEventListener(this, Event_Game_Restart::k_type);
	}

	void Ground::onDestroy(IPlatformContext* context)
	{
		EventManager* eventManager = context->getEventManager();
		eventManager->removeEventListener(this);

		GameObject::onDestroy(context);
	}

	void Ground::handleEvent(EventPtr evt)
	{
		if(evt->getType() == Event_Game_Stop::k_type)
		{
			m_isMoving = false;
		}

		if(evt->getType() == Event_Game_Restart::k_type)
		{
			m_isMoving = true;
		}
	}

	void Ground::onCreateSceneNode(Atlas* atlas, SceneManager* sceneManager, const Vector3& spawnPoint)
	{
		LOGI("obtaining ground sprite...");
		SpritePtr ground = atlas->getSprite("ground");
		ground->setPivot(Sprite::k_pivotCenter);

		float spriteWidth = ground->width() * GameWorld::getSpriteScale();
		float spriteHeight = ground->height() * GameWorld::getSpriteScale();
		Rect2D screenRect = GameScreen::getScreenRect();
		s_groundLevel = screenRect.height() - spriteHeight;

		Matrix4x4 scale, translate, world;

		scale.identity();
		scale.scale(spriteWidth, spriteHeight, 1.0f);

		translate.identity();
		translate.translate((screenRect.width() * 0.5f), (screenRect.height() - (spriteHeight * 0.5f)), 0.0f);
		world = scale * translate;

		LOGI("creating ground scene node #1...");
		m_sceneNodes[0] = new SpriteSceneNode(ground);
		m_sceneNodes[0]->setZIndex(-8.0f);
		m_sceneNodes[0]->setTransform(world);


		SceneNode* rootNode = sceneManager->getRootNode();
		LOGI("put ground scene node #1 to scene...");
		rootNode->attachChild(m_sceneNodes[0]);

		translate.identity();
		translate.translate(spriteWidth, 0.0f, 0.0f);
		world = world * translate;

		LOGI("creating ground scene node #2...");
		m_sceneNodes[1] = new SpriteSceneNode(ground);
		m_sceneNodes[1]->setZIndex(-8.0f);
		m_sceneNodes[1]->setTransform(world);

		LOGI("put ground scene node #2 to scene...");
		rootNode->attachChild(m_sceneNodes[1]);

		m_isMoving = true;
	}

	void Ground::update(MILLISECONDS deltaTime)
	{
		if(!m_isMoving) return;

		float dt = (deltaTime * 1.0f) / 1000.0f;
		float offset = GameWorld::getColumnVelocity() * dt;

		Matrix4x4 translate;
		translate.identity();
		translate.translate(offset, 0.0f, 0.0f);

		Matrix4x4 transform = m_sceneNodes[0]->getLocalTransform();
		transform = transform * translate;
		m_sceneNodes[0]->setTransform(transform);

		translate.identity();
		translate.translate(transform._11, 0.0f, 0.0f);
		transform = transform * translate;
		m_sceneNodes[1]->setTransform(transform);

		Rect2D screenRect = GameScreen::getScreenRect();
		Rect2D aabb = m_sceneNodes[0]->getBoundBox();
		if(aabb._bottomRight._x <= screenRect._topLeft._x)
		{
			transform = transform * translate;
			m_sceneNodes[0]->setTransform(transform);

			SceneNode* temp = m_sceneNodes[0];
			m_sceneNodes[0] = m_sceneNodes[1];
			m_sceneNodes[1] = temp;
		}
	}

	//===============================================================================
	const std::string Column::k_name = "column";

	Column::Column()
		: m_isAboutToDestroy(false), m_isMoving(false)
	{
		m_sceneNodes[k_up] = 0;
		m_sceneNodes[k_down] = 0;
	}

	void Column::onCreateSceneNode(Atlas* atlas, SceneManager* sceneManager, const Vector3& spawnPoint)
	{
		LOGI("Column::onCreateSceneNode this = %x", this);

		m_currentPosition = spawnPoint;

		//��������� �� ������ ������� ������-����������
		LOGI("obtaining column sprites...");
		//������ �������
		SpritePtr spriteColumnUP = atlas->getSprite("column_green_up");
		spriteColumnUP->setPivot(Sprite::k_pivotCenter);
		//������� �������
		SpritePtr spriteColumnDown = atlas->getSprite("column_green_down");
		spriteColumnDown->setPivot(Sprite::k_pivotCenter);

		float spriteWidth = spriteColumnUP->width() * GameWorld::getSpriteScale();
		float spriteHeight = spriteColumnUP->height() * GameWorld::getSpriteScale();
		float windowHeight = GameWorld::getColumnWindowHeight();

		Matrix4x4 scale, translate, world;
		//������� �������� - ������ ������� ������
		scale.identity();
		scale.scale(spriteWidth, spriteHeight, 1.0f);

		//��������� ������� �������
		translate.identity();
		translate.translate(spawnPoint._x, (spawnPoint._y - (spriteHeight * 0.5f) - (windowHeight * 0.5f)), 0.0f);
		world = scale * translate;

		//������� ���� ����� ��� �������
		LOGI("creating column scene node k_up...");
		m_sceneNodes[k_up] = new SpriteSceneNode(spriteColumnUP);
		m_sceneNodes[k_up]->setZIndex(-9.0f);
		m_sceneNodes[k_up]->setTransform(world);

		translate.identity();
		translate.translate(spawnPoint._x, (spawnPoint._y + (spriteHeight * 0.5f) + (windowHeight * 0.5f)), 0.0f);
		world = scale * translate;

		//��������� ������ �������
		LOGI("creating column scene node k_down...");
		m_sceneNodes[k_down] = new SpriteSceneNode(spriteColumnDown);
		m_sceneNodes[k_down]->setZIndex(-9.0f);
		m_sceneNodes[k_down]->setTransform(world);

		//�������� ���� �� �����
		SceneNode* rootNode = sceneManager->getRootNode();
		LOGI("put column scene node k_up to scene...");
		rootNode->attachChild(m_sceneNodes[k_up]);


		LOGI("put column scene node k_down to scene...");
		rootNode->attachChild(m_sceneNodes[k_down]);

		//������� ����������� ���� ����� ��� ���� � �������, ����� ������� ����� ��������� ������
		//�� ��� ������ ����� ������ ������� ������-������ ��� ����������� �������� ����� ������������� ����
		scale.identity();
		scale.scale(spriteWidth * 0.5f, windowHeight, 1.0f);
		translate.identity();
		translate.translate(spawnPoint._x, spawnPoint._y, 0.0f);
		world = scale * translate;

		LOGI("creating column scene node k_window...");
		m_sceneNodes[k_window] = new SceneNode(rootNode);
		m_sceneNodes[k_window]->setZIndex(-9.0f);
		m_sceneNodes[k_window]->setTransform(world);

		LOGI("put column scene node k_window to scene...");
		rootNode->attachChild(m_sceneNodes[k_window]);

		//������� �������� ������� �������: ��� ����������� ����������
		//� ���� ����-������
		EventManager* eventManager = m_context->getEventManager();
		eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Obstacle::k_name, (void*)m_sceneNodes[k_up])));
		eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Obstacle::k_name, (void*)m_sceneNodes[k_down])));
		eventManager->pushEventToQueye(EventPtr(new Event_Create_GameObject(Trigger::k_name, (void*)m_sceneNodes[k_window])));

		m_isMoving = true;

		LOGI("Column::onCreateSceneNode end");
	}

	void Column::update(MILLISECONDS deltaTime)
	{
		//LOGW_TAG("Pegas_debug", "Column::update");

		if(!m_isMoving) return;
		if(m_isAboutToDestroy) return;

		float dt = (deltaTime * 1.0f) / 1000.0f;
		float offset = GameWorld::getColumnVelocity() * dt;

		Matrix4x4 translate;
		translate.identity();
		translate.translate(offset, 0.0f, 0.0f);

		for(int i = 0; i < k_max; i++)
		{
			Matrix4x4 matTransform = m_sceneNodes[i]->getLocalTransform();
			matTransform = matTransform * translate;
			m_sceneNodes[i]->setTransform(matTransform);
		}

		m_currentPosition = m_currentPosition * translate;
		if(m_currentPosition._x < GameWorld::getDeadLine())
		{
			m_isAboutToDestroy = true;

			EventManager* eventManager = m_context->getEventManager();
			eventManager->pushEventToQueye(EventPtr(new Event_Create_NextColumn()));

			killMe();
		}
	}

	void Column::onCreate(IPlatformContext* context, void* pData)
	{
		LOGI("Column::onCreate this = %x", this);

		GameObject::onCreate(context, pData);

		LOGI("add event listener");
		EventManager* eventManager = context->getEventManager();
		eventManager->addEventListener(this, Event_Game_Stop::k_type);
		eventManager->addEventListener(this, Event_Game_Restart::k_type);

		LOGI("Column::onCreate end");
	}

	void Column::onDestroy(IPlatformContext* context)
	{
		LOGI("Column::onDestroy this = %x", this);

		LOGI("remove event listener");
		EventManager* eventManager = context->getEventManager();
		eventManager->removeEventListener(this);

		LOGI("remove column scene nodes");
		if(m_sceneNodes[k_down] && m_sceneNodes[k_up])
		{
			SceneNode* rootNode = m_sceneNodes[k_down]->getParentNode();
			if(rootNode)
			{
				for(int i = 0; i < k_max; i++)
				{
					rootNode->removeChild(m_sceneNodes[i], true);
					m_sceneNodes[i] = NULL;
				}
			}
		}

		GameObject::onDestroy(context);

		LOGI("Column::onDestroy end");
	}

	void Column::handleEvent(EventPtr evt)
	{
		if(evt->getType() == Event_Game_Stop::k_type)
		{
			m_isMoving = false;
		}

		if(evt->getType() == Event_Game_Restart::k_type)
		{
			m_isAboutToDestroy = true;
			killMe();
		}
	}

	//======================================================================================
	const std::string Bird::k_name = "bird";

	Bird::Bird()
		:  m_radius(0.0f)
		 , m_impulsVelocity(0.0f)
		 , m_fallVelocity(0.0f)
		 , m_impulsAngle(0.0f)
		 , m_fallAngle(0.0f)
		 , m_velocity(0.0f)
		 , m_gravity(0.0f)
		 , m_birdNode(NULL)
		 , m_mode(0)
		 , m_physicsManager(NULL)
		 , m_isAboutToDestroy(false)
		 , m_mainMenuMode(false)
	{

	}

	void Bird::onCreateSceneNode(Atlas* atlas,
								 SceneManager* sceneManager,
								 const Vector3& spawnPoint)
	{
		LOGI("Bird::onCreateSceneNode this = %x", this);

		m_gravity = Ground::getGroundLevel();
		m_impulsVelocity = -(Ground::getGroundLevel() / 2.0f);
		m_fallVelocity = -m_impulsVelocity;
		m_impulsAngle	= Math::PI / 12.0;
		m_fallAngle = -Math::PI / 3.0;

		Rect2D screenRect = GameScreen::getScreenRect();
		m_position._x = m_mainMenuMode ? (screenRect.width() * 0.5f) : (screenRect.width() * 0.3f);
		m_position._y = Ground::getGroundLevel() * 0.6f;

		LOGI("creating sprite...");
		SpritePtr spriteBird = atlas->getSprite("bird");
		spriteBird->setPivot(Sprite::k_pivotCenter);

		float spriteWidth = spriteBird->width() * GameWorld::getSpriteScale();
		float spriteHeight = spriteBird->height() * GameWorld::getSpriteScale();
		m_radius = std::min(spriteWidth, spriteHeight) * 0.5f;
		m_mode = k_modeIdle;

		Matrix4x4 matPosition, matTransform;

		m_size.identity();
		m_size.scale(spriteWidth, spriteHeight, 0.0f);

		matPosition.identity();
		matPosition.translate(m_position._x, m_position._y, 0.0f);

		matTransform = m_size * matPosition;

		LOGI("creating scene node...");
		m_birdNode = new SpriteSceneNode(spriteBird);
		m_birdNode->setZIndex(-7.0f);
		m_birdNode->setTransform(matTransform);

		LOGI("attach bird scene node to scene...");
		SceneNode* rootNode = sceneManager->getRootNode();
		rootNode->attachChild(m_birdNode);

		LOGI("creating sprite animation...");
		SpriteAnimation* animation = new SpriteAnimation(spriteBird);
		animation->setNumFrames(0, 4);
		animation->setFPS(8);

		m_animation = ProcessPtr(animation);
		ProcessManager* processManager = m_context->getProcessManager();
		processManager->attachProcess(m_animation);

		LOGI("Bird::onCreateSceneNode end");
	}

	void Bird::onCreateCollisionHull(IPhysics* physicsManager)
	{
		if(m_mainMenuMode) return;

		LOGI("Bird::onCreateCollisionHull this = %x", this);

		LOGI("register collidable circle");
		m_physicsManager = physicsManager;
		m_physicsManager->registerCircle((int32)this, k_collisionGroup, Vector3(), m_radius);
	}

	void Bird::onCollission(GameObject* other)
	{
		if(m_mode == k_modeIdle || m_mode == k_modeShock)
		{
			return;
		}

		if(other->getName() == Obstacle::k_name)
		{
			m_mode = k_modeShock;
			m_animation->suspend();
			if(m_velocity < m_fallVelocity)
			{
				m_velocity = m_fallVelocity;
			}

			EventManager* eventManager = m_context->getEventManager();
			eventManager->pushEventToQueye(EventPtr(new Event_Game_Stop()));
			eventManager->pushEventToQueye(EventPtr(new Event_Show_Flash()));
		}
	}

	void Bird::update(MILLISECONDS deltaTime)
	{
		if(m_isAboutToDestroy) return;

		static float elapsed = 0.0f;
		float dt = (deltaTime * 1.0f) / 1000.0f;
		elapsed += dt;

		float offset;
		if(m_mode == k_modeIdle)
		{
			const float k_deviation = 20.0f;
			offset = k_deviation * std::sin(Math::PI * elapsed);

			updateNodePosition(offset, true);

		}else if(m_mode != k_modeDead)
		{
			m_velocity += m_gravity * dt;

			if(m_velocity >= m_fallVelocity)
			{
				m_mode = (m_mode == k_modeShock) ? k_modeShock : k_modeFall;
				m_animation->suspend();

				setAngle(m_fallAngle);
			}else
			{
				float angle = interpollateAngle(m_velocity);
				setAngle(angle);
			}

			offset = m_velocity * dt;

			//птичка оказалась на земле
			if(isOnTheGround(offset))
			{
				LOGI("Bird grounded");

				m_mode = k_modeDead;

				EventPtr evt(new Event_Bird_Dead());
				EventManager* em = m_context->getEventManager();
				em->pushEventToQueye(evt);
			}

			offset = checkUpBound(offset);
			offset = checkDownBound(offset);
			m_position._y += offset;

			updateNodePosition(offset);
		}
	}

	void Bird::impuls()
	{
		if(m_mode == k_modeIdle || m_mode == k_modeFall || m_mode == k_modeFly)
		{
			m_velocity = m_impulsVelocity;

			if(m_animation->getStatus() == k_processStatusSuspended)
			{
				m_animation->resume();
			}

			m_mode = k_modeFly;
			setAngle(m_impulsAngle);
		}
	}

	bool  Bird::isOnTheGround(float offset)
	{
		float nextPosition = m_position._y + offset;
		float facingPoint = nextPosition + m_radius;

		if(facingPoint >= Ground::getGroundLevel())
		{
			return true;
		}

		return false;
	}

	float Bird::checkUpBound(float offset)
	{
		float rightOffset = offset;
		float nextPosition = m_position._y + offset;
		float facingPoint = nextPosition - m_radius;
		if(facingPoint <= 0)
		{
			nextPosition = m_radius;
			rightOffset = nextPosition - m_position._y;
		}

		return rightOffset;
	}

	float Bird::checkDownBound(float offset)
	{
		float rightOffset = offset;
		float nextPosition = m_position._y + offset;
		float facingPoint = nextPosition + m_radius;
		if(facingPoint >= Ground::getGroundLevel())
		{
			nextPosition = Ground::getGroundLevel() - m_radius;
			rightOffset = nextPosition - m_position._y;
		}

		return rightOffset;
	}

	void  Bird::updateNodePosition(float offset, bool absolute)
	{
		Matrix4x4 matTransform = m_birdNode->getLocalTransform();
		if(absolute)
		{
			matTransform._42 = m_position._y + offset;
		}else
		{
			Matrix4x4 matPosition;
			matPosition.identity();
			matPosition.translate(0.0f, offset, 0.0f);

			matTransform = matTransform * matPosition;
		}
		m_birdNode->setTransform(matTransform);

		if(!m_mainMenuMode)
		{
			m_physicsManager->transformObject((int32) this, matTransform);
		}
	}

	void Bird::setAngle(float angle)
	{
		Matrix4x4 rotation;
		rotation.identity();
		rotation.rotateZ(angle);

		Matrix4x4 matTransform = m_birdNode->getLocalTransform();

		Matrix4x4 position;
		position.identity();
		position.translate(m_position._x, m_position._y, 0.0f);

		matTransform = m_size * rotation * position;
		m_birdNode->setTransform(matTransform);
	}

	float Bird::interpollateAngle(float velocity)
	{
		float angle = m_impulsAngle;
		//float k = (velocity - m_impulsVelocity) / (m_fallVelocity - m_impulsVelocity);
		if(velocity > 0.0f)
		{
			float k = velocity / m_fallVelocity;
			angle = m_impulsAngle + (m_fallAngle - m_impulsAngle) * k;
		}

		return angle;
	}

	void Bird::onMouseButtonDown(MouseButton button, float x, float y, MouseFlags flags)
	{
		if(m_mode == k_modeIdle)
		{
			EventManager* eventManager = m_context->getEventManager();
			eventManager->pushEventToQueye(EventPtr(new Event_Game_Start()));
		}

		if(m_mode == k_modeDead)
		{
			EventManager* eventManager = m_context->getEventManager();
			eventManager->pushEventToQueye(EventPtr(new Event_Game_Restart()));

			return;
		}

		impuls();
	}

	void Bird::onCreate(IPlatformContext* context, void* pData)
	{
		LOGI("Bird::onCreate this = %x", (int)this);

		GameObject::onCreate(context, pData);

		if(pData != NULL)
		{
			GameWorld *gameWorld = (GameWorld *) pData;
			m_mainMenuMode = gameWorld->isMenuMode();
		}

		if(!m_mainMenuMode)
		{
			LOGI("add mouse controller");
			context->addMouseController(this);
		}

		LOGI("add event listener");
		EventManager* eventManager = context->getEventManager();
		eventManager->addEventListener(this, Event_Game_Restart::k_type);

		LOGI("Bird::onCreate end");
	}

	void Bird::onDestroy(IPlatformContext* context)
	{
		LOGI("Bird::onDestroy this = %x", (int)this);

		LOGI("removing animation...");
		m_animation->terminate();

		LOGI("removing scene node...");
		SceneNode* rootNode = m_birdNode->getParentNode();
		if(rootNode != NULL) {
			rootNode->removeChild(m_birdNode, true);
			m_birdNode = NULL;
		}

		if(!m_mainMenuMode)
		{
			LOGI("remove mouse controller...");
			context->removeMouseController(this);

			LOGI("unregister collision hull...");
			m_physicsManager->unregisterCollisionHull((int32)this);
		}

		LOGI("remove event listener...");
		EventManager* eventManager = context->getEventManager();
		eventManager->removeEventListener(this);

		GameObject::onDestroy(context);

		LOGI("Bird::onDestroy end");
	}

	void Bird::handleEvent(EventPtr evt)
	{
		if(evt->getType() == Event_Game_Restart::k_type)
		{
			m_isAboutToDestroy = true;
			killMe();
		}
	}

	//===============================================================================
	const std::string Obstacle::k_name = "Obstacle";
	const std::string Trigger::k_name = "Trigger";

	CollidableObject::CollidableObject()
		:m_physicsManager(0)
	{

	}

	void CollidableObject::onCreate(IPlatformContext* context, void* pData)
	{
		GameObject::onCreate(context, pData);

		SceneNode* sceneNode = reinterpret_cast<SceneNode*>(pData);
		sceneNode->addListener(this);
	}

	void CollidableObject::onCreateCollisionHull(IPhysics* physicsManager)
	{
		GameObject::onCreateCollisionHull(physicsManager);

		m_physicsManager = physicsManager;

		IPhysics::PointList points;
		points.push_back(Vector3(-0.5f, -0.5f, 0.0f));
		points.push_back(Vector3(0.5f, -0.5f, 0.0f));
		points.push_back(Vector3(0.5f, 0.5f, 0.0f));
		points.push_back(Vector3(-0.5f, 0.5f, 0.0f));

		int32 group = (getName() == Obstacle::k_name) ? Obstacle::k_collisionGroup : Trigger::k_collisionGroup;
		m_physicsManager->registerPoligon((int32)this, group, points);
	}

	void CollidableObject::onDestroy(IPlatformContext* context)
	{
		GameObject::onDestroy(context);

		m_physicsManager->unregisterCollisionHull((int32)this);
	}

	void CollidableObject::onTransformChanged(SceneNode *sender)
	{
		//LOGW_TAG("Pegas_debug", "CollidableObject::onTransformChanged");

		Matrix4x4 m = sender->getWorldTransform();
		m_physicsManager->transformObject((int32)this, m);
	}

	void CollidableObject::onNodeRemoved(SceneNode* sender)
	{
		killMe();
	}

	//======================================================================================
	void Trigger::onCollission(GameObject* other)
	{
		CollidableObject::onCollission(other);

		if(!m_isTriggered)
		{
			if(other->getName() == Bird::k_name)
			{
				m_isTriggered = true;

				EventManager* em = m_context->getEventManager();
				em->pushEventToQueye(EventPtr(new Event_Add_Scores()));
			}
		}
	}

	//===============================================================================
	class FlappyBirdsFactory: public GameObjectFactory
	{
	public:
		virtual GameObject* create(const std::string& name)
		{
			//LOGW_TAG("Pegas_debug", "FlappyBirdsFactory::create: %s", name.c_str());

			if(name == GameWorld::k_name)
			{
				return new GameWorld();
			}

			if(name == Background::k_name)
			{
				return new Background();
			}

			if(name == Ground::k_name)
			{
				return new Ground();
			}

			if(name == Bird::k_name)
			{
				return new Bird();
			}

			if(name == Column::k_name)
			{
				return new Column();
			}

			if(name == Trigger::k_name)
			{
				return new Trigger();
			}

			if(name == Obstacle::k_name)
			{
				return new Obstacle();
			}

			return NULL;
		}
	};

	static FlappyBirdsFactory gs_game_object_factory;
}



