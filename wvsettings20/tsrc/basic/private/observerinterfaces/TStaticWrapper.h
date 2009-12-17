/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     Wrap any function or method call to a class and ask CAsyncWrapper class
*		to call it after certain timeout
*
*
*
*/

#ifndef __TStaticWrapper_H__
#define __TStaticWrapper_H__

// INCLUDES
#include <e32base.h>
#include <e32svr.h>


//You can use these classes to call any method or function after certain time
//All of the method and function wrappers are derived from MStaticWrapper
//##ModelId=3EBA47ED027F
class MStaticWrapper
    {
    public:
        //##ModelId=3EBA47ED028A
        virtual void CallL() = 0;
    };

//Give the function or method wrapper to this class and the timeout
//##ModelId=3EBA47ED029A
class CAsyncCall : public CActive
    {
        //##ModelId=3EBA47ED029D
        MStaticWrapper* iWrapper;
        //##ModelId=3EBA47ED02AA
        TInt iLeaveCode;
        //##ModelId=3EBA47ED02AF
        RTimer iTimer;
    public:
        //##ModelId=3EBA47ED02B9
        static CAsyncCall* NewLC()
            {
            CAsyncCall* _that = new( ELeave )CAsyncCall;
            CleanupStack::PushL( _that );
            _that->ConstructL();
            return _that;
            }

        //##ModelId=3EBA47ED02BB
        static CAsyncCall* NewL()
            {
            CAsyncCall* _that = NewLC();
            CleanupStack::Pop();
            return _that;
            }
        //##ModelId=3EBA47ED02BD
        void Start( MStaticWrapper* aWrapper, TInt aTimeout )
            {
            if ( IsActive() )
                {
                User::Leave( KErrAlreadyExists );
                }
            iWrapper = aWrapper;
            iTimer.After( iStatus, aTimeout );
            SetActive();
            }
        //##ModelId=3EBA47ED02C9
        TInt LeaveCode() const
            {
            return iLeaveCode;
            }
        //##ModelId=3EBA47ED02CB
        void Stop()
            {
            iTimer.Cancel();
            Cancel();
            }
        //##ModelId=3EBA47ED02CC
        ~CAsyncCall()
            {
            Stop();
            iTimer.Cancel();
            iTimer.Close();
            }
    protected:
        //##ModelId=3EBA47ED02CD
        CAsyncCall()
                : CActive( CActive::EPriorityStandard )
            {
            CActiveScheduler::Add( this );
            }

        //##ModelId=3EBA47ED02CE
        void ConstructL()
            {
            User::LeaveIfError( iTimer.CreateLocal() );
            }

        //##ModelId=3EBA47ED02CF
        void RunL()
            {
            if ( iWrapper )
                {
                TRAP( iLeaveCode, iWrapper->CallL() );
                }
            }

        //##ModelId=3EBA47ED02D8
        void DoCancel()
            {
            }


        //##ModelId=3EBA47ED02D9
        TInt RunError( TInt /*aError*/ )
            {
            return KErrNone;
            }

    };


//This is same as the CAsyncWrapper class but the call will be made from another thread.
//Not tested
//##ModelId=3EBA47ED02E8
class CThreadCall : public CTimer
    {
        //##ModelId=3EBA47ED02F9
        MStaticWrapper* iWrapper;
    public:
        //##ModelId=3EBA47ED02FD
        CThreadCall()
                : CTimer( CActive::EPriorityStandard )
            {
            CActiveScheduler::Add( this );
            CTimer::ConstructL();
            }
        //##ModelId=3EBA47ED02FE
        ~CThreadCall()
            {
            }

        //##ModelId=3EBA47ED02FF
        void ThreadFunc()
            {
            iWrapper->CallL();
            }

        //##ModelId=3EBA47ED0307
        static TInt CallIt( TAny* aAny )
            {
            CTrapCleanup* tc = CTrapCleanup::New();
            CThreadCall* p = ( CThreadCall* )aAny;
            TRAPD( err, p->ThreadFunc() );
            delete tc;
            return KErrNone;
            }

        //##ModelId=3EBA47ED030A
        void StartCallingThread()
            {
            RThread t;
            t.Create( _L( "WrapperCompletingThread" ), CallIt, KDefaultStackSize, 0, 10000, this );
            t.Resume();
            t.Close();
            }


        //##ModelId=3EBA47ED030B
        void RunL()
            {
            if ( iWrapper )
                {
                StartCallingThread();
                }
            }
        //##ModelId=3EBA47ED030C
        void DoCancel()
            {
            }
        //##ModelId=3EBA47ED030D
        TInt RunError( TInt /*aError*/ )
            {
            return KErrNone;
            }
        //##ModelId=3EBA47ED0317
        void Start( MStaticWrapper* aWrapper, TInt aTimeout )
            {
            iWrapper = aWrapper;
            After( aTimeout );
            }
    };




