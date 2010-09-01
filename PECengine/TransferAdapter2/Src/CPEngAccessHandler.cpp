/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Access handler of the network session
*
*/

// INCLUDE FILES
#include    "CPEngAccessHandler.h"
#include    <CIMPSSAPSettings.h>
#include    <ImpsServices.h>

#include    "PEngLoginDataCodec.h"
#include    "MPEngSessionStatusObserver.h"
#include    "MPEngSessionManager.h"
#include    "PEngTransAdapterPanics.h"

#include    "PEngWVPresenceErrors2.h"
#include    "PresenceDebugPrint.h"


//DATA TYPES
/**
 * Function map structure.
 */
struct SCspFunctionMap
    {
    TInt           iPeFunc;
    TImpsFunctions iImpsEngFunc;

#ifdef PENG_ENABLE_DEBUG_PRINT
    const TText*   iPeFuncName;
#endif
    };


/**
 * Sub-Function map structure.
 */
struct SCspSubFunctionMap
    {
    TInt           iPeSubFunc;
    TInt           iImpsEngSubFunc;

#ifdef PENG_ENABLE_DEBUG_PRINT
    const TText*   iPeSubFuncName;
#endif
    };

//CONTANTS
const TInt KPEngNullFunction = 0;


//MACROS
#ifdef PENG_ENABLE_DEBUG_PRINT
#define MAPPING( a1, a2 ) { a1, a2, _S( #a1 ) }
#define MAPPING_TERMINATION { KPEngNullFunction }
#else
#define MAPPING( a1, a2 ) { a1, a2 }
#define MAPPING_TERMINATION { KPEngNullFunction }
#endif





/**
 * Fundamental functions & sub-functions.
 */
const SCspFunctionMap KFuncMapFundamental[] =
    {
    MAPPING( KPEngFFServiceFunction, EServiceFunc ),
    MAPPING( KPEngFFSearchFunction,  ESearchFunc  ),
    MAPPING( KPEngFFInviteFunction,  EInviteFunc  ),
    MAPPING_TERMINATION
    };

const SCspSubFunctionMap KSubFuncMapFundamental[] =
    {
    MAPPING( KPEngFFSubFuncGETSPI,   KImpsServiceGETSPI ),
    MAPPING( KPEngFFSubFuncSRCH,     KImpsServiceSRCH   ),
    MAPPING( KPEngFFSubFuncSTSRC,    KImpsServiceSTSRC  ),
    MAPPING( KPEngFFSubFuncINVIT,    KImpsServiceINVIT  ),
    MAPPING( KPEngFFSubFuncCAINV,    KImpsServiceCAINV  ),
    MAPPING_TERMINATION
    };

/**
 * Presence functions & sub-functions.
 */
const SCspFunctionMap KFuncMapPresence[] =
    {
    MAPPING( KPEngWVContListFunction,  EContListFunc        ),
    MAPPING( KPEngWVPresAuthFunction,  EPresenceAuthFunc    ),
    MAPPING( KPEngWVPresDelivFunction, EPresenceDeliverFunc ),
    MAPPING( KPEngWVAttrListFunction,  EAttrListFunc        ),
    MAPPING_TERMINATION
    };


const SCspSubFunctionMap KSubFuncMapPresence[] =
    {
    MAPPING( KPEngWVSubFuncGCLI,   KImpsServiceGCLI  ),
    MAPPING( KPEngWVSubFuncCCLI,   KImpsServiceCCLI  ),
    MAPPING( KPEngWVSubFuncDCLI,   KImpsServiceDCLI  ),
    MAPPING( KPEngWVSubFuncMCLS,   KImpsServiceMCLS  ),
    MAPPING( KPEngWVSubFuncGETWL,  KImpsServiceGETWL ),
    MAPPING( KPEngWVSubFuncREACT,  KImpsServiceREACT ),
    MAPPING( KPEngWVSubFuncCAAUT,  KImpsServiceCAAUT ),
    //{ KPEngWVSubFuncGETAUT, Not yet in WV Engine },
    MAPPING( KPEngWVSubFuncGETPR,  KImpsServiceGETPR ),
    MAPPING( KPEngWVSubFuncUPDPR,  KImpsServiceUPDPR ),
    MAPPING( KPEngWVSubFuncCALI,   KImpsServiceCALI  ),
    MAPPING( KPEngWVSubFuncDALI,   KImpsServiceDALI  ),
    MAPPING( KPEngWVSubFuncGALS,   KImpsServiceGALS  ),
    MAPPING_TERMINATION
    };

