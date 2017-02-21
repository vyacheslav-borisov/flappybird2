#include "../common.h"
#include "../system/includes.h"

#include "scene.h"
#include "../core/includes.h"

namespace pegas
{
	//-----------------------------------------------------------------------------
	//	SceneManager class implementation
	//-----------------------------------------------------------------------------
	SceneManager::SceneManager()
		:m_quadTree()
	{
		LOGD_LOOP("SceneManager constructor");
	}

	SceneManager::~SceneManager()
	{
		LOGD_LOOP("SceneManager destructor");

		destroy();
	}

	void SceneManager::create(const Rect2D& worldArea)
	{
		LOGD_LOOP("SceneManager::create [worldArea: x1 = %0.2f, y1 = %0.2f, x2 = %0.2f, y2 = %0.2f]",
				worldArea._topLeft._x, worldArea._topLeft._y,
				worldArea._bottomRight._x, worldArea._bottomRight._y);

		LOGD_LOOP("setup listener");
		m_rootNode.addListener(this);

		LOGD_LOOP("setup quad tree");
		m_quadTree.create(worldArea);
	}

	void SceneManager::destroy()
	{
		LOGD_LOOP("SceneManager::destroy");

		m_quadTree.destroy();
		m_rootNode.removeAllChilds(true);
	}

	SceneNode* SceneManager::getRootNode()
	{
		return &m_rootNode;
	}

	void SceneManager::render(Gfx* gfx, const Rect2D& rect)
	{
		std::list<SceneNode*> nodesToRender;
		m_quadTree.query(rect, nodesToRender);

		LOGD_LOOP("nodes to render = %d", nodesToRender.size());

		for(std::list<SceneNode*>::iterator it = nodesToRender.begin();
				it != nodesToRender.end(); ++it)
		{
			if((*it)->isVisible())
			{
				(*it)->render(gfx);
			}
		}
	}

	void SceneManager::query(const Rect2D& rect, std::list<SceneNode*>& result)
	{
		m_quadTree.query(rect, result);
	}

	void SceneManager::query(const Point2D& point, std::list<SceneNode*>& result)
	{
		m_quadTree.query(point, result);
	}

	void SceneManager::onTransformChanged(SceneNode *sender)
	{
		LOGD_LOOP("SceneManager::onTransformChanged [sender = 0x%X]");

		Rect2D newAABB = sender->getBoundBox();
		LOGD_LOOP("AABB: x1: %.2f, y1: %.2f, x2: %.2f, y2: %.2f", newAABB._topLeft._x,
				newAABB._topLeft._y, newAABB._bottomRight._x, newAABB._bottomRight._y);

		bool r1 = m_quadTree.removeObject(sender);
		if(r1)
		{
			LOGD_LOOP("previous QuadTreeNode removed");
		}else
		{
			LOGD_LOOP("previous QuadTreeNode not removed");
		}

		bool r2 = m_quadTree.insertObject(sender, newAABB);
		if(r2)
		{
			LOGD_LOOP("new QuadTreeNode inserted");
		}else
		{
			LOGD_LOOP("new QuadTreeNode not inserted");
		}

		if((r1 && !r2) || (!r1 && r2))
		{
			LOGE("AHTUNG!!! sender = 0x%X", sender);
			LOGE("removed: %d", r1);
			LOGE("inserted: %d", r2);
		}
	}

	void SceneManager::onNodeRemoved(SceneNode* sender)
	{
		LOGD_LOOP("SceneManager::onNodeRemoved [sender = 0x%X]", sender);

		bool r = m_quadTree.removeObject(sender);
		if(r)
		{
			LOGD_LOOP("previous QuadTreeNode removed");
		}else
		{
			LOGD_LOOP("previous QuadTreeNode not removed");
			LOGE("AHTUNG!!! sender = 0x%X", sender);
		}
	}

	void SceneManager::onChildAttach(SceneNode* sender, SceneNode* child)
	{
		LOGD_LOOP("SceneManager::onChildAttach [sender = 0x%X, child = 0x%X]", sender, child);
		child->addListener(this);
		onTransformChanged(child);
	}

	//-----------------------------------------------------------------------------
	//	SceneNode class implementation
	//-----------------------------------------------------------------------------
	SceneNode::SceneNode(SceneNode* parentNode)
		:m_parentNode(parentNode), m_zIndex(1.0f), m_visible(true)
	{
		LOGD_LOOP("SceneNode constructor [this: 0x%X]", this);

		m_transform.identity();

		if(m_parentNode != NULL)
		{
			m_parentNode->attachChild(this);
		}
	}

	SceneNode::~SceneNode()
	{
		LOGD_LOOP("SceneNode destructor [this: 0x%X]", this);

		notifyListeners(k_nodeRemoved);
		removeAllChilds(true);
	}

	SceneNode* SceneNode::getParentNode()
	{
		return m_parentNode;
	}