/*
Use this class to "wrap" methods with return "normal" value ( "normal" return value IS NOT reference to some object or any value that can't be copied with assignment operator)
For example:
class CHomma
	{
	TInt DoIt( TInt aValue, const TDesC& aText );
	public:
	}

CHomma h;
_LIT( KText, "Text" );
TStaticWrapper<CHomma, TInt, TInt, const TDesC&> wrap( &h, &CHomma::DoIt, 10, KText );
iAsyncWrapper->Start( &wrap, 1000000 );
CAOWait::WaitLD( 1500000 );
wrap.ReturnValue();//returns the value that was returned by DoIt call

This example will call "h.DoIt( 10, KText)" after one second
*/
//##ModelId=3EBA47ED0346
template < class C, class R, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticWrapper : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47ED03D2
        typedef R ( C::*TFuncType1 )();
        //##ModelId=3EBA47ED03D4
        typedef R ( C::*TFuncType2 )( P1 aP1 );
        //##ModelId=3EBA47ED03E2
        typedef R ( C::*TFuncType3 )( P1 aP1, P2 aP2 );
        //##ModelId=3EBA47ED03E4
        typedef R ( C::*TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 );
        //##ModelId=3EBA47EE000B
        typedef R ( C::*TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );
        //##ModelId=3EBA47EE000D
        typedef R ( C::*TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 );

        //##ModelId=3EBA47ED035A
        TStaticWrapper( C* aTestClass, TFuncType1 aFunction )
                : iTestClass( aTestClass ), iF1( aFunction )
            {
            }
        //##ModelId=3EBA47ED035D
        TStaticWrapper( C* aTestClass, TFuncType2 aFunction, P1 aP1 )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47ED0361
        TStaticWrapper( C* aTestClass, TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47ED0366
        TStaticWrapper( C* aTestClass, TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47ED036C
        TStaticWrapper( C* aTestClass, TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47ED0375
        TStaticWrapper( C* aTestClass, TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47ED037D
        void CallL()
            {

            if ( iF1 )
                {
                iRet = ( iTestClass->*iF1 )();
                }
            else if ( iF2 )
                {
                iRet = ( iTestClass->*iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                iRet = ( iTestClass->*iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                iRet = ( iTestClass->*iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                iRet = ( iTestClass->*iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                iRet = ( iTestClass->*iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }

        //##ModelId=3EBA47ED037E
        R& ReturnValue()
            {
            return iRet;
            }

    private:

        //##ModelId=3EBA47ED037F
        C* iTestClass;
        //##ModelId=3EBA47ED0384
        P1 iP1;
        //##ModelId=3EBA47ED0385
        P2 iP2;
        //##ModelId=3EBA47ED0386
        P3 iP3;
        //##ModelId=3EBA47ED0387
        P4 iP4;
        //##ModelId=3EBA47ED0388
        P5 iP5;
        //##ModelId=3EBA47ED0394
        R iRet;
        //##ModelId=3EBA47ED0396
        TFuncType1 iF1;
        //##ModelId=3EBA47ED03A4
        TFuncType2 iF2;
        //##ModelId=3EBA47ED03A9
        TFuncType3 iF3;
        //##ModelId=3EBA47ED03B4
        TFuncType4 iF4;
        //##ModelId=3EBA47ED03B9
        TFuncType5 iF5;
        //##ModelId=3EBA47ED03C4
        TFuncType6 iF6;
    };






/*
Use this class to "wrap" methods without return value
For example:
class CHomma
	{
	void DoIt( TInt aValue, const TDesC& aText );
	public:
	}

CHomma h;
_LIT( KText, "Text" );
TStaticWrapperVoid<CHomma, TInt, const TDesC&> wrap( &h, &CHomma::DoIt, 10, KText );
iAsyncWrapper->Start( &wrap, 1000000 );
CAOWait::WaitLD( 1500000 );

This example will call "h.DoIt( 10, KText)" after one second
*/
//##ModelId=3EBA47EE0077
template < class C, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt, class P6 = TInt, class P7 = TInt, class P8 = TInt >
class TStaticWrapperVoid : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47EE0147
        typedef void ( C::*TFuncType1 )();
        //##ModelId=3EBA47EE0153
        typedef void ( C::*TFuncType2 )( P1 aP1 );
        //##ModelId=3EBA47EE0161
        typedef void ( C::*TFuncType3 )( P1 aP1, P2 aP2 );
        //##ModelId=3EBA47EE0163
        typedef void ( C::*TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 );
        //##ModelId=3EBA47EE0171
        typedef void ( C::*TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );
        //##ModelId=3EBA47EE0173
        typedef void ( C::*TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 );
        //##ModelId=3EBA47EE0182
        typedef void ( C::*TFuncType7 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5, P6 aP6 );
        //##ModelId=3EBA47EE0190
        typedef void ( C::*TFuncType8 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5, P6 aP6, P7 aP7 );
        //##ModelId=3EBA47EE0192
        typedef void ( C::*TFuncType9 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5, P6 aP6, P7 aP7, P8 aP8 );

        //##ModelId=3EBA47EE0092
        TStaticWrapperVoid( C* aTestClass, TFuncType1 aFunction )
                : iTestClass( aTestClass ), iF1( aFunction )
            {
            }
        //##ModelId=3EBA47EE0095
        TStaticWrapperVoid( C* aTestClass, TFuncType2 aFunction, P1 aP1 )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47EE0099
        TStaticWrapperVoid( C* aTestClass, TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47EE009E
        TStaticWrapperVoid( C* aTestClass, TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47EE00A4
        TStaticWrapperVoid( C* aTestClass, TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47EE00AC
        TStaticWrapperVoid( C* aTestClass, TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }
        //##ModelId=3EBA47EE00B5
        TStaticWrapperVoid( C* aTestClass, TFuncType7 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5, P6 aP6  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( NULL ), iF7( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 ), iP6( aP6 )
            {
            }
        //##ModelId=3EBA47EE00BE
        TStaticWrapperVoid( C* aTestClass, TFuncType8 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5, P6 aP6, P7 aP7  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( NULL ), iF7( NULL ), iF8( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 ), iP6( aP6 ), iP7( aP7 )
            {
            }
        //##ModelId=3EBA47EE00C9
        TStaticWrapperVoid( C* aTestClass, TFuncType9 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5, P6 aP6, P7 aP7, P8 aP8  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( NULL ), iF7( NULL ), iF8( NULL ), iF9( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 ), iP6( aP6 ), iP7( aP7 ), iP8( aP8 )
            {
            }

        //##ModelId=3EBA47EE00D5
        void CallL()
            {
            if ( iF1 )
                {
                ( iTestClass->*iF1 )();
                }
            else if ( iF2 )
                {
                ( iTestClass->*iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                ( iTestClass->*iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                ( iTestClass->*iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                ( iTestClass->*iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                ( iTestClass->*iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            else if ( iF7 )
                {
                ( iTestClass->*iF7 )( iP1, iP2, iP3, iP4, iP5, iP6 );
                }
            else if ( iF8 )
                {
                ( iTestClass->*iF8 )( iP1, iP2, iP3, iP4, iP5, iP6, iP7 );
                }
            else if ( iF9 )
                {
                ( iTestClass->*iF9 )( iP1, iP2, iP3, iP4, iP5, iP6, iP7, iP8 );
                }
            }
    private:

        //##ModelId=3EBA47EE00D6
        C* iTestClass;
        //##ModelId=3EBA47EE00D7
        P1 iP1;
        //##ModelId=3EBA47EE00D8
        P2 iP2;
        //##ModelId=3EBA47EE00D9
        P3 iP3;
        //##ModelId=3EBA47EE00DA
        P4 iP4;
        //##ModelId=3EBA47EE00E4
        P5 iP5;
        //##ModelId=3EBA47EE00E5
        P6 iP6;
        //##ModelId=3EBA47EE00E6
        P7 iP7;
        //##ModelId=3EBA47EE00E7
        P8 iP8;
        //##ModelId=3EBA47EE00F5
        TFuncType1 iF1;
        //##ModelId=3EBA47EE0105
        TFuncType2 iF2;
        //##ModelId=3EBA47EE010A
        TFuncType3 iF3;
        //##ModelId=3EBA47EE0114
        TFuncType4 iF4;
        //##ModelId=3EBA47EE0119
        TFuncType5 iF5;
        //##ModelId=3EBA47EE0124
        TFuncType6 iF6;
        //##ModelId=3EBA47EE0129
        TFuncType7 iF7;
        //##ModelId=3EBA47EE0133
        TFuncType8 iF8;
        //##ModelId=3EBA47EE0143
        TFuncType9 iF9;
    };



/*
Use this class to "wrap" methods that return a reference or any value that can't be copied with assignment operator
For example:
class CHomma
	{
	const TDesC& DoIt( TInt aValue, const TDesC& aText );
	public:
	}

CHomma h;
_LIT( KText, "Text" );
TStaticWrapper<CHomma, const TDesC&, const TDesC, TInt, const TDesC&> wrap( &h, &CHomma::DoIt, 10, KText );
iAsyncWrapper->Start( &wrap, 1000000 );
CAOWait::WaitLD( 1500000 );
wrap.ReturnValue();//returns the value that was returned by DoIt call, returns const TDesC*

This example will call "h.DoIt( 10, KText)" after one second
*/
//##ModelId=3EBA47EE01FE
template < class C, class R, class RET, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticWrapperRef : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47EE028A
        typedef R ( C::*TFuncType1 )();
        //##ModelId=3EBA47EE028C
        typedef R ( C::*TFuncType2 )( P1 aP1 );
        //##ModelId=3EBA47EE029B
        typedef R ( C::*TFuncType3 )( P1 aP1, P2 aP2 );
        //##ModelId=3EBA47EE02A9
        typedef R ( C::*TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 );
        //##ModelId=3EBA47EE02B9
        typedef R ( C::*TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );
        //##ModelId=3EBA47EE02BB
        typedef R ( C::*TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 );

        //##ModelId=3EBA47EE0215
        TStaticWrapperRef( C* aTestClass, TFuncType1 aFunction )
                : iTestClass( aTestClass ), iF1( aFunction )
            {
            }
        //##ModelId=3EBA47EE0218
        TStaticWrapperRef( C* aTestClass, TFuncType2 aFunction, P1 aP1 )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47EE021C
        TStaticWrapperRef( C* aTestClass, TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47EE0221
        TStaticWrapperRef( C* aTestClass, TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47EE0227
        TStaticWrapperRef( C* aTestClass, TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47EE022E
        TStaticWrapperRef( C* aTestClass, TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47EE0236
        void CallL()
            {

            if ( iF1 )
                {
                iRet = &( iTestClass->*iF1 )();
                }
            else if ( iF2 )
                {
                iRet = &( iTestClass->*iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                iRet = &( iTestClass->*iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                iRet = &( iTestClass->*iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                iRet = &( iTestClass->*iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                iRet = &( iTestClass->*iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }

        //##ModelId=3EBA47EE0237
        RET* ReturnValue()
            {
            return iRet;
            }

    private:

        //##ModelId=3EBA47EE023C
        C* iTestClass;
        //##ModelId=3EBA47EE023D
        P1 iP1;
        //##ModelId=3EBA47EE023E
        P2 iP2;
        //##ModelId=3EBA47EE023F
        P3 iP3;
        //##ModelId=3EBA47EE0240
        P4 iP4;
        //##ModelId=3EBA47EE024C
        P5 iP5;
        //##ModelId=3EBA47EE024E
        TFuncType1 iF1;
        //##ModelId=3EBA47EE0253
        TFuncType2 iF2;
        //##ModelId=3EBA47EE025E
        TFuncType3 iF3;
        //##ModelId=3EBA47EE026C
        TFuncType4 iF4;
        //##ModelId=3EBA47EE0271
        TFuncType5 iF5;
        //##ModelId=3EBA47EE027C
        TFuncType6 iF6;
        //##ModelId=3EBA47EE0280
        RET* iRet;
    };









//const versions


//##ModelId=3EBA47EE0336
template < class C, class R, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticWrapperConst : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47EE03C3
        typedef R ( C::*TFuncType1 )() const;
        //##ModelId=3EBA47EE03C5
        typedef R ( C::*TFuncType2 )( P1 aP1 ) const;
        //##ModelId=3EBA47EE03D3
        typedef R ( C::*TFuncType3 )( P1 aP1, P2 aP2 ) const;
        //##ModelId=3EBA47EE03E2
        typedef R ( C::*TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 ) const;
        //##ModelId=3EBA47EF000A
        typedef R ( C::*TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 ) const;
        //##ModelId=3EBA47EF000C
        typedef R ( C::*TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 ) const;

        //##ModelId=3EBA47EE034D
        TStaticWrapperConst( C* aTestClass, TFuncType1 aFunction )
                : iTestClass( aTestClass ), iF1( aFunction )
            {
            }
        //##ModelId=3EBA47EE0350
        TStaticWrapperConst( C* aTestClass, TFuncType2 aFunction, P1 aP1 )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47EE0354
        TStaticWrapperConst( C* aTestClass, TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47EE0359
        TStaticWrapperConst( C* aTestClass, TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47EE035F
        TStaticWrapperConst( C* aTestClass, TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47EE0367
        TStaticWrapperConst( C* aTestClass, TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47EE036F
        void CallL()
            {

            if ( iF1 )
                {
                iRet = ( iTestClass->*iF1 )();
                }
            else if ( iF2 )
                {
                iRet = ( iTestClass->*iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                iRet = ( iTestClass->*iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                iRet = ( iTestClass->*iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                iRet = ( iTestClass->*iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                iRet = ( iTestClass->*iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }

        //##ModelId=3EBA47EE0370
        R& ReturnValue()
            {
            return iRet;
            }

    private:

        //##ModelId=3EBA47EE0375
        C* iTestClass;
        //##ModelId=3EBA47EE0376
        P1 iP1;
        //##ModelId=3EBA47EE0377
        P2 iP2;
        //##ModelId=3EBA47EE0378
        P3 iP3;
        //##ModelId=3EBA47EE0379
        P4 iP4;
        //##ModelId=3EBA47EE0384
        P5 iP5;
        //##ModelId=3EBA47EE0385
        R iRet;
        //##ModelId=3EBA47EE0387
        TFuncType1 iF1;
        //##ModelId=3EBA47EE0395
        TFuncType2 iF2;
        //##ModelId=3EBA47EE039A
        TFuncType3 iF3;
        //##ModelId=3EBA47EE03A4
        TFuncType4 iF4;
        //##ModelId=3EBA47EE03A9
        TFuncType5 iF5;
        //##ModelId=3EBA47EE03B4
        TFuncType6 iF6;
    };






//##ModelId=3EBA47EF0096
template < class C, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticWrapperVoidConst : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47EF0113
        typedef void ( C::*TFuncType1 )() const;
        //##ModelId=3EBA47EF0123
        typedef void ( C::*TFuncType2 )( P1 aP1 ) const;
        //##ModelId=3EBA47EF0125
        typedef void ( C::*TFuncType3 )( P1 aP1, P2 aP2 ) const;
        //##ModelId=3EBA47EF0133
        typedef void ( C::*TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 ) const;
        //##ModelId=3EBA47EF0143
        typedef void ( C::*TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 ) const;
        //##ModelId=3EBA47EF0152
        typedef void ( C::*TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 ) const;

        //##ModelId=3EBA47EF009F
        TStaticWrapperVoidConst( C* aTestClass, TFuncType1 aFunction )
                : iTestClass( aTestClass ), iF1( aFunction )
            {
            }
        //##ModelId=3EBA47EF00A7
        TStaticWrapperVoidConst( C* aTestClass, TFuncType2 aFunction, P1 aP1 )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47EF00AB
        TStaticWrapperVoidConst( C* aTestClass, TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47EF00B0
        TStaticWrapperVoidConst( C* aTestClass, TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47EF00B8
        TStaticWrapperVoidConst( C* aTestClass, TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47EF00BF
        TStaticWrapperVoidConst( C* aTestClass, TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47EF00C8
        void CallL()
            {

            if ( iF1 )
                {
                ( iTestClass->*iF1 )();
                }
            else if ( iF2 )
                {
                ( iTestClass->*iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                ( iTestClass->*iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                ( iTestClass->*iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                ( iTestClass->*iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                ( iTestClass->*iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }
    private:

        //##ModelId=3EBA47EF00C9
        C* iTestClass;
        //##ModelId=3EBA47EF00CA
        P1 iP1;
        //##ModelId=3EBA47EF00CB
        P2 iP2;
        //##ModelId=3EBA47EF00CC
        P3 iP3;
        //##ModelId=3EBA47EF00D5
        P4 iP4;
        //##ModelId=3EBA47EF00D6
        P5 iP5;
        //##ModelId=3EBA47EF00D8
        TFuncType1 iF1;
        //##ModelId=3EBA47EF00E5
        TFuncType2 iF2;
        //##ModelId=3EBA47EF00EA
        TFuncType3 iF3;
        //##ModelId=3EBA47EF00F5
        TFuncType4 iF4;
        //##ModelId=3EBA47EF00FA
        TFuncType5 iF5;
        //##ModelId=3EBA47EF0105
        TFuncType6 iF6;
    };



//##ModelId=3EBA47EF01EE
template < class C, class R, class RET, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticWrapperRefConst : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47EF027B
        typedef R ( C::*TFuncType1 )() const;
        //##ModelId=3EBA47EF028A
        typedef R ( C::*TFuncType2 )( P1 aP1 ) const;
        //##ModelId=3EBA47EF028C
        typedef R ( C::*TFuncType3 )( P1 aP1, P2 aP2 ) const;
        //##ModelId=3EBA47EF029B
        typedef R ( C::*TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 ) const;
        //##ModelId=3EBA47EF02AA
        typedef R ( C::*TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 ) const;
        //##ModelId=3EBA47EF02B9
        typedef R ( C::*TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 ) const;

        //##ModelId=3EBA47EF0206
        TStaticWrapperRefConst( C* aTestClass, TFuncType1 aFunction )
                : iTestClass( aTestClass ), iF1( aFunction )
            {
            }
        //##ModelId=3EBA47EF0209
        TStaticWrapperRefConst( C* aTestClass, TFuncType2 aFunction, P1 aP1 )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47EF020D
        TStaticWrapperRefConst( C* aTestClass, TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47EF0212
        TStaticWrapperRefConst( C* aTestClass, TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47EF0218
        TStaticWrapperRefConst( C* aTestClass, TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47EF021F
        TStaticWrapperRefConst( C* aTestClass, TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iTestClass( aTestClass ), iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47EF0227
        void CallL()
            {

            if ( iF1 )
                {
                iRet = &( iTestClass->*iF1 )();
                }
            else if ( iF2 )
                {
                iRet = &( iTestClass->*iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                iRet = &( iTestClass->*iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                iRet = &( iTestClass->*iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                iRet = &( iTestClass->*iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                iRet = &( iTestClass->*iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }

        //##ModelId=3EBA47EF0228
        RET* ReturnValue()
            {
            return iRet;
            }

    private:

        //##ModelId=3EBA47EF0229
        C* iTestClass;
        //##ModelId=3EBA47EF022C
        P1 iP1;
        //##ModelId=3EBA47EF022D
        P2 iP2;
        //##ModelId=3EBA47EF022E
        P3 iP3;
        //##ModelId=3EBA47EF022F
        P4 iP4;
        //##ModelId=3EBA47EF0230
        P5 iP5;
        //##ModelId=3EBA47EF023D
        TFuncType1 iF1;
        //##ModelId=3EBA47EF0242
        TFuncType2 iF2;
        //##ModelId=3EBA47EF024D
        TFuncType3 iF3;
        //##ModelId=3EBA47EF0252
        TFuncType4 iF4;
        //##ModelId=3EBA47EF025C
        TFuncType5 iF5;
        //##ModelId=3EBA47EF0261
        TFuncType6 iF6;
        //##ModelId=3EBA47EF026E
        RET* iRet;
    };








//function wrappers







//##ModelId=3EBA47EF0365
template < class R, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticFunctionWrapper : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47EF03E2
        typedef R ( *TFuncType1 )();
        //##ModelId=3EBA47F0000A
        typedef R ( *TFuncType2 )( P1 aP1 );
        //##ModelId=3EBA47F0000C
        typedef R ( *TFuncType3 )( P1 aP1, P2 aP2 );
        //##ModelId=3EBA47F0001A
        typedef R ( *TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 );
        //##ModelId=3EBA47F0002A
        typedef R ( *TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );
        //##ModelId=3EBA47F00039
        typedef R ( *TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 );

        //##ModelId=3EBA47EF036E
        TStaticFunctionWrapper( TFuncType1 aFunction )
                : iF1( aFunction )
            {
            }
        //##ModelId=3EBA47EF0375
        TStaticFunctionWrapper( TFuncType2 aFunction, P1 aP1 )
                : iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47EF0378
        TStaticFunctionWrapper( TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47EF037C
        TStaticFunctionWrapper( TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47EF0381
        TStaticFunctionWrapper( TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47EF0389
        TStaticFunctionWrapper( TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47EF0390
        void CallL()
            {

            if ( iF1 )
                {
                iRet = ( iF1 )();
                }
            else if ( iF2 )
                {
                iRet = ( iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                iRet = ( iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                iRet = ( iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                iRet = ( iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                iRet = ( iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }

        //##ModelId=3EBA47EF0391
        R& ReturnValue()
            {
            return iRet;
            }

    private:

        //##ModelId=3EBA47EF0394
        P1 iP1;
        //##ModelId=3EBA47EF0395
        P2 iP2;
        //##ModelId=3EBA47EF0396
        P3 iP3;
        //##ModelId=3EBA47EF0397
        P4 iP4;
        //##ModelId=3EBA47EF0398
        P5 iP5;
        //##ModelId=3EBA47EF03A3
        R iRet;
        //##ModelId=3EBA47EF03A5
        TFuncType1 iF1;
        //##ModelId=3EBA47EF03AA
        TFuncType2 iF2;
        //##ModelId=3EBA47EF03B4
        TFuncType3 iF3;
        //##ModelId=3EBA47EF03B9
        TFuncType4 iF4;
        //##ModelId=3EBA47EF03C6
        TFuncType5 iF5;
        //##ModelId=3EBA47EF03D3
        TFuncType6 iF6;
    };






//##ModelId=3EBA47F000E4
template < class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticFunctionWrapperVoid : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47F00152
        typedef void ( *TFuncType1 )();
        //##ModelId=3EBA47F00162
        typedef void ( *TFuncType2 )( P1 aP1 );
        //##ModelId=3EBA47F00171
        typedef void ( *TFuncType3 )( P1 aP1, P2 aP2 );
        //##ModelId=3EBA47F00181
        typedef void ( *TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 );
        //##ModelId=3EBA47F00190
        typedef void ( *TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );
        //##ModelId=3EBA47F001A0
        typedef void ( *TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 );

        //##ModelId=3EBA47F000EC
        TStaticFunctionWrapperVoid( TFuncType1 aFunction )
                : iF1( aFunction )
            {
            }
        //##ModelId=3EBA47F000F5
        TStaticFunctionWrapperVoid( TFuncType2 aFunction, P1 aP1 )
                : iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47F000F8
        TStaticFunctionWrapperVoid( TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47F000FC
        TStaticFunctionWrapperVoid( TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47F00101
        TStaticFunctionWrapperVoid( TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47F00109
        TStaticFunctionWrapperVoid( TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47F00110
        void CallL()
            {

            if ( iF1 )
                {
                ( *iF1 )();
                }
            else if ( iF2 )
                {
                ( *iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                ( *iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                ( *iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                ( *iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                ( *iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }
    private:

        //##ModelId=3EBA47F00113
        P1 iP1;
        //##ModelId=3EBA47F00114
        P2 iP2;
        //##ModelId=3EBA47F00115
        P3 iP3;
        //##ModelId=3EBA47F00116
        P4 iP4;
        //##ModelId=3EBA47F00117
        P5 iP5;
        //##ModelId=3EBA47F00124
        TFuncType1 iF1;
        //##ModelId=3EBA47F00129
        TFuncType2 iF2;
        //##ModelId=3EBA47F00133
        TFuncType3 iF3;
        //##ModelId=3EBA47F00138
        TFuncType4 iF4;
        //##ModelId=3EBA47F00143
        TFuncType5 iF5;
        //##ModelId=3EBA47F00148
        TFuncType6 iF6;
    };



//##ModelId=3EBA47F0026B
template < class R, class RET, class P1 = TInt, class P2 = TInt, class P3 = TInt, class P4 = TInt, class P5 = TInt >
class TStaticFunctionWrapperRef : public MStaticWrapper
    {
    public:
        //##ModelId=3EBA47F002E8
        typedef R ( *TFuncType1 )();
        //##ModelId=3EBA47F002F8
        typedef R ( *TFuncType2 )( P1 aP1 );
        //##ModelId=3EBA47F00307
        typedef R ( *TFuncType3 )( P1 aP1, P2 aP2 );
        //##ModelId=3EBA47F00317
        typedef R ( *TFuncType4 )( P1 aP1, P2 aP2, P3 aP3 );
        //##ModelId=3EBA47F00326
        typedef R ( *TFuncType5 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );
        //##ModelId=3EBA47F00336
        typedef R ( *TFuncType6 )( P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5 );

        //##ModelId=3EBA47F00275
        TStaticFunctionWrapperRef( TFuncType1 aFunction )
                : iF1( aFunction )
            {
            }
        //##ModelId=3EBA47F0027C
        TStaticFunctionWrapperRef( TFuncType2 aFunction, P1 aP1 )
                : iF1( NULL ), iF2( aFunction ), iP1( aP1 )
            {
            }
        //##ModelId=3EBA47F0027F
        TStaticFunctionWrapperRef( TFuncType3 aFunction, P1 aP1 , P2 aP2  )
                : iF1( NULL ), iF2( NULL ), iF3( aFunction ), iP1( aP1 ), iP2( aP2 )
            {
            }
        //##ModelId=3EBA47F00283
        TStaticFunctionWrapperRef( TFuncType4 aFunction, P1 aP1 , P2 aP2, P3 aP3  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {
            }
        //##ModelId=3EBA47F0028A
        TStaticFunctionWrapperRef( TFuncType5 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {
            }
        //##ModelId=3EBA47F00290
        TStaticFunctionWrapperRef( TFuncType6 aFunction, P1 aP1 , P2 aP2, P3 aP3, P4 aP4, P5 aP5  )
                : iF1( NULL ), iF2( NULL ), iF3( NULL ), iF4( NULL ), iF5( NULL ), iF6( aFunction ), iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 ), iP5( aP5 )
            {
            }

        //##ModelId=3EBA47F00297
        void CallL()
            {

            if ( iF1 )
                {
                iRet = &( *iF1 )();
                }
            else if ( iF2 )
                {
                iRet = &( *iF2 )( iP1 );
                }
            else if ( iF3 )
                {
                iRet = &( *iF3 )( iP1, iP2 );
                }
            else if ( iF4 )
                {
                iRet = &( *iF4 )( iP1, iP2, iP3 );
                }
            else if ( iF5 )
                {
                iRet = &( *iF5 )( iP1, iP2, iP3, iP4 );
                }
            else if ( iF6 )
                {
                iRet = &( *iF6 )( iP1, iP2, iP3, iP4, iP5 );
                }
            }

        //##ModelId=3EBA47F0029A
        RET* ReturnValue()
            {
            return iRet;
            }

    private:

        //##ModelId=3EBA47F0029B
        P1 iP1;
        //##ModelId=3EBA47F0029C
        P2 iP2;
        //##ModelId=3EBA47F0029D
        P3 iP3;
        //##ModelId=3EBA47F0029E
        P4 iP4;
        //##ModelId=3EBA47F0029F
        P5 iP5;
        //##ModelId=3EBA47F002AA
        TFuncType1 iF1;
        //##ModelId=3EBA47F002AF
        TFuncType2 iF2;
        //##ModelId=3EBA47F002BA
        TFuncType3 iF3;
        //##ModelId=3EBA47F002BF
        TFuncType4 iF4;
        //##ModelId=3EBA47F002CA
        TFuncType5 iF5;
        //##ModelId=3EBA47F002CF
        TFuncType6 iF6;
        //##ModelId=3EBA47F002D8
        RET* iRet;
    };



/*


class CAsyncCall : public CTimer
	{
	RArray<TInt32> iParams;
	TAny* iFunction;
	TAny* iObject;
	public:
		CAsyncCall()
			: CTimer( CActive::EPriorityStandard )
			{
			CActiveScheduler::Add( this );
			CTimer::ConstructL();
			}
		~CAsyncCall()
			{
			}
		void RunL()
			{
			TInt32* p = &iParams[0];
			TInt counter = iParams.Count() ;
			TAny* func = iFunction;
			TAny* object = iObject;
				__asm
					{
					mov eax, p;
					mov ecx, counter;
					dec ecx;

			juttu:
					mov ebx, [eax+ecx*4];
					push ebx;
					dec ecx;
					jns juttu;

					mov ecx, dword ptr [object]
					call func;

					mov eax, counter;
					shl eax, 2;
					add esp,eax;
					};
			}
		void DoCancel()
			{
			}
		TInt RunError( TInt aError )
			{
			return KErrNone;
			}
		TRequestStatus* Status()
			{
			return &iStatus;
			}
		void Call( TAny* aObject, TAny* aFunction, TInt aTimeout, TInt aParamCount, ... )
			{
			VA_LIST list;
			VA_START(list, aParamCount );
			iFunction = aFunction;
			iObject = aObject;
			for( TInt i( 0 ); i < aParamCount; i++ )
				{
				TInt32 p = VA_ARG( list, TInt32 );
				iParams.Append( p );
				}
			VA_END( list );
			After( aTimeout );
			}
	};
  */


#endif // __TStaticWrapper_H__
// End of File









