#ifndef SINGLETON__HEADER
#define SINGLETON__HEADER


// Include in the .cpp file
#define DECLARE_SINGLETON(CLASS) CLASS* CLASS::instance = NULL;  bool CLASS::instanced = false;

// Include in the .h file
#define DECLARE_SINGLETON_CLASS(CLASS)		                                    \
                                                                                \
    public:                                                                     \
                                                                                \
    static CLASS* GetInstance()                                                 \
    {                                                                           \
        CreateInstance();                                                       \
        return CLASS::instance;                                                 \
    };                                                                          \
                                                                                \
    static void SetInstance(CLASS* c)                                           \
    {                                                                           \
        if (CLASS::instance)                                                    \
        {                                                                       \
            FreeInstance();                                                     \
        }                                                                       \
        CLASS::instance = c;                                                    \
    };                                                                          \
                                                                                \
    static void CreateInstance()                                                \
    {		                                                                    \
        if (!CLASS::instance)                                                   \
        {				                                                        \
            CLASS::instance = new CLASS;                                        \
            CLASS::instanced = true;                                            \
        }		                                                                \
    };									                                        \
                                                                                \
    static void FreeInstance()                                                  \
    {                                                                           \
        delete CLASS::instance;				                                    \
        CLASS::instance = NULL;                                                 \
        CLASS::instanced = false;  				                                \
    };									                                        \
                                                                                \
    static bool isInstance()                                                    \
    {                                                                           \
        return CLASS::instanced;                                                \
    };                                                                          \
                                                                                \
    private:								                                    \
                                                                                \
    static CLASS* instance;	                                    				\
    static bool instanced;                                                      \
                                                                                \
    protected:						                                    		\
                                                                                \
    CLASS();								                                    \

#endif // SINGLETON__HEADER
