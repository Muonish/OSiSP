

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Sun Dec 21 16:39:37 2014
 */
/* Compiler settings for lr4_v3.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __lr4_v3_h_h__
#define __lr4_v3_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __Ilr4_v3_FWD_DEFINED__
#define __Ilr4_v3_FWD_DEFINED__
typedef interface Ilr4_v3 Ilr4_v3;

#endif 	/* __Ilr4_v3_FWD_DEFINED__ */


#ifndef __lr4_v3_FWD_DEFINED__
#define __lr4_v3_FWD_DEFINED__

#ifdef __cplusplus
typedef class lr4_v3 lr4_v3;
#else
typedef struct lr4_v3 lr4_v3;
#endif /* __cplusplus */

#endif 	/* __lr4_v3_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __lr4_v3_LIBRARY_DEFINED__
#define __lr4_v3_LIBRARY_DEFINED__

/* library lr4_v3 */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_lr4_v3;

#ifndef __Ilr4_v3_DISPINTERFACE_DEFINED__
#define __Ilr4_v3_DISPINTERFACE_DEFINED__

/* dispinterface Ilr4_v3 */
/* [uuid] */ 


EXTERN_C const IID DIID_Ilr4_v3;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A7814054-F1DD-4C24-8ABE-8E1EA5A43223")
    Ilr4_v3 : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct Ilr4_v3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Ilr4_v3 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Ilr4_v3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Ilr4_v3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Ilr4_v3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Ilr4_v3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Ilr4_v3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Ilr4_v3 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } Ilr4_v3Vtbl;

    interface Ilr4_v3
    {
        CONST_VTBL struct Ilr4_v3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Ilr4_v3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Ilr4_v3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Ilr4_v3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Ilr4_v3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Ilr4_v3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Ilr4_v3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Ilr4_v3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __Ilr4_v3_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_lr4_v3;

#ifdef __cplusplus

class DECLSPEC_UUID("05DADF5F-6361-49DD-9BAA-8CF0E688299D")
lr4_v3;
#endif
#endif /* __lr4_v3_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


