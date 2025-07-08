// include/pch.h
#pragma once

/*
    该文件是预编译头文件(Precompiled Header File)，包含了第三方库、标准库以及项目中常包含的头文件，以便提高编译速度.
    注意：必须要包含更新频率较低的头文件，否则每次更新这个文件，都要导致预编译头文件(pch.h.gch)重新生成. 请谨慎更新！！
*/

// STL基础库
#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <map>
#include <future>
#include <array>
#include <deque>
#include <functional>
#include <unordered_set>
#include <windows.h>
#include <algorithm>
#include <cmath>
#include "kits/strings.h"
#include "kits/streams.h"
#include "kits/file_system.h"
#include "kits/time.h"

// 第三方库文件
#include "kits/glfw.h"
#include "kits/glm.h"
#include "kits/json.h"