	void SceneNode::attachChild(SceneNode* childNode)
	{
		LOGD_LOOP("SceneNode::attachChild [this: 0x%X, child = 0x%X]", this, childNode);

		SceneNode* prevParent = childNode->m_parentNode;
		if(prevParent)
		{
			prevParent->removeChild(childNode);
		}

		ChildNodeListIt it = std::find(m_childsNodes.begin(),
				m_childsNodes.end(), childNode);

		if(it == m_childsNodes.end())
		{
			childNode->m_parentNode = this;
			m_childsNodes.push_back(childNode);

			notifyListeners(k_attachChild, childNode);
		}
	}

	void SceneNode::removeChild(SceneNode* childNode, bool deleteChild)
	{
		LOGD_LOOP("SceneNode::removeChild [this: 0x%X, child = 0x%X, delete: %d]", this, childNode, deleteChild);

		ChildNodeListIt it = std::find(m_childsNodes.begin(),
						m_childsNodes.end(), childNode);

		if(it != m_childsNodes.end())
		{
			notifyListeners(deleteChild ? k_removeChild : k_dettachChild, (*it));

			if(deleteChild)
			{
				LOGI("deleting child 0x%X", childNode);
				delete (*it);
			}
			m_childsNodes.erase(it);

			LOGI("SceneNode 0x%X removed", childNode);
		}
	}

	void SceneNode::removeAllChilds(bool deleteChild)
	{
		LOGD_LOOP("SceneNode::removeAllChilds [this: 0x%X, delete: %d]", this, deleteChild);

		for(ChildNodeListIt it = m_childsNodes.begin();
						it != m_childsNodes.end(); ++it)
		{
			notifyListeners(deleteChild ? k_removeChild : k_dettachChild, (*it));

			if(deleteChild)
			{
				delete (*it);
			}
		}
		m_childsNodes.clear();
	}

	void SceneNode::setVisible(bool visible)
	{
		m_visible = visible;
	}

	bool SceneNode::isVisible() const
	{
		if(m_parentNode != NULL)
		{
			return m_visible && m_parentNode->isVisible();
		}

		return m_visible;
	}

	void SceneNode::setTransform(const Matrix4x4 &transform)
	{
		m_transform = transform;

		notifyListeners(k_transfromChanged);
		updateChildTransforms(this);
	}

	void SceneNode::updateChildTransforms(SceneNode* sender)
	{
		for(ChildNodeListIt it = m_childsNodes.begin();
			it != m_childsNodes.end(); ++it)
		{
			(*it)->onTransformChanged(sender);
			(*it)->updateChildTransforms(sender);
		}
	}

	Matrix4x4  SceneNode::getLocalTransform()
	{
		return m_transform;
	}

	Matrix4x4  SceneNode::getWorldTransform()
	{
		Matrix4x4 local = getLocalTransform();
		Matrix4x4 world = local;

		if(m_parentNode)
		{
			world = local * m_parentNode->getWorldTransform();
		}

		return world;
	}

	Rect2D SceneNode::getBoundBox()
	{
		return Rect2D();
	}

	void SceneNode::render(Gfx* gfx)
	{
		for(ChildNodeListIt it = m_childsNodes.begin();
						it != m_childsNodes.end(); ++it)
		{
			(*it)->render(gfx);
		}
	}

	void SceneNode::addListener(SceneNodeEventListener* listener)
	{
		LOGD_LOOP("SceneNode::addListener [this: 0x%X, listener: 0x%X]", this, listener);

		ListenersIt found_it = std::find(m_listeners.begin(), m_listeners.end(), listener);
		if(found_it == m_listeners.end())
		{
			m_listeners.push_back(listener);
		}
	}

	void SceneNode::removeListener(SceneNodeEventListener* listener)
	{
		LOGD_LOOP("SceneNode::removeListener [this: 0x%X, listener: 0x%X]", this, listener);


		ListenersIt found_it = std::find(m_listeners.begin(), m_listeners.end(), listener);
		if(found_it != m_listeners.end())
		{
			m_listeners.erase(found_it);
			LOGI("listener removed");
		}
	}

	void SceneNode::removeListenerSafe(SceneNodeEventListener* listener)
	{
		LOGD_LOOP("SceneNode::removeListenerSafe [this: 0x%X, listener: 0x%X]", this, listener);

		m_removedListeners.push_back(listener);
	}

	void SceneNode::notifyListeners(SceneNodeEventType e, SceneNode* child)
	{
		LOGI("SceneNode::notifyListeners this = %x, event = %d, child = %x", this, e, child);

		for(ListenersIt it = m_listeners.begin(); it != m_listeners.end(); it++)
		{
			SceneNodeEventListener* listener = (*it);
			LOGI("listener = %x", listener);

			switch(e)
			{
			case k_transfromChanged:
				listener->onTransformChanged(this);
				break;
			case k_nodeRemoved:
				listener->onNodeRemoved(this);
				break;
			case k_attachChild:
				listener->onChildAttach(this, child);
				break;
			case k_dettachChild:
				listener->onChildDettach(this, child);
				break;
			case k_removeChild:
				listener->onChildRemove(this, child);
				break;
			default:
				LOGW("unknown event");
				break;
			};
		}

		for(ListenersIt it = m_removedListeners.begin(); it != m_removedListeners.end(); it++)
		{
			removeListener(*it);
		}
		m_removedListeners.clear();
	}
}




