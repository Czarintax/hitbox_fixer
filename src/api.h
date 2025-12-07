#pragma once

struct players_api
{
	virtual size_t GetMaxClients() = 0;
	virtual bool Init() = 0;
	virtual client_t* GetClient(size_t index) = 0;
	virtual model_t* GetModel(size_t model_index) = 0;
};

struct rehlds_api : players_api
{
	bool Init() override
	{
#ifdef _WIN32
		if (GetModuleHandleA("hw.dll"))
		{
			return false;
		}
#endif
		return RehldsApi_Init();
	}
	client_t* GetClient(size_t index) override
	{
		return g_RehldsSvs->GetClient_t(index);
	}
	size_t GetMaxClients() override
	{
		return g_RehldsSvs->GetMaxClients();
	}
	model_t* GetModel(size_t model_index)
	{
		return g_RehldsApi->GetServerData()->GetModel(model_index);
	}
};

struct hlds_api : players_api
{
	bool Init() override
	{
	#ifdef _WIN32
	    HMODULE handle = LoadLibraryA("xash.dll");
	    svs = (server_static_t*)GetProcAddress(handle, "svs");
	    sv  = (server_t*)GetProcAddress(handle, "sv");
	#else
	    void* handle = dlopen("libxash.so", RTLD_NOW);
	    svs = (server_static_t*)dlsym(handle, "svs");
	    sv  = (server_t*)dlsym(handle, "sv");
	#endif
	
	    if (!svs || !sv) {
			UTIL_ServerPrint("Failed to get svs or sv symbols");
	        return false;
	    }
	
	    return true;
	}

    client_t* GetClient(size_t index) override
    {
        return &svs->clients[index];
    }

    size_t GetMaxClients() override
    {
        return svs->maxclients;
    }

    model_t* GetModel(size_t model_index)
    {
        return sv->models[model_index];
    }

    server_static_t* svs;
    server_t* sv;
};
extern std::unique_ptr<players_api> api;
