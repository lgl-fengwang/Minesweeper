#ifndef _GAMECLASS_H
#define _GAMECLASS_H
enum DrawBox { Unknow, Flag, Question, Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Blast, ClickBlast, QuestionBlast, FlagBlast, Wrong };
enum MouseState { Move, LBtnUp, RBtnDown };
struct BoxState {
	DrawBox drawBox;
	bool layMine;
};
class Game {
private:

	BoxState **boxState;//格子数组
	int row;//当前格子所在行
	int column;//当前格子所在列
	POINTS point;//当前鼠标坐标
	MouseState mouseState;//鼠标状态

	bool gameStart;//是否开始排雷
	bool gameOver;//游戏是否失败
	bool gameSuccess;//游戏是否成功

	int timer;//计时
	int mine;//雷数

	
	HPEN hBlackPen;//黑色画笔
	HBRUSH hBlueBrush;//蓝色画刷
	HBRUSH hRedBrush;//红色画刷
	HFONT hCountFont;//字体
	HWND hStaticTime;//倒计时文本框
	HWND hStaticMine;//剩余雷数文本框

	HBITMAP hUnknow;
	HBITMAP hFlag;
	HBITMAP hQuestion;
	HBITMAP hLightUnknow;
	HBITMAP hLightFlag;
	HBITMAP hLightQuestion;
	HBITMAP hClickBlast;
	HBITMAP hWrong;
	HBITMAP hBlast;
	HBITMAP hFlagBlast;
	HBITMAP hQuestionBlast;
	HBITMAP hZero;
	HBITMAP hOne;
	HBITMAP hTwo;
	HBITMAP hThree;
	HBITMAP hFour;
	HBITMAP hFive;
	HBITMAP hSix;
	HBITMAP hSeven;
	HBITMAP hEight;
	HICON hBlock;
	HBITMAP compatibleBmp;

public:
	void SetMouseState(MouseState);//设置鼠标按键状态
	void GameInitial(HWND);//游戏初始化
	void DrawWindow(HWND);//绘制窗口
	void GetMousePoint(HWND, UINT, WPARAM, LPARAM);//获取鼠标位置
	bool Conversion();//坐标换算
	void LayMines();//布雷
	void SetDrawBox(int, int);//设置格子状态
	void SetDrawBox();//游戏失败时设置未知格子状态
	void GetShowTime();//获取并显示游戏进行时间
	void SetAddMine();//设置剩余雷数加一
	void SetRemoveMine();//设置剩余雷数减一
	char* GetMine();//获取当前剩余雷数
	char* GetTime();//游戏结束时获取游戏时间
	bool GameSuccess();//判断游戏是否成功结束
	void GameDestroy();
};
#endif
