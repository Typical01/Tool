
@echo off
chcp 65001
setlocal EnableDelayedExpansion
cd /d "build"
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo 生成 Debug 配置失败
    pause
    exit /b %errorlevel%
)
echo 生成 Debug 配置成功
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo 生成 Release 配置失败
    pause
    exit /b %errorlevel%
)
echo 生成 Release 配置成功
cmake --build . --config Debug_Win
if %errorlevel% neq 0 (
    echo 生成 Debug_Win 配置失败
    pause
    exit /b %errorlevel%
)
echo 生成 Debug_Win 配置成功
cmake --build . --config Release_Win
if %errorlevel% neq 0 (
    echo 生成 Release_Win 配置失败
    pause
    exit /b %errorlevel%
)
echo 生成 Release_Win 配置成功



echo 所有操作成功完成.
pause
