#include "stdafx.h"
#include "framework.h"

unique_ptr<GameFramework>	g_framework;
mt19937						g_randomEngine{ random_device{}() };