/**
 * IM Functions & sub-functions.
 */
const SCspFunctionMap KFuncMapIm[] =
    {
    MAPPING( KPEngIMSendFunctions,     EIMSendFunc    ),
    MAPPING( KPEngIMReceiveFunctions,  EIMReceiveFunc ),
    MAPPING( KPEngIMAuthorFunctions,   EIMAuthFunc    ),
    MAPPING_TERMINATION
    };

const SCspSubFunctionMap KSubFuncMapIm[] =
    {
    // Send Functions
    MAPPING( KPEngIMSubFuncMDELIV, KImpsServiceMDELIV ),
    MAPPING( KPEngIMSubFuncFWMSG,  KImpsServiceFWMSG  ),
    // Receive functions
    MAPPING( KPEngIMSubFuncSETD,   KImpsServiceSETD   ),
    MAPPING( KPEngIMSubFuncGETLM,  KImpsServiceGETLM  ),
    MAPPING( KPEngIMSubFuncGETM,   KImpsServiceGETM   ),
    MAPPING( KPEngIMSubFuncREJCM,  KImpsServiceREJCM  ),
    MAPPING( KPEngIMSubFuncNOTIF,  KImpsServiceNOTIF  ),
    MAPPING( KPEngIMSubFuncNEWM,   KImpsServiceNEWM   ),
    // Authorize functions
    MAPPING( KPEngIMSubFuncGLBLU,  KImpsServiceGLBLU  ),
    MAPPING( KPEngIMSubFuncBLENT,  KImpsServiceBLENT  ),
    MAPPING_TERMINATION
    };

/**
 * Group Functions & sub-functions.
 */
/**
* IM Functions & sub-functions.
*/
const SCspFunctionMap KFuncMapGroup[] =
    {
    MAPPING( KPEngGRManageFunctions, EGroupMgmtFunc ),
    MAPPING( KPEngGRUseFunctions,    EGroupUseFunc  ),
    MAPPING( KPEngGRAuthorFunctions, EGroupAuthFunc ),
    MAPPING_TERMINATION
    };

const SCspSubFunctionMap KSubFuncMapGroup[] =
    {
    // Manager functions
    MAPPING( KPEngGRSubFuncCREAG,  KImpsServiceCREAG  ),
    MAPPING( KPEngGRSubFuncDELGR,  KImpsServiceDELGR  ),
    MAPPING( KPEngGRSubFuncGETGP,  KImpsServiceGETGP  ),
    MAPPING( KPEngGRSubFuncSETGP,  KImpsServiceSETGP  ),
    // Use functions
    MAPPING( KPEngGRSubFuncSUBGCN, KImpsServiceSUBGCN ),
    MAPPING( KPEngGRSubFuncGRCHN,  KImpsServiceGRCHN  ),
    // Authorization functions
    MAPPING( KPEngGRSubFuncGETGM,  KImpsServiceGETGM  ),
    MAPPING( KPEngGRSubFuncADDGM,  KImpsServiceADDGM  ),
    MAPPING( KPEngGRSubFuncRMVGM,  KImpsServiceRMVGM  ),
    MAPPING( KPEngGRSubFuncMBRAC,  KImpsServiceMBRAC  ),
    MAPPING( KPEngGRSubFuncREJEC,  KImpsServiceREJEC  ),
    MAPPING_TERMINATION
    };


// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DebugDumpFeatureSubtree()
// -----------------------------------------------------------------------------
//
#ifdef PENG_ENABLE_DEBUG_PRINT
void DebugDumpFeatureSubtree( const TDesC& aFeatureName,
                              TPEngWVCspFeature2& aPEFeature,
                              const SCspFunctionMap aFunctionMap[],
                              const SCspSubFunctionMap aSubFunctionMap[] )

    {
    //Whole feature
    PENG_DP( D_PENG_LIT( "%S, support status: %d" ),
             &aFeatureName,  aPEFeature.FeatureSupported() );

    //Functions
    PENG_DP( D_PENG_LIT( "%S, supported functions:" ), &aFeatureName );
    for ( TInt i( 0 ); aFunctionMap[ i ].iPeFunc != KPEngNullFunction; ++i )
        {
        const SCspFunctionMap functionMap = aFunctionMap[ i ];

        if ( aPEFeature.FunctionSupported( functionMap.iPeFunc ) )
            {
            TPtrC peFuncName( functionMap.iPeFuncName );
            PENG_DP( D_PENG_LIT( "   %S" ), &peFuncName );
            }
        }


    //Sub-functions
    PENG_DP( D_PENG_LIT( "%S, supported sub-functions:" ), &aFeatureName );
    for ( TInt ii( 0 ); aSubFunctionMap[ ii ].iPeSubFunc != KPEngNullFunction; ++ii )
        {
        const SCspSubFunctionMap subFuncMap = aSubFunctionMap[ ii ];
        if ( aPEFeature.SubFunctionSupported( subFuncMap.iPeSubFunc ) )
            {
            TPtrC peSubFuncName( subFuncMap.iPeSubFuncName );
            PENG_DP( D_PENG_LIT( "   %S" ), &peSubFuncName );
            }
        }
    }
#endif


// -----------------------------------------------------------------------------
// CopyFeatureSubtree()
// -----------------------------------------------------------------------------
//
void CopyFeatureSubtree( TPEngWVCspFeature2& aPEFeature,
                         TImpsFeature& aWVFeature,
                         const SCspFunctionMap aFunctionMap[],
                         const SCspSubFunctionMap aSubFunctionMap[] )

    {
    // Is feature supported at all
    aPEFeature.iSupported = aWVFeature.IsSupported();

    TInt holder;

    //Functions
    holder = KPEngNullFunction;
    for ( TInt i( 0 ); aFunctionMap[ i ].iPeFunc != KPEngNullFunction; ++i )
        {
        const SCspFunctionMap functionMap = aFunctionMap[ i ];
        if ( aWVFeature.Function( functionMap.iImpsEngFunc ) )
            {
            holder |=  functionMap.iPeFunc;
            }
        }
    aPEFeature.iFunctions = holder;

    //Sub-functions
    holder = KPEngNullFunction;
    for ( TInt ii( 0 ); aSubFunctionMap[ ii ].iPeSubFunc != KPEngNullFunction; ++ii )
        {
        const SCspSubFunctionMap subFunctionMap = aSubFunctionMap[ ii ];
        if ( aWVFeature.Capability( subFunctionMap.iImpsEngSubFunc ) )
            {
            holder = holder + subFunctionMap.iPeSubFunc;
            }
        }
    aPEFeature.iSubFunctions = holder;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAccessHandler::CPEngAccessHandler
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngAccessHandler::CPEngAccessHandler(
    MPEngSessionManager& aSessManager,
    const CPEngSessionSlotId& aSessionId )
        : iCount( 1 ), // init count to 1
        iSessionManager( aSessManager ),
        iSessionId( aSessionId ),
        iSessStatus( EPEngNWPresenceSessionClosed ),
        iOperation( EIdle )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::CPEngAccessHandler" ) );
    iSessionManager.OpenRef();
    }

// -----------------------------------------------------------------------------
// CPEngAccessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::ConstructL(
    RImpsEng& aSessClient )
    {
    iAccessClient.RegisterL( aSessClient, this );
    iAccessClient.RegisterErrorObserverL( *this );
    iAccessClient.RegisterStatusObserverL( this );
    }

// -----------------------------------------------------------------------------
// CPEngAccessHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAccessHandler* CPEngAccessHandler::NewL(
    RImpsEng& aSessClient,
    MPEngSessionManager& aSessManager,
    const CPEngSessionSlotId& aSessionId )
    {
    CPEngAccessHandler* self = new ( ELeave ) CPEngAccessHandler(
        aSessManager,
        aSessionId );
    CleanupClosePushL( *self );
    self->ConstructL( aSessClient );
    CleanupStack::Pop();
    return self;
    }


