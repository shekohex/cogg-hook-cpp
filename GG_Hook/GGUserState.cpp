#include "GGUserState.h"
#include <aixlog/aixlog.hpp>

namespace COGG {
	GGUserState* GGUserState::instance = 0;
	GGUserState::GGUserState()
	{
		LOG(DEBUG) << "GG User State is ready\n";
		isPasswordSet = false;
		isUsernameSet = false;
	}

	char * COGG::GGUserState::getUsername()
	{
		LOG(DEBUG) << "GG User State getUsername\n";
		if (!isUsernameSet)
			return nullptr;
		return Username;
	}

	char * COGG::GGUserState::getPassword()
	{
		LOG(DEBUG) << "GG User State getPassword\n";
		if (!isPasswordSet)
			return nullptr;
		return Password;
	}

	void COGG::GGUserState::setPassword(char * value)
	{
		LOG(DEBUG) << "GG User State setPassword\n";
		if (isPasswordSet) return;
		isPasswordSet = true;
		Password = value;
	}

	void COGG::GGUserState::setUsername(char * value)
	{
		LOG(DEBUG) << "GG User State setUsername\n";
		if (isUsernameSet) return;
		isUsernameSet = true;
		Username = value;
	}


	GGUserState::~GGUserState()
	{
		LOG(DEBUG) << "GG User State is deleting \n";
		delete Username;
		delete Password;
		delete instance;
	}
	
	GGUserState* GGUserState::getInstance()
	{
		LOG(DEBUG) << "GG User State get  Instance\n";
		if (instance == 0)
		{
			instance = new GGUserState();
		}

		return instance;
	}
}