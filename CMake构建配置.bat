
@echo off
chcp 65001
if exist "build" rmdir /s /q "build"
mkdir "build"
cd /d "build"
echo 切换到 C:\Users\22793\source\repos\Tool\build 目录
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo 未安装 cmake 或 cmake 不在系统 PATH 中
    pause
    exit /b 1
)
cmake ..
if %errorlevel% neq 0 (
    echo 运行 cmake 失败 ..
    pause
    exit /b %errorlevel%
)
echo 运行 cmake 成功



pause
