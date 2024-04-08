#include "stdafx.h"
#include "framework.h"

wstring						g_title;
unique_ptr<GameFramework>	g_framework;
mt19937						g_randomEngine{ random_device{}() };