// Destructor
CPEngAccessHandler::~CPEngAccessHandler()
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::~CPEngAccessHandler" ) );
    __ASSERT_DEBUG( ( iObservers.Count() == 0 ), User::Panic(
                        KPEngTransferAdapterPanic, EOpIdConsistencyLost ) );

    delete iLogInData;
    iAccessClient.Unregister();
    iSessionManager.CloseAccessHandler();
    iObservers.Reset();
    }


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// =============================================================================
// =============== New Functions of the MPEngAccessHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAccessHandler::Close()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::Close()
    {
    iCount--;
    if ( iCount == 0 )
        {
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// CPEngAccessHandler::Open()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::Open()
    {
    iCount++;
    }

// -----------------------------------------------------------------------------
// CPEngAccessHandler::LogInL(
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::LogInL(
    TRequestStatus& aStatus,
    HBufC8* aLoginBuffer )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::LogInL" ) );

    if ( iOperation != EIdle )
        {
        // client is busy, leave
        delete aLoginBuffer;
        User::Leave( KErrNotReady );
        }
    // Are we logged?
    if ( iSessStatus == EPEngNWPresenceSessionOpen )
        {
        // we are already logged, this should not happened
        delete aLoginBuffer;
        User::Leave( KErrAlreadyExists );
        }

    delete iLogInData;
    iLogInData = aLoginBuffer;

    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::LogInL() - doing reactive login" ) );
    CIMPSSAPSettings* sapSetting = CIMPSSAPSettings::NewLC();
    PEngLoginDataCodec::UnPackLoginDataL( *iLogInData, *sapSetting );
    TImpsCspIdentifier sessIndentifier;
    sessIndentifier.SetUserIdL( sapSetting->SAPUserId() );
    sessIndentifier.SetSapL( sapSetting->SAPAddress() );

    // panic if we get other session log in data, user's gone wacko
    CheckConsistency( iSessionId, sessIndentifier );
    iOpId = iAccessClient.ReactiveLoginL(
                sessIndentifier,                    // Identifier
                sapSetting->SAPUserPassword(),      // User Password
                sapSetting->ClientId(),             // Client Id
                sapSetting->AccessPoint(),          // Access Point
                &( sapSetting->ServerAuthenticationName() ),    // Operator key 1
                &( sapSetting->ServerAuthenticationPassword() ) ); // Operator key 2

    CleanupStack::PopAndDestroy( sapSetting );

    // log in issued, set operation to the Log In
    aStatus = KRequestPending;
    iStatus = &aStatus;
    iOperation = EDoingLogIn;
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::LogOutL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::LogOutL( TRequestStatus& aStatus )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::LogOutL" ) );

    if ( iOperation != EIdle )
        {
        // client is busy, leave
        User::Leave( KErrNotReady );
        }

    // Are we even logged
    if ( iSessStatus != EPEngNWPresenceSessionOpen )
        {
        // we are already logged out
        return;
        }

    iOpId = iAccessClient.LogoutL();
    aStatus = KRequestPending;
    iStatus = &aStatus;
    iOperation = EDoingLogOut;
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::LogInData( )
// -----------------------------------------------------------------------------
//
const TDesC8& CPEngAccessHandler::LogInData( ) const
    {
    return *iLogInData;
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::CancelL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::Cancel()
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::Cancel, Status:%d operation:%d" ),
             iSessStatus,
             iOperation  );

    if ( iStatus )
        {
        switch ( iOperation )
            {
            case EDoingLogIn:
                {
                // cancel login
                TRAP_IGNORE( iAccessClient.CancelLoginL( iOpId ) );
                User::RequestComplete( iStatus, KErrCancel );
                iStatus = NULL;
                iOperation = ECanceling;
                break;
                }
            case EDoingLogOut:
                {
                // cancel logout
                // no cancel, complete and let it delete
                User::RequestComplete( iStatus, KErrCancel );
                iStatus = NULL;
                iOperation = ECanceling;
                break;
                }
            default:
                {
                // this should not happen, active, status, and idle operation
                // completed status
                User::RequestComplete( iStatus, KErrCancel );
                iStatus = NULL;
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::RegisterSessionObserver(
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::RegisterSessionObserverL( MPEngSessionStatusObserver& aObserver )
    {
    if ( KErrNotFound != iObservers.Find( &aObserver ) )
        {
        User::Leave( KErrAlreadyExists );
        }
    iObservers.AppendL( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::UnregisterSessionObserver(
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::UnregisterSessionObserver( MPEngSessionStatusObserver& aObserver )
    {
    TInt index ( iObservers.Find( &aObserver ) );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::ServiceCapabilities()
// -----------------------------------------------------------------------------
//
TPEngWVCspServicesTree2& CPEngAccessHandler::ServiceCapabilities()
    {
    return iServices;
    }

// -----------------------------------------------------------------------------
// CPEngAccessHandler::AssertLoginDataL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::AssertLoginDataL(
    TDesC8& aLogInData )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::AssertLoginDataL()" ) );
    __ASSERT_DEBUG( iLogInData, User::Panic(
                        KPEngTransferAdapterPanic, ELogInDataNotValid ) );

    CIMPSSAPSettings* originalData = CIMPSSAPSettings::NewLC();
    PEngLoginDataCodec::UnPackLoginDataL( *iLogInData, *originalData );
    CIMPSSAPSettings* assertData = CIMPSSAPSettings::NewLC();
    PEngLoginDataCodec::UnPackLoginDataL( aLogInData, *assertData );
    if (
        ( KErrNone != originalData->SAPUserPassword().Compare( assertData->SAPUserPassword() ) )
        ||
        ( KErrNone != originalData->SAPAddress().Compare( assertData->SAPAddress() ) )
        ||
        ( KErrNone != originalData->SAPUserId().Compare( assertData->SAPUserId() ) )
    )
        {
        User::Leave( KPEngNwErrInvalidPassword );
        }
    CleanupStack::PopAndDestroy( 2 ); // assertData, originalData
    }

// =============================================================================
// =============== New Functions of the MImpsAccessHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAccessHandler::HandleLoginL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::HandleLoginL(
    TInt aId,
    TImpsCspIdentifier& aCspId )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::HandleLoginL - id:  %d" ), aId );

    // panic if we get other session notification
    CheckConsistency( iSessionId, aCspId );
    // if operation came from some other session it should be 0
    __ASSERT_DEBUG( ( aId == iOpId ), User::Panic(
                        KPEngTransferAdapterPanic, EOpIdConsistencyLost ) );
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::HandleLogoutL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::HandleLogoutL(
    TInt aId,
    TImpsCspIdentifier& aCspId )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::HandleLogoutL - id:  %d" ), aId );

    // panic if we get other session notification
    CheckConsistency( iSessionId, aCspId );
    TPEngNWSessionSlotEvent event;
    // Operation id -1 means that server kicked us out
    if ( aId == -1 )
        {
        event = EPEngEventNWSessionClosedByServer;
        }
    else
        {
        event = EPEngEventAppNWPresenceSessionClosed;
        }

    if ( iStatus )
        {
        if ( iOperation == EDoingLogOut )
            {
            User::RequestComplete( iStatus, KErrNone );
            }
        else
            {
            User::RequestComplete( iStatus, KErrCancel );
            }
        iStatus = NULL;
        iOperation = EIdle;
        iOpId = KErrNone;
        }

    // notify observers
    NotifyIfNewStateL( EPEngNWPresenceSessionClosed, event );
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::HandleLoginCancelL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::HandleLoginCancelL(
    TInt aCancelledOpId,
    TImpsCspIdentifier& aCspId )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::HandleLoginCancelL - id:  %d" ),
             aCancelledOpId );

    // panic if we get other session notification
    CheckConsistency( iSessionId, aCspId );
    __ASSERT_DEBUG( ( aCancelledOpId == iOpId ),
                    User::Panic( KPEngTransferAdapterPanic, EOpIdConsistencyLost ) );

    iSessStatus = EPEngNWPresenceSessionClosed;
    iOperation = EIdle;
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::HandleNbrSessionsL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::HandleNbrSessionsL(
    TInt /* aId */,
    TInt /* aNbr */,
    TImpsCspIdentifier& /* aCspId */ )
    {

    }


// =============================================================================
// =============== New Functions of the MImpsErrorHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAccessHandler::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::HandleErrorL(
    TInt aStatus,
    TInt /*aOpId*/,
    const TDesC* /* aDescription */,
    const CImpsDetailed* /* aDetailedRes */,
    TImpsCspIdentifier& aCspId )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::HandleErrorL - id:  %d" ), aStatus );
    // panic if we get other session notification
    CheckConsistency( iSessionId, aCspId );

    // we get here if service negotiation failed
    if ( iStatus )
        {
        iOperation = EIdle;
        User::RequestComplete( iStatus, aStatus );
        iStatus = NULL;
        }
    }


// =============================================================================
// =============== New Functions of the MImpsStatusHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAccessHandler::HandleStatusChangeL()
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::HandleStatusChangeL(
    TImpsServiceStatus aStatus,
    TImpsCspIdentifier& aCspId )
    {
    PENG_DP( D_PENG_LIT( "CPEngAccessHandler::HandleStatusChangeL:  %d" ), aStatus );
    // panic if we get other session notification
    CheckConsistency( iSessionId, aCspId );
    switch ( aStatus )
        {
        case EImps_ON_LINE:
            {
            HandleLogInCompletionL();
            break;
            }
        case EImps_NOT_LOGGED:
            {
            NotifyIfNewStateL( EPEngNWPresenceSessionClosed,
                               EPEngEventAppNWPresenceSessionClosed );
            iServiceFetched = EFalse;
            break;
            }
        case EImps_SHUTTING_DOWN:
            {
            NotifyIfNewStateL( EPEngNWPresenceSessionClosed,
                               EPEngEventNWSessionClosedByServer );
            iServiceFetched = EFalse;
            break;
            }
        default:
            {
            // do nothing
            User::Panic( KPEngTransferAdapterPanic, EUnknownUnumeration );
            }
        }
    }


// =============================================================================
// =============== New Private Functions of main class =========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAccessHandler::CopyTree()
// Copy service tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::CopyServiceTree(
    TImpsServices& aFrom )
    {
    PENG_DP( D_PENG_LIT( "Network session capability details:" ) );

    TImpsFeature* wvFeature = NULL;
    // fundamental features
    wvFeature = aFrom.Feature( EFundamentalFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iFundamentalFeat,
                            *wvFeature,
                            KFuncMapFundamental,
                            KSubFuncMapFundamental );
#ifdef PENG_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "FundamentalFeatures" ), // CSI: 78 #
                                 iServices.iFundamentalFeat,
                                 KFuncMapFundamental,
                                 KSubFuncMapFundamental );
#endif
        }

    wvFeature = NULL;
    // Presence features
    wvFeature = aFrom.Feature( EPresenceFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iPresenceFeat,
                            *wvFeature,
                            KFuncMapPresence,
                            KSubFuncMapPresence );
#ifdef PENG_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "PresenceFeatures" ), // CSI: 78 #
                                 iServices.iPresenceFeat,
                                 KFuncMapPresence,
                                 KSubFuncMapPresence );
#endif
        }

    wvFeature = NULL;
    // Im features
    wvFeature = aFrom.Feature( EIMFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iIMFeat,
                            *wvFeature,
                            KFuncMapIm,
                            KSubFuncMapIm );
