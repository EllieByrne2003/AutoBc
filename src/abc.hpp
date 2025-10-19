#pragma once

#if defined(ABC_NAMESPACE)
namespace ABC_NAMESPACE
{
#elif defined(__cplusplus)
extern "C"
{
#endif

// procedures to start and stop the ABC framework
// (should be called before and after the ABC procedures are called)
void   Abc_Start();
void   Abc_Stop();

// procedures to get the ABC framework and execute commands in it
typedef struct Abc_Frame_t_ Abc_Frame_t;
typedef struct Abc_Ntk_t_ Abc_Ntk_t;
typedef struct st__table_ st__table;
typedef struct Vec_Ptr_t_ Vec_Ptr_t;
typedef struct Vec_Int_t_ Vec_Int_t;
typedef struct Gia_Man_t_ Gia_Man_t;
typedef struct Abc_Cex_t_ Abc_Cex_t;
typedef struct Abc_Nam_t_ Abc_Nam_t;
typedef struct Vec_Wec_t_ Vec_Wec_t;
typedef struct Hsh_VecMan_t_ Hsh_VecMan_t;

typedef void (*Abc_Frame_Callback_BmcFrameDone_Func)(int frame, int po, int status);


Abc_Frame_t * Abc_FrameGetGlobalFrame();
int    Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );

// Creating and destroying frames
Abc_Frame_t * Abc_FrameAllocate();
void Abc_FrameDeallocate( Abc_Frame_t * p );
void Abc_FrameInit( Abc_Frame_t * pAbc );
void Abc_FrameEnd( Abc_Frame_t * pAbc );

// Duplicating networks
Abc_Ntk_t * Abc_NtkDup( Abc_Ntk_t * pNtk );
Abc_Ntk_t * Abc_FrameReadNtk( Abc_Frame_t * p );
void Abc_FrameSetCurrentNetwork( Abc_Frame_t * p, Abc_Ntk_t * pNtkNew );
void Abc_FrameDeleteAllNetworks( Abc_Frame_t * p );

// Verifying equivalence
int    Abc_ApiCec( Abc_Frame_t * pAbc, int argc, char ** argv );

// Getting stats on the ntk
int Abc_NtkNodeNum( Abc_Ntk_t * pNtk );
int Abc_NtkLevel( Abc_Ntk_t * pNtk );


#if defined(ABC_NAMESPACE)
}
using namespace ABC_NAMESPACE;
#elif defined(__cplusplus)
}
#endif
