#include "pch.h"
#include "Edit.h"


void Typical_Tool::WindowsSystem::WindowEdit::Init(CharHandleOfConfigFile& 配置文件)
{
    this->标签选项卡计数 = 0;

    //选项卡初始化
    this->标签选项卡 = { 0 };
    this->标签选项卡.mask = TCIF_IMAGE | TCIF_TEXT;

    //读取标签主窗口大小
    this->主窗口宽 = 400;
    this->主窗口高 = 300;

    //读取标签
    auto 标签区域 = 配置文件.Get指定区域内容(_T("标签"));
    if (标签区域.size() > 0) {
        for (auto 标签 = 标签区域.begin(); 标签 != 标签区域.end(); 标签++) {
            Ustr 标签名 = 标签->first;
            Ustr 标签文件路径 = 标签->second;

            CharHandleOfConfigFile 标签文件;
            Ustr 标签内容;
            if (标签文件.Init_Str(标签文件路径, 标签内容)) {

            }
            this->添加标签(标签名, 标签内容);
            this->显示标签窗口();
        }
    }
    else { //没有 标签区域
        this->添加标签(_T("标签1"), _T(""));
        this->显示标签窗口();
    }
}

void Typical_Tool::WindowsSystem::WindowEdit::标签窗口刷新()
{
    //根据主窗口客户区修改选项卡控件大小
    RECT rcTabDisplay_Tab;
    //获取Tab控件客户区的的大小（Tab窗口坐标系）
    GetClientRect(this->hWnd_标签主窗口, &rcTabDisplay_Tab);//获取Tab窗口的大小（而不是在屏幕坐标系中）
    //TabCtrl_AdjustRect(hWnd, FALSE, &rcTabDisplay_Tab);//通过窗口大小，获取客户区大小。
    MoveWindow(this->hWnd_标签选项卡, rcTabDisplay_Tab.left, rcTabDisplay_Tab.top - 6, rcTabDisplay_Tab.right, rcTabDisplay_Tab.bottom - 20, 0);
    
    //根据选项卡控件客户区修改编辑框大小
    RECT rcTabDisplay_Edit;
    GetClientRect(this->hWnd_标签选项卡, &rcTabDisplay_Edit);//获取选项卡控件的大小（而不是在屏幕坐标系中）
    TabCtrl_AdjustRect(this->hWnd_标签选项卡, FALSE, &rcTabDisplay_Edit);//通过窗口大小，获取客户区大小。
    MoveWindow(this->hWnd_编辑窗口, rcTabDisplay_Edit.left - 5, rcTabDisplay_Edit.top, rcTabDisplay_Edit.right + 5, rcTabDisplay_Edit.bottom, 0);
}

void Typical_Tool::WindowsSystem::WindowEdit::标签内容保存(ToolsConfigFile& 工具箱配置文件, CharHandleOfConfigFile& 配置文件)
{
    //创建进度条
    WinHost::创建进度条(_T("保存标签中..."));

    //保存文件路径到配置文件
    std::vector<Ustr> temp;
    for (auto tempStr = this->hWnd_标签内容.begin(); tempStr != this->hWnd_标签内容.end(); tempStr++) {
        Ustr 标签名 = tempStr->first;
        //保存标签内容到 标签名.txt
        Ustr 标签名文件路径 = 工具箱配置文件.Get程序父文件夹路径() + _T("\\config") + _T("\\") + 标签名 + _T(".txt");
        
        CharHandleOfConfigFile 标签文件;
        if (标签文件.Init(标签名文件路径, false)) {
            标签文件.写入文件_Str(tempStr->second);
        }
        temp.push_back(标签名 + _T("=") + 标签名文件路径);

        //进度条推进
        WinHost::增加进度条进度();
        Timers::sleep_s(100);
    }
    配置文件.添加区域(_T("标签"), temp);
    配置文件.写入文件();

    //进度条结束
    WinHost::增加进度条进度(100);
}

