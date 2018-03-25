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

	BoxState **boxState;//��������
	int row;//��ǰ����������
	int column;//��ǰ����������
	POINTS point;//��ǰ�������
	MouseState mouseState;//���״̬

	bool gameStart;//�Ƿ�ʼ����
	bool gameOver;//��Ϸ�Ƿ�ʧ��
	bool gameSuccess;//��Ϸ�Ƿ�ɹ�

	int timer;//��ʱ
	int mine;//����

	
	HPEN hBlackPen;//��ɫ����
	HBRUSH hBlueBrush;//��ɫ��ˢ
	HBRUSH hRedBrush;//��ɫ��ˢ
	HFONT hCountFont;//����
	HWND hStaticTime;//����ʱ�ı���
	HWND hStaticMine;//ʣ�������ı���

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
	void SetMouseState(MouseState);//������갴��״̬
	void GameInitial(HWND);//��Ϸ��ʼ��
	void DrawWindow(HWND);//���ƴ���
	void GetMousePoint(HWND, UINT, WPARAM, LPARAM);//��ȡ���λ��
	bool Conversion();//���껻��
	void LayMines();//����
	void SetDrawBox(int, int);//���ø���״̬
	void SetDrawBox();//��Ϸʧ��ʱ����δ֪����״̬
	void GetShowTime();//��ȡ����ʾ��Ϸ����ʱ��
	void SetAddMine();//����ʣ��������һ
	void SetRemoveMine();//����ʣ��������һ
	char* GetMine();//��ȡ��ǰʣ������
	char* GetTime();//��Ϸ����ʱ��ȡ��Ϸʱ��
	bool GameSuccess();//�ж���Ϸ�Ƿ�ɹ�����
	void GameDestroy();
};
#endif
