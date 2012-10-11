#define FUNCTION_MYFUNC "myfunc"

#ifdef _DEBUG
#define DEBUGBOX(msg) MessageBox(NULL, msg, TEXT("debug"), MB_OK)
#else
#define DEBUGBOX(msg)
#endif
