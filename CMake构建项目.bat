
@echo off
chcp 65001
setlocal EnableDelayedExpansion
cd /d "build"
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo -e "[31m生成 Debug 配置失败.[0m"
    pause
    exit /b %errorlevel%
)
echo 生成 Debug 配置成功.
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo -e "[31m生成 Release 配置失败.[0m"
    pause
    exit /b %errorlevel%
)
echo 生成 Release 配置成功.
cmake --build . --config Debug_Win
if %errorlevel% neq 0 (
    echo -e "[31m生成 Debug_Win 配置失败.[0m"
    pause
    exit /b %errorlevel%
)
echo 生成 Debug_Win 配置成功.
cmake --build . --config Release_Win
if %errorlevel% neq 0 (
    echo -e "[31m生成 Release_Win 配置失败.[0m"
    pause
    exit /b %errorlevel%
)
echo 生成 Release_Win 配置成功.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo -e "[32m所有操作成功完成.[0m"
pause
