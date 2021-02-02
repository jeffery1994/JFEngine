///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JFRENDER_SYSTEM;
class JFINPUT_SYSTEM;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JFENGINE
{
public:
	JFENGINE(){}
	~JFENGINE(){}
	bool Init();
	void Deinit();
	void Update(float DeltaTime);
	static JFENGINE* GetInstance()
	{
		static JFENGINE Instance;
		return &Instance;
	}
	bool ShouldExit();

	JFRENDER_SYSTEM* GetRenderSystem();
	JFINPUT_SYSTEM* GetInputSystem();
	bool GetInited() { return Inited;  }
private:
	JFRENDER_SYSTEM* RenderSystem;
	JFINPUT_SYSTEM* InputSystem;
	bool Inited = false;
};