#ifdef PENG_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "IMFeatures" ), // CSI: 78 #
                                 iServices.iIMFeat,
                                 KFuncMapIm,
                                 KSubFuncMapIm );
#endif
        }

    wvFeature = NULL;
    // Group features
    wvFeature = aFrom.Feature( EGroupFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iGroupFeat,
                            *wvFeature,
                            KFuncMapGroup,
                            KSubFuncMapGroup );
#ifdef PENG_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "GroupFeatures" ), // CSI: 78 #
                                 iServices.iGroupFeat,
                                 KFuncMapGroup,
                                 KSubFuncMapGroup );
#endif
        }
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::NotifyIfNewStateL()
// Notify if new state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::NotifyIfNewStateL(
    TPEngNWSessionSlotState aNewState,
    TPEngNWSessionSlotEvent aEvent )
    {
    if ( iSessStatus != aNewState )
        {
        iSessStatus = aNewState;
        for ( TInt x( iObservers.Count() - 1 ) ; x >= 0 ; --x )
            {
            iObservers[ x ]->StatusChangedL( iSessStatus, aEvent );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAccessHandler::HandleLogInCompletionL()
// Handle completion of the log in request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAccessHandler::HandleLogInCompletionL( )
    {
    if ( !iServiceFetched )
        {
        // copy tree of capabilities
        TImpsServices services;
        iAccessClient.GetServicesL( services );
        CopyServiceTree( services );
        iServiceFetched = ETrue;
        }

    NotifyIfNewStateL( EPEngNWPresenceSessionOpen,
                       EPEngEventAppNWPresenceSessionOpened );

    if ( iStatus )
        {
        // handle Operation
        iOperation = EIdle;
        User::RequestComplete( iStatus, KErrNone );
        iStatus = NULL;
        iOpId = KErrNone;
        }
    }

// End of File


