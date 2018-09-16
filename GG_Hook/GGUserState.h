#pragma once
namespace COGG {

	class GGUserState
	{
	public:
		static GGUserState *getInstance();
		char * getUsername();
		char * getPassword();
		void setUsername(char * value);
		void setPassword(char * value);

		~GGUserState();
	private:
		GGUserState();
		static GGUserState* instance;
		bool isPasswordSet;
		bool isUsernameSet;
		char *Password;
		char *Username;
		char *IP;

	};

}