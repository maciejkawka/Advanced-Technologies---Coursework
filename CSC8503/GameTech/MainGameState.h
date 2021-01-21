#pragma once
#include"CourseworkGame.h"
#include"../CSC8503Common/PushdownState.h"
#include"../CSC8503Common/PushdownMachine.h"
#include "../../Common/Window.h"




namespace NCL {
	namespace CSC8503 {

		class Pause : public PushdownState {
		public:
			Pause(CourseworkGame* g) : game(g) {}

		private:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				game->UpdatePause(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
					return PushdownResult::Pop;
		
				return PushdownResult::NoChange;
			}

			void OnAwake() override
			{
				std::system("cls");
				std::cout << "Pause" << std::endl;
			}

		protected:
			CourseworkGame* game;
		};


		class Singleplayer : public PushdownState {
		public:
			Singleplayer(CourseworkGame* g) : game(g) {}

		private:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				game->UpdateGame(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
				{
					*newState = new Pause(game);
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
					return PushdownResult::Pop;

				return PushdownResult::NoChange;
			}

			void OnAwake() override
			{		
				std::system("cls");
				std::cout << "Singleplayer" << std::endl;
			}

		protected:
			CourseworkGame* game;
		};

		class Multiplayer : public PushdownState {
		public:
			Multiplayer(CourseworkGame* g) : game(g) {}

		private:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				game->UpdateGame(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
				{
					*newState = new Pause(game);
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
					return PushdownResult::Pop;

				return PushdownResult::NoChange;
			}

			void OnAwake() override
			{
				std::system("cls");
				std::cout << "Multiplayer" << std::endl;
			}

		protected:
			CourseworkGame* game;
		};

		class MainMenu : public PushdownState {
		public:
			MainMenu(CourseworkGame* g): game(g) {}

		private:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				game->UpdateMenu(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1))
				{
					*newState = new Singleplayer(game);
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2))
				{
					*newState = new Multiplayer(game);
					game->InitMultiplayer();
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3))
				{
					return PushdownResult::Pop;
				}

				return PushdownResult::NoChange;
			}

			void OnAwake() override
			{
				game->InitSingleplayer();
				std::system("cls");
				std::cout << "Main menu" << std::endl;
			}

		protected:
			CourseworkGame* game;
		};

		class MainGameState {
		public:
			MainGameState(Window* w)
			{
				window = w;
				game = new CourseworkGame();
				machine = new PushdownMachine(new MainMenu(game));
			}

			~MainGameState()
			{
				delete game;
				delete machine;
			}


			bool Update(float dt)
			{
				return machine->Update(dt);	
			}

		protected:
			Window* window;
			CourseworkGame* game;
			PushdownMachine* machine;
		};
		

	}
}