void Typical_Tool::WindowsSystem::WindowEdit::创建标签窗口(Ustr 标签窗口类名, WinHost& wh)
{
    this->hWnd_标签主窗口 = CreateWindowEx(NULL,
        标签窗口类名.c_str(), _T("标签"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, this->主窗口宽, this->主窗口高,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    wh.添加窗口托管(_T("hWnd_标签主窗口"), this->hWnd_标签主窗口, false);

    this->标签菜单_Add = WinHost::GetHMENU();
    this->标签菜单_Set = WinHost::GetHMENU();
    this->标签菜单_Help = WinHost::GetHMENU();
    this->标签菜单_Del = WinHost::GetHMENU();
    this->标签菜单_Save = WinHost::GetHMENU();

    HMENU hMain = CreateMenu();
    AppendMenu(hMain, MF_POPUP, this->标签菜单_Save, __TEXT("保存"));
    AppendMenu(hMain, MF_POPUP, this->标签菜单_Add, __TEXT("增加"));
    AppendMenu(hMain, MF_POPUP, this->标签菜单_Set, __TEXT("修改"));
    AppendMenu(hMain, MF_POPUP, this->标签菜单_Del, __TEXT("帮助"));
    AppendMenu(hMain, MF_POPUP, this->标签菜单_Help, __TEXT("删除"));
    SetMenu(this->hWnd_标签主窗口, hMain);

    this->hWnd_标签选项卡 = CreateWindow(
        WC_TABCONTROL,
        _T("标签列控件"),
        WS_VISIBLE | WS_CHILD | TCS_TABS,
        0, 0, 435, 300, 
        this->hWnd_标签主窗口, NULL, GetModuleHandle(NULL), NULL);
    wh.添加窗口托管(_T("hWnd_标签选项卡"), this->hWnd_标签选项卡);

    this->hWnd_编辑窗口 = CreateWindow(
        _T("EDIT"), _T("标签内容"),
        WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | WS_TABSTOP,
        0, 24, 550, 237,
        this->hWnd_标签主窗口, NULL, GetModuleHandle(NULL), NULL);
    //需要命名时再显示
    wh.添加窗口托管(_T("hWnd_编辑窗口"), this->hWnd_编辑窗口);


    //命名窗口
    this->hWnd_标签命名 = CreateWindow(
        标签窗口类名.c_str(), _T("标签命名"),
        WS_BORDER | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 150,
        this->hWnd_标签主窗口, NULL, GetModuleHandle(NULL), NULL);
    wh.添加窗口托管(_T("hWnd_标签命名"), hWnd_标签命名, false);

    this->标签命名_编辑框 = WinHost::GetHMENU();
    this->hWnd_标签命名_编辑框 = CreateWindow(
        _T("EDIT"), _T(""),
        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_NOHIDESEL, // 左对齐, 输入文本时水平滚动文本
        0, 0, 200, 30,
        this->hWnd_标签命名, (HMENU)this->标签命名_编辑框, GetModuleHandle(NULL), NULL);
    wh.添加窗口托管(_T("hWnd_标签命名_编辑框"), this->hWnd_标签命名_编辑框);

    this->标签命名_确认按键 = WinHost::GetHMENU();
    this->hWnd_标签命名_确认按键 = CreateWindow(
        _T("BUTTON"), _T("确定"),
        WS_CHILD | WS_VISIBLE, 
        0, 30, 80, 40,   
        this->hWnd_标签命名, (HMENU)this->标签命名_确认按键, GetModuleHandle(NULL), NULL);
    wh.添加窗口托管(_T("hWnd_标签命名_确认按键"), this->hWnd_标签命名_确认按键);

    标签窗口刷新();
}

void Typical_Tool::WindowsSystem::WindowEdit::显示标签窗口(int 显示窗口)
{
    if (this->hWnd_标签主窗口 != NULL) {
        ShowWindow(this->hWnd_标签主窗口, 显示窗口);
    }
    /*if (this->hWnd_标签选项卡 != NULL) {
        ShowWindow(this->hWnd_标签选项卡, 显示窗口);
    }
    if (this->hWnd_编辑窗口 != NULL) {
        ShowWindow(this->hWnd_编辑窗口, 显示窗口);
    }*/
}

void Typical_Tool::WindowsSystem::WindowEdit::显示命名窗口(int 显示窗口)
{
    if (this->hWnd_标签命名 != NULL) {
        ShowWindow(this->hWnd_标签命名, 显示窗口);
    }
}

void Typical_Tool::WindowsSystem::WindowEdit::添加标签()
{
    Ustr 标签命名;
    获取句柄文本(this->hWnd_标签命名_编辑框, 标签命名);
    this->标签选项卡.pszText = (LPWSTR)标签命名.c_str();

    if (SendMessage(this->hWnd_标签选项卡, TCM_INSERTITEM, this->Get标签计数(), (LPARAM)&this->标签选项卡)){
        lgc(_T("添加标签: ") + 标签命名, lgm::wr);
    }
    this->hWnd_标签内容.insert(std::make_pair(标签命名, _T("")));
    SendMessage(this->hWnd_编辑窗口, WM_SETTEXT, 0, (LPARAM)_T(""));
    this->显示标签内容(标签命名);
}

void Typical_Tool::WindowsSystem::WindowEdit::添加标签(Ustr 标签名, Ustr& 标签内容)
{
    this->标签选项卡.pszText = (LPWSTR)标签名.c_str();
    if (SendMessage(this->hWnd_标签选项卡, TCM_INSERTITEM, this->Get标签计数(), (LPARAM)&this->标签选项卡)) {
        lgc(_T("添加标签: ") + 标签名, lgm::wr);
    }
    this->hWnd_标签内容.insert(std::make_pair(标签名, 标签内容));
    SendMessage(this->hWnd_编辑窗口, WM_SETTEXT, 0, (LPARAM)_T(""));
    this->显示标签内容(标签名);
}

void Typical_Tool::WindowsSystem::WindowEdit::添加标签(Ustr 标签名, Ustr&& 标签内容)
{
    this->标签选项卡.pszText = (LPWSTR)标签名.c_str();
    if (SendMessage(this->hWnd_标签选项卡, TCM_INSERTITEM, this->Get标签计数(), (LPARAM)&this->标签选项卡)) {
        lgc(_T("添加标签: ") + 标签名, lgm::wr);
    }
    this->hWnd_标签内容.insert(std::make_pair(标签名, 标签内容));
    SendMessage(this->hWnd_编辑窗口, WM_SETTEXT, 0, (LPARAM)_T(""));
    this->显示标签内容(标签名);
}

void Typical_Tool::WindowsSystem::WindowEdit::修改标签()
{
    Ustr 标签命名;
    获取句柄文本(this->hWnd_标签命名_编辑框, 标签命名);
    this->标签选项卡.pszText = (LPWSTR)标签命名.c_str();

    int 当前标签选项卡 = SendMessage(this->hWnd_标签选项卡, TCM_GETCURSEL, 0, 0);
    //修改 hWnd_标签内容
    auto 当前标签 = this->hWnd_标签内容.find(this->Get标签名(当前标签选项卡)); //查找当前标签
    if (当前标签 != this->hWnd_标签内容.end()) {
        Ustr temp = 当前标签->first; //暂存
        this->hWnd_标签内容.erase(当前标签); //删除
        this->hWnd_标签内容.insert(std::make_pair(标签命名, temp)); //再添加新项
    }
    //修改 hWnd_标签选项卡
    if (SendMessage(this->hWnd_标签选项卡, TCM_SETITEM, 当前标签选项卡, (LPARAM)&this->标签选项卡)) {
        lgc(_T("修改标签: ") + 当前标签->first, lgm::wr);
    }
    SendMessage(this->hWnd_标签选项卡, TCM_SETCURFOCUS, 当前标签选项卡, 0); //设置焦点
    SendMessage(this->hWnd_编辑窗口, WM_SETTEXT, 0, (LPARAM)_T(""));
    this->显示标签内容(当前标签->first);
}

void Typical_Tool::WindowsSystem::WindowEdit::删除标签()
{
    int 当前标签选项卡总数 = SendMessage(this->hWnd_标签选项卡, TCM_GETITEMCOUNT, 0, 0);
    //最后一个选项卡不能删除
    if (当前标签选项卡总数 > 0) {
        int 当前标签选项卡 = SendMessage(this->hWnd_标签选项卡, TCM_GETCURSEL, 0, 0);
        Ustr 标签名 = Get标签名(当前标签选项卡);
        this->hWnd_标签内容.erase(标签名);
        if (SendMessage(this->hWnd_标签选项卡, TCM_DELETEITEM, 当前标签选项卡, 0)) { //总数 - 1
            lgc(_T("删除标签: ") + 标签名, lgm::wr);
        }
        当前标签选项卡总数 -= 2; //总数与元素访问位置相差 1
        SendMessage(this->hWnd_标签选项卡, TCM_SETCURFOCUS, 当前标签选项卡总数, 0); //设置焦点
        
        this->显示标签内容(标签名);
    }
}

void Typical_Tool::WindowsSystem::WindowEdit::显示标签内容(Ustr 标签名)
{
    Ustr temp = this->hWnd_标签内容.find(标签名)->second;
    SendMessage(this->hWnd_编辑窗口, WM_SETTEXT, 0, (LPARAM)temp.c_str());
}

Ustr Typical_Tool::WindowsSystem::WindowEdit::Get标签名(int 标签计数)
{
    TCITEM tcItem = { 0 };
    tcItem.mask = TCIF_TEXT;
    tcItem.cchTextMax = 256; // 假设文本不会超过255个字符（加上终止的空字符）  
    Uchar szBuffer[256];
    tcItem.pszText = szBuffer;
    
    if (标签计数 != -1) {
        if (SendMessage(this->hWnd_标签选项卡, TCM_GETITEM, (WPARAM)标签计数, (LPARAM)&tcItem))
        {
            // 成功检索到文本  
            return (Ustr)szBuffer;
        }
        else
        {
            // 检索失败  
            lgc(_T("Get标签名() TCM_GETITEM(return false): ") + Uto_string(标签计数), lgm::er);
            return Ustr();
        }
    }
    lgc(_T("Get标签名() 标签计数 == -1: ") + Uto_string(标签计数), lgm::er);
    return Ustr();
}

int Typical_Tool::WindowsSystem::WindowEdit::Get当前标签选项()
{
    return SendMessage(this->hWnd_标签选项卡, TCM_GETCURSEL, 0, 0);
}

bool Typical_Tool::WindowsSystem::WindowEdit::获取句柄文本(HWND hWnd, Ustr& 文本)
{
    // 获取编辑控件中文本的长度（不包括终止的null字符）  
    int length = GetWindowTextLength(hWnd);

    // 如果长度为0，则没有文本，清空Ustr对象并返回true  
    if (length == 0) {
        文本.clear();
        return true;
    }

    // 分配足够的内存来存储文本（加1是为了终止的null字符）  
    // 注意：这里我们不直接使用Uchar*指针，而是稍后直接操作Ustr  
    Uchar* 文本指针 = new Uchar[length + 1]; // 使用new而不是malloc，因为我们在C++中  

    if (文本指针 == nullptr) {
        // 内存分配失败  
        return false;
    }

    // 获取文本  
    GetWindowText(hWnd, 文本指针, length + 1);

    // 将文本复制到Ustr对象中  
    // 正确的做法是使用Ustr的构造函数或赋值操作符  
    文本 = Ustr(文本指针); // 这会复制整个字符串，包括null终止符（但Ustr会忽略它）  

    // 释放之前分配的内存  
    delete[] 文本指针; // 使用delete[]来匹配new[]  

    // 函数成功执行  
    return true;
}

int Typical_Tool::WindowsSystem::WindowEdit::Get标签计数()
{
    return this->标签选项卡计数++;
}
