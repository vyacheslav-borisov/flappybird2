#ifndef PEGAS_PROCESSES_H
#define PEGAS_PROCESSES_H
#pragma once

#include "../core/smart_pointer.h"
#include "../system/timer.h"
#include "interfaces.h"

namespace pegas
{
		enum ProcessStatus
		{
			k_processStatusNotStarted, //������� ������ ��� ������, � ����� ������� � ��������� �����
			k_processStatusSuspended, //������� �������� �������������
			k_processStatusRunning, //������� ������������
			k_processStatusKilled, //������� �������� ������ � ����� ������ � ��������� �����
			k_processStatusUnknown //��������� �������� ������-�� ���� ���������� �� ��������, �������� ��� ������ ���
		};		

		class ProcessManager
		{
		public:
			static const MILLISECONDS kTimeNoLimit;

			ProcessManager(): m_currentQueue(0), m_processTimeLimit(0), m_timer(NULL) {};
			virtual ~ProcessManager() {};

			void init(Timer* timer) { m_timer = timer; }

			//deltaTime - ���������� ����������� � ������� ���������� ������
			//timeLimit - ����� ������� �� ���������� ���� ���������
			void updateProcesses(MILLISECONDS deltaTime, MILLISECONDS timeLimit = kTimeNoLimit);
			
			ProcessHandle attachProcess(ProcessPtr process);
			ProcessPtr getProcess(const ProcessHandle& handle) const;
			void suspendProcess(const ProcessHandle& handle);
			void resumeProcess(const ProcessHandle& handle);
			void terminateProcess(const ProcessHandle& handle);
			ProcessStatus getProcessStatus(const ProcessHandle& handle) const;

			void suspendAllProcesses();
			void resumeAllProcesses();
			void terminateAllProcesses();
			void dettachAllProcesses();

			MILLISECONDS getProcessTimeLimit() const { return m_processTimeLimit; }
			
		private:
            static ProcessHandle s_nextHandle;
			static ProcessHandle getNextHandle();

			typedef std::map<ProcessHandle, ProcessPtr> ProcessMap;
			typedef std::queue<ProcessHandle> ProcessQueue;
			typedef std::list<ProcessHandle> ProcessList;

			ProcessPtr findActiveProcess(const ProcessHandle& handle);
						
			ProcessMap m_activeProcesses;
			ProcessMap m_newProcesses;
			
			ProcessQueue m_processQueue[2];
			size_t m_currentQueue;
			
			MILLISECONDS m_processTimeLimit;
			Timer*		 m_timer;

		private:
			ProcessManager(const ProcessManager& src);
			ProcessManager& operator=(const ProcessManager& src);
		};

		typedef ProcessManager* ProcessManagerPtr;

		/**********************************************************
		        ������� ����� ��������
		*********************************************************/
		class Process
		{
		public:
			Process();
			virtual ~Process() {};
			virtual void update(MILLISECONDS deltaTime) = 0;
			
			virtual void suspend();
			virtual void resume();
			virtual void terminate();

			virtual void attachNext(ProcessPtr nextProcess);
			ProcessPtr getNext() const;

			ProcessStatus getStatus() const { return m_currentStatus; }

		protected:

			virtual void start(ProcessHandle myHandle, ProcessManagerPtr owner);

			ProcessHandle m_handle;
			ProcessManagerPtr m_owner;
			ProcessPtr m_next;
			ProcessStatus m_currentStatus;

		private:
			//����� ����������� ������������� �� ProcessManager
			void _start(ProcessHandle myHandle, ProcessManagerPtr owner);
			friend class ProcessManager;

			Process(const Process& src);
			Process& operator=(const Process& src);
		};	
}//namespace pegas

#endif
