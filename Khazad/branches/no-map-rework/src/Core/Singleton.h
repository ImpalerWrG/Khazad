#ifndef SINGLETON__HEADER
#define SINGLETON__HEADER


// Include in the .cpp file
#define DECLARE_SINGLETON(CLASS) CLASS* CLASS::instance = NULL;

// Include in the .h file
#define DECLARE_SINGLETON_CLASS(CLASS)		                                    \
                                                                                \
	public: 								                                    \
                                                                                \
    static CLASS *GetInstance() { return CLASS::instance; };                    \
                                                                                \
	static void SetInstance(CLASS * c) {CLASS::instance = c;};                  \
                                                                                \
	static void CreateInstance()                                                \
	{		                                                                    \
        if (!CLASS::instance)                                                   \
        {				                                                        \
            CLASS::instance = new CLASS;                                        \
        }		                                                                \
	};									                                        \
                                                                                \
    static void FreeInstance()                                                  \
    {                                                                           \
        delete CLASS::instance;				                                    \
        CLASS::instance = NULL;				                                    \
	};									                                        \
                                                                                \
	private:								                                    \
                                                                                \
	static CLASS* instance;	                                    				\
                                                                                \
	protected:						                                    		\
                                                                                \
	CLASS();								                                    \

#endif // SINGLETON__HEADER
